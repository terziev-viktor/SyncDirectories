#pragma once
#include "Command.h"

#include <vector>
using std::vector;

namespace cmds
{
	class CommandExecutor
	{
	private:
		vector<const Command*> commands;
	public:
		CommandExecutor();

		~CommandExecutor();

		void RegisterCommand(const Command *);

		CommandResult Execute(const std::string & command, int argc, char * argv[]) const;
	};
}
