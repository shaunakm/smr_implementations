#ifndef STACK_IBR_HPP
#define STACK_IBR_HPP

#include <atomic>
#include <iostream>
#include "IntervalBasedReclamation.hpp"

#define STACK_EPOCH_FREQUENCY 150
#define STACK_RECLAIM_FREQUENCY 30

template<typename T>
class Stack_IBR {
private:
    struct Node {
        T* item;
        uint64_t birth_epoch;
        uint64_t retire_epoch;
        std::atomic<Node*> next;

        Node(T* item)
        {
            item = item;
            next.store(nullptr);
            birth_epoch = 0;
            retire_epoch = 0;
        }
    };

    std::atomic<Node*> top;
    const int numOfThreads;
    IntervalBasedReclamation<Node> ibrStack{numOfThreads,STACK_EPOCH_FREQUENCY,STACK_RECLAIM_FREQUENCY};

public:

    Stack_IBR(int numOfThreads) : numOfThreads{numOfThreads} {
        Node* sentinel = new Node(nullptr);
        top.store(sentinel, std::memory_order_relaxed);
    }

    ~Stack_IBR(){
        while(pop(0) != nullptr);
        delete top.load();
    }

    bool push(T* item, int threadID)
    {
        if(item == nullptr)
        {
            return false;
        }
        Node* node = new Node(item);
        node = ibrStack.allocNode(threadID,node);
        ibrStack.start_op(threadID);
        while(true)
        {
            Node* temp = top.load();
            if(temp == top.load())
            {
                node->next.store(temp, std::memory_order_relaxed);
                if(top.compare_exchange_strong(temp,node))
                {
                    ibrStack.end_op(threadID);
                    return true;
                }
            }
        }   
    }

    T* pop(int threadID)
    {
        Node *temp, *next;
        T* ret_data;
        ibrStack.start_op(threadID);
        while(true)
        {
            temp = top.load();
            if(temp == nullptr)
            {
                ibrStack.end_op(threadID);
                return nullptr;
            }
            if(top.load() != temp)
            {
                continue;
            }
            next = temp->next.load();
            if(top.compare_exchange_strong(temp, next))
            {
                ret_data = temp->item;
                ibrStack.retireNode(temp,threadID);
                ibrStack.end_op(threadID);
                return ret_data;;
            }
        }
    }

    uint64_t getRetiredCountStack(int threadID)
    {
        return ibrStack.getRetiredNodeCount(threadID);
    }
};

#endif