#include "tqdm/_line.h"

#include <cstring>

#include "util.h"


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
