#include "DirectoryTree.h"
#include "GenerateHash.h"
#include <iostream>
#include <fstream>

bool Entity::CompareByteByByte(const Entity & other) const
{
	if (this->Size() != other.Size())
	{
		return false;
	}
	std::ifstream in1(this->FullPath.string(), std::ios::binary);
	std::ifstream in2(other.FullPath.string(), std::ios::binary);
	if (!in1 || !in2)
	{
		in1.close();
		in2.close();
		throw std::exception("Could not open directory");
	}

	unique_ptr<char> thisBuffer(new char[Entity::BlockSize]);
	unique_ptr<char> otherBuffer(new char[Entity::BlockSize]);

	while (in1.eof())
	{
		in1.read(thisBuffer.get(), Entity::BlockSize);
		in2.read(otherBuffer.get(), Entity::BlockSize);
		for (size_t i = 0; i < in1.gcount(); ++i)
		{
			if (thisBuffer.get()[i] != otherBuffer.get()[i])
			{
				in1.close();
				in2.close();
				return false;
			}
		}
	}

	in1.close();
	in2.close();

	return true;
}

void Entity::SetSize(uintmax_t fileSize)
{
	this->fileSize = fileSize;
}

uintmax_t Entity::Size() const
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

vector<HashValue>& Entity::GetBlockHashes()
{
	return this->fileAsHashes;
}

const vector<HashValue>& Entity::GetBlockHashes() const
{
	return this->fileAsHashes;
}

bool Entity::operator==(const Entity & other) const
{
	if (this->Type != other.Type)
	{
		return false;
	}
	if (this->Type == File)
	{
		if (Entity::HashOnly)
		{
			return this->Hash == other.Hash;
		}
		else
		{
			return this->CompareByteByByte(other);
		}
	}
	else
	{
		return this->RelativePath == other.RelativePath;
	}
}

bool Entity::operator<(const Entity & other) const
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

bool Entity::CompareByLabel(const Entity & other) const
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
				unique_ptr<char> data(new char[currentDataSize]);
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

void dirtree::TreeComparingTable::CheckIsomorphisSubtrees(TreeComparingTable & other)
{
	for (size_t i = 0; i < this->size(); ++i)
	{
		Level & thisLvl = this->NthLevel(i);
		Level & otherLvl = other.NthLevel(i);
		
		for (auto& j : thisLvl)
		{
			Entity & thisEntity = j.second;
			const string & thisEntityName = j.first;
			auto iter = otherLvl.find(thisEntityName);

			// found by name
			if (iter != otherLvl.end() && iter->second.Hash == thisEntity.Hash)
			{
				iter->second.Check = true;
				thisEntity.Check = true;
			}
			else // not found by name or hashes don't match
			{
				for (auto& k : otherLvl)
				{
					if (thisEntity.Hash == k.second.Hash)
					{
						thisEntity.Check = true;
						k.second.Check = true;
						v1::path relativeParentPath = thisEntity.RelativePath.parent_path();
						if (relativeParentPath != k.second.RelativePath.parent_path())
						{
							k.second.ShouldBeMovedTo = relativeParentPath;
						}
						if (thisEntity.Name != k.second.Name)
						{
							k.second.ShouldBeRenamedTo = thisEntity.Name;
						}
						break;
					}
				}
			}
		}
	}
}

Entity * dirtree::TreeComparingTable::FindFile(const Entity & file)
{
	EntityInfo info = 
	{
		file.Hash,
		0,
		""
	};
	int ind = this->files.Find(info);
	if (ind == -1)
	{
		return nullptr;
	}
	// Go to the left most collision
	int n = ind;
	while (ind > 0 && this->files[ind - 1].Hash == this->files[ind].Hash) { --ind; }
	for (size_t i = 0; i <= n; ++i)
	{
		Entity & e = (*this)[this->files[ind]];
		if (e == file)
		{
			return &e;
		}
	}
	return nullptr;
}

Entity & dirtree::TreeComparingTable::operator[](const EntityInfo & info)
{
	return this->NthLevel(info.depth)[info.RelativePath];
}

Entity & dirtree::TreeComparingTable::TreeRoot()
{
	return this->at(0).begin()->second;
}

void dirtree::TreeComparingTable::SaveHashes(const std::string path_to_bin_file)
{
	std::ofstream out;
	out.open(path_to_bin_file, std::ios::binary | std::ios::app);
	if (!out)
	{
		throw exception("Could not open binary file");
	}
	for (size_t i = 0; i < this->files.Size(); ++i)
	{
		Entity & file = (*this)[this->files[i]];
		size_t s = file.GetBlockHashes().size();
		string fpath = file.FullPath.string();
		size_t fpathLen = fpath.length() + 1;
		out.write(reinterpret_cast<char*>(&fpathLen), sizeof(size_t));
		out.write(fpath.c_str(), fpathLen);
		out.write(reinterpret_cast<char*>(&s), sizeof(s));
		for (size_t i = 0; i < s; ++i)
		{
			out.write(reinterpret_cast<char*>(&file.GetBlockHashes()[i]), sizeof(HashValue));
		}
	}
	out.close();
}

bool Entity::Block = false;

bool Entity::HashOnly = false;
