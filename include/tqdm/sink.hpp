#pragma once

#include <atomic>

#include "tqdm/fwd.hpp"


namespace tqdm
{
    struct SinkOptions
    {
        // Only mandatory field. Everything else can just be zeroed.
        int fd;

        int tty_width;
        int tty_height;
        // Additional options will be added in future.
    };

    class Sink
    {
        SinkOptions opts;
        std::atomic<AbstractLine *> lines;
    public:
        explicit Sink(SinkOptions o) : opts(o) {}
        Sink(Sink&&) = delete;
        Sink& operator = (Sink&&) = delete;
    };

    extern Sink standard_sink;
}
