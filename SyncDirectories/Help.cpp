#include "Help.h"
using cmds::Help;
using cmds::CommandResult;
using cmds::Command;

Help::Help()
	:Command("help")
{

}


Help::~Help()
{
}

CommandResult cmds::Help::Execute(int argc, const char * argv[])
{
	std::cout << "Syncing Two directories\nAvailable commands (all of which are case *sensitive*):\n";
	std::cout << "\thelp - Display information about the tool and all available commands\n\n";
	std::cout << "\tanalyze [standard|safe|mirror] [<args>] <left> <right> - Iterate and analyze 2 directories <left> and <right>\n";
	std::cout << "\t\t[<args>] could be:\n\t\t-hash-only for comparing directories and files using only their hashes,\n";
	std::cout << "\t\t-block for comparing each hash of a block of 64MB for every file,\n";
	std::cout << "\t\tor both in no particular order\n";
	std::cout << "\t\tThe result is stored in a file called sync.txt where the user can edit the results of the analysis\n\n";
	std::cout << "\tperform - reads sync.txt and performs the commands in it\n";

	return CommandResult() =
	{
		true, 
		""
	};
}

Command * cmds::Help::Create()
{
	return new Help();
}
