#include "GetStateCommand.h"
#include "JSON.hpp"
#include <sstream>
using Value = Qentem::Value<char>;

std::string GetStateCommand::GetResponse()
{
	Value j;
	j["Status"] = "FAILED";
	j["CANBus"] = "DisConnected";
	j["ErrorMessage"] = "Device is not configured";

	std::stringstream osstream;
	osstream << j.Stringify();

	return StateCommandBase::GetResponse(osstream.str());
}
