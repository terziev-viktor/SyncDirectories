#include "Perform.h"
using namespace cmds;


Perform::Perform()
	:Command("perform")
{

}


Perform::~Perform()
{
}

CommandResult cmds::Perform::Execute(int argc, char * argv[]) const
{
	return CommandResult() =
	{
		false,
		"!! Not implemented !!"
	};
}

const Command * cmds::Perform::Create()
{
	return new const Perform();
}
