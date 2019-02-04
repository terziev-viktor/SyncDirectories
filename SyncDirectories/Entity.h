#pragma once
#include <experimental/filesystem>
using namespace std::experimental::filesystem;
#include <string>
using std::string;
#include "HashValue.h"
#include <vector>
using std::vector;
enum EntityType
{
	Folder, File
};
class EntityInfo
{
public:
	HashValue Hash;

	// Coordinates in the table ---
	int depth;

	string RelativePath;
	//-----------------------------

	string FullPath;

	bool operator==(const EntityInfo & other) const
	{
		return this->Hash == other.Hash;
	}

	bool operator<(const EntityInfo & other) const
	{
		return this->Hash < other.Hash;
	}

	bool operator<=(const EntityInfo & other) const
	{
		return this->Hash <= other.Hash;
	}

	bool operator>(const EntityInfo & other) const
	{
		return this->Hash > other.Hash;
	}

	bool operator>=(const EntityInfo & other) const
	{
		return this->Hash >= other.Hash;
	}
};

// Entity that represents a file OR a folder
// Not using hierarchy because dynamic casts are expensive
class Entity
{
private:
	bool CompareByteByByte(const Entity & other) const;

	// hashes of blocks of 64MB each representing the whole file
	vector<HashValue> fileAsHashes;

	uintmax_t fileSize;
public:
	// 64 MB
	static const size_t BlockSize = 10;

	// Comparing only file hashes not byte-by-byte
	static bool HashOnly;

	// using block-hash comparrison for files
	static bool Block;
	// For convenience in the algorithm Im using
	Entity()
	{
		this->Type = Folder;
		this->ShouldBeDeleted = false;
		this->Check = false;
	}

	void SetSize(uintmax_t fileSize);

	v1::path ShouldBeMovedTo;

	string ShouldBeRenamedTo;

	// Full path of the entity to copy from
	EntityInfo ShouldCopyFrom;

	bool ShouldBeDeleted;

	// Last modified date
	v1::file_time_type Date;

	// Size of the file in bytes or the count of the directories in the folder
	uintmax_t Size() const;

	// Names of subfolders and files in the current directory
	// Their location is one step deeper than this entity
	vector<string> Subdirectories;

	// Full path to the entity
	v1::path FullPath;

	// Path to the enity relative to the path given by the user
	v1::path RelativePath;

	// Unique id of the folder, 
	// if 2 subtrees have the same label they are the isomorphic
	HashValue Hash;

	vector<HashValue> & GetBlockHashes();
	const vector<HashValue> & GetBlockHashes() const;

	// Title of the file/folder == path.filename().string()
	string Name;

	bool Check; // meta data for the main algorithm

				// Folder or File
	EntityType Type;

	// Folders are compared by name and files by hash, date and size
	bool operator==(const Entity & other) const;

	// Comparing folders by name and files by content
	bool operator<(const Entity & other) const;

	// Comparing hashes for files and names for folders
	bool CompareByLabel(const Entity & other) const;
};