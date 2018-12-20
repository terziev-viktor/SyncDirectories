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
		virtual CommandResult Execute(int argc, char * argv[]) const override;

		static const Command * Create();
	};
}

