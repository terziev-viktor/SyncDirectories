#include "HashValue.h"

#include <string>
#include <vector>
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

	struct Entity
	{
		// For convenience in the algorithm Im using
		Entity()
		{
			this->Type = Folder;
		}
		vector<string> Subdirectories;

		v1::path Path;

		bool Check; // meta data

		EntityType Type;

		vector<HashValue> Hash;
	};

	// Level in the directory tree
	// Mapping the name of the file/directory to an Entity object
	typedef unordered_map<string, Entity> Level;

	// Rows of Levels
	// So the Level 0 is the root and level 1 are the sub directories of the root and so on
	class TreeComparingTable : public vector<Level>
	{
	public:
		Level & NthLevel(size_t n)
		{
			this->reserve(n + 1);
			while (this->size() < n + 1)
			{
				this->push_back(unordered_map<string, Entity>());
			}

			return this->at(n);
		}
	};
}