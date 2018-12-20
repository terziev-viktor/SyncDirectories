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

CommandResult cmds::Help::Execute(int argc, char * argv[]) const
{
	std::cout << "SyncDirectories.exe: Syncing 2 directories\n-Available commands:\n";
	std::cout << "\thelp - Display information about the tool and all available commands\n";
	std::cout << "\tanalyze [<args>] <left> <right> - Iterate and analyze 2 directories <left> and <right>\n";

	return CommandResult() =
	{
		true, 
		""
	};
}

const Command * cmds::Help::Create()
{
	return new const Help();
}
