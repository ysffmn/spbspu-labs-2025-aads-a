#ifndef KEYSUM_HPP
#define KEYSUM_HPP
#include <string>
#include <utility>
namespace nikonov
{
  struct KeySum
  {
    long long int res = 0;
    std::string elems;

    void operator()(std::pair< long long int, std::string > p);
  };
}
#endif
