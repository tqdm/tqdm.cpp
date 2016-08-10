#include "util.hpp"

#include <cassert>
#include <cerrno>

#include <poll.h>
#include <unistd.h>


namespace tqdm
{
    static void wait_for_write(int fd)
    {
        struct pollfd pfd;
        pfd.fd = fd;
        pfd.events = POLLOUT;
        (void)::poll(&pfd, 1, -1);
    }

    // If false is returned, caller may check errno to see if it's EAGAIN
    // or a real error.
    bool write_harder(int fd, const char *buf, size_t len)
    {
        bool did_anything = false;

        while (len)
        {
            ssize_t res = ::write(fd, buf, len);
            if (res == -1)
            {
                if (errno == EAGAIN)
                {
                    if (!did_anything)
                    {
                        return false;
                    }
                    wait_for_write(fd);
                    continue;
                }
                return false;
            }
            assert (res != 0);
            did_anything = true;
            buf += res;
            len -= res;
        }
        return true;
    }
}
