#pragma once
#include "Command.h"
#include <vector>
#include <memory>
using std::vector;
using std::unique_ptr;

namespace cmds
{
	class CommandExecutor
	{
	private:
		vector<unique_ptr<Command>> commands;
	public:

		void RegisterCommand(unique_ptr<Command> c);

		CommandResult Execute(const std::string & command, int argc, const char * argv[]);
	};
}
