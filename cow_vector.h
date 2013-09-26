#pragma once
#include <memory>
#include <vector>
#include <xutility>

namespace util {


namespace _impl_cow_vector {
  template <typename vector_type>
  class mutable_access_wrapper {
  public:
    typedef typename vector_type::value_type value_type;
    typedef typename vector_type::iterator iterator;
    typedef typename vector_type::reverse_iterator reverse_iterator;
    typedef typename vector_type::pointer pointer;
    typedef typename vector_type::size_type size_type;

    mutable_access_wrapper(vector_type& vector_ref) : vector_ref_(vector_ref) {}
    iterator begin() { return vector_ref_.begin(); }
    iterator end() { return vector_ref_.end(); }
    reverse_iterator rend() { return vector_ref_.rend(); }
    reverse_iterator rbegin() { return vector_ref_.rbegin(); }
    value_type& at(size_type idx) { return vector_ref_.at(idx); }
    value_type& operator[](size_type idx) { return vector_ref_[idx]; }
    value_type& front() { return vector_ref_.front(); }
    value_type& back() { return vector_ref_.back(); }
    pointer data() { return vector_ref_.data(); }

  private:
    vector_type& vector_ref_;
  };
} // namespace _impl_cow_vector


template <typename T, typename A = std::allocator<T> >
class cow_vector {
public:
  typedef cow_vector<T, A> my_type;
  typedef std::vector<T, A> vector_type;
  typedef typename vector_type::pointer pointer;
  typedef typename vector_type::const_pointer const_pointer;
  typedef typename vector_type::difference_type difference_type;
  typedef typename vector_type::size_type size_type;
  typedef typename vector_type::reference reference;
  typedef typename vector_type::const_reference const_reference;
  typedef typename vector_type::value_type value_type;
  typedef typename vector_type::iterator iterator;
  typedef typename vector_type::const_iterator const_iterator;
  typedef typename vector_type::reverse_iterator reverse_iterator;
  typedef typename vector_type::const_reverse_iterator const_reverse_iterator;
  typedef typename _impl_cow_vector::mutable_access_wrapper<vector_type> mutable_access_wrapper_type;
  
  // Construct\Assign
  cow_vector(size_t n = 0, const T& val = T()) { ptr_ = std::make_shared<vector_type>(n, val); }
  cow_vector(const cow_vector& other) { ptr_ = other.ptr_; }
  cow_vector(cow_vector&& other) { ptr_ = std::move(other.ptr_); }
  cow_vector& operator=(const cow_vector& other) { 
    ptr_ = other.ptr_; 
    return *this; 
  }
  cow_vector& operator=(cow_vector&& other) { 
    ptr_ = std::move(other.ptr_);
    return *this; 
  }
  
  // Construct\Assign from vector_type
  cow_vector(const vector_type& other) { 
    ptr_ = std::make_shared<vector_type>( other ); 
  }
  cow_vector(vector_type&& other) { 
    ptr_ = std::make_shared<vector_type>( std::move(other) ); 
  }
  cow_vector& operator=(const vector_type& other) { 
    if (unique())
      raw_vector_() = other;
    else // Avoid unnecessary copy
      ptr_ = std::make_shared<vector_type>( other );
    return *this; 
  }  
  cow_vector& operator=(vector_type&& other) { 
    if (unique())
      raw_vector_() = std::move(other);
    else // Avoid unnecessary copy
      ptr_ = std::make_shared<vector_type>( std::move(other) );
    return *this; 
  }
  
  // Comparison
  bool operator==(const cow_vector& other) const { return ptr_ == other.ptr_ || read() == other.read(); }
  bool operator!=(const cow_vector& other) const { return !(*this == other); }
  bool operator==(const vector_type& other) const { return read() == other; }
  bool operator!=(const vector_type& other) const { return read() != other; }
  bool operator<(const cow_vector& other) const { return read() < other.read(); }

  // Copy-on-write related
  size_t use_count() const { return ptr_.use_count(); }
  bool unique() const { return ptr_.unique(); }
  mutable_access_wrapper_type write() {
    uniqued_();
    return mutable_access_wrapper_type( raw_vector_() );
  }
  const vector_type& read() const { return *ptr_; }
  
  // Non-mutating function wrappers
  size_t size() const { return read().size(); }
  bool empty() const { return read().empty(); }
  size_t capacity() const { return read().capacity(); }

  // Mutating function wrappers
  void resize(size_t n, const value_type& val = value_type()) {
    if (size() != n)
      uniqued_().resize(n, val); 
  }
  void reserve(size_t n) { 
    if (capacity() < n)
      uniqued_().reserve(n); 
  }
  void swap(cow_vector& other) { ptr_.swap(other.ptr_); }
  void clear() { 
    if (unique()) 
      raw_vector_().clear() 
    else { // Avoid unnessesary copy before clearing
      const auto n = size();
      ptr_ = std::make_shared<vector_type>();
      raw_vector_().reserve(n);
    }
  }

  // Push\Pop
  void push_back(const value_type& val) { uniqued_().push_back(val); }
  void push_back(value_type&& val) { uniqued_().push_back( std::move(val) ); }
  void emplace_back(value_type&& val) { uniqued_().emplace_back( std::move(val) ); }
  void pop_back() { 
    if(!empty()) 
      uniqued_().pop_back(); 
  }

  // Erase
  iterator erase(const_iterator pos) {
    return erase(pos, std::next(pos));
  }
  iterator erase(const_iterator start, const_iterator stop) {
    if (unique())
      return raw_vector_().erase(start, stop);
    // Store indices as the vector is relocated
    const auto start_index = std::distance(cbegin(), start); 
    const auto stop_index = std::distance(cbegin(), stop);
    this->uniqued_();
    return raw_vector_().erase( begin() + start_index, begin() + stop_index );
  }

  // Assign
  template <typename InputIterator>
  void assign(InputIterator start, InputIterator stop) {
    if (start == stop)
      return;
    this->uniqued_().assign(start, stop);
  }

  // Insert\Emplace
  iterator emplace(const_iterator position, value_type&& value) {
    if (unique()) 
      return raw_vector_().insert(position, std::move(value));
    const auto index = std::distance(cbegin(), position); 
    this->uniqued_();
    return raw_vector_().emplace(begin() + index, std::move(value));
  }
  template <typename InputIterator>
  iterator insert(const_iterator position, InputIterator start, InputIterator stop) {
    if (unique()) 
      return raw_vector_().insert(position, start, stop);
    const auto index = std::distance(cbegin(), position);
    this->uniqued_();
    return raw_vector_().insert(begin() + index, start, stop);
  }
  iterator insert(const_iterator position, const value_type& value) {
    if (unique()) 
      return raw_vector_().insert(position, value);
    const auto index = std::distance(cbegin(), position); 
    this->uniqued_();
    return raw_vector_().insert(begin() + index, value);
  }
  iterator insert(const_iterator position, value_type&& value) {
    return this->emplace(position, std::move(value));
  }

  // Non-mutating element access
  const_iterator begin() const { return read().begin(); }
  const_iterator end() const { return read().end(); }
  const_iterator cbegin() const { return read().cbegin(); }
  const_iterator cend() const { return read().cend(); }
  const_reverse_iterator rend() const { return read().rend(); }
  const_reverse_iterator rbegin() const { return read().rbegin(); }
  const_reverse_iterator crend() const { return read().crend(); }
  const_reverse_iterator crbegin() const { return read().crbegin(); }
  const value_type& operator[](size_t idx) const { return read()[idx]; }
  const value_type& at(size_t idx) const { return read().at(idx); }
  const value_type& front() const { return read().front(); }
  const value_type& back() const { return read().back(); }
  const_pointer data() const { return read().data(); }
  
private:
  vector_type& uniqued_() { 
    if (!unique())
      ptr_ = std::make_shared<vector_type>( read() );
    return raw_vector_(); 
  }
  vector_type& raw_vector_() { return *ptr_; }
  std::shared_ptr<vector_type> ptr_;
};


} // namespace util