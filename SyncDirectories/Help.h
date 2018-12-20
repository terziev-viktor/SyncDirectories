#pragma once
#include "Command.h"

namespace cmds
{
	class Help : public Command
	{
	public:
		Help();
		~Help();

		// Inherited via Command
		virtual CommandResult Execute(int argc, char * argv[]) const override;

		static const Command * Create();
	};
}
