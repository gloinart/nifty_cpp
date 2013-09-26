//
//  Uninitialized Vector
//    Creates a vector of size N without initializing the objects, use with care.
//
//   USAGE:
//     auto my_vector = util::uninitialized_vector<double>(100000);
//
//  NOTES:
//    Does not work with custom allocators.
//
//  Contact:
//    viktor.sehr(at)gmail.com
//

#pragma once
#include <vector>

namespace util {


namespace detail {
template <typename T>
struct hollow_type_base {
  typedef T RealType;
  hollow_type_base() {}
  hollow_type_base(const hollow_type_base&) {}
  hollow_type_base(hollow_type_base&&) {}
  hollow_type_base& operator=(const hollow_type_base&) { return *this; }
  hollow_type_base& operator=(hollow_type_base&&) { return *this; }
  ~hollow_type_base() {}
private:
  uint8_t block[sizeof(RealType)];
};
template <typename RealVector, typename HollowVector>
RealVector
make_uninitialized_vector_by_types(size_t num_elements) {
  typedef typename RealVector::value_type RealType;
  typedef typename HollowVector::value_type HollowType;
  static_assert(sizeof(RealType) == sizeof(HollowType), "Error creating HollowType");
  HollowVector hollow_vector(num_elements);
  RealVector* disguised_hollow_vector = reinterpret_cast<RealVector*>(&hollow_vector);
  RealVector real_vector;
  ::std::swap(real_vector, *disguised_hollow_vector);
  return real_vector;
}
} // namespace detail

template <typename RealType>
std::vector<RealType>
uninitialized_vector(size_t num_elements) {
  typedef detail::hollow_type_base<RealType> HollowType;
  return detail::make_uninitialized_vector_by_types<std::vector<RealType>, std::vector<HollowType>>(num_elements);
}


} // namespace util
