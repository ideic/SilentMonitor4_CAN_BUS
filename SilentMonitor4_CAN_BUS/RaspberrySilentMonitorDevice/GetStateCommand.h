#pragma once
#include <string>
#include "StateCommandBase.h"
class GetStateCommand : protected StateCommandBase
{
public:
	std::string GetResponse();
};

