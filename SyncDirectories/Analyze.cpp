#include "Analyze.h"
#include "DirectoryTree.h"
#include "GenerateHash.h"

#include <fstream>
#include <future>
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
void BuildTCT(TreeComparingTable * tct, const char * p)
{
	tct->RelativeRootPath = p;
	recursive_directory_iterator end = recursive_directory_iterator();

	for (auto i = recursive_directory_iterator(tct->RelativeRootPath); i != end; ++i)
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
			e.Type = EntityType::File;
			e.SetSize(file_size(e.FullPath));

			uint32_t buffer[5];
			GenerateHashForFile(e.FullPath.string(), buffer);
			e.Hash = buffer; // the label of every file is its hash
			if (Entity::Block)
			{
				GenerateHashForFile(e.FullPath.string(), e.GetBlockHashes());
			}

			e.Date = last_write_time(e.FullPath);
			tct->Files().PushBack(EntityInfo
				{
					e.Hash,
					depth,
					relativePathStr
				});

			tct->NthLevel(depth)[relativePathStr] = e;

			// Every file is resposible for informing his parent for its existance
			tct->NthLevel(depth - 1)[e.RelativePath.parent_path().string()].Subdirectories.push_back(relativePathStr);
		}
		else
		{
			// Every folder is identified by its content and name.
			// Because every file informs its parent for its existance 
			// The folder will be built by the end of this loop
			tct->NthLevel(depth)[relativePathStr] = e;
			tct->NthLevel(depth - 1)[e.RelativePath.parent_path().string()].Subdirectories.push_back(relativePathStr);
		}
	}
}

void BuildTCTs(TreeComparingTable * tctLeft, TreeComparingTable * tctRight, const char * argv[], size_t off)
{
	thread build_thread1(BuildTCT, tctLeft, argv[0 + off]);
	thread build_thread2(BuildTCT, tctRight, argv[1 + off]);
	
	if (build_thread1.joinable())
	{
		build_thread1.join();
	}
	// Labeling folders with hash of their contents
	thread label_thread1(&TreeComparingTable::LabelByHash, tctLeft);

	if (build_thread2.joinable())
	{
		build_thread2.join();
	}
	thread label_thread2(&TreeComparingTable::LabelByHash, tctRight);
	
	if (label_thread1.joinable())
	{
		label_thread1.join();
	}
	if (label_thread2.joinable())
	{
		label_thread2.join();
	}

	if (Entity::HashOnly)
	{
		tctLeft->CheckIsomorphisSubtrees(tctRight);
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
						rightEntity.ShouldCopyFrom.RelativePath = leftEntityIter->second.RelativePath.string();
						rightEntity.ShouldCopyFrom.depth = i;
						rightEntity.ShouldCopyFrom.Hash = leftEntityIter->second.Hash;
						rightEntity.ShouldCopyFrom.FullPath = leftEntityIter->second.FullPath.string();
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
							rightEntity.ShouldBeMovedTo = f->RelativePath;

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
CommandResult Analyze::Mirror(int argc, const char * argv[]) const
{
	TreeComparingTable tctLeft;
	TreeComparingTable tctRight;
	size_t off = 0;
	CommandResult r = this->ReadOptions(argc, argv, off);
	if (!r.Successful)
	{
		return r;
	}
	BuildTCTs(&tctLeft, &tctRight, argv, off);

	if (Entity::HashOnly && tctLeft.TreeRoot().Hash == tctRight.TreeRoot().Hash)
	{
		return CommandResult
		{
			true,
			"The Directories are already synchronized"
		};
	}

	// Handling the different subdirectories
	vector<Entity> ShouldBeCreated;
	_Mirror(tctLeft, tctRight, ShouldBeCreated);

	std::ofstream out("sync.txt");
	if (!out)
	{
		out.close();
		return CommandResult
		{
			false, "Could not open file"
		};
	}

	out << (Entity::HashOnly ? "hash-only" : "not-hash-only") << " " << (Entity::Block ? "block" : "not-block") << endl;
	for (size_t i = 0; i < ShouldBeCreated.size(); ++i)
	{
		if (ShouldBeCreated[i].Type == File)
		{
			out << "COPY " << tctRight.RelativeRootPath << '\\' << ShouldBeCreated[i].RelativePath
				<< " FROM " << ShouldBeCreated[i].FullPath << endl;
		}
		else
		{
			out << "CREATE " << tctRight.RelativeRootPath << '\\' << ShouldBeCreated[i].RelativePath << '\n';
		}
	}
	for (int i = tctRight.size() - 1; i >= 0; --i)
	{
		Level & lvl = tctRight.NthLevel(i);
		for (auto& j : lvl)
		{
			Entity & e = j.second;
			if (!e.ShouldCopyFrom.RelativePath.empty())
			{
				out << "COPY " << e.FullPath << " FROM " << tctLeft.RelativeRootPath << '\\' << e.ShouldCopyFrom.RelativePath << '\n';
			}
			if (!e.ShouldBeMovedTo.empty())
			{
				out << "MOVE " << e.FullPath << " TO " << tctRight.RelativeRootPath << '\\' << e.ShouldBeMovedTo << "\\" << e.Name << endl;
			}
			if (!e.ShouldBeRenamedTo.empty())
			{
				if (!e.ShouldBeMovedTo.empty())
				{
					out << "RENAME " << tctRight.RelativeRootPath << '\\' << e.ShouldBeMovedTo << "\\" << e.Name << " TO " << tctRight.RelativeRootPath << '\\' << e.ShouldBeMovedTo << "\\" << e.ShouldBeRenamedTo
						<< " # Moved by the previus command" << endl;
				}
				else
				{
					out << "RENAME " << e.FullPath << " TO " << tctRight.RelativeRootPath << "\\" << e.RelativePath << "\\" << e.ShouldBeRenamedTo << endl;
				}
			}
			if (e.ShouldBeDeleted)
			{
				out << "DELETE " << e.FullPath << endl;
			}
		}
	}
	out.close();
	if (Entity::Block)
	{
		tctLeft.SaveHashes("sync.bin");
		tctRight.SaveHashes("sync.bin");
	}
	return CommandResult
	{
		true,
		"Directories analyzed. The output is in sync.txt"
	};
}

CommandResult Analyze::Safe(int argc, const char * argv[]) const
{
	TreeComparingTable tctLeft;
	TreeComparingTable tctRight;
	size_t off = 0;
	CommandResult r = this->ReadOptions(argc, argv, off);
	if (!r.Successful)
	{
		return r;
	}
	BuildTCTs(&tctLeft, &tctRight, argv, off);

	if (Entity::HashOnly && tctLeft.TreeRoot().Hash == tctRight.TreeRoot().Hash)
	{
		return CommandResult
		{
			true,
			"The Directories are already synchronized"
		};
	}

	// Handling the different subdirectories
	vector<Entity> ShouldBeCreated;
	// The same as in Mirror but deletions are now creations of directories (or copying files)
	_Mirror(tctLeft, tctRight, ShouldBeCreated);

	std::ofstream out("sync.txt");
	if (!out)
	{
		out.close();
		return CommandResult
		{
			false, "Could not open file"
		};
	}

	out << (Entity::HashOnly ? "hash-only" : "not-hash-only") << " " << (Entity::Block ? "block" : "not-block") << endl;
	for (size_t i = 0; i < ShouldBeCreated.size(); ++i)
	{
		if (ShouldBeCreated[i].Type == File)
		{
			out << "COPY " << tctRight.RelativeRootPath << '\\' << ShouldBeCreated[i].RelativePath
				<< " FROM " << ShouldBeCreated[i].FullPath << endl;
		}
		else
		{
			out << "CREATE " << tctRight.RelativeRootPath << '\\' << ShouldBeCreated[i].RelativePath << '\n';
		}
	}
	for (int i = tctRight.size() - 1; i >= 0; --i)
	{
		Level & lvl = tctRight.NthLevel(i);
		for (auto& j : lvl)
		{
			Entity & e = j.second;
			if (e.ShouldBeDeleted && e.Type == Folder)
				out << "CREATE " << tctLeft.RelativeRootPath << '\\' << e.RelativePath << endl;
		}
	}

	for (int i = tctRight.size() - 1; i >= 0; --i)
	{
		Level & lvl = tctRight.NthLevel(i);
		for (auto& j : lvl)
		{
			Entity & e = j.second;
			if (!e.ShouldCopyFrom.RelativePath.empty())
			{
				out << "COPY " << e.FullPath << " FROM " << tctLeft.RelativeRootPath << '\\' << e.ShouldCopyFrom.RelativePath << endl;
			}
			if (!e.ShouldBeMovedTo.empty())
			{
				out << "MOVE " << e.FullPath << " TO " << tctRight.RelativeRootPath << '\\' << e.ShouldBeMovedTo << "\\" << e.Name << endl;
			}
			if (!e.ShouldBeRenamedTo.empty())
			{
				if (!e.ShouldBeMovedTo.empty())
				{
					out << "RENAME " << tctRight.RelativeRootPath << '\\' << e.ShouldBeMovedTo << "\\" << e.Name << " TO " << tctRight.RelativeRootPath << '\\' << e.ShouldBeMovedTo << "\\" << e.ShouldBeRenamedTo
						<< " # Moved by the previous command" << endl;
				}
				else
				{
					out << "RENAME " << e.FullPath << " TO " << tctRight.RelativeRootPath << "\\" << e.RelativePath << "\\" << e.ShouldBeRenamedTo << endl;
				}
			}
			if (e.ShouldBeDeleted)
			{
				if (e.Type == File)
				{
					out << "COPY " << tctLeft.RelativeRootPath << '\\' << e.RelativePath << " FROM " << e.FullPath << endl;
				}
			}
		}
	}
	out.close();
	if (Entity::Block)
	{
		tctLeft.SaveHashes("sync.bin");
		tctRight.SaveHashes("sync.bin");
	}
	return CommandResult
	{
		true,
		"Directories analyzed. The output is in sync.txt"
	};
}

CommandResult Analyze::Standard(int argc, const char * argv[]) const
{
	TreeComparingTable tctLeft;	
	TreeComparingTable tctRight;
	size_t off = 0;
	CommandResult r = this->ReadOptions(argc, argv, off);
	if (!r.Successful)
	{
		return r;
	}
	BuildTCTs(&tctLeft, &tctRight, argv, off);

	if (Entity::HashOnly && tctLeft.TreeRoot().Hash == tctRight.TreeRoot().Hash)
	{
		return CommandResult
		{
			true,
			"The Directories are already synchronized"
		};
	}

	// Handling the different subdirectories
	vector<Entity> ShouldBeCreated;
	// The same as in Mirror but deletions are now creations of directories (or copying files with newer date)
	_Mirror(tctLeft, tctRight, ShouldBeCreated);
	std::ofstream out("sync.txt");
	if (!out)
	{
		out.close();
		return CommandResult
		{
			false, "Could not open file"
		};
	}

	out << (Entity::HashOnly ? "hash-only" : "not-hash-only") << " " << (Entity::Block ? "block" : "not-block") << endl;
	for (size_t i = 0; i < ShouldBeCreated.size(); ++i)
	{
		if (ShouldBeCreated[i].Type == File)
		{
			out << "COPY " << tctRight.RelativeRootPath << '\\' << ShouldBeCreated[i].RelativePath
				<< " FROM " << ShouldBeCreated[i].FullPath << endl;
		}
		else
		{
			out << "CREATE " << tctRight.RelativeRootPath << '\\' << ShouldBeCreated[i].RelativePath << '\n';
		}
	}
	for (int i = tctRight.size() - 1; i >= 0; --i)
	{
		Level & lvl = tctRight.NthLevel(i);
		for (auto& j : lvl)
		{
			Entity & e = j.second;
			if (e.ShouldBeDeleted && e.Type == Folder)
				out << "CREATE " << tctLeft.RelativeRootPath << '\\' << e.RelativePath << endl;
		}
	}
	for (int i = tctRight.size() - 1; i >= 0; --i)
	{
		Level & lvl = tctRight.NthLevel(i);
		for (auto& j : lvl)
		{
			Entity & e = j.second;
			if (!e.ShouldCopyFrom.RelativePath.empty())
			{
				// Saving the newer copy
				Entity & otherE = tctLeft[e.ShouldCopyFrom];
				if (otherE.Date < e.Date)
				{
					out << "COPY " << otherE.FullPath << " FROM " << e.FullPath << " # saving the newer copy" << endl;
				}
				else
				{
					out << "COPY " << e.FullPath << " FROM " << otherE.FullPath << endl;
				}
			}
			if (!e.ShouldBeMovedTo.empty())
			{
				out << "MOVE " << e.FullPath << " TO " << tctRight.RelativeRootPath << '\\' << e.ShouldBeMovedTo << "\\" << e.Name << endl;
			}
			if (!e.ShouldBeRenamedTo.empty())
			{
				if (!e.ShouldBeMovedTo.empty())
				{
					out << "RENAME " << tctRight.RelativeRootPath << '\\' << e.ShouldBeMovedTo << "\\" << e.Name << " TO " << tctRight.RelativeRootPath << '\\' << e.ShouldBeMovedTo << "\\" << e.ShouldBeRenamedTo
						<< " # Moved by the previus command" << endl;
				}
				else
				{
					out << "RENAME " << e.FullPath << " TO " << tctRight.RelativeRootPath << "\\" << e.RelativePath << "\\" << e.ShouldBeRenamedTo << endl;
				}
			}
			if (e.ShouldBeDeleted)
			{
				if (e.Type == File)
				{
					out << "COPY " << tctLeft.RelativeRootPath << '\\' << e.RelativePath << " FROM " << e.FullPath << endl;
				}
			}
		}
	}
	out.close();
	if (Entity::Block)
	{
		tctLeft.SaveHashes("sync.bin");
		tctRight.SaveHashes("sync.bin");
	}
	return CommandResult
	{
		true,
		"Directories analyzed. The output is in sync.txt"
	};
}

CommandResult cmds::Analyze::ReadOptions(int argc, const char * argv[], size_t & off) const
{
	off = 0;
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
			return CommandResult
			{
				false,
				"Invalid option for command analyze"
			};
		}
	}
	return CommandResult
	{
		true
	};
}

CommandResult Analyze::Execute(int argc, const char * argv[])
{
	std::cout << "Analyzing...\n";
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

	return CommandResult
	{
		false,
		string(argv[0]) + " is not an analyze option. Type sync.exe help for more details."
	};
}

std::unique_ptr<Command> cmds::Analyze::Create()
{
	return std::make_unique<cmds::Analyze>();
}
