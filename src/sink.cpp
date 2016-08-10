#include "tqdm/sink.hpp"

#include <unistd.h>


namespace tqdm
{
    Sink standard_sink((SinkOptions){fd: STDERR_FILENO});
}
