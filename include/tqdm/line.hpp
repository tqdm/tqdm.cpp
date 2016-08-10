#pragma once

#include <atomic>

#include <cstddef>

#include "tqdm/fwd.hpp"


namespace tqdm
{
    class AbstractLine
    {
        friend class Sink;
        std::atomic<AbstractLine *> intrusive_link;

        struct
        {
            bool dirty : 1;
        } flags;
    public:
        AbstractLine() : flags{} {}
        virtual ~AbstractLine();

        virtual void write(int fd) = 0;
    protected:
        void not_dirty()
        {
            this->flags.dirty = false;
        }
    };

    class StaticTextLine : public AbstractLine
    {
        const char *text;
    public:
        template<size_t n>
        StaticTextLine(const char(&lit)[n]) : text(lit) {}
        void write(int fd) override;
    };
}
