#pragma once

#include "util_src/util.h"
#include <algorithm>
#include <type_traits>

#define PIPE_OPERATOR operator|


#define PIPE_FUNCTION(PIPE_NAME, REGULAR_FUNCTION) \
namespace pipe { \
static struct PIPE_NAME##_tag{} PIPE_NAME; \
template <typename ArgumentType> \
ArgumentType \
PIPE_OPERATOR(const ArgumentType& arg0, const PIPE_NAME##_tag&) { \
  return REGULAR_FUNCTION(arg0); \
} \
} 



// Math
PIPE_FUNCTION(sin, ::sin);
PIPE_FUNCTION(cos, ::cos);
PIPE_FUNCTION(tan, ::tan);
PIPE_FUNCTION(abs, ::abs);
PIPE_FUNCTION(log, ::log);
PIPE_FUNCTION(ceil, ::ceil);
PIPE_FUNCTION(floor, ::floor);
PIPE_FUNCTION(sqrt, ::sqrt);


// STL
namespace pipe {


// move
static struct MoveTag {} move;
template <typename T>
T 
PIPE_OPERATOR(T& val, const MoveTag&) {
  return ::std::move(val);
}


// next
static struct NextTag{} next;
template <typename ArgumentType>
ArgumentType
PIPE_OPERATOR(const ArgumentType& arg0, const NextTag&) { 
  return std::next(arg0);
}


// prev
static struct PrevTag{} prev;
template <typename ArgumentType>
ArgumentType
PIPE_OPERATOR(const ArgumentType& arg0, const PrevTag&) { 
  return std::prev(arg0);
}


// get
template <size_t Index>
struct get {};
template <typename TupleType, size_t TupleIndex> 
typename std::tuple_element<TupleIndex, TupleType>::type
PIPE_OPERATOR(const TupleType& value, const get<TupleIndex>&) {
  return std::get<TupleIndex>(value);
}


// any_of_equal
template <typename T>
struct AnyOfEqualTag {
  AnyOfEqualTag(const T& val) : value(val) {}
  const T& value;
};
template <typename T>
AnyOfEqualTag<T>
any_of_equal(const T& val) {
  return AnyOfEqualTag<T>(val);
}
template <typename C, typename T>
bool 
PIPE_OPERATOR(const C& container, const AnyOfEqualTag<T>& tag) {
  return std::any_of(std::begin(container), end, [&](const T& val) { return val == tag.value; });
}


// all_of_equal
template <typename T>
struct AllOfEqualTag {
  AllOfEqualTag(const T& val) : value(val) {}
  const T& value;
};
template <typename T>
AllOfEqualTag<T>
any_of_equal(const T& val) {
  return AllOfEqualTag<T>(val);
}
template <typename C, typename T>
bool PIPE_OPERATOR(const C& container, const AllOfEqualTag<T>& tag) {
  return std::all_of(std::begin(container), end, [&](const T& val) { return val == tag.value; });
}


// min_element
static struct MinElementTag {} min_element;
template <typename C, typename T>
typename C::iterator
PIPE_OPERATOR(const C& container, const MinElementTag&) {
  return std::min_element(std::begin(container), std::end(container));
}


// max_element
static struct MaxElementTag {} max_element;
template <typename C, typename T>
typename C::iterator
PIPE_OPERATOR(const C& container, const MaxElementTag&) {
  return std::max_element(std::begin(container), std::end(container));
}


// minmax_element
static struct MinMaxElementTag {} minmax_element;
template <typename C, typename T>
typename std::pair<typename C::iterator, typename C::iterator>
PIPE_OPERATOR(const C& container, const MinMaxElementTag&) {
  return std::minmax_element(std::begin(container), std::end(container));
}


// fill
template <typename T>
struct FillTag {
  FillTag(const T& val) : value(val) {}
  const T& value;
};
template <typename T>
FillTag<T>
fill(const T& val) {
  return FillTag<T>(val);
}
template <typename C, typename T>
void
PIPE_OPERATOR(const C& container, const FillTag<T>& filler) {
  std::fill(std::begin(container), std::end(container), filler.value);
}


// find
template <typename T>
struct FindTag {
  FindTag(const T& val) : value(val) {}
  const T& value;
};
template <typename T>
FindTag<T>
find(const T& val) {
  return FindTag<T>(val);
}
template <typename ContainerType, typename ValueType>
typename ContainerType::const_iterator
PIPE_OPERATOR(const ContainerType& container, const FindTag<ValueType>& valueHolder) {
  return std::find(std::begin(container), std::end(container), valueHolder.value);
}
template <typename ContainerType, typename ValueType>
typename ContainerType::iterator
PIPE_OPERATOR(ContainerType& container, const FindTag<ValueType>& valueHolder) {
  return std::find(std::begin(container), std::end(container), valueHolder.value);
}


// accumulate
template <typename T>
struct AccumulateTag {
  AccumulateTag(const T& val) : value(val) {}
  const T& value;
};
template <typename T>
AccumulateTag<T>
accumulate(const T& val) {
  return AccumulateTag<T>(val);
}
template <typename ContainerType, typename ValueType>
ValueType
PIPE_OPERATOR(ContainerType& container, const AccumulateTag<ValueType>& valueHolder) {
  return std::accumulate(std::begin(container), std::end(container), valueHolder.value);
}


// count
template <typename T>
struct CountTag {
  CountTag(const T& val) : value(val) {}
  const T& value;
};
template <typename T>
CountTag<T>
count(const T& val) {
  return CountTag<T>(val);
}
template <typename ContainerType, typename ValueType>
ValueType
PIPE_OPERATOR(ContainerType& container, const CountTag<ValueType>& valueHolder) {
  return std::accumulate(std::begin(container), std::end(container), valueHolder.value);
}

}




// Convenience
namespace pipe {

// index_of
template <typename T>
struct IndexOfTag {
  IndexOfTag(const T& val) : value(val) {}
  const T& value;
};
template <typename T>
IndexOfTag<T>
index_of(const T& val) {
  return IndexOfTag<T>(val);
}
template <typename C, typename T>
size_t 
PIPE_OPERATOR(const C& container, const IndexOfTag<T>& tag) {
  const auto& begin = std::begin(container);
  const auto& end = std::end(container);
  const auto& pos = std::find(begin, end, tag.value);
  return pos == end ? -1 : std::distance(begin, pos);
}

// adress
static struct AdressTag{} adress;
template <typename ValueType> 
typename const ValueType*
PIPE_OPERATOR(const ValueType& value, const AdressTag&) {
  return &value;
}
template <typename ValueType> 
typename ValueType*
PIPE_OPERATOR(ValueType& value, const AdressTag&) {
  return &value;
}


// deref
static struct DerefTag{} deref;
template <typename IteratorType> 
typename IteratorType::const_reference
PIPE_OPERATOR(const IteratorType& value, const DerefTag&) {
  return (*value);
}
template <typename IteratorType> 
typename IteratorType::reference
PIPE_OPERATOR(IteratorType& value, const DerefTag&) {
  return (*value);
}


// to
template <typename T>
struct ToTag {};
template <typename T>
ToTag<T>
to() {
  return ToTag<T>();
}
template <typename InType, typename OutType> 
const OutType 
PIPE_OPERATOR(const InType& value, const ToTag<OutType>&) {
  return (const OutType)(value);
}
template <typename InType, typename OutType> 
OutType 
PIPE_OPERATOR(InType& value, const ToTag<OutType>&) {
  return (OutType)(value);
}


// as
template <typename T>
struct AsTag {};
template <typename T>
AsTag<T>
as() {
  return AsTag<T>();
}
template <typename InType, typename OutType> 
OutType PIPE_OPERATOR(const InType& value, const AsTag<OutType>&) {
  static_assert(std::is_pointer<InType>::value, "");
  static_assert(std::is_pointer<OutType>::value, "");
  return dynamic_cast<OutType>(value);
}


// reinterval
template <typename T>
struct
ReintervalTag {
  ReintervalTag(const T& in_min, const T& in_max, const T& out_min, const T& out_max) 
  : in_min_(in_min)
  , in_max_(in_max)
  , out_min_(out_min)
  , out_max_(out_max) 
  {}
  const T& in_min_;
  const T& in_max_;
  const T& out_min_;
  const T& out_max_;
};
template <typename T>
ReintervalTag<T>
reinterval(const T& in_min, const T& in_max, const T& out_min, const T& out_max) {
  return ReintervalTag<T>(in_min, in_max, out_min, out_max);
}
template <typename T> 
T PIPE_OPERATOR(const T& value, const ReintervalTag<T>& arguments) {
  return util::reinterval<T>(value, arguments.in_min_, arguments.in_max_, arguments.out_min_, arguments.out_max_);
}


// reinterval_clamped
template <typename T>
struct
ReintervalClampedTag {
  ReintervalClampedTag(const T& in_min, const T& in_max, const T& out_min, const T& out_max) 
  : in_min_(in_min)
  , in_max_(in_max)
  , out_min_(out_min)
  , out_max_(out_max) 
  {}
  const T& in_min_;
  const T& in_max_;
  const T& out_min_;
  const T& out_max_;
};
template <typename T>
ReintervalClampedTag<T>
reinterval_clamped(const T& in_min, const T& in_max, const T& out_min, const T& out_max) {
  return ReintervalClampedTag<T>(in_min, in_max, out_min, out_max);
}
template <typename T> 
T PIPE_OPERATOR(const T& value, const ReintervalClampedTag<T>& arguments) {
  return util::reinterval_clamped<T>(value, arguments.in_min_, arguments.in_max_, arguments.out_min_, arguments.out_max_);
}


// clamp
template <typename T>
struct
ClampTag {
  ClampTag(const T& out_min, const T& out_max) 
  : out_min_(out_min)
  , out_max_(out_max) 
  {}
  const T& out_min_;
  const T& out_max_;
};
template <typename T>
ClampTag<T>
clamp(const T& out_min, const T& out_max) {
  return ClampTag<T>(out_min, out_max);
}
template <typename T> 
T PIPE_OPERATOR(const T& value, const ClampTag<T>& arguments) {
  return util::clamp<T>(value, arguments.out_min_, arguments.out_max_);
}


// min_value
static struct MinValueTag {} min_value;
template <typename C, typename T>
typename C::iterator
PIPE_OPERATOR(const C& container, const MinValueTag&) {
  return *std::min_element(std::begin(container), std::end(container));
}


// max_value
static struct MaxValueTag {} max_value;
template <typename C, typename T>
typename C::iterator
PIPE_OPERATOR(const C& container, const MaxValueTag&) {
  return *std::max_element(std::begin(container), std::end(container));
}


// minmax_value
static struct MinMaxValueTag {} minmax_value;
template <typename C, typename T>
typename std::pair<typename C::reference, typename C::reference>
PIPE_OPERATOR(const C& container, const MinMaxValueTag&) {
  auto locations = std::minmax_element(std::begin(container), std::end(container));
  return std::pair<typename C::reference, typename C::reference>(*locations.first, *locations.second);
}


}

#undef PIPE_OPERATOR
