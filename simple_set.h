#pragma once

#include <vector>
#include <algorithm>
#include "util_macro.h"
#include "util.h"

namespace util {

template <typename T, typename A = std::allocator<T> >
class simple_set {
  typedef simple_set<T, A> my_type;

public:
  simple_set() : sorted_(false) { data_.reserve(64); }
  simple_set(const simple_set& other) : sorted_(other.sorted_), data_(other.data_) : {}
  simple_set& operator=(const simple_set& other) { sorted_ = other.sorted_; data_ = other.data_; return *this; }
  bool operator==(const simple_set& other) const { static_assert(false, "Cannot compare simple_sets, use std::set instead."); }
  bool operator!=(const simple_set& other) const { static_assert(false, "Cannot compare simple_sets, use std::set instead."); }
  bool operator<(const simple_set& other) const { static_assert(false, "Cannot compare simple_sets, use std::set instead."); }
  // Element access
  size_t count(const T& val) const { 
    if (sorted_) {
      const auto pos = std::lower_bound(data_.begin(), data_.end(), val);
      const auto idx = pos - data_.begin();
      if (pos == data_.end() || *pos != val || erased_[idx])
        return 0;
      return 1;
    }
    else
      return std::find(data_.begin(), data_.end(), val) != data_.end() ? 1 : 0;
  }
  bool empty() const { return data_.empty(); }
  size_t size() const { static_assert(false, "Cannot get size of simple_sets, use std::set instead."); return -1; }
  // Modify collection
  void sort() {
    std::sort(data_.begin(), data_.end());
    auto new_end = std::unique(data_.begin(), data_.end());
    data_.erase(new_end, data_.end());
    std::fill(erased_.begin(), erased_.end(), false);
    erased_.resize(data_.size(), false);
    sorted_ = true;
  }
  void clear() { data_.clear(); }
  void insert(const T& val) { 
    data_.push_back(val); 
    sorted_ = false;
  }
  void erase(const T& val) { 
    // If sorted, replace with neighbouring values, ie preserve order of elements
    if (sorted_) {
      auto bound = std::equal_range(data_.begin(), data_.end(), val);
      auto left_idx = bound.first - data_.begin();
      auto right_idx = bound.second - data_.begin();
      std::fill(erased_.begin() + left_idx, erased_.begin() + right_idx, true);
    } 
    // Unsorted
    else {
      util::erase_all_unstable(data_, val); 
    }
  }

private:
  std::vector <T, A> data_;
  std::vector <bool> erased_;
  bool sorted_;
  IMPLEMENTS_MOVEABLE( (data_) (sorted_) );
};


} // namespace util
