#pragma once
#include <array>

namespace util {


template <typename T>
std::array<typename T, 1> make_array(const T& a0) {
  std::array<T, 1> ret = {a0};
  return ret;
}
template <typename T>
std::array<typename T, 2> make_array(const T& a0, const T& a1) {
  std::array<T, 2> ret = {a0, a1};
  return ret;
}
template <typename T>
std::array<typename T, 3> make_array(const T& a0, const T& a1, const T& a2) {
  std::array<T, 3> ret = {a0, a1, a2};
  return ret;
}
template <typename T>
std::array<typename T, 4> make_array(const T& a0, const T& a1, const T& a2, const T& a3) {
  std::array<T, 4> ret = {a0, a1, a2, a3};
  return ret;
}
template <typename T>
std::array<typename T, 5> make_array(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4) {
  std::array<T, 5> ret = {a0, a1, a2, a3, a4};
  return ret;
}
template <typename T>
std::array<typename T, 6> make_array(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5) {
  std::array<T, 6> ret = {a0, a1, a2, a3, a4, a5};
  return ret;
}
template <typename T>
std::array<typename T, 7> make_array(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5, const T& a6) {
  std::array<T, 7> ret = {a0, a1, a2, a3, a4, a5, a6};
  return ret;
}
template <typename T>
std::array<typename T, 8> make_array(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5, const T& a6, const T& a7) {
  std::array<T, 8> ret = {a0, a1, a2, a3, a4, a5, a6, a7};
  return ret;
}
template <typename T>
std::array<typename T, 9> make_array(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5, const T& a6, const T& a7, const T& a8) {
  std::array<T, 9> ret = {a0, a1, a2, a3, a4, a5, a6, a7, a8};
  return ret;
}
template <typename T>
std::array<typename T, 10> make_array(const T& a0, const T& a1, const T& a2, const T& a3, const T& a4, const T& a5, const T& a6, const T& a7, const T& a8, const T& a9) {
  std::array<T, 10> ret = {a0, a1, a2, a3, a4, a5, a6, a7, a8, a9};
  return ret;
}


} // namespace util