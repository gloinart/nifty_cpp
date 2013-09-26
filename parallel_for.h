//
//  PARALLEL_FOR\PARALLEL_FOREACH
//    Easy to use macro for making for-/foreach loops run in parallel
//
//  NOTES:
//  - Requires C++11 compatible compiler
//  - The scope must be surrounded by braces and followed by a semicolon
//  - pforeach can only use auto as value type, or no value type at all
//     incorrect: pforeach(some_type& val, container) {};
//     incorrect: pforeach(val, container) {};
//     correct: pforeach(auto const& val, container) {};
//     correct: pforeach(auto val, container) {};
//     correct: pforeach(& val, container) {};
//     correct: pforeach(const& val, container) {};
//  - It is not possible to break out the loop
//  - The continue keyword is replaced by pcontinue
//  - pfor will always increment by one
//  - sfor(...) and sforeach(...) has the same syntax as pfor\pforeach but executes at the current thread
//
//  USAGE:
//    Range based loop (read only)
//    pforeach(auto const& val, container) { 
//      if (val == 42)
//        pcontinue;
//      std::cout << val << std::endl;
//    };
//
//    Range based loop (read and write)
//    pforeach(auto &val, container) { 
//      val = sin(val); 
//    };
//
//    Index based for-loop
//    auto new_container = container;
//    pfor(size_t i, 0, container.size()) { 
//      new_container[i] = sin(container[i]); 
//    };
//
//    Index based for-loop with specific range
//    pfor(int i, -1000, 1000) {
//      // do something
//    };
//
//
#pragma once


// Configuration
#ifdef PARALLEL_FOR_STD_THREAD_ENABLED
#  include <thread>
#endif

#ifdef PARALLEL_FOR_BOOST_THREAD_ENABLED
#  include <boost/thread.hpp>
#endif

#ifdef PARALLEL_FOR_MICROSOFT_PPL_THREAD_ENABLED
#  include <ppl.h>
#endif

#ifndef PARALLEL_FOR_ABSOLUTE_MAX_THREADS // Regardless of the number of hardware threads, no more than this amount of threads will be used
#  define PARALLEL_FOR_ABSOLUTE_MAX_THREADS 16
#endif


// Loop macros
#define PARALLEL_FOREACH_STANDARD(VALUE, CONTAINER) \
  ::_impl_parallel_for::ParallelForeachHelper<decltype(CONTAINER), ::std::thread>(CONTAINER) << [&](PARALLEL_FOR_DEDUCT_VALUE_TYPE(CONTAINER) MAKE_INVISIBLE_##VALUE)

#define PARALLEL_FOREACH_BOOST(VALUE, CONTAINER) \
  ::_impl_parallel_for::ParallelForeachHelper<decltype(CONTAINER), ::boost::thread>(CONTAINER) << [&](PARALLEL_FOR_DEDUCT_VALUE_TYPE(CONTAINER) MAKE_INVISIBLE_##VALUE)

#define PARALLEL_FOREACH_MICROSOFT_PPL(VALUE, CONTAINER) \
  ::_impl_parallel_for::MicrosoftPPLParallelForeachHelper<decltype(CONTAINER)>(CONTAINER) << [&](PARALLEL_FOR_DEDUCT_VALUE_TYPE(CONTAINER) MAKE_INVISIBLE_##VALUE)

#define SINGLE_FOREACH(VALUE, CONTAINER) \
  ::_impl_parallel_for::SingleForeachHelper<decltype(CONTAINER)>(CONTAINER) << [&](PARALLEL_FOR_DEDUCT_VALUE_TYPE(CONTAINER) MAKE_INVISIBLE_##VALUE)

#define PARALLEL_FOR_STANDARD(INDEX_TYPE_AND_NAME, START_INDEX, STOP_INDEX) \
  for(INDEX_TYPE_AND_NAME##_disguised = 0, _index_type = 0; _index_type == 0; _index_type = 1) \
    ::_impl_parallel_for::ParallelForHelper<decltype(_index_type), ::std::thread>(START_INDEX, STOP_INDEX) << [&](INDEX_TYPE_AND_NAME)

#define PARALLEL_FOR_BOOST(INDEX_TYPE_AND_NAME, START_INDEX, STOP_INDEX) \
  for(INDEX_TYPE_AND_NAME##_disguised = 0, _index_type = 0; _index_type == 0; _index_type = 1) \
    ::_impl_parallel_for::ParallelForHelper<decltype(_index_type), ::boost::thread>(START_INDEX, STOP_INDEX) << [&](INDEX_TYPE_AND_NAME)

#define PARALLEL_FOR_MICROSOFT_PPL(INDEX_TYPE_AND_NAME, START_INDEX, STOP_INDEX) \
  for(INDEX_TYPE_AND_NAME##_disguised = 0, _index_type = 0; _index_type == 0; _index_type = 1) \
    ::_impl_parallel_for::MicrosoftPPLParallelForHelper<decltype(_index_type)>(START_INDEX, STOP_INDEX) << [&](INDEX_TYPE_AND_NAME)

#define SINGLE_FOR(INDEX_TYPE_AND_NAME, START_INDEX, STOP_INDEX) \
  for(INDEX_TYPE_AND_NAME##_disguised = 0, _index_type = 0; _index_type == 0; _index_type = 1) \
    ::_impl_parallel_for::SingleForHelper<decltype(_index_type)>(START_INDEX, STOP_INDEX) << [&](INDEX_TYPE_AND_NAME)

#ifndef PARALLEL_CONTINUE
#  define PARALLEL_CONTINUE return // Equivalent to continue in a regular for-loop
#endif


// Details
#include <iterator>
#include <array>
#include <type_traits>


#define PARALLEL_FOR_DEDUCT_VALUE_TYPE(CONTAINER) std::remove_reference<decltype(*std::begin(CONTAINER))>::type // Deducts value_type of a container
#define MAKE_INVISIBLE_auto // Makes the auto keyword invisible in order to use auto in foreach-loops
#define MAKE_INVISIBLE_ // Makes it possible to not specify any type in foreach-loops
#define MAKE_INVISIBLE_const const // Makes it possible to use const as left most word in foreach-loops


namespace _impl_parallel_for {


// Static parameters
template <typename ThreadType>
size_t num_utilized_threads() {
  static_assert(PARALLEL_FOR_ABSOLUTE_MAX_THREADS > 0, "PARALLEL_FOR_ABSOLUTE_MAX_THREADS must be defined be above 0.");
  static size_t utilized_threads = 0;
  if (utilized_threads == 0) {
    size_t num_hardware_threads = ThreadType::hardware_concurrency();
    utilized_threads = std::min<size_t>(num_hardware_threads, PARALLEL_FOR_ABSOLUTE_MAX_THREADS);
  }
  return utilized_threads;
}
const static size_t ThreadContainerSize = static_cast<size_t>(PARALLEL_FOR_ABSOLUTE_MAX_THREADS);


// ParallelForeach - Iterates over container in parallel using functor
template <typename ContainerType, typename FunctorType, typename ThreadType>
void 
ParallelForeach(ContainerType& container, FunctorType& functor) {
  typedef decltype(std::begin(container)) IteratorType;
  auto ProcessChunk = [&functor](IteratorType start, const IteratorType stop) -> void {
    for(; start != stop; ++start)
      functor(*start);
  };
  ::std::array<ThreadType, ThreadContainerSize> threads;
  const size_t num_threads = ::_impl_parallel_for::num_utilized_threads<ThreadType>();
  const size_t total_length = container.size();
  const size_t chunk_length = total_length / num_threads;
  IteratorType chunk_start = std::begin(container);
  for (size_t i = 0; i < num_threads; ++i) { 
    const bool is_last_chunk = i + 1 == num_threads;
    const auto chunk_stop = is_last_chunk ? std::end(container) : std::next(chunk_start, chunk_length); 
    threads[i] = ThreadType(ProcessChunk, chunk_start, chunk_stop);
    chunk_start = chunk_stop;
  }
  for(size_t i = 0; i < num_threads; ++i)
    threads[i].join();
}

// ParallelFor - Iterates from start to stop using functor in parallel
template <typename IndexType, typename FunctorType, typename ThreadType>
void 
ParallelFor(IndexType start, IndexType stop, FunctorType& functor) {
  auto ProcessChunk = [&functor](IndexType subrange_start, const IndexType subrange_stop) -> void {
    for(; subrange_start != subrange_stop; ++subrange_start)
      functor(subrange_start);
  };
  ::std::array<ThreadType, ThreadContainerSize> threads;
  const IndexType num_threads = static_cast<IndexType>(::_impl_parallel_for::num_utilized_threads<ThreadType>()); // Avoid unsigned\signed warning
  const IndexType total_length = stop - start;
  const IndexType chunk_length = total_length/num_threads;
  for(IndexType i = 0; i < num_threads; ++i) {
    const bool is_last_chunk = i + 1 == num_threads;
    const IndexType chunk_start = start + (i * chunk_length);
    const IndexType chunk_stop = is_last_chunk ? stop : chunk_start + chunk_length;
    threads[i] = ThreadType(ProcessChunk, chunk_start, chunk_stop);
  }
  for(size_t i = 0, i_end = (size_t) num_threads; i < i_end; ++i)
    threads[i].join();
}


// ParallelForHelper
template <typename IndexType, typename ThreadType>
struct ParallelForHelper {
  ParallelForHelper(IndexType start, IndexType stop) : start_(start), stop_(stop) {}
  template <typename FunctorType> 
  void operator<<(FunctorType& functor) {
    ParallelFor<IndexType, FunctorType, ThreadType>(start_, stop_, functor);
  }
  const IndexType start_;
  const IndexType stop_;
};
// ParallelForeachHelper
template <typename ContainerType, typename ThreadType>
struct ParallelForeachHelper {
  ParallelForeachHelper(ContainerType& container) : container_(container){}
  template <typename FunctorType> 
  void operator<<(FunctorType& functor) {
    ParallelForeach<ContainerType, FunctorType, ThreadType>(container_, functor);
  }
  ContainerType& container_;
};


#ifdef PARALLEL_FOR_MICROSOFT_PPL_THREAD_ENABLED
// MicrosoftPPLParallelForHelper
template <typename IndexType>
struct MicrosoftPPLParallelForHelper {
  MicrosoftPPLParallelForHelper(IndexType start, IndexType stop) : start_(start), stop_(stop) {}
  template <typename FunctorType> 
  void operator<<(FunctorType& functor) {
    ::concurrency::parallel_for(start_, stop_, functor);
  }
  const IndexType start_;
  const IndexType stop_;
};
// MicrosoftPPLParallelForeachHelper
template <typename ContainerType>
struct MicrosoftPPLParallelForeachHelper {
  MicrosoftPPLParallelForeachHelper(ContainerType& container) : container_(container) {}
  template <typename FunctorType> 
  void operator<<(FunctorType& functor) {
    ::concurrency::parallel_for_each(std::begin(container_), std::end(container_), functor);
  }
  ContainerType& container_;
};
#endif

// SingleForHelper
template <typename IndexType>
struct SingleForHelper {
  SingleForHelper(const IndexType& start, const IndexType& stop) : start_(start), stop_(stop) {}
  template <typename FunctorType> 
  void operator<<(FunctorType& functor) {
    for(IndexType i = start_; i != stop_; ++i)
      functor(i);
  }
  const IndexType start_;
  const IndexType stop_;
};
// SingleForeachHelper
template <typename ContainerType>
struct SingleForeachHelper {
  SingleForeachHelper(ContainerType& container) : container_(container) {}
  template <typename FunctorType> 
  void operator<<(FunctorType& functor) {
    std::for_each(std::begin(container_), std::end(container_), functor);
  }
  ContainerType& container_;
};


} // namespace _impl_parallel_for