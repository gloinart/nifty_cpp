//
//  LOOP
//    Iterates over single and multiple indices as easy as possible
//      Replaces loops like:
//        for(size_t i = 0; i < i_end; ++i)
//          for(size_t j = 0; j < j_end; ++j)
//            something(i, j);
//      with:
//        loopm((auto i = i_end)(auto j = j_end))
//          something(i, j);
//
//
//  NOTES:
//  - Expressions like "loop(unsigned int k = 3){...}", where the type contains a space,
//    will not compile. Instead, use uint8_t, uint16_t etc for these kind of types.
//  - Performance ought to be equal to a regular loop using for(;;)
//  - Reversed loops will start at one below the value, ie rloop(auto k = 10) 
//    will start with k = 9 at its first iteration. 
//    (In order to loop over vectors in reverse as easy as possible)
//  - Only built in types and auto are alloved
//  - Requires boost
//
//
//   USAGE:
//     Single forward loop source code:
//       std::vector<int> vec(2);
//       loop(auto i = vec.size())
//         std::cout << " i=" << i;
//     Output:
//      i=0 i=1
//
//     Single reversed loop source code:
//       std::vector<int> vec(3);
//       rloop(auto i = 3)
//         std::cout << " i=" << i;
//     Output:
//       i=2 i=1 i=0
//
//     Multiple forward loops source code:
//       loopm( (int x=2) (float y=3) )
//         std::cout << "x=" << x << " y=" << y << std::endl;
//     Output:
//       x=0 y=0
//       x=0 y=1
//       x=0 y=2
//       x=1 y=0
//       x=1 y=1
//       x=1 y=2
//
//     Multiple reversed forward loops source code:
//       rloopm( (int a=2) (float b=3) )
//         std::cout << "a=" << a << " b=" << b << std::endl;
//     Output:
//       a=1 b=2
//       a=1 b=1
//       a=1 b=0
//       a=0 b=2
//       a=0 b=1
//       a=0 b=0
//
//
// CONTACT:
//     viktor.sehr( at ) gmail.com
//
#pragma once
#include <boost/preprocessor.hpp>

#define loop FORWARD_LOOP
#define rloop REVERSED_LOOP
#define loopm MULTIPLE_FORWARD_LOOPS
#define rloopm MULTIPLE_REVERSED_LOOPS

// Single forward loop macro
#define FORWARD_LOOP(EXPRESSION)                                                               \
  for(EXPRESSION,                                                                              \
      &_loop_index_ref = _LOOP_HIDE_##EXPRESSION,                                              \
			_loop_end = ::_loop_impl::return_original_value_and_set_zero(_loop_index_ref);           \
			_loop_index_ref < _loop_end;                                                             \
      ++_loop_index_ref)

// Single reversed loop macro
#define REVERSED_LOOP(EXPRESSION)                                                              \
  for(EXPRESSION,                                                                              \
			&_rloop_index_ref = _LOOP_HIDE_##EXPRESSION,                                             \
      _rloop_end = ::_loop_impl::subtract_one_and_return_end_of_type(_rloop_index_ref);        \
      _rloop_index_ref != _rloop_end;                                                          \
      --_rloop_index_ref) 

// Multiple forward loops macro
#define _IMPL_LOOPM(UNUSED_ARG_R, UNUSED_ARG_DATA, EXPRESSION) FORWARD_LOOP(EXPRESSION)
#define MULTIPLE_FORWARD_LOOPS(LOOP_EXPRESSIONS) \
  BOOST_PP_SEQ_FOR_EACH(_IMPL_LOOPM, BOOST_PP_EMPTY, LOOP_EXPRESSIONS)

// Multiple reversed loops macro
#define _IMPL_RLOOPM(UNUSED_ARG_R, UNUSED_ARG_DATA, EXPRESSION) REVERSED_LOOP(EXPRESSION)
#define MULTIPLE_REVERSED_LOOPS(RLOOP_EXPRESSIONS) \
  BOOST_PP_SEQ_FOR_EACH(_IMPL_RLOOPM, BOOST_PP_EMPTY, RLOOP_EXPRESSIONS)

// Details
namespace _loop_impl {
template <typename T>
T return_original_value_and_set_zero(T& grab_my_value_then_zero_me) {
  const T val = grab_my_value_then_zero_me;
  grab_my_value_then_zero_me = static_cast<T>(0);
  return val;
}
template <typename T>
T subtract_one_and_return_end_of_type(T& subtract_one_on_me) {
  --subtract_one_on_me;
  return static_cast<T>(-1);
}
} // namespace _loop_impl


// Used for hiding keywords
#define _LOOP_HIDE___int8
#define _LOOP_HIDE___int16
#define _LOOP_HIDE___int32
#define _LOOP_HIDE___int64
#define _LOOP_HIDE___wchar_t
#define _LOOP_HIDE_intmax_t
#define _LOOP_HIDE_uintmax_t
#define _LOOP_HIDE_intptr_t
#define _LOOP_HIDE_uintptr_t
#define _LOOP_HIDE_int_fast8_t
#define _LOOP_HIDE_int_fast16_t
#define _LOOP_HIDE_int_fast32_t
#define _LOOP_HIDE_int_fast64_t
#define _LOOP_HIDE_uint_fast8_t
#define _LOOP_HIDE_uint_fast16_t
#define _LOOP_HIDE_uint_fast32_t
#define _LOOP_HIDE_uint_fast64_t
#define _LOOP_HIDE_int_least8_t
#define _LOOP_HIDE_int_least16_t
#define _LOOP_HIDE_int_least32_t
#define _LOOP_HIDE_int_least64_t
#define _LOOP_HIDE_uint_least8_t
#define _LOOP_HIDE_uint_least16_t
#define _LOOP_HIDE_uint_least32_t
#define _LOOP_HIDE_uint_least64_t
#define _LOOP_HIDE_auto
#define _LOOP_HIDE_ptrdiff_t
#define _LOOP_HIDE_size_t
#define _LOOP_HIDE_wchar_t
#define _LOOP_HIDE_signed
#define _LOOP_HIDE_unsigned
#define _LOOP_HIDE_char16_t
#define _LOOP_HIDE_char32_t
#define _LOOP_HIDE_char
#define _LOOP_HIDE_short
#define _LOOP_HIDE_long
#define _LOOP_HIDE_int
#define _LOOP_HIDE_int8_t
#define _LOOP_HIDE_int16_t
#define _LOOP_HIDE_int32_t
#define _LOOP_HIDE_int64_t
#define _LOOP_HIDE_uint8_t
#define _LOOP_HIDE_uint16_t
#define _LOOP_HIDE_uint32_t
#define _LOOP_HIDE_uint64_t
#define _LOOP_HIDE_float
#define _LOOP_HIDE_double