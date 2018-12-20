#include "Command.h"
using cmds::Command;

Command::Command(const std::string & trigger)
{
	this->trigger = trigger;
}

Command::~Command()
{
}

const std::string & cmds::Command::GetTrigger() const
{
	return this->trigger;
}
