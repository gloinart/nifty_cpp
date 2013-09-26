#pragma once

// Parallel variable macros
#define pvar_init(TYPE, NAME, DEFAULT) ::std::array<TYPE, 32> _pvar__##NAME; \
  for(size_t i = 0; i < ::util::impl_parallel::num_parallel_threads_; ++i) _pvar__##NAME[i] = DEFAULT;

#define pvar(NAME) _pvar__##NAME[thread_idx]
#define pcontainer_merge(NAME) auto NAME = ::util::impl_parallel::pcontainer_merger(_pvar__##NAME);


// Details
#include <array>
#include <type_traits>
namespace util { namespace impl_parallel {


template <typename C, size_t N>
typename C
pcontainer_merger(std::array<C, N>& containers) {
  auto container = std::move(containers.front());
  for(size_t i = 1; i < num_parallel_threads_; ++i) {
    container.insert(container.end(), containers[i].begin(), containers[i].end());
  }
  return container;
}

// Merge parallel std::vector
template <typename T, size_t N>
std::vector<T>
pcontainer_merger(std::array<std::vector<T>, N>& vectors) {
  size_t sz = 0;
  for(auto it = vectors.begin(); it != vectors.end(); ++it)
    sz += it->size();
  std::vector<int> container;
  container.reserve(sz);
  for(size_t i = 0; i < num_parallel_threads_; ++i)
    container.insert(container.end(), vectors[i].begin(), vectors[i].end());
  return container;
}

// Merge parallel std::list
template <typename T, size_t N>
std::list<T>
pcontainer_merger(std::array<std::list<T>, N>& lists) {
  auto list = std::move( lists.front() ); 
  for(size_t i = 1; i < num_parallel_threads_; ++i) 
    list.splice(list.end(), lists[i]);
  return list;
}

// Merge parallel std::map
template <typename K, typename V, size_t N>
std::map<K, V>
pcontainer_merger(std::array<std::map<K, V>, N>& containers) {
  auto container = std::move(containers.front());
  for(size_t i = 1; i < num_parallel_threads_; ++i) {
    container.insert(containers[i].begin(), containers[i].end());
  }
  return container;
}

// Merge parallel std::set
template <typename T, size_t N>
std::set<T>
pcontainer_merger(std::array<std::set<T>, N>& containers) {
  auto container = std::move(containers.front());
  for(size_t i = 1; i < num_parallel_threads_; ++i) {
    container.insert(maps[i].begin(), maps[i].end());
  }
  return container;
}


} // namespace impl_parallel
} // namespace util