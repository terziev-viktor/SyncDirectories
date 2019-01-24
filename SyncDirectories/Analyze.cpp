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
		e.Check = false;
		e.Path = i->path();
		e.Name = e.Path.filename().string();

		string name = e.Name;
		string parent = e.Path.parent_path().filename().string();

		if (is_regular_file(i->path()))
		{
			e.Type = dirtree::EntityType::File;
			uint32_t buffer[5];
			GenerateHash(i->path().string(), buffer);
			e.Hash = buffer; // the label of every file is its hash
			e.SetSize(file_size(e.Path));
			e.Date = last_write_time(e.Path);

			tct.NthLevel(depth)[e.Path.string()] = e;

			// Every file is resposible for informing his parent for its existance
			tct.NthLevel(depth - 1)[parent].Subdirectories.push_back(e.Path.string());
		}
		else
		{
			// Every folder is identified by its content and name.
			// Because every file informs its parent for its existance 
			// The folder will be built by the end of this loop
			tct.NthLevel(depth)[e.Path.string()] = e;
			tct.NthLevel(depth - 1)[parent].Subdirectories.push_back(e.Path.string());
		}
	}
}

// Right directory must become an exact copy of left directory
CommandResult Analyze::Mirror(char * argv[]) const
{
	TreeComparingTable tctLeft;
	TreeComparingTable tctRight;
	size_t off = 0;
	string syncTXT;

	if (argv[0][0] == '-') // a command
	{
		for (size_t i = 0; i < 2; ++i)
		{
			++off;
			if (strcmp(argv[i], "-hash-only") == 0)
			{
				Entity::HashOnly = true;

			}
			else if (strcmp(argv[i], "-block") == 0)
			{
				Entity::Block = true;
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
	}

	BuildTCT(tctLeft, argv[0 + off]);
	BuildTCT(tctRight, argv[1 + off]);

	tctLeft.LabelByHash();
	tctRight.LabelByHash();

	// Handling the different subdirectories
	this->HandleMissingDirectories(tctLeft, tctRight);

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

	if (strcmp(argv[0], "mirror") == 0)
	{
		return this->Mirror(argv + 1);
	}

	if (strcmp(argv[0], "safe") == 0)
	{
		return this->Safe(argv + 1);
	}

	if (strcmp(argv[0], "standard") == 0)
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
