#pragma once
#include "Command.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "HashValue.h"

namespace cmds
{
	class Copy : public Command
	{
	private:
		std::unordered_map<std::string, std::vector<HashValue>> files;
	public:
		Copy();

		CommandResult LoadFiles();

		// Inherited via Command
		CommandResult Execute(int argc, const char * argv[]) override;

		static std::unique_ptr<Command> Create();
	};
}
