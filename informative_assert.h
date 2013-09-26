//  
//  INFORMATIVE_ASSERT
//    Assertment which loggs the values used in the assertions.
//  	
//  USAGE:
//    Assertment: INFORMATIVE_ASSERT(std::vector<int>(10).size() == std::vector<int>(12).size());
//    Results in: "std::vector<int>(10).size() > std::vector<int>(12).size() | 10 | 12"
//    
//    Assertment: INFORMATIVE_ASSERT(5*5 < 2*2);
//    Results in: "5*5 < 2*2 | 25 | 4"
//
//    Assertment: INFORMATIVE_ASSERT(1*1 == 2*2 && 3*3 == 4*4);
//    Results in: "1*1 == 2*2 && 3*3 == 4*4 | 1 | 16"
//    (Only the leftmost and rightmost values will be logged)
//
//  NOTES:
//  - Any comparisson operator can be used (ie ==, !=, <, <=, >, >=)
//  - Values are written to the expression string
//  - Implemented to be as similar to BOOST_ASSERT as possible
//  - It's up to the user to provide implementations for assertion_failed(...) (as with BOOST_ASSERT)
//  - If no global ostream<< operator is implemented for the type asserted, the byte data for the class is logged.
//  - Requires boost
//  
//  KNOWN ISSUES:
//  - Assertments using shift operators on the left side will fail to correctly log the left value
//      Example: "INFORMATIVE_ASSERT(2 << 2 == 42);" will log 2 as the left value.
//  - Assertments containing && and || will only log the leftmost and rightmost value in the expression
//		  Example: "INFORMATIVE_ASSERT(1*1 == 2*2 && 3*3 == 4*4);" will log 1 and 16.
//
//  CONTACT:
//    viktor.sehr( at ) gmail.com
//  

#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <boost/type_traits/has_left_shift.hpp>

#define INFORMATIVE_ASSERT(EXPRESSION)                          \
  do { if (false == (EXPRESSION)) {                             \
    using namespace ::informative_assert::detail;               \
    value_fetcher left, right;                                  \
    bool dummy = static_cast<bool>(left << EXPRESSION >> right);\
    (void) dummy;                                               \
    ::std::ostringstream expression_osstr;                      \
    expression_osstr << #EXPRESSION << " | " << left.value_string << " | " << right.value_string; \
    ::assertion_failed(expression_osstr.str().c_str(), __FUNCTION__, __FILE__, __LINE__); \
  } } while(false)

#define INFORMATIVE_ASSERT_MSG(EXPRESSION, MESSAGE)             \
  do { if (false == (EXPRESSION)) {                             \
    std::ostringstream msg_osstr;                               \
    msg_osstr << MESSAGE;                                       \
    using namespace ::informative_assert::detail;               \
    value_fetcher left, right;                                  \
    bool dummy = static_cast<bool>(left << EXPRESSION >> right);\
    (void) dummy;                                               \
    ::std::ostringstream expression_osstr;                      \
    expression_osstr << #EXPRESSION << " | " << left.value_string << " | " << right.value_string; \
		::assertion_failed(expression_osstr.str().c_str(), msg_osstr.str().c_str(), __FUNCTION__, __FILE__, __LINE__); \
  } } while(false)

// Implementation has to be provided by the user
void assertion_failed(const char* expression, const char* function, const char* file, int line);
void assertion_failed(const char* expression, const char* msg, const char* function, const char* file, int line);

namespace informative_assert { namespace detail {

	// serialize_any is used to store hex values of types which are missing a global std::ostream& operator<<
	template <typename ValueType>
	std::string _impl_serialize_any(const ValueType& value, boost::false_type) {
		std::ostringstream stream;
		stream << "0x";
		const char* adress = (const char*) &value;
		const char* rbegin = adress + sizeof(ValueType) - 1;
		const char* rend = adress - 1;
		for (const char* ptr = rbegin; ptr != rend; --ptr)
			stream << std::hex << std::setfill('0') << std::setw(2) << (int)(*ptr);
		return stream.str();
	}
	template <typename ValueType>
	std::string _impl_serialize_any(const ValueType& value, boost::true_type) {
		std::ostringstream stream;
		stream << value;
		return stream.str();
	}
	template <typename ValueType>
	std::string serialize_any(const ValueType& value) {
		return _impl_serialize_any(value, boost::has_left_shift<std::ostream, ValueType>());
	}

	// Locally overloaded operators for grabbing of leftmost and rightmost values
	struct value_fetcher {
		std::string value_string;
	};
	template <typename ValueType> 
	const ValueType& operator<<(value_fetcher& fetcher, const ValueType& value) {
		fetcher.value_string = serialize_any(value);
		return value;
	}
	template <typename ValueType> 
	const ValueType& operator>>(const ValueType& value, value_fetcher& fetcher) {
		fetcher.value_string = serialize_any(value);
		return value;
	}

} // namespace detail
} // namespace informative_assert
