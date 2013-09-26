#pragma once


template <typename T0, typename T1>
struct ref_pair {
public:
  typedef T0 first_value_type;
  typedef T1 second_value_type;
  ref_pair(T0* thefirst, T1* thesecond) : first_(thefirst), second_(thesecond), owner_(false) {
  }
  ref_pair(const ref_pair& other) : owner_(true) {
    first_ = new (&first_data_[0]) first_value_type(*other.first_);
    second_ = new (&second_data_[0]) second_value_type(*other.second_);
  }
  ref_pair(ref_pair&& other) : owner_(true) {
    first_ = new (&first_data_[0]) first_value_type(std::move(*other.first_));
    second_ = new (&second_data_[0]) second_value_type(std::move(*other.second_));
  }
  ref_pair& operator=(const ref_pair& other) { 
    // Preserve state of owner_;
    *first_ = *other.first_;
    *second_ = *other.second_;
    return *this;
  }
  ref_pair& operator=(ref_pair&& other) { 
    // Preserve state of owner_;
    *first_ = std::move(*other.first_);
    *second_ = std::move(*other.second_);
    return *this;
  }
  ~ref_pair() {
    if (false == owner_) 
      return;
    first_->~first_value_type();
    second_->~second_value_type();
  }
  void swap(ref_pair& other) {
    // Preserve state of owner_;
    std::swap(*first_, *other.first_);
    std::swap(*second_, *other.second_);
  }

  // Element Access
  first_value_type& first() { return *first_; }
  second_value_type& second() { return *second_; }
  const first_value_type& first() const { return *first_; }
  const second_value_type& second() const { return *second_; }

  // Comparisson
  bool operator<(const ref_pair& other) const { return *first_ < *other.first_; }
  bool operator==(const ref_pair& other) const { return *first_ == *other.first_; }
  bool operator!=(const ref_pair& other) const { return *first_ != *other.first_; }

private:
  bool owner_;
  first_value_type* first_;
  second_value_type* second_;
  uint8_t first_data_[sizeof(first_value_type)];
  uint8_t second_data_[sizeof(second_value_type)];
};
namespace std {
  template <typename T0, typename T1>
  void
  swap(ref_pair<T0, T1>& left, ref_pair<T0, T1>& right) {
    left.swap(right);
  }
};



template <typename I0, typename I1, typename T0, typename T1>
class paired_iterator {
public:
  typedef paired_iterator<I0, I1, T0, T1> my_type;
  typedef I0 first_iterator_type;
  typedef I1 second_iterator_type;
  typedef T0 first_value_type;
  typedef T1 second_value_type;
  typedef ref_pair<first_value_type, second_value_type> value_type;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t difference_type;

  paired_iterator(const first_iterator_type& f, const second_iterator_type& s) : first_(f), second_(s) {}
  paired_iterator(const my_type& other) : first_(other.first_), second_(other.second_) {}
  paired_iterator& operator=(const my_type& other) {
    first_ = other.first_;
    second_ = other.second_;
    return *this;
  }
  void swap(my_type& other) {
    std::swap(first_, other.first_);
    std::swap(second_, other.second_);
  }
  // Compare
  bool operator<(const my_type& other) const { return first_ < other.first_; }
  bool operator==(const my_type& other) const { return first_ == other.first_; }
  bool operator!=(const my_type& other) const { return first_ != other.first_; }
  // Step
  my_type& operator++() {
    ++first_;
    ++second_;
    return *this;
  }
  my_type& operator--() {
    --first_;
    --second_;
    return *this;
  }
  my_type operator++(int) {
    my_type ans = *this;
    ++(*this);
    return ans;
  }
  my_type operator--(int) {
    my_type ans = *this;
    --(*this);
    return ans;
  }
  my_type& operator+=(size_t n) { 
    first_ += n;
    second_ += n;
    return *this;
  }
  my_type& operator-=(size_t n) { 
    first_ -= n;
    second_ -= n;
    return *this;
  }
  size_t operator-(const my_type& other) const { return this->first_ - other.first_; }
  my_type operator+(size_t n) const { return my_type(first_ + n, second_ + n); }
  my_type operator-(size_t n) const { return my_type(first_ - n, second_ - n); }
  value_type get() { 
    return value_type(&*first_, &*second_);
  }
  const value_type get() const { 
    return value_type(&*first_, &*second_);
  }
  value_type operator*() { return get(); }
  value_type operator->() { return get(); }

private:
  first_iterator_type first_;
  second_iterator_type second_;
};





template <typename R0, typename R1>
class paired_range {
public:
  typedef R0 first_range_type;
  typedef R1 second_range_type;
  typedef typename first_range_type::value_type first_value_type;
  typedef typename second_range_type::value_type second_value_type;
  typedef typename paired_iterator<
    typename first_range_type::iterator, 
    typename second_range_type::iterator, 
    first_value_type, 
    second_value_type> 
      iterator;
  typedef typename paired_iterator<
    typename first_range_type::const_iterator, 
    typename second_range_type::const_iterator, 
    const first_value_type, 
    const second_value_type> 
      const_iterator;
  typedef ref_pair<first_value_type, second_value_type> value_type;
  typedef value_type& reference;

  paired_range(first_range_type& first, second_range_type& second)
  : first_(first)
  , second_(second) 
  {
    DASSERT(first_.size() == second_.size());
  }
  paired_range(const paired_range& other)
  : first_(other.first_)
  , second_(other.second_)
  {
    DASSERT(first_.size() == second_.size());
  }

  value_type front() { return *begin(); }
  const value_type front() const { return *cbegin(); }
  value_type back() { return *std::prev(end()); }
  const value_type back() const { return *std::prev(cend()); }

  iterator begin() { return iterator(first_.begin(), second_.begin()); }
  iterator end() {  return iterator(first_.end(), second_.end()); }
  const_iterator cbegin() const { return const_iterator(first_.cbegin(), second_.cbegin()); }
  const_iterator cend() const { return const_iterator(first_.cend(), second_.cend()); }
  const_iterator begin() const { return cbegin(); }
  const_iterator end() const { return cend(); }

private:
  first_range_type& first_;
  second_range_type& second_;
};


template <typename R0, typename R1> 
typename paired_range<R0, R1>
make_paired_range(R0& first_range, R1& second_range) {
  return paired_range<R0, R1>(first_range, second_range);
}

template <typename R0, typename R1> 
typename paired_range<R0, R1>
make_const_paired_range(const R0& first_range, const R1& second_range) {
  return paired_range<const R0, const R1>(first_range, second_range);
}

