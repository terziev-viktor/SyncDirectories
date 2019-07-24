#include "HashValue.h"
#include "SortedVector.h"
#include "Entity.h"

#include <unordered_map>
using std::unordered_map;

// Using this Idea:
// http://crypto.cs.mcgill.ca/~crepeau/CS250/2004/HW5+.pdf
namespace dirtree
{
	// Level in the directory tree
	// Mapping the name of the file/directory to an Entity object
	typedef unordered_map<string, Entity> Level;

	// Rows of Levels
	// So the Level 0 is the root and level 1 are the sub directories of the root and so on
	class TreeComparingTable : public vector<Level>
	{
	private:
		SortedVector<EntityInfo> files;

	public:
		Level & NthLevel(size_t n);

		// Labeling folders by generating hash on their contents
		void LabelByHash();

		void SortByHash(vector<string> & subdirs, size_t depth);

		bool CompareSubdirsByHash(const string & a, const string & b, size_t depth);

		const SortedVector<EntityInfo> & Files() const { return files; }
		
		// Checks subtrees on the same level and with the same hash
		// If names or relative paths dont match <other>'s ShouldBeRenamedTo and ShouldBeMovedTo are changed
		void CheckIsomorphisSubtrees(TreeComparingTable * other);

		v1::path RelativeRootPath;

		SortedVector<EntityInfo> & Files() { return files; }

		Entity * FindFile(const Entity & file);

		Entity & operator[](const EntityInfo & info);

		Entity & TreeRoot();

		void SaveHashes(const std::string path_to_bin_file);
	};
}