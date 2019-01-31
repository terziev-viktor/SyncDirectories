#pragma once
#include "Command.h"
namespace cmds
{
	class Move : public Command
	{
	public:
		Move();

		// Inherited via Command
		virtual CommandResult Execute(int argc, const char * argv[]) override;

		static Command * Create();
	};
}