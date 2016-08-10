#pragma once


namespace tqdm
{
    // A non-owning intrusive linked list,
    // using atomics to ensure thread- and signal- safety.
    template<class Node>
    class AtomicList
    {
        std::atomic<Node *> head;
        std::atomic<std::atomic<Node *> *> approx_tail;
    public:
        AtomicList() : head(nullptr), approx_tail(&head) {}
        // Nothing to do - we didn't allocate any objects, merely borrow.
        ~AtomicList() {}

        // Forbid moves completely
        AtomicList(AtomicList&&) = delete;
        AtomicList& operator = (AtomicList&&) = delete;

        void append(Node *node);
    };
}


#include "tqdm/list.tcc"
