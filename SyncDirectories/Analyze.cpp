#include "Analyze.h"
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

CommandResult Analyze::Execute(int argc, char * argv[]) const
{
	std::cout << "This command will analyze these directories: " << std::endl;
	for (int i = 0; i < argc; ++i)
	{
		std::cout << argv[i] << std::endl;
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
