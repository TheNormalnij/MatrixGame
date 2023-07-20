
#pragma once

#include "IGameCommand.h"

class CGameNetwork {

	void SendCommands(std::list<IGameCommand *> commands);
};