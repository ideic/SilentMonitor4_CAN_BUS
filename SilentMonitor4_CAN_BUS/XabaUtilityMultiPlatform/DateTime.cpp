#include "DateTime.h"
#include <sstream>
#include <iomanip>

using namespace std::string_literals;
using namespace std::chrono;
using namespace Xaba;
#ifndef WIN32

#endif // !WIN32

class StringFormat{
	std::string _raw;
public:
	StringFormat(std::string from) : _raw(std::move(from)) {};
	std::string Digit(uint8_t digit) {
		std::ostringstream oss;
		oss << std::setw(digit) << std::setfill('0') << _raw;
		return oss.str();
	}
};
StringFormat string(const std::string& value) {
	return StringFormat(value);
}

StringFormat string(int value) {
	return StringFormat(std::to_string(value));
}

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
		string(utc_tm.tm_mon + 1).Digit(2) + "-"s +
		string(utc_tm.tm_mday).Digit(2) + " "s +
		string(utc_tm.tm_hour).Digit(2) + ":"s +
		string(utc_tm.tm_min).Digit(2) + ":"s +
		string(utc_tm.tm_sec).Digit(2) + "."s +
		string(std::chrono::duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000).Digit(3);
}

std::string DateTime::DateString() {
	auto now = _timeValue;

	auto timet = std::chrono::system_clock::to_time_t(now);

	tm utc_tm = *gmtime(&timet);

	return  std::to_string(utc_tm.tm_year + 1900) + "-"s +
		string(utc_tm.tm_mon + 1).Digit(2) + "-"s +
		string(utc_tm.tm_mday).Digit(2);

}

std::ostream& operator<<(std::ostream& os, const Xaba::DateTime& dt)
{
	os << dt.to_string();
	return os;
}
