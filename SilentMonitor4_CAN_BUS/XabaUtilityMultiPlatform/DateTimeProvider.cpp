#include "DateTimeProvider.h"

std::shared_ptr<DateTime> DateTimeProvider::DateTme()
{
	static std::shared_ptr<DateTime>  dt = std::make_shared<DateTime>();
	return dt;
}
