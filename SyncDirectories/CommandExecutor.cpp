#include "CommandExecutor.h"

using cmds::CommandExecutor;
using cmds::CommandResult;

void cmds::CommandExecutor::RegisterCommand(unique_ptr<Command> c)
{
	this->commands.push_back(std::move(c));
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
	return CommandResult
	{
		false,
		command + " is not a command. Type sync.exe help for more details on the usage of the program."
	};
}
