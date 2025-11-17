#ifndef BASIC_H
#define BASIC_H

#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// ----I/O ----
#define FAST_IO                     \
  std::ios::sync_with_stdio(false); \
  std::cin.tie(nullptr)
#define readInt(n) (std::cin >> (n))
#define readLong(n) (std::cin >> (n))
#define readDouble(n) (std::cin >> (n))
#define readString(s) (std::cin >> (s))

// ----Constants / Types ----
constexpr int SIZE = 100000 + 5;
constexpr int MAXN = 0xFFFFFF;
using ll = long long;
using ull = unsigned long long;
using vi = std::vector<int>;
using pii = std::pair<int, int>;

#define ar(n) std::array<int, (n)>
#define pb push_back
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ADD(a, b) ((a) + (b))
#define CLR(x) std::memset((x), 0, sizeof((x)))

// ----Loops ----
#define FOR(i, n) for (int i = 0; i < (n); ++i)
#define REP(i, a, b) for (int i = (a); i <= (b); ++i)
#define FORD(i, n) for (int i = (n) - 1; i >= 0; --i)

// ----ostream helpers ----
template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v);
template <class T>
std::ostream &operator<<(std::ostream &os, const std::set<T> &s);

#endif   // BASIC_H