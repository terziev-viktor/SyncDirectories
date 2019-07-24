#pragma once
#include "CommandExecutor.h"
using cmds::CommandExecutor;

class SyncCommandExecutor : public CommandExecutor
{
public:
	SyncCommandExecutor();
};

