#pragma once

#if defined(_WIN32) || defined(_WIN64)
#define TQDM_ISWIN
#else
#define TQDM_ISNIX
#endif

namespace tqdm
{

}  // tqdm
