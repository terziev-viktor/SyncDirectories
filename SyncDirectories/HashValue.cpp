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

bool HashValue::operator<(const HashValue & other) const
{
	size_t i = 0;
	while (this->GetValue()[i] == other.GetValue()[i])
		++i;
	if (i == HashValue::VALUESIZE)
	{
		return false;
	}
	return this->GetValue()[i] < other.GetValue()[i];
}

bool HashValue::operator<=(const HashValue & other) const
{
	return !(*this > other);
}

bool HashValue::operator>(const HashValue & other) const
{
	return !(*this == other) && !(*this < other);
}

bool HashValue::operator>=(const HashValue & other) const
{
	return !(*this < other);
}

bool HashValue::operator==(const HashValue & other) const
{
	for (size_t i = 0; i < HashValue::VALUESIZE; ++i)
	{
		if (this->GetValue()[i] != other.GetValue()[i])
		{
			return false;
		}
	}
	return true;
}

bool HashValue::operator!=(const HashValue & other) const
{
	return !(*this == other);
}

std::ostream & operator<<(std::ostream & out, const HashValue & obj)
{
	for (size_t i = 0; i < HashValue::VALUESIZE; ++i)
	{
		out << obj.GetValue()[i];
	}
	return out;
}