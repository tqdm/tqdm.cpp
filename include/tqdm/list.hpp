#pragma once

#include <type_traits>

#include "tqdm/fwd.hpp"


namespace tqdm
{
    // CRTP
    template<class Node>
    class AtomicNode
    {
        friend class AtomicList<Node>;

        std::atomic<Node *> intrusive_link_next;
        std::atomic<Node *> intrusive_link_prev;

        AtomicNode(Node *next, Node *prev);
    public:
        // Node is initially unattached
        AtomicNode();
        ~AtomicNode();
    };

    // A non-owning intrusive linked list,
    // using atomics to ensure thread- and signal- safety.
    template<class Node>
    class AtomicList
    {
        AtomicNode<Node> meta;
    public:
        AtomicList();
        ~AtomicList();

        void append(Node *node);
    };
}


#include "tqdm/list.tcc"
