#include "SyncCommandExecutor.h"
#include "Analyze.h"
#include "Help.h"
#include "Perform.h"


SyncCommandExecutor::SyncCommandExecutor()
{
	this->RegisterCommand(cmds::Analyze::Create());
	this->RegisterCommand(cmds::Help::Create());
	this->RegisterCommand(cmds::Perform::Create());
}

