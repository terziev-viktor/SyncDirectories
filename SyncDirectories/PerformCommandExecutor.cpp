#include "PerformCommandExecutor.h"
#include "Move.h"
#include "Copy.h"
#include "Rename.h"
#include "Create.h"
#include "DELETE.h"

PerformCommandExecutor::PerformCommandExecutor()
{
	this->RegisterCommand(cmds::Move::Create());
	this->RegisterCommand(cmds::Copy::Create());
	this->RegisterCommand(cmds::Rename::Create());
	this->RegisterCommand(cmds::CREATE::Create());
	this->RegisterCommand(cmds::DELETE::Create());
}
