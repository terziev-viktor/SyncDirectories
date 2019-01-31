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
		virtual CommandResult Execute(int argc, const char * argv[]) override;

		static Command * Create();
	};
}
