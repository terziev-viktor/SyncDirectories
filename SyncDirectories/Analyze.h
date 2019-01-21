#pragma once
#include "Command.h"

namespace cmds
{
	class Analyze : public Command
	{
		CommandResult Mirror(char * argv[]) const;

		CommandResult Safe(char * argv[]) const;

		CommandResult Standard(char * argv[]) const;

	public:
		Analyze();
		~Analyze();

		// Inherited via Command
		virtual CommandResult Execute(int, char * argv[]) const override;

		static const Command * Create();
	};
}
