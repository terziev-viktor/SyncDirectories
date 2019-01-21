#include "Analyze.h"
#include <string.h>
#include <experimental/filesystem>
using namespace std::experimental::filesystem;

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

CommandResult Analyze::Mirror(char * argv[]) const
{
	std::cout << argv[0] << ": \n";
	// for(const directory_entry & p: recursive_directory_iterator(argv[0]))
    // {
	// 	std::cout << p.path() << '\n';
	// }

	// std::cout << argv[1] << ": \n";
	// for(const directory_entry & p: recursive_directory_iterator(argv[1]))
    // {
	// 	std::cout << p.path() << '\n';
	// }

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
