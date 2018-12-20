#pragma once
#include "Command.h"

namespace cmds
{
	class Analyze : public Command
	{
	public:
		Analyze();
		~Analyze();

		// Inherited via Command
		virtual CommandResult Execute(int, char * argv[]) const override;

		static const Command * Create();
	};
}
