// no include guards - only included from one file

#include <cassert>


namespace tqdm
{
    template<class Node>
    AtomicNode<Node>::AtomicNode(Node *next, Node *prev)
    {
        intrusive_link_next.store(next);
        intrusive_link_prev.store(prev);
    }
    template<class Node>
    AtomicNode<Node>::AtomicNode()
    {
        intrusive_link_next.store(nullptr);
        intrusive_link_prev.store(nullptr);
    }
    template<class Node>
    AtomicNode<Node>::~AtomicNode()
    {
    }

    // To more easily maintain the doubly-linked structure, loop to itself
    // rather than using NULL pointers.
    template<class Node>
    AtomicList<Node>::AtomicList()
    : meta(static_cast<Node *>(&meta), static_cast<Node *>(&meta))
    {
    }

    // Nothing to do - we didn't allocate any objects, merely borrow.
    template<class Node>
    AtomicList<Node>::~AtomicList()
    {
        // TODO: We *really* shouldn't get here with a non-empty node set.
        // Should we set all nodes to NULL to indicate they have?
        // Otherwise we're stuck with dangling pointers in the edges ...
    }

    template<class Node>
    void AtomicList<Node>::append(Node *node)
    {
        (void)node;
#if 0
        Node *singular = &meta;
        // TODO when deletion is added, need to disable that
        // for the duration of this function (using a spinlock).
        // But, if the spinlock is currently held by the main thread,
        // and we are a signal handler, we need to advance anyway.
        // I think our spinlocks need to be recursive, which means tracking
        // the thread ID.
        //
        // Actually, we might not need to disable it, just make sure it
        // always points to a later node, perhaps? But what if there
        // is no later node? Maybe use the list-itself-is-a-node trick?
        assert (node->intrusive_link_next.load() == nullptr);
        assert (node->intrusive_link_prev.load() == nullptr);
        // Note that tail is a pointer to a (usually) NULL pointer.
        std::atomic<Node *> *tail = this->approx_tail;
        assert (tail != nullptr);
        while (true)
        {
            // The tail may have moved, if someone else is also appending.
            while (Node *tmp = tail->load())
            {
                tail = &tmp->intrusive_link_next;
            }
            // First argument is a reference, but we can't re-use
            // the new value, because we want to advance.
            Node *expected = nullptr;
            if (tail->compare_exchange_weak(expected, node))
            {
                break;
            }
        }
        // If we're wrong, nobody cares until the next append,
        // which will fix this anyway.
        this->approx_tail = &node->intrusive_link_next;
#endif
    }
}
