#include "DirectoryTree.h"
#include "GenerateHash.h"
#include <iostream>

bool dirtree::Entity::CompareByteByByte(const Entity & other) const
{
	return false;
}

void dirtree::Entity::SetSize(uintmax_t fileSize)
{
	this->fileSize = fileSize;
}

uintmax_t dirtree::Entity::Size() const
{
	if (this->Type == File)
	{
		return this->fileSize;
	}
	else
	{
		return this->Subdirectories.size();
	}
}

bool dirtree::Entity::operator==(const Entity & other) const
{
	if (this->Type != other.Type)
	{
		return false;
	}
	if (this->Type == File)
	{
		if (Entity::HashOnly)
		{
			return this->Hash == other.Hash && this->Date == other.Date && this->Size() == other.Size();
		}
		else
		{
			return this->Date == other.Date && this->Size() == other.Size() &&
				this->CompareByteByByte(other);
		}
	}
	else
	{
		return this->Name == other.Name;
	}
}

bool dirtree::Entity::operator<(const Entity & other) const
{
	if (this->Type != other.Type)
	{
		return false;
	}
	if (this->Type == File)
	{
		return this->Hash < other.Hash;
	}
	else
	{
		return this->Name < other.Name;
	}
}

bool dirtree::Entity::CompareByLabel(const Entity & other) const
{
	return this->Type == other.Type &&
		this->Hash == other.Hash;
}

dirtree::Level & dirtree::TreeComparingTable::NthLevel(size_t n)
{
	this->reserve(n + 1);
	while (this->size() < n + 1)
	{
		this->push_back(unordered_map<string, Entity>());
	}

	return this->at(n);
}

void dirtree::TreeComparingTable::LabelByHash()
{
	// Starting at the next to last Level because the last
	// one consists of files or empty folders thus are already labeled
	// with their hashes
	vector<HashValue> labels;
	size_t labelsIndex = 0;
	uint32_t buffer[HashValue::VALUESIZE];

	// Iterating Levels
	for (int i = this->size() - 2; i >= 0; i--)
	{
		Level & level = this->at(i);

		// Iterating entities on current level
		for (auto& j : level)
		{
			Entity & entity = j.second;
			// files and empty folders are already labeled
			if (entity.Type == File || entity.Subdirectories.size() == 0)
			{
				continue;
			}
			this->SortByHash(entity.Subdirectories, i + 1);
			labelsIndex = 0;
			for (size_t k = 0; k < entity.Subdirectories.size(); ++k)
			{
				// The subdirectories are a sorted array so vector labels will be sorted as well
				if (labelsIndex < labels.size())
				{
					labels[labelsIndex++] = (this->NthLevel(i + 1)[entity.Subdirectories[k]]).Hash;
				}
				else
				{
					labels.push_back((this->NthLevel(i + 1)[entity.Subdirectories[k]]).Hash);
					labelsIndex = labels.size();
				}
			}

			for (size_t k = labelsIndex; k < labels.size(); ++k)
			{
				labels.pop_back();
			}

			{
				unsigned long long currentDataSize = labels.size() * sizeof(HashValue);
				unique_ptr<unsigned char> data(new unsigned char[currentDataSize]);
				try
				{
					memcpy(data.get(), labels.data(), currentDataSize);
				}
				catch (const std::exception& e)
				{
					std::cout << "Failed reinterpretation memcopy:\n";
					std::cout << e.what() << std::endl;
					throw e;
				}

				GenerateHash(data.get(), currentDataSize, buffer);
				entity.Hash = buffer;
			}

		}
	}
}

void dirtree::TreeComparingTable::SortByHash(vector<string> & subdirs, size_t depth)
{
	// selection sort for now
	int i, j, min_idx;

	// One by one move boundary of unsorted subarray 
	for (i = 0; i < subdirs.size() - 1; i++)
	{
		// Find the minimum element in unsorted array 
		min_idx = i;
		for (j = i + 1; j < subdirs.size(); j++)
		{
			if (this->CompareSubdirsByHash(subdirs[j], subdirs[min_idx], depth))
			{
				min_idx = j;
			}
		}
		// Swap the found minimum element with the first element
		iter_swap(subdirs.begin() + min_idx, subdirs.begin() + i);
	}
}

bool dirtree::TreeComparingTable::CompareSubdirsByHash(const string & a, const string & b, size_t depth)
{
	Entity & entity_a = this->NthLevel(depth)[a];
	Entity & entity_b = this->NthLevel(depth)[b];

	return entity_a.Hash < entity_b.Hash;
}

void dirtree::TreeComparingTable::CheckIsomorphicSubtrees(TreeComparingTable & left, TreeComparingTable & right) const
{
	size_t n = min(left.size(), right.size());
	
	for (int i = n - 1; i >= 0; i--)
	{
		Level & lvlLeft = left.NthLevel(i);
		Level & lvlRight = right.NthLevel(i);

		for (auto& j : lvlLeft)
		{
			Entity & eLeft = j.second;
			for (auto& k : lvlRight)
			{
				Entity & eRight = k.second;
				if (eLeft == eRight)
				{
					eRight.Check = true;
					eLeft.Check = true;
					continue;
				}
			}
		}
	}

}

dirtree::Entity & dirtree::TreeComparingTable::TreeRoot()
{
	return this->at(0).begin()->second;
}

bool dirtree::Entity::Block = false;

bool dirtree::Entity::HashOnly = false;
