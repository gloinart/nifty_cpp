#pragma once

namespace util {


template <typename T>
class pod_vector {
  typedef pod_vector<T> my_type;
  
public:
  typedef T value_type;
  static const size_t value_size = sizeof(value_type);
  typedef T* iterator;
  typedef const T* const_iterator;
  pod_vector(size_t sz = 0) : size_(sz), data_(nullptr) {
    reallocate_(sz);
  }
  pod_vector(const pod_vector& other) : size_(0), data_(nullptr) {
    const auto sz = other.size();
    reallocate_(sz);
    if (sz > 0)
      memcpy((void*) begin(), (void*) other.begin(), value_size*sz);
  }
  pod_vector(pod_vector&& other) : size_(0), data_(nullptr) {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
  }
  pod_vector& operator=(pod_vector&& other) {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    return *this;
  }
  ~pod_vector() { 
    auto ptr = reintrerpret_cast<uint8_t*>(data_);
    delete [] ptr;
  }
  template <typename IT>
  void assign(IT start, IT stop) {
    const auto new_size = std::distance(start, stop);
    reallocate_(new_size);
    std::copy(start, stop, begin());
  }
  T* data() { return &data_[0]; }
  const T* data() const { return &data_[0]; }
  iterator begin() { return &data_[0]; }
  const_iterator begin() const { return &data_[0]; }
  iterator end() { return &data_[size_]; }
  const_iterator end() const { return &data_[size_]; }
  size_t size() const { return size_; }
  bool empty() const { return size_ == 0; }

private:
  void reallocate_(const size_t new_size) {
    if (new_size == size_)
      return;
    if (data_ != nullptr) {
      auto ptr = reintrerpret_cast<uint8_t*>(data_);
      delete [] ptr;
    }
    if (new_size > 0) {
      auto ptr = new uint8_t[new_size*value_size];
      data_ = reintrerpret_cast<value_type*>(ptr);
    } else {
      data_ = nullptr;
    }
    size_ = new_size;
  }
  T* data_;
  size_t size_;
};


} // namespace util