#include "Analyze.h"
#include "DirectoryTree.h"
#include "GenerateHash.h"

#include <fstream>
using namespace dirtree;
using cmds::Analyze;
using cmds::CommandResult;
using cmds::Command;

Analyze::Analyze()
	:Command("analyze")
{
}


Analyze::~Analyze()
{
}

// Recursively iterate a directory and create a tree comparing table
void BuildTCT(TreeComparingTable & tct, char * path)
{
	for (auto i = recursive_directory_iterator(path); i != recursive_directory_iterator(); ++i)
	{
		int depth = i.depth() + 1; // the iterator counts in a retarded way

		if (depth <= 0) // we are on or above the root so nothing to do here
		{
			continue;
		}

		Entity e;
		e.FullPath = i->path();
		e.Name = e.FullPath.filename().string();
		v1::path p1 = e.FullPath;

		for (size_t j = 0; j < i.depth(); ++j)
		{
			e.RelativePath = p1.parent_path().filename() / e.RelativePath;

			p1 = p1.parent_path();
		}
		e.RelativePath = e.RelativePath / e.FullPath.filename();
		string relativePathStr = e.RelativePath.string();

		if (is_regular_file(i->path()))
		{
			e.Type = dirtree::EntityType::File;
			e.SetSize(file_size(e.FullPath));
			uint32_t buffer[5];
			GenerateHashForFile(e.FullPath.string(), e.Size(), buffer);
			e.Hash = buffer; // the label of every file is its hash
			e.Date = last_write_time(e.FullPath);
			tct.Files().PushBack(EntityInfo() =
				{
					e.Hash,
					depth,
					relativePathStr
				});

			tct.NthLevel(depth)[relativePathStr] = e;

			// Every file is resposible for informing his parent for its existance
			tct.NthLevel(depth - 1)[e.RelativePath.parent_path().string()].Subdirectories.push_back(relativePathStr);
		}
		else
		{
			// Every folder is identified by its content and name.
			// Because every file informs its parent for its existance 
			// The folder will be built by the end of this loop
			tct.NthLevel(depth)[relativePathStr] = e;
			tct.NthLevel(depth - 1)[e.RelativePath.parent_path().string()].Subdirectories.push_back(relativePathStr);
		}
	}
}

void _Mirror(TreeComparingTable & tctLeft, TreeComparingTable & tctRight, vector<Entity> & shouldBeCreatedOnTheRight)
{
	size_t MinDepth = min(tctLeft.size(), tctRight.size());

	for (size_t i = 1; i < MinDepth; ++i)
	{
		Level & leftlvl = tctLeft.NthLevel(i);
		Level & rightlvl = tctRight.NthLevel(i);

		for (auto& j : rightlvl)
		{
			Entity & rightEntity = j.second;
			if (rightEntity.Check)
			{
				continue;
			}
			const string & rightEntityName = j.first;
			rightEntity.Check = true;
			// Try to find the entity on the left by name
			auto leftEntityIter = leftlvl.find(rightEntityName);

			bool found = leftEntityIter != leftlvl.end();

			if (rightEntity.Type == Folder)
			{
				if (!found)
				{
					rightEntity.ShouldBeDeleted = true;
				}
				else
				{
					leftEntityIter->second.Check = true;
				}
			}
			else // is a file
			{
				if (found) // found the file by name
				{
					bool same = rightEntity == leftEntityIter->second;
					leftEntityIter->second.Check = true;
					// The files have the same name but not the same content
					if (!same)
					{
						rightEntity.ShouldCopyFrom = leftEntityIter->second.FullPath;
					}
				}
				else
				{
					// Try to find it in the files of the left directory
					Entity * f = tctLeft.FindFile(rightEntity);
					if (f != nullptr)
					{
						f->Check = true;

						if (rightEntity.RelativePath.parent_path() != f->RelativePath.parent_path())
							rightEntity.ShouldBeMovedTo = f->RelativePath.parent_path();

						if (rightEntity.Name != f->Name)
							rightEntity.ShouldBeRenamedTo = f->Name;
					}
					else
					{
						rightEntity.ShouldBeDeleted = true;
					}
				}
			}
		}
	}

	for (size_t i = 1; i < MinDepth; ++i)
	{
		Level & leftlvl = tctLeft.NthLevel(i);
		Level & rightlvl = tctRight.NthLevel(i);

		for (auto& j : leftlvl)
		{
			Entity & leftEntity = j.second;
			if (leftEntity.Check)
			{
				continue;
			}
			// If the entity is not checked that means it's missing on the right:
			shouldBeCreatedOnTheRight.push_back(leftEntity);
		}
	}

	if (MinDepth < tctRight.size())
	{
		for (size_t i = MinDepth; i < tctRight.size(); ++i)
		{
			Level & lvl = tctRight.NthLevel(i);
			for (auto& j : lvl)
			{
				j.second.ShouldBeDeleted = true;
			}
		}
	}
}

// Right directory must become an exact copy of left directory
CommandResult Analyze::Mirror(int argc, char * argv[]) const
{
	TreeComparingTable tctLeft;
	TreeComparingTable tctRight;
	size_t off = 0;

	for (size_t i = 0; i < argc && argv[i][0] == '-'; ++i)
	{
		if (strcmp(argv[i], "-hash-only") == 0)
		{
			Entity::HashOnly = true;
			++off;
		}
		else if (strcmp(argv[i], "-block") == 0)
		{
			Entity::Block = true;
			++off;
		}
		else
		{
			return CommandResult() =
			{
				false,
				"Invalid option for command analyze"
			};
		}
	}

	BuildTCT(tctLeft, argv[0 + off]);
	BuildTCT(tctRight, argv[1 + off]);

	// Labeling folders with hash of their contents
	tctLeft.LabelByHash();
	tctRight.LabelByHash();

	if (Entity::HashOnly && tctLeft.TreeRoot().Hash == tctRight.TreeRoot().Hash)
	{
		return CommandResult() =
		{
			true,
			"The Directories are already synchronized"
		};
	}

	if (Entity::HashOnly)
	{
		tctLeft.CheckIsomorphisSubtrees(tctRight);
	}

	// Handling the different subdirectories
	vector<Entity> ShouldBeCreated;
	_Mirror(tctLeft, tctRight, ShouldBeCreated);

	for (size_t i = 0; i < ShouldBeCreated.size(); ++i)
	{
		std::cout << "CREATE " << ShouldBeCreated[i].FullPath << std::endl;
	}
	for (size_t i = 1; i < tctRight.size(); ++i)
	{
		Level & lvl = tctRight.NthLevel(i);
		for (auto& j : lvl)
		{
			Entity & e = j.second;
			if (!e.ShouldBeMovedTo.empty())
			{
				cout << "MOVE " << e.FullPath << " TO " << e.ShouldBeMovedTo << endl;
			}
			else if (!e.ShouldBeRenamedTo.empty())
			{
				cout << "RENAME " << e.FullPath << " TO " << e.ShouldBeRenamedTo << endl;
			}
			else if (!e.ShouldCopyFrom.empty())
			{
				cout << "COPY TO " << e.FullPath << " FROM " << e.ShouldCopyFrom << endl;
			}
			else if (e.ShouldBeDeleted)
			{
				cout << "DELETE " << e.FullPath << endl;
			}
		}
	}

	return CommandResult() =
	{
		true,
		"Directories analyzed. The output is in sync.txt"
	};
}

CommandResult Analyze::Safe(int argc, char * argv[]) const
{
	TreeComparingTable tctLeft;
	TreeComparingTable tctRight;
	size_t off = 0;

	for (size_t i = 0; i < argc && argv[i][0] == '-'; ++i)
	{
		if (strcmp(argv[i], "-hash-only") == 0)
		{
			Entity::HashOnly = true;
			++off;
		}
		else if (strcmp(argv[i], "-block") == 0)
		{
			Entity::Block = true;
			++off;
		}
		else
		{
			return CommandResult() =
			{
				false,
				"Invalid option for command analyze"
			};
		}
	}

	BuildTCT(tctLeft, argv[0 + off]);
	BuildTCT(tctRight, argv[1 + off]);

	// Labeling folders with hash of their contents
	tctLeft.LabelByHash();
	tctRight.LabelByHash();



	return CommandResult() =
	{
		true,
		"Directories analyzed. The output is in sync.txt"
	};
}

CommandResult Analyze::Standard(int argc, char * argv[]) const
{
	return CommandResult() =
	{
		false,
		"Analyze::Standard not implemented yet"
	};
}

CommandResult Analyze::Execute(int argc, char * argv[]) const
{
	std::cout << endl;
	if (strcmp(argv[0], "mirror") == 0)
	{
		return this->Mirror(argc - 1, argv + 1);
	}

	if (strcmp(argv[0], "safe") == 0)
	{
		return this->Safe(argc - 1, argv + 1);
	}

	if (strcmp(argv[0], "standard") == 0)
	{
		return this->Standard(argc - 1, argv + 1);
	}

	return CommandResult() =
	{
		false,
		"!! This command is not implemented yet !!"
	};
}

const Command * cmds::Analyze::Create()
{
	return new const Analyze();
}
