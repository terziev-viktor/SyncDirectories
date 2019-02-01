#pragma once
#include "Command.h"

namespace cmds
{

	class DELETE : public Command
	{
	public:
		DELETE();

		// Inherited via Command
		CommandResult Execute(int argc, const char * argv[]) override;

		static Command * Create();
	};
}
