#pragma once
#include <iostream>
#include <string>

namespace cmds
{
	struct CommandResult
	{
		bool Successful;

		std::string Message;
	};

	class Command
	{
	private:
		std::string trigger;
	public:
		Command(const std::string & trigger)
		{
			this->trigger = trigger;
		}
		virtual ~Command() {}
		
		virtual CommandResult Execute(int argc, char * argv[]) const = 0;

		const std::string & GetTrigger() const
		{
			return this->trigger;
		}
	};

}

