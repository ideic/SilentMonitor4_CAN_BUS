#include "StateCommandBase.h"

std::string StateCommandBase::GetResponse(const std::string& originalResponse)
{
    constexpr uint8_t stringSize = 4;
    auto size = std::to_string(originalResponse.size());
    if (size.size() < stringSize)
        size.insert(0, stringSize - size.size(), ' ');
    return size + originalResponse;
}
