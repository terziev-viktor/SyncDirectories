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
		virtual CommandResult Execute(int argc, const char * argv[]) override;

		static Command * Create();
	};
}
