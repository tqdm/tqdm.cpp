// no include guards - only included from one file


namespace tqdm
{
    template<class Node>
    void AtomicList<Node>::append(Node *node)
    {
        // TODO when deletion is added, need to disable that
        // for the duration of this function (using a spinlock).
        // But, if the spinlock is currently held by the main thread,
        // and we are a signal handler, we need to advance anyway.
        assert (node->intrusive_link.load() == nullptr);
        // Note that tail is a pointer to a (usually) NULL pointer.
        std::atomic<Node *> *tail = this->approx_tail.load();
        assert (tail != nullptr);
        while (true)
        {
            // The tail may have moved, if someone else is also appending.
            while (Node *tmp = tail->load())
            {
                tail = &tmp->intrusive_link;
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
        this->approx_tail.store(&node->intrusive_link);
    }
}
