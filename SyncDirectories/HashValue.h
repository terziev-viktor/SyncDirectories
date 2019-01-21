#pragma once
#include <cstdint>
#include <ostream>

class HashValue
{
public:
	static const size_t VALUESIZE = 5;

	HashValue();

	HashValue(uint32_t v[5]);

	bool operator==(const HashValue & other);

	const uint32_t * GetValue() const {	return this->v; }

private:
	uint32_t v[VALUESIZE];
};

std::ostream & operator<<(std::ostream & out, const HashValue & obj);