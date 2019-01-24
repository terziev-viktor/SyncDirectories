#include "HashValue.h"
#include "SortedVector.h"

#include <string>
#include <unordered_map>
#include <experimental/filesystem>
using namespace std::experimental::filesystem;
using std::vector;
using std::string;
using std::unordered_map;

// Using this Idea:
// http://crypto.cs.mcgill.ca/~crepeau/CS250/2004/HW5+.pdf
namespace dirtree
{
	enum EntityType
	{
		Folder, File
	};

	// Entity that represents a file OR a folder
	// Not using hierarchy because dynamic casts are expensive
	class Entity
	{
	private:
		bool CompareByteByByte(const Entity & other) const;

		uintmax_t fileSize;
	public:
		// Comparing only file hashes not byte-by-byte
		static bool HashOnly;

		// using block-hash comparrison for files
		static bool Block;
		// For convenience in the algorithm Im using
		Entity()
		{
			this->Type = Folder;
		}

		void SetSize(uintmax_t fileSize);

		// Last modified date
		v1::file_time_type Date;

		// Size of the file in bytes or the count of the directories in the folder
		uintmax_t Size() const;

		// Names of subfolders and files in the current directory
		// Their location is one step deeper than this entity
		vector<string> Subdirectories;

		v1::path Path;
		
		// Unique id of the folder, 
		// if 2 subtrees have the same label they are the isomorphic
		HashValue Hash;

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

	// Level in the directory tree
	// Mapping the name of the file/directory to an Entity object
	typedef unordered_map<string, Entity> Level;

	// Rows of Levels
	// So the Level 0 is the root and level 1 are the sub directories of the root and so on
	class TreeComparingTable : public vector<Level>
	{
	public:
		Level & NthLevel(size_t n);

		// Labeling folders by generating hash on their contents
		void LabelByHash();

		void SortByHash(vector<string> & subdirs, size_t depth);

		bool CompareSubdirsByHash(const string & a, const string & b, size_t depth);

		void CheckIsomorphicSubtrees(TreeComparingTable & left, TreeComparingTable & right) const;

		Entity & TreeRoot();
	};
}