#pragma once
#include "Command.h"

namespace cmds
{
	class Analyze : public Command
	{
	private:
		CommandResult Mirror(int, const char * argv[]) const;

		CommandResult Safe(int, const char * argv[]) const;

		CommandResult Standard(int, const char * argv[]) const;

		CommandResult ReadOptions(int, const char * argv[], size_t &) const;

	public:
		Analyze();

		~Analyze();

		// Inherited via Command
		CommandResult Execute(int, const char * argv[]) override;

		static std::unique_ptr<Command> Create();
	};
}
