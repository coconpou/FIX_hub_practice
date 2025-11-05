#include "basic.h"

// ----ostream helpers ----
template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
  for (const auto &x : v) os << x << ' ';
  return os;
}
template std::ostream &operator<<(std::ostream &, const std::vector<int> &);
template std::ostream &operator<<(std::ostream &, const std::vector<std::string> &);

template <class T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &s) {
  for (const auto &x : s) os << x << ' ';
  return os;
}
template std::ostream &operator<<(std::ostream &, const std::set<int> &);
template std::ostream &operator<<(std::ostream &, const std::set<std::string> &);
