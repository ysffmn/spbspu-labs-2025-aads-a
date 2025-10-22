#include "KeySum.hpp"
#include <limits>
#include <stdexcept>

namespace nikonov
{
  void KeySum::operator()(std::pair< long long int, std::string > value)
  {
    bool overflow = (value.first > 0 && res > std::numeric_limits< int >::max() - value.first);
    bool underflow = (value.first < 0 && res < std::numeric_limits< int >::min() - value.first);
    if (overflow || underflow)
    {
      throw std::overflow_error("overflow");
    }

    res += value.first;
    if (!elems.empty())
    {
      elems += " ";
    }
    elems += value.second;
  }
}
