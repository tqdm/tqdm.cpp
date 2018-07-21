![][Logo]

tqdm
====

**Official** C++ port of the popular python module, [tqdm](https://github.com/tqdm/tqdm). Work in progress (pre-alpha)

[![][Build-Status]][Build-Status-lnk] [![][Coverage-Status]][Coverage-Status-lnk]

<!--
[Build-Status] [Coverage-Status] [Branch-Coverage-Status]

[DOI-URI]
-->

[![][Licence]][Licence-lnk]

`tqdm` (read taqadum, تقدّم) means "progress" in arabic.

Instantly make your loops show a smart progress meter - just wrap any
iterable with `tqdm(iterable)`, and you're done!

``` cpp
#include "tqdm/tqdm.h"

for (int i : tqdm::range(5))  // for(int i = 0; i < 5; ++i)
  ...

// even more pythonic
// for(float i = 2.3f; i < 9.1f; i += 0.1f)
for (auto i : tqdm::range(2.3f, 9.1f, 0.1f))
  ...

// container and iterator wrappers
std::vector<unsigned int> a = {0, 1, 2, 3, 4, 5};
for (auto &i : tqdm::tqdm(a))
  for (auto &j : tqdm::tqdm(a.begin(), a.end()))
    i += j;
```

Here's what the output will look like:

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


Installation
------------

No installation or dependencies are required. `tqdm` is header-only.
Simply copy or add the `include` directory to your project.
Best-practice recommendation is to add this repo as a submodule to projects.
Once added, simply `#include "tqdm/tqdm.h"`.


Contributions
-------------

All source code is hosted on [GitHub](https://github.com/tqdm/tqdm.cpp).
Contributions are welcome.

See the
[CONTRIBUTE.md](https://raw.githubusercontent.com/tqdm/tqdm.cpp/master/CONTRIBUTE.md)
file for more information.


Licence
-------

Open Source (OSI approved): [![][Licence]][Licence-lnk]


![][Readme-Hits] (Since 9 August 2016)

  [Logo]: https://raw.githubusercontent.com/tqdm/tqdm/master/logo.png
  [Screenshot]: https://raw.githubusercontent.com/tqdm/tqdm/master/images/tqdm.gif
  [Github-Status]: https://img.shields.io/github/tag/tqdm/tqdm.cpp.svg?maxAge=2592000
  [Github-Status-lnk]: https://github.com/tqdm/tqdm.cpp/releases
  [Github-Forks]: https://img.shields.io/github/forks/tqdm/tqdm.cpp.svg
  [Github-Forks-lnk]: https://github.com/tqdm/tqdm.cpp/network
  [Github-Stars]: https://img.shields.io/github/stars/tqdm/tqdm.cpp.svg
  [Github-Stars-lnk]: https://github.com/tqdm/tqdm.cpp/stargazers
  [Licence]: https://img.shields.io/pypi/l/tqdm.svg
  [Licence-lnk]: https://raw.githubusercontent.com/tqdm/tqdm.cpp/master/LICENCE
  [Readme-Hits]: http://hitt.herokuapp.com/tqdm/tqdm_cpp.svg
  [Coverage-Status]: https://coveralls.io/repos/github/tqdm/tqdm.cpp/badge.svg?branch=master
  [Coverage-Status-lnk]: https://coveralls.io/github/tqdm/tqdm.cpp?branch=master
  [Build-Status]: https://travis-ci.org/tqdm/tqdm.cpp.svg?branch=master
  [Build-Status-lnk]: https://travis-ci.org/tqdm/tqdm.cpp
