#pragma once
#include <array>

template<typename T> 
std::array<char, sizeof(T) >  to_bytes(const T& object)
{
	std::array<char, sizeof(T) > bytes;

	const char * begin = reinterpret_cast<const char *>(std::addressof(object));
	const char * end = begin + sizeof(T);
	std::copy(begin, end, std::begin(bytes));

	return bytes;
}