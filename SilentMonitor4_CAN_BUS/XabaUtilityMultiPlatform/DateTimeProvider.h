#pragma once
#include <memory>
#include "DateTime.h"
namespace Xaba {
	class DateTimeProvider
	{
	public:
		static std::shared_ptr<DateTime> DateTme();
	};

}