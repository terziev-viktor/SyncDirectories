#pragma once
#include "Command.h"

#include <vector>
using std::vector;

namespace cmds
{
	class CommandExecutor
	{
	private:
		vector<Command*> commands;
	public:
		CommandExecutor();

		~CommandExecutor();

		void RegisterCommand(Command *);

		CommandResult Execute(const std::string & command, int argc, const char * argv[]);
	};
}
