#pragma once

#include <array>
#include <algorithm>

namespace util {


template <typename T, size_t MAX_ELEMENTS>
class fixed_vector {
public:
  typedef fixed_vector<T, MAX_ELEMENTS> my_type;
  typedef std::array<T, MAX_ELEMENTS> array_type;

  typedef typename array_type::pointer pointer;
  typedef typename array_type::const_pointer const_pointer;
  typedef typename array_type::difference_type difference_type;
  typedef typename array_type::size_type size_type;
  typedef typename array_type::reference reference;
  typedef typename array_type::const_reference const_reference;
  typedef typename array_type::value_type value_type;
  typedef typename array_type::iterator iterator;
  typedef typename array_type::const_iterator const_iterator;
  typedef typename array_type::reverse_iterator reverse_iterator;
  typedef typename array_type::const_reverse_iterator const_reverse_iterator;

  fixed_vector( size_t size = 0) : size_(size) {}
  fixed_vector( size_t size, const value_type& val) : size_(size) { std::fill(begin(), end(), val); }
  fixed_vector( const fixed_vector& other) { 
    std::copy(other.begin(), other.end(), begin());
    size_ = other.size_;
  }
  fixed_vector& operator=(const fixed_vector& other) {
    std::copy(other.begin(), other.end(), begin());
    size_ = other.size_;
    return *this;
  }
  bool empty() const { return size_ == 0; }
  size_t size() const { return size_; }

  // Modify range
  void erase(iterator it) {
    DASSERT(std::distance(begin(), it) <= size_);
    std::copy(it+1, end(), it);
    --size_;
  }
  void erase(iterator start, iterator stop) {
    DASSERT(std::distance(begin(), stop) <= size_);
    std::copy(stop, end(), start);
    size_ -= std::distance(start, stop);
  }
  void push_back(const value_type& val) {
    DASSERT(size_ <= MAX_ELEMENTS);
    data_[size_] = val;
    ++size_;
  }
  void emplace_back(value_type&& val) {
    DASSERT(size_ <= MAX_ELEMENTS);
    std::move(val, data_[size_]);
    ++size_;
  }
  void pop_back() { DASSERT(!empty()); --size_; }
  void clear() { size_ = 0; }
  
  void reserve(size_t n) { DASSERT(n <= MAX_ELEMENTS); } // Doesn't actually do anything, only here for syntax-compatiblity with std::vector
  void resize(size_t n, const value_type& val = value_type()){
    DASSERT(n <= MAX_ELEMENTS);
    const auto new_end = begin() + n;
    if ( new_end > end() )
      std::fill(end(), new_end, val);
    size_ = n;
  }

  // Access by iterator
  iterator begin() { return data_.begin(); }
  const_iterator begin() const { return data_.begin(); }
  iterator end() { return data_.begin() + size_; }
  const_iterator end() const { return data_.begin() + size_; }
  reverse_iterator rend() { return data_.rend(); }
  const_reverse_iterator rend() const { return data_.rend(); }
  reverse_iterator rbegin() { return rend() - size_; }
  const_reverse_iterator rbegin() const { return rend() - size_; }

  // Element access
  value_type& operator[](size_t idx) { DASSERT(idx < size_); return data_[idx]; }
  const value_type& operator[](size_t idx) const { DASSERT(idx < size_); return data_[idx]; }
  value_type& at(size_t idx) { DASSERT(idx < size_); return data_.at(idx); }
  const value_type& at(size_t idx) const { DASSERT(idx < size_); return data_.at(idx); }
  value_type& front() { DASSERT(!empty()); return data_.front(); }
  const value_type& front() const { DASSERT(!empty()); return data_.front(); }
  value_type& back() { DASSERT(!empty()) ;return *rbegin(); }
  const value_type& back() const { DASSERT(!empty()); return *rbegin(); }

private:
  array_type data_;
  size_t size_;
};


} // namespace util
