#pragma once

#include <bitset>
#include <sstream>
#include <string>
#include <limits>
#include <algorithm>
#include <map>
#include <type_traits>

#define UTIL_ASSERT DASSERT
#define UTIL_DEDUCT_VALUE_TYPE(container) std::remove_reference<decltype(*std::begin(container))>::type
#define UTIL_STATIC_ASSERT(expression) static_assert(expression, #expression)

namespace util {

template <typename T>
class initialized {
public:
  typedef T value_type;
  typedef initialized<value_type> my_type;
  initialized() : value_(0) {}
  initialized(value_type value) : value_(value) {}
  operator value_type&() { return value_; }
  operator const value_type&() const { return value_; }
private:
  value_type value_;
};

static uint64_t next_uid() {
  static uint64_t current = 1000;
  ++current;
  return current;
}

struct uid {
  typedef uint64_t value_type;
  uid() { this->next(); }
  uid(const uid& other) : value_(other.value_) {}
  uid& operator=(const uid& other) { value_ = other.value_; return *this; }
  bool operator==(const value_type& rvalue) const { return rvalue == value_; }
  bool operator!=(const value_type& rvalue) const { return rvalue != value_; }
  const value_type& value() const { return value_; }
  void next() { value_ = next_uid(); }
  operator const value_type&() const { return value_; }
private:
  value_type value_;
};

// scope_exit
struct scope_exit {
  scope_exit(std::function<void (void)> f) : f_(f) {}
  ~scope_exit(void) { f_(); }
private:
  scope_exit(const scope_exit& other);
  scope_exit& operator=(const scope_exit& other);
  std::function<void (void)> f_;
};

// value_or_default
template <typename MAP_TYPE, typename K, typename V>
const V& value_or_default(const MAP_TYPE& dictionary, const K& key, const V& default_value) {
  auto it = dictionary.find(key);
  return it == dictionary.end() ? default_value : it->second;
}

// reinterval
template <typename T>
T reinterval(const T& inVal, const T& oldMin, const T& oldMax, const T& newMin, const T& newMax) {
  RASSERT_MSG(oldMax != oldMin, oldMin << ":" << oldMax);
  T val = inVal;
  val -= oldMin;
  val /= (oldMax - oldMin);
  val *= (newMax - newMin);
  val += newMin;
  return val;
}

// reinterval_clamped
template <typename T>
T reinterval_clamped(const T& inVal, const T& oldMin, const T& oldMax, const T& newMin, const T& newMax) {
  if (oldMax == oldMin) {
    static const T TWO = T(2);
    if (inVal == oldMax)
      return (newMin + newMax) / TWO;
    if (inVal < oldMax)
      return newMin;
    return newMax;
  }
  RASSERT_MSG(oldMax != oldMin, oldMin << ":" << oldMax);
  T val = inVal;
  val -= oldMin;
  val /= (oldMax - oldMin);
  val *= (newMax - newMin);
  val += newMin;
  val = std::max<T>(val, newMin);
  val = std::min<T>(val, newMax);
  return val;
}

// flt_eq
template<typename T>
bool flt_eq(T left, T right) {
  UTIL_STATIC_ASSERT(std::is_floating_point<T>::value);
  return std::abs(left - right) <= std::numeric_limits<T>::epsilon();
}

// keys
template <typename C>
std::vector<typename C::key_type> keys(const C& assoc_container) {
  std::vector <typename C::key_type> key_vector(assoc_container.size());
  std::transform(std::begin(assoc_container), std::end(assoc_container), key_vector.begin(), [](const UTIL_DEDUCT_VALUE_TYPE(assoc_container)& kvp) { return kvp.first; });
  return key_vector;
}

// values
template <typename C>
std::vector<typename C::mapped_type> values(const C& assoc_container) {
  std::vector <typename C::mapped_type> value_vector(assoc_container.size());
  std::transform(std::begin(assoc_container), std::end(assoc_container), value_vector.begin(), [](const UTIL_DEDUCT_VALUE_TYPE(assoc_container)& kvp) { return kvp.second; });
  return value_vector;
}

// value_ptrs (all elements)
template <typename T>
std::vector<typename T::mapped_type*> value_ptrs(const T& assoc_container) {
  std::vector <typename T::mapped_type*> value_vector(assoc_container.size());
  std::transform(std::begin(assoc_container), std::end(assoc_container), value_vector.begin(), [](const UTIL_DEDUCT_VALUE_TYPE(assoc_container)& kvp) { return &kvp.second; });
  return value_vector;
}

// value_ptrs (specified key)
template <typename T>
std::vector<typename T::mapped_type*> value_ptrs(const T& assoc_container, const typename T::key_type& key) {
  const auto range = assoc_container.equal_range(key);
  std::vector <typename T::mapped_type*> value_vector(range.size(), nullptr);
  std::transform(range.first, range.second, value_vector.begin(), [](const UTIL_DEDUCT_VALUE_TYPE(assoc_container)& kvp) { return &kvp.second; });
  return value_vector;
}

// inversed_map
template <typename K, typename V>
typename std::map<V, K> inversed_map(const std::map<K, V>& in_map) {
  std::map<V, K> inversed_map;
  for(std::map<V, K>::const_iterator it = in_map.begin(), it_end = in_map.end(); it != it_end; ++it) {
    UTIL_ASSERT(inversed_map.count(it->second) == 0);
    inversed_map.insert( std::make_pair(it->second, it->first) );
  }
  return inversed_map;
}

// clear
template <typename C>
void clear(C& container) {
  C empty;
  std::swap(container, empty);
}

// minmax_value (iterator version)
template <typename IT>
std::pair<typename IT::value_type, typename IT::value_type>
minmax_value(IT first, IT last) {
  typedef IT::value_type value_type;
  UTIL_ASSERT(std::distance(first, last) > 0 && "minmax_value needs a range > 0");
  value_type min_val = *first;
  value_type max_val = min_val;
  ++first;
  for(; first != last; ++first) {
    min_val = std::min(min_val, *first);
    max_val = std::max(max_val, *first);
  }
  return std::make_pair(min_val, max_val);
}

// minmax_value (range version)
template <typename C>
std::pair<typename C::value_type, typename C::value_type>
minmax_value(const C& container) {
  return util::minmax_value(std::begin(container), std::end(container));
}

// find_first_not_of
template <typename IT>
IT find_first_not_of(IT first, IT last, const typename IT::value_type& val) {
  return std::find_if_not(first, last, [](typename IT::value_type arg0){ return arg0 == val; });
}

// squeeze
template <typename C>
void squeeze(C& container) {
  C c(container);
  std::swap(c, container);
}

// make_vector
template <typename T>
class make_vector {
  typedef make_vector<T> my_type;
public:
  my_type& operator<< (const T& val) {
    data_.push_back(val);
    return *this;
  }
  operator std::vector<T>() const {
    return data_;
  }
private:
  std::vector<T> data_;
};

// make_string
class make_string {
public:
  make_string() {}
  template <typename T>
  make_string& operator<<( const T& val ) {
    buffer_ << val;
    return *this;
  }
  operator std::string() const {
    return buffer_.str();
  }
private:
  make_string(const make_string& other) {}
  make_string(make_string&& other) {}
  make_string& operator=(const make_string& other) { return *this;}
  make_string& operator=(make_string&& other) { return *this;}
  
  std::ostringstream buffer_;
};

// make_wstring
class make_wstring {
public:
  template <typename T>
  make_wstring& operator<<( const T& val ) {
    buffer_ << val;
    return *this;
  }
  operator std::wstring() const {
    return buffer_.str();
  }
private:
  std::wostringstream buffer_;
};

// wstring_to_string
inline std::string wstring_to_string(const std::wstring& wstr) {
  std::string str(wstr.begin(), wstr.end());
  return str;
}

// string_to_wstring
inline std::wstring string_to_wstring(const std::string& str) {
  std::wstring wstr(str.begin(), str.end());
  return wstr;
}

// slide_to (moves element 'from' to 'to' while preserving the order of remaining elements)
template <typename ForwardIterator>
void slide_to(ForwardIterator from, const ForwardIterator& to) {
  if (from == to)
    return;
  for(; std::next(from) != to; ++from) 
    std::iter_swap(from, std::next(from));
}

// index_of
template <typename C, typename T>
size_t index_of(const C& container, const T& val) {
  C::const_iterator first = std::begin(container);
  C::const_iterator last = std::end(container);
  C::const_iterator pos = std::find(first, last, val);
  return pos == last ? -1 : pos - first;
}

// contains (iterator version)
template <typename T>
bool contains(const T& first, const T& last, const typename T::value_type& val) {
  return std::find(first, last, val) != last;
}

// contains (range version)
template <typename C, typename T>
bool contains(const C& container, const T& val) {
  return util::contains(std::begin(container), std::end(container), val);
}

// contains_if (iterator version)
template <typename T, typename P>
bool contains_if(const T& first, const T& last, const P& pred) {
  return std::find_if(first, last, pred) != last;
}

// contains_if (range version)
template <typename C, typename P>
bool contains_if(const C& container, const P& pred) {
  return util::contains_if(std::begin(container), std::end(container), pred);
}

// contains_any (range version)
template <typename C0, typename C1>
bool contains_any(const C0& container0, const C1& container1) {
  for (C0::const_iterator it = std::begin(container0), it_end = std::end(container0); it != it_end; ++it)
    if( util::contains(container1, *it) )
      return true;
  return false;
}

// all_unique (iterator version)
template <typename T>
bool all_unique(const T& first, const T& last) {
  if (std::distance(first, last) < 2) {
    return true;
  }
  for (T it = first, it_end = std::prev(last); it != it_end; ++it) {
    if (util::contains(std::next(it), last, *it)) {
      return false;
    }
  }
  return true;
}

// all_unique (range version)
template <typename C>
bool all_unique(const C& container) {
  return util::all_unique(std::begin(container), std::end(container));
}

// is_sorted (iterator version)
template <typename T>
bool is_sorted(const T& first, const T& last) {
  UTIL_ASSERT(std::distance(first, last) > 0);
  for(T it = first, it_end = std::prev(last); it != it_end; ++it) {
    if (*std::next(it) < *it) {
      return false;
    }
  }
  return true;
}

// is_sorted (range version)
template <typename C>
bool is_sorted(const C& container) {
  return util::is_sorted(std::begin(container), std::end(container));
}

// trimin
template <typename T>
T trimin(const T& a, const T& b, const T& c) {
  return std::min(std::min(a, b), c);
}

// trimax
template <typename T>
T trimax(const T& a, const T& b, const T& c) {
  return std::max(std::max(a, b), c);
}

// erase_duplicates_unstable
template <typename C>
void erase_duplicates_unstable(C& container) {
  std::sort(std::begin(container), std::end(container));
  C::iterator new_end = std::unique(std::begin(container), std::end(container));
  container.erase(new_end, std::end(container));
}

// erase_duplicates_stable
template <typename C>
void erase_duplicates_stable(C& container) {
  typedef typename C::value_type value_type;
  C other;
  for(C::iterator it = std::begin(container); it != std::end(container); ++it)
    if (!contains(other, *it))
      other.emplace_back(std::move(*it));
  std::swap(other, container);
}

// remove_unstable
template <typename IT, typename T>
IT remove_unstable(IT first, IT last, const T& val) {
  while(first != last) {
    if (*first == val) {
      --last;
      *first = std::move(*last); 
    } else {
      ++first;
    }
  }
  return last;
}

// remove_if_unstable
template <typename IT, typename P>
IT remove_if_unstable(IT first, IT last, P& pred) {
  while(first != last) {
    if (pred(*first)) {
      --last;
      std::swap(*first, *last);
    } else {
      ++first;
    }
  }
  return last;
}

// erase_first
template <typename C>
void erase_first(C& container, const typename C::value_type& val) {
  C::iterator it = std::find(std::begin(container), std::end(container), val);
  container.erase(it);
}

// erase_last
template <typename C>
void erase_last(C& container, const typename C::value_type& val) {
  C::iterator it = std::find(std::rbegin(container), std::rend(container), val);
  container.erase(it);
}

// erase_all
template <typename C>
void erase_all(C& container, const typename C::value_type& val) {
  C::iterator it = std::remove(std::begin(container), std::end(container), val);
  container.erase(it, std::end(container));
}

// erase_all_if
template <typename C, typename P>
void erase_all_if(C& container, P& pred) {
  C::iterator it = std::remove_if(std::begin(container), std::end(container), pred);
  container.erase(it, std::end(container));
}

// erase_all_unstable
template <typename C>
void erase_all_unstable(C& container, const typename C::value_type& val) {
  C::iterator new_end = remove_unstable(std::begin(container), std::end(container), val);
  container.erase(new_end, std::end(container));
}

// erase_all_if_unstable
template <typename C, typename P>
void erase_all_if_unstable(C& container, P& pred) {
  C::iterator it = remove_if_unstable(std::begin(container), std::end(container), pred);
  container.erase(it, std::end(container));
}

// count_bits
template <typename T>
size_t count_bits(T val) {
  UTIL_ASSERT(sizeof(val) <= 4 && "count_bits can only handle types whose size <= 4bytes");
  unsigned long dummy = 0;
  memcpy((void*)&dummy, (void*)&val, sizeof(val));
  return std::bitset<4>(dummy).count();
}

// one_bit_set
template <typename T> 
bool one_bit_set(const T& field) {
  return 0 != field && field == (field& -field);
}

// test_bit
template <typename T>
bool test_bit(const T bits, const T mask) {
  enum {DummyEnum};
  //UTIL_ASSERT(sizeof(DummyEnum) == sizeof(T) && "test_bit is only intended to be used with enums");
  //UTIL_ASSERT(count_bits(mask) == 1 && "test_bit is only intended for masks containing a single bit set");
  return (bits & mask) == mask;
}

// test_any_bit
template <typename T>
bool test_any_bit(const T bits, const T mask) {
  enum {DummyEnum};
  UTIL_ASSERT(sizeof(DummyEnum) == sizeof(T));
  return int (bits & mask) != 0;
}

// count_sort (iterator version)
template <typename IT>
void count_sort(IT first, IT last, typename IT::value_type max_value) {
  typedef typename IT::value_type value_type;
  UTIL_STATIC_ASSERT(std::is_unsigned<value_type>::value == true);
  std::vector <value_type> counter(max_value + 1, 0);
  for (IT it = first; it != last; ++it) {
    counter[*it]++;
  }
  IT left = first;
  for (IT it = counter.begin(); it != counter.end(); ++it) {
    IT right = left;
    std::advance(right, *it);
    IT::value_type val = static_cast <value_type> (it - counter.begin());
    std::fill(left, right, val);
    left = right;
  }
}

// count_sort (range version)
template <typename C, typename T>
void count_sort(C& container, T max_value) {
  util::count_sort(std::begin(container), std::end(container), max_value);
}

// count_sort_fixed (iterator version)
template <size_t MAX_VALUE, typename IT>
void count_sort_fixed(IT first, IT last) {
  typedef typename IT::value_type value_type;
  UTIL_STATIC_ASSERT(std::is_unsigned<value_type>::value == true);
  typedef std::array <value_type, MAX_VALUE+1> counter_type;
  typedef counter_type::iterator counter_iterator;
  counter_type counter;
  std::fill(std::begin(counter), std::end(counter), 0);
  for (IT it = first; it != last; ++it) {
    ++(counter[*it]);
  }
  IT left = first;
  for (counter_iterator it = counter.begin(); it != counter.end(); ++it) {
    IT right = left;
    std::advance(right, *it);
    IT::value_type val = static_cast <value_type> (it - counter.begin());
    std::fill(left, right, val);
    left = right;
  }
}

// count_sort_fixed (range version)
template <size_t MAX_VALUE, typename C>
void count_sort_fixed(C& container) {
  util::count_sort_fixed<MAX_VALUE>(std::begin(container), std::end(container));
}

// median (iterator version)
template <typename IT>
typename IT::value_type median(IT start, IT stop) {
  return _impl::median_from_vector(std::vector<C::value_type>(start, stop));
}

// median (range version)
template <typename C>
typename C::value_type median(const C& container) {
  return _impl::median_from_vector(std::vector<C::value_type>(std::begin(container), std::end(container)));
}
namespace _impl {
  template <typename T>
  typename T median_from_vector(std::vector<T>& container) {
    UTIL_ASSERT(!container.empty());
    auto middle_pos = container.size() / 2;
    auto middle_iterator = container.begin() + middle_pos;
    std::nth_element(container.begin(), middle_iterator, container.end());
    return container[middle_pos];
  }
}

// mean (iterator version)
template <typename IT>
typename IT::value_type mean(IT start, IT stop) {
  typedef IT::value_type value_type;
  size_t N = std::distance(start, stop);
  UTIL_ASSERT(N > 0);
  value_type sum = std::accumulate(std::next(start), stop, *start);
  sum /= N;
  return sum;
}

// mean (range version)
template <typename C>
typename C::value_type mean(const C& container) {
  return util::mean(std::begin(container), std::end(container));
}

// zero_vector
template <typename VectorType>
void zero_vector(VectorType& vec) {
  typedef typename VectorType::value_type ValueType;
  memset(vec.data(), 0, sizeof(ValueType) * vec.size());
}

// zero_fill
template <typename T>
void zero_fill(T& data) {
  memset(&data, 0, sizeof(T));
}

// push_front
template <typename C>
void push_front(C& container, typename C::value_type& val) {
  container.insert(container.begin(), val);
}

// pop_front
template <typename C>
void pop_front(C& container) {
  container.erase(container.begin());
}

// equal_any (use as util::equal_any(7) | 3 | 6)
namespace _impl {
  template <typename T>
  struct equal_any_impl {
    equal_any_impl(const T& ref) : ref_(ref), status_(false) {}
    equal_any_impl& operator|(const T& test) {
      if (test == ref_)
        status_ = true;
      return *this;
    }
    operator bool() const {
      return status_;
    }
  private:
    bool status_;
    const T& ref_;
  };
}
template <typename T>
_impl::equal_any_impl<T> equal_any(const T& ref) {
  return _impl::equal_any_impl<T>(ref);
}


} // namespace util

#undef UTIL_ASSERT
#undef UTIL_DEDUCT_VALUE_TYPE
#undef UTIL_STATIC_ASSERT