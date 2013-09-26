#pragma once
#include <memory>
#include <vector>
#include <xutility>

namespace util {


template <typename T>
class copy_on_write {
public:
  typedef T value_type;
  copy_on_write() { ptr_ = std::make_shared<value_type>(); }
  copy_on_write(const copy_on_write& other) { ptr_ = other.ptr_; }
  copy_on_write& operator=(const copy_on_write& other) { 
    ptr_ = other.ptr_;
    return *this;
  }
  copy_on_write(value_type other) { ptr_ = std::make_shared<value_type>(std::move(other)); }
  copy_on_write& operator=(value_type other) {
    if (unique())
      *ptr_ = std::move(other)
    else
      ptr_ = std::make_shared<value_type>(std::move(other)); 
    return *this;
  }
  bool operator==(const copy_on_write& other) const { return ptr_ == other.ptr_ || read() == other.read(); }
  bool operator!=(const copy_on_write& other) const { return !(*this == other); }
  bool operator==(const value_type& other) const { return read() == other; }
  bool operator!=(const value_type& other) const { return read() != other; }

  // Non-mutating functions
  size_t use_count() const { return ptr_.use_count(); }
  bool unique() const { return ptr_.unique(); }
  const value_type& read() const { return *ptr_; }
  const value_type& operator*() const { return read(); }
  operator const value_type&() const { return read(); }
  const value_type* read_ptr() const { return ptr_.get(); }
  const value_type* get() const { return read_ptr(); }
  const value_type* operator->() const { return read_ptr(); }

  // Mutating functions
  value_type& write() { ensure_unique(); return *ptr_; }
  void ensure_unique() { 		
    if (!unique()) 
      ptr_ = std::make_shared<value_type>(*ptr_);
  }

public:
  std::shared_ptr<value_type> ptr_;
};


} // namespace util