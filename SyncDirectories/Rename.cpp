#include "Rename.h"
using namespace cmds;


Rename::Rename()
	:Command("RENAME")
{
}

CommandResult cmds::Rename::Execute(int argc, const char * argv[])
{
	return mv.Execute(argc, argv);
}

cmds::Command * cmds::Rename::Create()
{
	return new Rename();
}
