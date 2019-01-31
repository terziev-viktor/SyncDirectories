#pragma once
#include "Command.h"

namespace cmds
{

	class CREATE : public Command
	{
	public:
		CREATE();

		// Inherited via Command
		virtual CommandResult Execute(int argc, const char * argv[]) override;

		static Command * Create();
	};

}