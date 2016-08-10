#include "tqdm/line.hpp"

#include <cstring>

#include "util.hpp"


namespace tqdm
{
    // Due to how vtables work, it is cheaper to *not* inline this.
    AbstractLine::~AbstractLine() {}

    void StaticTextLine::write(int fd)
    {
        bool ok = write_harder(fd, this->text, strlen(this->text));
        if (ok)
            this->not_dirty();
    }
}
