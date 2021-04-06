#pragma once
#include <string>
class StateCommandBase
{
protected:
	std::string GetResponse(const std::string &originalResponse);
};

