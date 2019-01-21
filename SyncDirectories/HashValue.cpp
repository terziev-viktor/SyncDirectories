#include "HashValue.h"

HashValue::HashValue()
{
	for (size_t i = 0; i < VALUESIZE; ++i)
	{
		v[i] = 0;
	}
}

HashValue::HashValue(uint32_t v[5])
{
	for (size_t i = 0; i < 5; ++i)
	{
		this->v[i] = v[i];
	}
}

bool HashValue::operator==(const HashValue & other)
{
	for (size_t i = 0; i < 5; ++i)
	{
		if (this->v[i] != other.v[i])
		{
			return false;
		}
	}
	return true;
}

std::ostream & operator<<(std::ostream & out, const HashValue & obj)
{
	for (size_t i = 0; i < HashValue::VALUESIZE; ++i)
	{
		out << obj.GetValue()[i];
	}
	return out;
}