#pragma once

namespace util {

template <typename T> 
incrementable {
public:
  typedef T value_type;
  typedef incrementable<value_type> my_type;
  
  incrementable(const T& val = 0) : value_(val) {}
  incrementable& operator=(const T& val) {
    RASSERT(value >= value);
    value_ = value;
    return *this;
  }
private:
  value_type value_;
};


}
