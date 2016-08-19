#pragma once

#include <atomic>

#include "tqdm/_fwd.h"
#include "tqdm/_list.h"


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

    class Sink : public AtomicNode<Sink>
    {
        SinkOptions opts;
        AtomicList<AbstractLine> lines;
    public:
        explicit Sink(SinkOptions o);
        Sink(Sink&&) = delete;
        Sink& operator = (Sink&&) = delete;
    };

    extern Sink standard_sink;
}
