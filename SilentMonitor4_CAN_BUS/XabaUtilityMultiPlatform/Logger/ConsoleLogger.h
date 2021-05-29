#pragma once
#include "ILogSink.h"

namespace Xaba {
	class ConsoleLogger : public ILogSink
	{
	public:
		void Info(const std::string& message) override;
		void Error(const std::string& message) override;
	};
}
