#pragma once
#include <chrono>
#include <string>
#include <ostream>
namespace Xaba {

	class DateTime {
		std::chrono::system_clock::time_point _timeValue{ std::chrono::system_clock::now() };
	public:
		virtual DateTime UtcNow();

		std::string to_string() const;
		std::string DateString();
	};
}

std::ostream& operator<<(std::ostream& os, const Xaba::DateTime& dt);