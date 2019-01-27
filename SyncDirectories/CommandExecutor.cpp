#include "CommandExecutor.h"
#include "Analyze.h"
#include "Help.h"
#include "Perform.h"

using cmds::CommandExecutor;
using cmds::CommandResult;

CommandExecutor::CommandExecutor()
{
	this->RegisterCommand(Analyze::Create());
	this->RegisterCommand(Help::Create());
	this->RegisterCommand(Perform::Create());
}

CommandExecutor::~CommandExecutor()
{
	for (size_t i = 0; i < this->commands.size(); ++i)
	{
		delete this->commands[i];
	}
}

void cmds::CommandExecutor::RegisterCommand(const Command * c)
{
	this->commands.push_back(c);
}

CommandResult cmds::CommandExecutor::Execute(const std::string & command, int argc, char * argv[]) const
{
	for (size_t i = 0; i < this->commands.size(); ++i)
	{
		if (commands[i]->GetTrigger() == command)
		{
			return commands[i]->Execute(argc, argv);
		}
	}
	return CommandResult() =
	{
		false,
		"This is not a command. type sync.exe help for more details on the usage of the program"
	};
}
