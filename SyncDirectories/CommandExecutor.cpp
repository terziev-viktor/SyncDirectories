#include "CommandExecutor.h"

using cmds::CommandExecutor;
using cmds::CommandResult;

CommandExecutor::CommandExecutor()
{

}

CommandExecutor::~CommandExecutor()
{
	for (size_t i = 0; i < this->commands.size(); ++i)
	{
		delete this->commands[i];
	}
}

void cmds::CommandExecutor::RegisterCommand(Command * c)
{
	this->commands.push_back(c);
}

CommandResult cmds::CommandExecutor::Execute(const std::string & command, int argc, const char * argv[])
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
		"This is not a command. Type sync.exe help for more details on the usage of the program."
	};
}
