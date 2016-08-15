![][Logo]

tqdm
====

C++ port of the popular python module, [tqdm](https://github/tqdm/tqdm). Work in progress (pre-alpha)

![][Coverage-Status]

<!--
![][Build-Status] ![][Coverage-Status] ![][Branch-Coverage-Status]

![][DOI-URI]
-->

![][Licence]

`tqdm` (read taqadum, تقدّم) means "progress" in arabic.

Instantly make your loops show a smart progress meter - just wrap any
iterable with "tqdm(iterable)", and you're done!

``` cpp
#include "tqdm/tqdm.h"

for (int i : tqdm::range(5))  // same as for(int i = 0; i < 5; ++i)
  ...

// or manually with and array or container:
std::vector<float> a = {0, 1, 2, 3, 4, 5};
for (float &i : tqdm::tqdm(a))
  ...
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

All source code is hosted on [github](https://github.com/tqdm/tqdm.cpp).
Contributions are welcome.

See the
[CONTRIBUTE](https://raw.githubusercontent.com/tqdm/tqdm.cpp/master/CONTRIBUTE)
file for more information.


Licence
-------

Open Source (OSI approved): ![][Licence]


![][Readme-Hits] (Since 9 August 2016)

  [Logo]: https://raw.githubusercontent.com/tqdm/tqdm/master/logo.png
  [Screenshot]: https://raw.githubusercontent.com/tqdm/tqdm.cpp/master/images/tqdm.gif
  [Github-Status]: https://img.shields.io/github/tag/tqdm/tqdm.cpp.svg?maxAge=2592000 "https://github.com/tqdm/tqdm.cpp/releases"
  [Github-Forks]: https://img.shields.io/github/forks/tqdm/tqdm.cpp.svg "https://github.com/tqdm/tqdm.cpp/network"
  [Github-Stars]: https://img.shields.io/github/stars/tqdm/tqdm.cpp.svg "https://github.com/tqdm/tqdm.cpp/stargazers"
  [Licence]: https://img.shields.io/pypi/l/tqdm.svg "https://raw.githubusercontent.com/tqdm/tqdm.cpp/master/LICENCE"
  [Readme-Hits]: http://hitt.herokuapp.com/tqdm/tqdm_cpp.svg
  [Coverage-Status]: https://coveralls.io/repos/github/tqdm/tqdm.cpp/badge.svg?branch=master "https://coveralls.io/github/tqdm/tqdm.cpp?branch=master"
