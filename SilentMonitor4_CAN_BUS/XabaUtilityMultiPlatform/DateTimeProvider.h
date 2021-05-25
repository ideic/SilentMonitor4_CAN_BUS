#pragma once
#include <memory>
#include <DateTime.h>

class DateTimeProvider
{
public:
	static std::shared_ptr<DateTime> DateTme();
};

