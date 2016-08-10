#include "tqdm/sink.hpp"

#include <unistd.h>


namespace tqdm
{
    // We do still need a global list of sinks in order to handle signals.
    // This is still a win over making a single global list of AbstractLine
    // instances, since we can skip entirely any Sink which does not express
    // interest in asynchronous updates.
    static AtomicList<Sink> all_sinks;

    Sink standard_sink((SinkOptions){fd: STDERR_FILENO});

    Sink::Sink(SinkOptions o) : opts(o)
    {
        all_sinks.append(this);
    }
}
