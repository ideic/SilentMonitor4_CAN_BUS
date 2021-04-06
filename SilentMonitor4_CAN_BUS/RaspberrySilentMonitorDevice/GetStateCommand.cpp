#include "GetStateCommand.h"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
std::string GetStateCommand::GetResponse()
{
	json j;
	j["Status"] = "FAILED";
	j["CANBus"] = "DisConnected";
	j["ErrorMessage"] = "Device is not configured";
	return StateCommandBase::GetResponse( j.dump());
}
