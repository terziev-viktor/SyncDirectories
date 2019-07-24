#pragma once
#include "Move.h"
namespace cmds
{
	class Rename : public Command
	{
	private:
		Move mv;
	public:
		Rename();

		// Inherited via Command
		CommandResult Execute(int argc, const char * argv[]) override;

		static std::unique_ptr<Command> Create();
	};
}
