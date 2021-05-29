#include "DateTime.h"
using namespace std::string_literals;
using namespace std::chrono;
using namespace Xaba;
#ifndef WIN32

#endif // !WIN32
DateTime DateTime::UtcNow()
{
	_timeValue = std::chrono::system_clock::now();
	return *this;
}

std::string DateTime::to_string() const
{
	//	timeval curTime;
	auto now = _timeValue;

	auto timet = std::chrono::system_clock::to_time_t(now);

	tm utc_tm = *gmtime(  &timet);

	return  std::to_string(utc_tm.tm_year + 1900) + "-"s +
		std::to_string(utc_tm.tm_mon + 1) + "-"s +
		std::to_string(utc_tm.tm_mday) + " "s +
		std::to_string(utc_tm.tm_hour) + ":"s +
		std::to_string(utc_tm.tm_min) + ":"s +
		std::to_string(utc_tm.tm_sec) + "."s +
		std::to_string(std::chrono::duration_cast<milliseconds>(now.time_since_epoch()).count() / 1000.0);
}

std::ostream& Xaba::operator<<(std::ostream& os, const DateTime& dt)
{
	os << dt.to_string();
	return os;
}

