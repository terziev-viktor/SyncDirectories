#pragma once
#include <vector>

using namespace std;

template<class T>
class SortedVector : private vector<T>
{
private:
	int InnerFind(int l, int r, const T & item) const
	{
		if (r >= l)
		{
			int mid = l + (r - l) / 2;

			if (this->at(mid) == item)
			{
				return mid;
			}

			if (this->at(mid) > item)
			{
				return InnerFind(l, mid - 1, item);
			}

			return InnerFind(mid + 1, r, item);
		}

		return -1;
	}

public:
	void SwapElementsAt(size_t a, size_t b)
	{
		T tmp = this->at(a);
		this->at(a) = this->at(b);
		this->at(b) = tmp;
	}

	void PushBack(const T & s)
	{
		size_t size = this->size();
		this->push_back(s);
		if (size == 0 || this->at(size - 1) <= s)
		{
			return;
		}
		else if (size == 1 && this->at(0) > s)
		{
			this->SwapElementsAt(0, 1);
		}
		else if (this->at(0) >= s)
		{
			for (size_t i = 0; i < size; ++i)
			{
				this->SwapElementsAt(this->size() - i - 1, this->size() - i - 2);
			}
		}
		else
		{
			size_t p1 = 0, p2 = size, i = 0;
			while (p2 - p1 > 1)
			{
				i = p1 + ((p2 - p1) / 2);
				if (this->at(i) == s)
				{
					p2 = i;
					p1 = i - 1;
					break;
				}
				else if (this->at(i) > s)
				{
					p2 = i;
				}
				else
				{
					p1 = i;
				}
			}
			for (size_t j = 0; j < size - p2; ++j)
			{
				this->SwapElementsAt(this->size() - j - 1, this->size() - j - 2);
			}
		}
	}

	int Find(const T & item) const
	{
		return InnerFind(0, this->size() - 1, item);
	}

	const size_t Size() const { return this->size(); }

	const T & operator[](size_t index) const
	{
		return (*this)[index];
	}

	T & operator[](size_t index)
	{
		return this->at(index);
	}
};