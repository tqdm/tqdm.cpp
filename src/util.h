#pragma once

#include <cstddef>


namespace tqdm
{
    // Write a buffer fully or not at all.
    bool write_harder(int fd, const char *buf, size_t len);
}
