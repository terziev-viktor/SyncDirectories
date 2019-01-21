#include "Analyze.h"
#include "DirectoryTree.h"
#include "GenerateHash.h"

using dirtree::TreeComparingTable;
using dirtree::Entity;
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
		e.Check = false;
		e.Path = i->path();
		string name = e.Path.filename().string();
		string parent = e.Path.parent_path().filename().string();

		if (is_regular_file(i->path()))
		{
			e.Type = dirtree::EntityType::File;
			uint32_t buffer[5];
			GenerateHash(i->path().string(), buffer);
			e.Hash.push_back(buffer);
			tct.NthLevel(depth)[name] = e;

			// Every file is resposible for informing his parent for its existance
			tct.NthLevel(depth - 1)[parent].Subdirectories.push_back(name);
		}
		else
		{
			// Every folder is identified by its content and name.
			// Because every file informs its parent for its existance 
			// The folder will be built by the end of this loop
			tct.NthLevel(depth)[name] = e;
			tct.NthLevel(depth - 1)[parent].Subdirectories.push_back(name);
		}
	}
}

CommandResult Analyze::Mirror(char * argv[]) const
{
	TreeComparingTable tctLeft;
	TreeComparingTable tctRight;
	BuildTCT(tctLeft, argv[0]);
	BuildTCT(tctRight, argv[1]);
	
	
	{ // Everything is working and for now just printing the results ----

		std::cout << "Table for " << argv[0] << std::endl;
		for (size_t i = 0; i < tctLeft.size(); ++i)
		{
			std::cout << i << " ";
			for (auto & j : tctLeft[i])
			{
				std::cout << "(name:" << j.first << " subdirs:";
				for (int k = 0; k < j.second.Subdirectories.size(); ++k)
				{
					std::cout << j.second.Subdirectories[k] << ',';
				}
				std::cout << "), ";
			}
			std::cout << std::endl;
		}

		std::cout << std::endl << "Table for " << argv[1] << std::endl;
		for (size_t i = 0; i < tctRight.size(); ++i)
		{
			std::cout << i << " ";
			for (auto & j : tctRight[i])
			{
				std::cout << j.first << ' ';
			}
			std::cout << std::endl;
		}
	} // -----------------------------------------------------------------

	return CommandResult() = 
	{
		false,
		"Implementing this function at the moment"
	};
}

CommandResult Analyze::Safe(char * argv[]) const
{
	return CommandResult() =
	{
		false,
		"Analyze::Safe not implemented yet"
	};
}

CommandResult Analyze::Standard(char * argv[]) const
{
	return CommandResult() =
	{
		false,
		"Analyze::Standard not implemented yet"
	};
}

CommandResult Analyze::Execute(int argc, char * argv[]) const
{
	std::cout << "Analyzing these directories: " << std::endl;
	for (int i = 1; i < argc; ++i)
	{
		std::cout << argv[i] << std::endl;
	}

	if(strcmp(argv[0], "mirror") == 0)
	{
		return this->Mirror(argv + 1);
	}

	if(strcmp(argv[0], "safe") == 0)
	{
		return this->Safe(argv + 1);
	}
	
	if(strcmp(argv[0], "standard") == 0)
	{
		return this->Standard(argv + 1);
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
