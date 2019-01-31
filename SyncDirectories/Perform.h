#pragma once
#include "Command.h"

namespace cmds
{
	class Perform : public Command
	{
	public:
		Perform();
		~Perform();

		// Inherited via Command
		virtual CommandResult Execute(int argc, const char * argv[]) override;

		static Command * Create();
	};
}

