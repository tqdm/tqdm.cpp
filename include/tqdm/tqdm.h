#pragma once

/**
Customisable progressbar decorator for iterators.
Includes a default range iterator printing to stderr.

TODO:
* handle s(tep) with operator+/-(=)
* chrono and delay printing
* class status printer
* iterator-less: fake range iterable, update() increments value

Usage:
  # include "tqdm/tqdm.h"
  for(int i : tqdm::range(4))
  // same as:
  //   for (int i : tqdm::tqdm({0, 1, 2, 3}))
  // or:
  //   std::vector<int> v{0, 1, 2, 3};
  //   for (int &i : tqdm::tqdm(v.begin(), v.end())
    ...

@author Casper dC-L <github.com/casperdcl>
@version 0.0.1-rc1.py4.8.4
*/

#include <cassert>      // assert
#include <cinttypes>    // PRIu64
#include <cstddef>      // ptrdiff_t, size_t
#include <cstdint>      // int64_t
#include <cstdio>       // printf
#include <iterator>     // iterator
#include <limits>       // numeric_limits
#include <stdexcept>    // throw
#include <string>       // string
#include <type_traits>  // is_pointer, ...
#include <utility>      // swap
#include <functional>   // function
// #include <wchar.h>      // wchar_t (TODO: stdafx, use for unicode!)
#include "tqdm/utils.h"

namespace tqdm {

struct Params {
  std::string desc;
  // size_t total = -1;
  bool leave = true;
  FILE *f = stderr;
  int ncols =
      60;  // TODO: -1, then dynamically alloc first time with fallback
  float mininterval = 0.1f;
  float maxinterval = 10.0f;
  bool dynamic_miniters = false;
  size_t miniters = 10;
  std::string ascii = " 123456789#";
  bool disable = false;
  std::string unit = "it";
  bool unit_scale = false;
  bool dynamic_ncols = false;
  float smoothing = 0.3f;
  std::string bar_format;
  size_t initial = 0;
  int position = -1;
  bool gui = false;
};

struct StatusPrinter {
  size_t last_n;
  FILE *f;

  StatusPrinter(FILE *f = stderr) : last_n(0), f(f) {}

  /**
   Prints to internal file without clearing previous line.
   */
  virtual void operator[](const std::string &s) const {
    size_t bytes_remaining = s.size();
    auto buf_p = s.c_str();
    while (bytes_remaining) {
      size_t bytes_written = fwrite(buf_p, 1, bytes_remaining, f);
      bytes_remaining -= bytes_written;
      buf_p += bytes_written;
      if (ferror(f)) {
        perror("fwrite");
      }
    }
  }

  /**
   Prints to internal file overwriting current line.
   */
  virtual void operator()(const std::string &s) {
    operator[](s);
    for (unsigned i = s.size(); i < last_n; ++i)
      fwrite(" ", 1, 1, f);
    fwrite("\r", 1, 1, f);
    fflush(f);
    last_n = s.size();
  }
};

template <typename _Iterator> class Tqdm : public IteratorWrapper<_Iterator> {
protected:
  // typedef std::iterator_traits<_Iterator> traits;
  typedef IteratorWrapper<_Iterator> TQDM_IT;

public:
  typedef _Iterator iterator_type;
  typedef typename TQDM_IT::difference_type difference_type;
  // not used here, but included for compatibility
  typedef typename TQDM_IT::value_type value_type;
  typedef typename TQDM_IT::pointer pointer;
  typedef typename TQDM_IT::reference reference;
  typedef typename TQDM_IT::iterator_category iterator_category;

protected:
  const iterator_type b;  // begin
  iterator_type e;        // end
  difference_type total;
  Params self;  // ha, ha
  time_point start_t;
  iterator_type last_print_n;
  time_point last_print_t;
  float avg_time;
  int pos;  // position
  // status printer
  StatusPrinter sp;

  // TODO
  void moveto(int pos) const {
    if (pos < 0)
      for (int i = 0; i > pos; --i)
        sp[_term_move_up()];
    else if (pos > 0)
      for (int i = 0; i < pos; ++i)
        sp["\n"];
  }

  template <typename String,
            typename = typename std::enable_if<
                std::is_same<String, std::string>::value ||
                std::is_same<String, std::wstring>::value>::type>
  // TODO
  static String format_meter(difference_type n, difference_type total,
                             float elapsed, int ncols, const String &prefix,
                             const String &ascii, const String &unit,
                             bool unit_scale, float rate,
                             const String &bar_format) {
    typedef typename String::value_type Char;
    // sanity check: total
    if (n > total)
      total = -1;

    std::string elapsed_str =
        elapsed < 0 ? "" : format_interval(size_t(elapsed));

    // if unspecified, attempt to use rate = average speed
    // (we allow manual override since predicting time is an arcane art)
    if (rate < 0 && elapsed)
      rate = n / elapsed;
    float inv_rate = (rate && (rate < 1)) ? 1.0f / rate : -1;
    std::string rate_fmt;
    if (rate < 0.0f) {
      rate_fmt = "?";
    } else {
      if (unit_scale) {
        rate_fmt = format_sizeof(inv_rate < 0 ? rate : inv_rate);
      } else {
        rate_fmt.reserve(80);
        sprintf(&rate_fmt.front(), "%5.2f", inv_rate < 0 ? rate : inv_rate);
      }
    }

    rate_fmt += (inv_rate < 0 ? unit : "s/") + (inv_rate < 0 ? "/s" : unit);
    std::string n_fmt;
    std::string total_fmt;
    if (unit_scale) {
      n_fmt = format_sizeof(n);
      if (total >= 0)
        total_fmt = format_sizeof(total);
    } else {
      n_fmt = std::to_string(n);
      total_fmt = std::to_string(total);
    }

    // no total: no progressbar, ETA, just progress stats
    if (total < 0) {
      return prefix + n_fmt + unit + " [" + elapsed_str + ", " + rate_fmt +
             "]";
    }
    // total is known: we can predict some stats
    else {
      // fractional and percentage progress
      float frac = float(n) / float(total);
      float percentage = frac * 100.0f;

      std::string remaining_str =
          rate <= 0 ? "?"
                    : format_interval(difference_type((total - n) / rate));

      // format the stats displayed to the left and right sides of the bar
      String l_bar;
      l_bar.reserve(256);
      sprintf(&l_bar.front(), "%s%3.0f%%%s", prefix.c_str(), percentage,
              ncols > 0 ? "|" : "");
      String r_bar = String(ncols > 0 ? "|" : "") + " " + n_fmt + "/" +
                     total_fmt + " [" + elapsed_str + "<" + remaining_str +
                     ", " + rate_fmt + "]";
      if (ncols <= 0)
        return l_bar + r_bar;
      if (!bar_format.empty()) {
        return "";
        /*
            # Custom bar formatting
            # Populate a dict with all available progress indicators
            bar_args = {'n': n,
                        'n_fmt': n_fmt,
                        'total': total,
                        'total_fmt': total_fmt,
                        'percentage': percentage,
                        'rate': rate if inv_rate is None else inv_rate,
                        'rate_noinv': rate,
                        'rate_noinv_fmt': ((format_sizeof(rate)
                                           if unit_scale else
                                           '{0:5.2f}'.format(rate))
                                           if rate else '?') + unit + '/s',
                        'rate_fmt': rate_fmt,
                        'elapsed': elapsed_str,
                        'remaining': remaining_str,
                        'l_bar': l_bar,
                        'r_bar': r_bar,
                        'desc': prefix if prefix else '',
                        # 'bar': full_bar  # replaced by procedure below
                        }

            # Interpolate supplied bar format with the dict
            if '{bar}' in bar_format:
                # Format left/right sides of the bar, and format the bar
                # later in the remaining space (avoid breaking display)
                l_bar_user, r_bar_user = bar_format.split('{bar}')
                l_bar = l_bar_user.format(**bar_args)
                r_bar = r_bar_user.format(**bar_args)
            else:
                # Else no progress bar, we can just format and return
                return bar_format.format(**bar_args)
        */
      }

      // Formatting progress bar
      // space available for bar's display
      const int N_BARS =
          ncols > 0
              ? std::max(1, ncols - int(l_bar.size()) - int(r_bar.size()))
              : 10;

      int bar_length, frac_bar_length;
      String bar;
      Char frac_bar;
      // format bar depending on availability of unicode/ascii chars
      if (!ascii.empty()) {
        std::tie(bar_length, frac_bar_length) =
            divmod(int(frac * N_BARS * int(ascii.size() - 1)),
                   int(ascii.size() - 1));

        bar = String(bar_length, Char(ascii.back()));
        frac_bar = frac_bar_length ? 48 + frac_bar_length : ' ';
      } else {
        std::tie(bar_length, frac_bar_length) =
            divmod(int(frac * N_BARS * 8), 8);

        bar = String(bar_length, Char(0x2588));
        frac_bar = frac_bar_length ? Char(0x2590 - frac_bar_length) : ' ';
      }
      String full_bar;
      // whitespace padding
      if (bar_length < N_BARS)
        full_bar = bar + frac_bar +
                   String(std::max(N_BARS - bar_length - 1, 0), ' ');
      else
        full_bar = bar + String(std::max(N_BARS - bar_length, 0), ' ');

      // Piece together the bar parts
      return String(l_bar.c_str()) + full_bar + r_bar;
    }
  }

public:
  /** containter-like methods */
  // actually current value
  Tqdm &begin() { return *this; }
  const Tqdm &begin() const { return *this; }
  Tqdm end() const { return Tqdm(e, e); }

  /** constructors */
  explicit Tqdm(iterator_type begin, iterator_type end,
                Params self = Params())
      : TQDM_IT(begin), b(begin), e(end), total(end - begin), self(self),
        start_t(steady_clock::now()), last_print_n(begin),
        last_print_t(start_t), avg_time(-1), pos(0), sp(self.f) {}

  explicit Tqdm(iterator_type begin, difference_type total,
                Params self = Params())
      : TQDM_IT(begin), b(begin), e(begin + total), total(total), self(self),
        start_t(steady_clock::now()), last_print_n(begin),
        last_print_t(start_t), avg_time(-1), pos(0), sp(self.f) {}

  explicit Tqdm(const Tqdm &other)
      : TQDM_IT(other.base()), b(other.b), e(other.e), total(other.total),
        self(other.self), start_t(other.start_t),
        last_print_n(other.last_print_n), last_print_t(other.last_print_t),
        avg_time(other.avg_time), pos(other.pos), sp(other.sp) {}

  template <typename _Container,
            typename = typename std::enable_if<
                !std::is_same<_Container, Tqdm>::value>::type>
  Tqdm(_Container &v, Params self = Params())
      : TQDM_IT(std::begin(v)), b(std::begin(v)), e(std::end(v)),
        total(e - b), self(self), start_t(steady_clock::now()),
        last_print_n(b), last_print_t(start_t), avg_time(-1), pos(0),
        sp(self.f) {}

  explicit Tqdm &operator=(Tqdm &other) { this->Tqdm(other); }
  explicit const Tqdm &operator=(const Tqdm &other) { this->Tqdm(other); }

  // this is scary, non-standard
  // explicit operator bool() const { return this->base() != e; }

  /** warning: only checks equality with end sentinel.
  does not check > end, so update(>=2) may continue forever
  */
  inline bool ended() const { return this->base() == e; }
  inline difference_type size_remaining() const { return e - this->base(); }
  inline difference_type size_done() const { return this->base() - b; }

  Params &params() { return self; }
  const Params &params() const { return self; }

  /** TODO: magic methods */

  /** Cleanup and (if leave=False) close the progressbar. */
  virtual void close() {
    if (self.disable)
      return;

    // Prevent multiple closures
    self.disable = true;

    // decrement instance pos and remove from internal set
    // TODO: int pos = self.position;
    // TODO: decr_instances();

    // GUI mode
    // TODO: make ptr: if (!sp) return;

    // TODO
    auto fp_write = [this](const std::string &s) {

      auto buf_p = s.c_str();
      size_t bytes_remaining = s.size();
      while (bytes_remaining) {
        size_t bytes_written = fwrite(buf_p, 1, bytes_remaining, self.f);
        bytes_remaining -= bytes_written;
        buf_p += bytes_written;
        if (ferror(self.f)) {
          perror("fwrite");
        }
      }
    };

    // TODO: ? try{fp_write(""); } catch (...){return;}

    if (pos)
      moveto(pos);

    if (self.leave) {
      if (last_print_n < this->base()) {

        time_point cur_t = steady_clock::now();
        // stats for overall rate (no weighted average)
        sp(format_meter(size_done(), total, diff(cur_t, start_t),
                        self.dynamic_ncols
                            ?
                            // TODO: self.dynamic_ncols(self.fp)
                            80
                            : self.ncols,
                        self.desc, self.ascii, self.unit, self.unit_scale, -1,
                        self.bar_format));
      }
      if (pos)
        moveto(-pos);
      else
        fp_write("\n");
    } else {
      sp("");  // clear up last bar
      if (pos)
        moveto(-pos);
      else
        fp_write("\r");
    }
  }

  virtual inline void update(difference_type n) override {
    if (ended())
      throw std::out_of_range(
          "exhausted");  // TODO: don't throw, just double total
    if (n < 0) {
      printf("n (%s) cannot be negative", _s(n));
      throw std::out_of_range("negative step in forward iterator");
    } else if (!n)
      return;

    TQDM_IT::update(n);  // increment pointer

    if (self.disable)  // only safe to skip now
      return;

    difference_type delta_it = this->base() - last_print_n;
    if (delta_it >= difference_type(self.miniters)) {
      // We check the counter first, to reduce the overhead of now()
      time_point cur_t = steady_clock::now();
      float delta_t = diff(cur_t, last_print_t);
      if (delta_t >= self.mininterval) {
        float elapsed = diff(cur_t, start_t);
        // EMA (not just overall average)
        if (self.smoothing && delta_t) {
          avg_time = avg_time < 0 ? delta_t / delta_it
                                  : self.smoothing * delta_t / delta_it +
                                        (1 - self.smoothing) * avg_time;
        }

        // TODO
        if (pos)
          moveto(pos);

        // Print bar's update
        sp(format_meter(size_done(), total, elapsed,
                        self.dynamic_ncols
                            ?
                            // TODO: self.dynamic_ncols(self.fp)
                            80
                            : self.ncols,
                        self.desc, self.ascii, self.unit, self.unit_scale,
                        avg_time ? 1 / avg_time : -1, self.bar_format));

        if (pos)
          moveto(-pos);

        // If no `miniters` was specified, adjust automatically to the
        // maximum iteration rate seen so far.
        // e.g.: After running `tqdm.update(5)`, subsequent
        // calls to `tqdm.update()` will only cause an update after
        // at least 5 more iterations.
        if (self.dynamic_miniters) {
          if (self.maxinterval && delta_t > self.maxinterval) {
            self.miniters = self.miniters * self.maxinterval / delta_t;
          } else if (self.mininterval && delta_t) {
            self.miniters =
                self.smoothing * delta_it * self.mininterval / delta_t +
                (1 - self.smoothing) * self.miniters;
          } else {
            self.miniters = self.smoothing * delta_it +
                            (1 - self.smoothing) * self.miniters;
          }
        }

        // Store old values for next call
        last_print_n = this->base();
        last_print_t = cur_t;
      }
    }

    if (ended())
      close();
  }
};

template <typename _Iterator, typename _Tqdm = Tqdm<_Iterator>>
_Tqdm tqdm(_Iterator begin, _Iterator end, Params p = Params()) {
  return _Tqdm(begin, end, p);
}

template <typename _Iterator, typename _Tqdm = Tqdm<_Iterator>>
_Tqdm tqdm(_Iterator begin, size_t total, Params p = Params()) {
  return _Tqdm(begin, total, p);
}

template <typename _Container,
          typename _Tqdm = Tqdm<typename _Container::iterator>>
_Tqdm tqdm(_Container &v, Params p = Params()) {
  return _Tqdm(v, p);
}

template <size_t N, typename T, typename _Tqdm = Tqdm<T *>>
_Tqdm tqdm(T (&tab)[N], Params p = Params()) {
  return _Tqdm(tab, N, p);
}

template <typename SizeType = int>
using RangeTqdm = Tqdm<RangeIterator<SizeType>>;
template <typename SizeType>
RangeTqdm<SizeType> range(SizeType n, Params p = Params()) {
  return RangeTqdm<SizeType>(RangeIterator<SizeType>(n),
                             RangeIterator<SizeType>(n), p);
}
template <typename SizeType>
RangeTqdm<SizeType> range(SizeType start, SizeType end, Params p = Params()) {
  return RangeTqdm<SizeType>(RangeIterator<SizeType>(start, end),
                             RangeIterator<SizeType>(start, end), p);
}
template <typename SizeType>
RangeTqdm<SizeType> range(SizeType start, SizeType end, SizeType step,
                          Params p = Params()) {
  return RangeTqdm<SizeType>(RangeIterator<SizeType>(start, end, step),
                             RangeIterator<SizeType>(start, end, step), p);
}

}  // tqdm

/** Things to port:

class TqdmTypeError(TypeError):
class TqdmKeyError(KeyError):
class TqdmDeprecationWarning(Exception):
ASCII_FMT = " 123456789#"
UTF_FMT = u" " + u''.join(map(_unich, range(0x258F, 0x2587, -1)))
class tqdm(object):
    @staticmethod
    def format_sizeof(num, suffix=''):
    @staticmethod
    def format_interval(t):
    @staticmethod
    def status_printer(file):
    @staticmethod
    def format_meter(n, total, elapsed, ncols=None, prefix='',
                     ascii=False, unit='it', unit_scale=False, rate=None,
                     bar_format=None):
    def __new__(cls, *args, **kwargs):
    @classmethod
    def _get_free_pos(cls, instance=None):
    @classmethod
    def _decr_instances(cls, instance):
    @classmethod
    def write(cls, s, file=sys.stdout, end="\n"):
    @classmethod
    def pandas(tclass, *targs, **tkwargs):
    def __init__(self, iterable=None, desc=None, total=None, leave=True,
                 file=sys.stderr, ncols=None, mininterval=0.1,
                 maxinterval=10.0, miniters=None, ascii=None, disable=False,
                 unit='it', unit_scale=False, dynamic_ncols=False,
                 smoothing=0.3, bar_format=None, initial=0, position=None,
                 gui=False, **kwargs):
        """
        Parameters
        ----------
        iterable  : iterable, optional
            Iterable to decorate with a progressbar.
            Leave blank to manually manage the updates.
        desc  : str, optional
            Prefix for the progressbar.
        total  : int, optional
            The number of expected iterations. If unspecified,
            len(iterable) is used if possible. As a last resort, only basic
            progress statistics are displayed (no ETA, no progressbar).
            If `gui` is True and this parameter needs subsequent updating,
            specify an initial arbitrary large positive integer,
            e.g. int(9e9).
        leave  : bool, optional
            If [default: True], keeps all traces of the progressbar
            upon termination of iteration.
        file  : `io.TextIOWrapper` or `io.StringIO`, optional
            Specifies where to output the progress messages
            [default: sys.stderr]. Uses `file.write(str)` and `file.flush()`
            methods.
        ncols  : int, optional
            The width of the entire output message. If specified,
            dynamically resizes the progressbar to stay within this bound.
            If unspecified, attempts to use environment width. The
            fallback is a meter width of 10 and no limit for the counter and
            statistics. If 0, will not print any meter (only stats).
        mininterval  : float, optional
            Minimum progress update interval, in seconds [default: 0.1].
        maxinterval  : float, optional
            Maximum progress update interval, in seconds [default: 10.0].
        miniters  : int, optional
            Minimum progress update interval, in iterations.
            If specified, will set `mininterval` to 0.
        ascii  : bool or str, optional
            If unspecified or False, use unicode (smooth blocks) to fill
            the meter.
            The fallback is to use ASCII characters " 123456789#".
        disable  : bool, optional
            Whether to disable the entire progressbar wrapper
            [default: False].
        unit  : str, optional
            String that will be used to define the unit of each iteration
            [default: it].
        unit_scale  : bool, optional
            If set, the number of iterations will be reduced/scaled
            automatically and a metric prefix following the
            International System of Units standard will be added
            (kilo, mega, etc.) [default: False].
        dynamic_ncols  : bool, optional
            If set, constantly alters `ncols` to the environment (allowing
            for window resizes) [default: False].
        smoothing  : float, optional
            Exponential moving average smoothing factor for speed estimates
            (ignored in GUI mode). Ranges from 0 (average speed) to 1
            (current/instantaneous speed) [default: 0.3].
        bar_format  : str, optional
            Specify a custom bar string formatting. May impact performance.
            If unspecified, will use '{l_bar}{bar}{r_bar}', where l_bar is
            '{desc}{percentage:3.0f}%|' and r_bar is
            '| {n_fmt}/{total_fmt} [{elapsed_str}<{remaining_str},
{rate_fmt}]'
            Possible vars: bar, n, n_fmt, total, total_fmt, percentage,
            rate, rate_fmt, elapsed, remaining, l_bar, r_bar, desc.
        initial  : int, optional
            The initial counter value. Useful when restarting a progress
            bar [default: 0].
        position  : int, optional
            Specify the line offset to print this bar (starting from 0)
            Automatic if unspecified.
            Useful to manage multiple bars at once (eg, from threads).
        gui  : bool, optional
            WARNING: internal parameter - do not use.
            Use tqdm_gui(...) instead. If set, will attempt to use
            matplotlib animations for a graphical output [default: False].

        Returns
        -------
        out  : decorated iterator.
        """
        if disable:
            self.iterable = iterable
            self.disable = disable
            self.pos = self._get_free_pos(self)
            self._instances.remove(self)
            return

        if kwargs:
            self.disable = True
            self.pos = self._get_free_pos(self)
            self._instances.remove(self)
            raise (TqdmDeprecationWarning("""\
`nested` is deprecated and automated. Use position instead for manual control.
""", fp_write=getattr(file, 'write', sys.stderr.write))
                if "nested" in kwargs else
                TqdmKeyError("Unknown argument(s): " + str(kwargs)))

        # Preprocess the arguments
        if total is None and iterable is not None:
            try:
                total = len(iterable)
            except (TypeError, AttributeError):
                total = None

        if ((ncols is None) and (file in (sys.stderr, sys.stdout))) or \
                dynamic_ncols:  # pragma: no cover
            if dynamic_ncols:
                dynamic_ncols = _environ_cols_wrapper()
                ncols = dynamic_ncols(file)
            else:
                ncols = _environ_cols_wrapper()(file)

        if miniters is None:
            miniters = 0
            dynamic_miniters = True
        else:
            dynamic_miniters = False

        if mininterval is None:
            mininterval = 0

        if maxinterval is None:
            maxinterval = 0

        if ascii is None:
            ascii = not _supports_unicode(file)

        if bar_format and not ((ascii is True) or _is_ascii(ascii)):
            # Convert bar format into unicode since terminal uses unicode
            bar_format = _unicode(bar_format)

        if smoothing is None:
            smoothing = 0

        # Store the arguments
        self.iterable = iterable
        self.desc = desc + ': ' if desc else ''
        self.total = total
        self.leave = leave
        self.fp = file
        self.ncols = ncols
        self.mininterval = mininterval
        self.maxinterval = maxinterval
        self.miniters = miniters
        self.dynamic_miniters = dynamic_miniters
        self.ascii = ascii
        self.disable = disable
        self.unit = unit
        self.unit_scale = unit_scale
        self.gui = gui
        self.dynamic_ncols = dynamic_ncols
        self.smoothing = smoothing
        self.avg_time = None
        self._time = time
        self.bar_format = bar_format

        # Init the iterations counters
        self.last_print_n = initial
        self.n = initial

        # if nested, at initial sp() call we replace '\r' by '\n' to
        # not overwrite the outer progress bar
        self.pos = self._get_free_pos(self) if position is None else position

        if not gui:
            # Initialize the screen printer
            self.sp = self.status_printer(self.fp)
            if self.pos:
                self.moveto(self.pos)
            self.sp(self.format_meter(self.n, total, 0,
                    (dynamic_ncols(file) if dynamic_ncols else ncols),
                    self.desc, ascii, unit, unit_scale, None, bar_format))
            if self.pos:
                self.moveto(-self.pos)

        # Init the time counter
        self.start_t = self.last_print_t = self._time()

    def __len__(self):
    def __enter__(self):
    def __exit__(self, *exc):
    def __del__(self):
    def __repr__(self):
    def __lt__(self, other):
    def __le__(self, other):
    def __eq__(self, other):
    def __ne__(self, other):
    def __gt__(self, other):
    def __ge__(self, other):
    def __hash__(self):
    def __iter__(self):
    def update(self, n=1):
        """
        Manually update the progress bar, useful for streams
        such as reading files.
        E.g.:
        >>> t = tqdm(total=filesize) # Initialise
        >>> for current_buffer in stream:
        ...    ...
        ...    t.update(len(current_buffer))
        >>> t.close()
        The last line is highly recommended, but possibly not necessary if
        `t.update()` will be called in such a way that `filesize` will be
        exactly reached and printed.

        Parameters
        ----------
        n  : int
            Increment to add to the internal counter of iterations
            [default: 1].
        """
        if self.disable:
            return

        if n < 0:
            raise ValueError("n ({0}) cannot be negative".format(n))
        self.n += n

        delta_it = self.n - self.last_print_n  # should be n?
        if delta_it >= self.miniters:
            # We check the counter first, to reduce the overhead of time()
            cur_t = self._time()
            delta_t = cur_t - self.last_print_t
            if delta_t >= self.mininterval:
                elapsed = cur_t - self.start_t
                # EMA (not just overall average)
                if self.smoothing and delta_t:
                    self.avg_time = delta_t / delta_it \
                        if self.avg_time is None \
                        else self.smoothing * delta_t / delta_it + \
                        (1 - self.smoothing) * self.avg_time

                if not hasattr(self, "sp"):
                    raise TqdmDeprecationWarning("""\
Please use `tqdm_gui(...)` instead of `tqdm(..., gui=True)`
""", fp_write=getattr(self.fp, 'write', sys.stderr.write))

                if self.pos:
                    self.moveto(self.pos)

                # Print bar's update
                self.sp(self.format_meter(
                    self.n, self.total, elapsed,
                    (self.dynamic_ncols(self.fp) if self.dynamic_ncols
                     else self.ncols),
                    self.desc, self.ascii, self.unit, self.unit_scale,
                    1 / self.avg_time if self.avg_time else None,
                    self.bar_format))

                if self.pos:
                    self.moveto(-self.pos)

                # If no `miniters` was specified, adjust automatically to the
                # maximum iteration rate seen so far.
                # e.g.: After running `tqdm.update(5)`, subsequent
                # calls to `tqdm.update()` will only cause an update after
                # at least 5 more iterations.
                if self.dynamic_miniters:
                    if self.maxinterval and delta_t > self.maxinterval:
                        self.miniters = self.miniters * self.maxinterval \
                            / delta_t
                    elif self.mininterval and delta_t:
                        self.miniters = self.smoothing * delta_it \
                            * self.mininterval / delta_t + \
                            (1 - self.smoothing) * self.miniters
                    else:
                        self.miniters = self.smoothing * delta_it + \
                            (1 - self.smoothing) * self.miniters

                # Store old values for next call
                self.last_print_n = self.n
                self.last_print_t = cur_t

    def close(self):
    def unpause(self):
    def set_description(self, desc=None):
    def moveto(self, n):
    def clear(self, nomove=False):
    def refresh(self):
*/
