#pragma once
#include <boost/preprocessor.hpp>


// Creates a singleton class
// Requires: my_type typedef, default constructor
#define IMPLEMENTS_SINGLETON()\
private:\
  my_type(const my_type& other);\
  my_type(my_type&& other);\
  my_type& operator=(const my_type& other);\
  my_type& operator=(my_type&& other);\
  static void kill_singleton() { boost::checked_delete(&the()); }\
public:\
  static my_type& the() {\
    static my_type* instance = nullptr;\
    if (instance == nullptr) {\
      if (false)::atexit(&kill_singleton);\
      instance = new my_type; }\
    return *instance;}\
private:


// Put variable in scope
// Usage: scope(MyVariable v) { ... }
#define scope(VARIABLE) \
for (bool scope_alive = true; scope_alive; scope_alive = false) \
for (VARIABLE; scope_alive; scope_alive = false) 


// Anonymous lambda type deduction from container name
// Usage: _A(container)& val;
#define _A(container) std::remove_reference<decltype(*std::begin(container))>::type


// Stringify
#define STRINGIFY(__STR__) #__STR__


// Requires: operator==, my_type typedef
// Implements: operator!=
#define IMPLEMENTS_NOT_EQUAL() \
  bool operator!=(const my_type& other) { return !(*this == other); }


// Requires: my_type typedef
// Usage: IMPLEMENTS_ALL((member0_)(member1_)(...))
// Implements: copy constructor, assignment operator, move constructor, move assignment, comparison operator, swap
#define COPY_CONSTRUCT_MEMBER(pR, pData, pMember) pMember(other.pMember)
#define MOVE_CONSTRUCT_MEMBER(pR, pData, pMember) ::std::swap(pMember, other.pMember);
#define MOVE_ASSIGN_MEMBER(pR, pData, pMember) ::std::swap(pMember, other.pMember);
#define IS_MEMBER_EQUAL(pR, pData, pMember) if(pMember != other.pMember) return false;

#define IMPLEMENTS_ALL(pMembers) \
public: \
  my_type(const my_type& other) : \
    BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_TRANSFORM(COPY_CONSTRUCT_MEMBER, BOOST_PP_EMPTY, pMembers)) { \
  } \
  my_type& operator=(const my_type& other) { \
    my_type copy_constructed(other); \
    *this = ::std::move(copy_constructed); \
    return *this; \
  } \
  my_type& operator=(my_type&& other) { \
    BOOST_PP_SEQ_FOR_EACH(MOVE_ASSIGN_MEMBER, BOOST_PP_EMPTY, pMembers) \
    return *this; \
  } \
  my_type(my_type&& other) { \
    BOOST_PP_SEQ_FOR_EACH(MOVE_CONSTRUCT_MEMBER, BOOST_PP_EMPTY, pMembers) \
  } \
  bool operator==(const my_type& other) const { \
    BOOST_PP_SEQ_FOR_EACH(IS_MEMBER_EQUAL, BOOST_PP_EMPTY, pMembers) \
    return true; \
  } \
  bool operator!=(const my_type& other) const { \
    return !(*this == other); \
  }


// Requires: my_type typedef
// Usage: IMPLEMENTS_MOVEABLE((m0_)(m1_))
// Implements: move semantics
#define IMPLEMENTS_MOVEABLE(pMembers) \
public: \
  my_type& operator=(my_type&& other) { \
    BOOST_PP_SEQ_FOR_EACH(MOVE_ASSIGN_MEMBER, BOOST_PP_EMPTY, pMembers) \
    return *this; \
  }\
  my_type(my_type&& other) { \
  BOOST_PP_SEQ_FOR_EACH(MOVE_CONSTRUCT_MEMBER, BOOST_PP_EMPTY, pMembers) \
  } 
  