#pragma once
#include "Command.h"
#include "PerformCommandExecutor.h"

namespace cmds
{
	class Perform : public Command
	{
	private:
		PerformCommandExecutor executor;
	public:
		Perform();

		// Inherited via Command
		CommandResult Execute(int argc, const char * argv[]) override;

		static std::unique_ptr<Command> Create();
	};
}

