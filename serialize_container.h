#pragma once
#include <sstream>
#include <vector>
#include <deque>
#include <map>
#include <set>
#include <hash_map>
#include <hash_set>
#include <array>


template <typename T, typename Y>
std::ostream& operator <<(std::ostream& os, const std::pair<T, Y>& container) {
  os << "[" << container.first << ", " << container.second << "]";
  return os;
}

template <typename T>
std::ostream& operator <<(std::ostream& os, const std::vector<T>& container) {
  os << "[";
  if (!container.empty()) {
    for (auto it = container.begin(); std::next(it) != container.end(); ++it)
      os << *it << ", ";
    os << container.back();
  }
  os << "]";
  return os;
}

template <typename T>
std::ostream& operator <<(std::ostream& os, const std::deque<T>& container) {
  os << "[";
  if (!container.empty()) {
    for (auto it = container.begin(); std::next(it) != container.end(); ++it)
      os << *it << ", ";
    os << container.back();
  }
  os << "]";
  return os;
}

template <typename T, size_t N>
std::ostream& operator <<(std::ostream& os, const std::array<T, N>& container) {
  os << "[";
  if (!container.empty()) {
    for (auto it = container.begin(); std::next(it) != container.end(); ++it)
      os << *it << ", ";
    os << container.back();
  }
  os << "]";
  return os;
}

template <typename K, typename V>
std::ostream& operator <<(std::ostream& os, const std::map<K, V>& container) {
  os << "[";
  if (!container.empty()) {
    for (auto it = container.begin(); std::next(it) != container.end(); ++it)
      os << "{" << it->first << ", " << it->second << "}, ";
    auto last = std::prev(container.end());
    os << "{" << last->first << ", " << last->second << "}, ";
  }
  os << "]";
  return os;
}

template <typename T>
std::ostream& operator <<(std::ostream& os, const std::set<T>& container) {
  os << "[";
  if (!container.empty()) {
    for (auto it = container.begin(); std::next(it) != container.end(); ++it)
      os << *it << ", ";
    os << container.back();
  }
  os << "]";
  return os;
}

template <typename K, typename V>
std::ostream& operator <<(std::ostream& os, const std::hash_map<K, V>& container) {
  os << "[";
  if (!container.empty()) {
    for (auto it = container.begin(); std::next(it) != container.end(); ++it)
      os << "{" << it->first << ", " << it->second << "}, ";
    os << container.back();
  }
  os << "]";
  return os;
}

template <typename T>
std::ostream& operator <<(std::ostream& os, const std::hash_set<T>& container) {
  os << "[";
  if (!container.empty()) {
    for (auto it = container.begin(); std::next(it) != container.end(); ++it)
      os << *it << ", ";
    os << container.back();
  }
  os << "]";
  return os;
}