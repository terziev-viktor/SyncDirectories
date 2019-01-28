#pragma once
#include "Command.h"

namespace cmds
{
	class Analyze : public Command
	{
	private:
		CommandResult Mirror(int, char * argv[]) const;

		CommandResult Safe(int, char * argv[]) const;

		CommandResult Standard(int, char * argv[]) const;

		CommandResult ReadOptions(int, char * argv[], size_t &) const;

	public:
		Analyze();

		~Analyze();

		// Inherited via Command
		virtual CommandResult Execute(int, char * argv[]) const override;

		static const Command * Create();
	};
}
