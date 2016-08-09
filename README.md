![][Logo]

tqdm
====

C++ port of the popular python module, [tqdm](https://tqdm.github.io). Work in progress (pre-alpha)

<!--
![][Build-Status] ![][Coverage-Status] ![][Branch-Coverage-Status]

![][DOI-URI]
-->

![][License]

`tqdm` (read taqadum, تقدّم) means "progress" in arabic.

Instantly make your loops show a smart progress meter - just wrap any
iterable with "tqdm(iterable)", and you're done!

``` cpp
#include "tqdm/tqdm.h"

int a[] = {0, 1, 2, 3, 4, 5};
for (int i : tqdm::tqdm(a, a + 5))
  ...
```

Here's what the output looks like:

``76%|████████████████████████████         | 7568/10000 [00:33<00:10, 229.00it/s]``

`tqdm::range(N)` can be also used as a convenient shortcut for
`tqdm::tqdm(std::vector<int>({0, ..., N - 1}))`.

![][Screenshot]

It can also be executed stand-alone with pipes:

``` sh
$ seq 9999999 | tqdm --unit_scale | wc -l
10.0Mit [00:02, 3.58Mit/s]
9999999
```


Contributions
-------------

All source code is hosted on [github](https://github.com/tqdm/tqdm.cpp).
Contributions are welcome.

See the
[CONTRIBUTE](https://raw.githubusercontent.com/tqdm/tqdm.cpp/master/CONTRIBUTE)
file for more information.


License
-------

Open Source (OSI approved): 

![][License]
![][Readme-Hits] (Since 19 May 2016)

  [Logo]: https://raw.githubusercontent.com/tqdm/tqdm/master/logo.png
  [Screenshot]: https://raw.githubusercontent.com/tqdm/tqdm.cpp/master/images/tqdm.gif
  [Github-Status]: https://img.shields.io/github/tag/tqdm/tqdm.cpp.svg?maxAge=2592000 "https://github.com/tqdm/tqdm.cpp/releases"
  [Github-Forks]: https://img.shields.io/github/forks/tqdm/tqdm.cpp.svg "https://github.com/tqdm/tqdm.cpp/network"
  [Github-Stars]: https://img.shields.io/github/stars/tqdm/tqdm.cpp.svg "https://github.com/tqdm/tqdm.cpp/stargazers"
  [License]: https://img.shields.io/pypi/l/tqdm.svg "https://raw.githubusercontent.com/tqdm/tqdm.cpp/master/LICENSE"
  [Readme-Hits]: http://hitt.herokuapp.com/tqdm/tqdm_cpp.svg
