#include "pch.h"
#include <optional>
#include <thread>
#include <memory>
#include "ThreadManager.h"
using namespace Xaba;
struct TMTest {
	bool _fetchCalled{ false };

	std::optional<std::chrono::milliseconds> _startWait{ std::nullopt };
	std::optional<bool> _result{ std::nullopt };
	
	bool Fetch() {
		if (_startWait.has_value()) {
			std::this_thread::sleep_for(_startWait.value());
		}
		
		_fetchCalled = true;

		if (_result.has_value()) return _result.value();
	}
};

TEST(ThreadManager, StartStop) {
	const std::shared_ptr<TMTest> inst = std::make_shared<TMTest>();
	ThreadManager<TMTest> t(inst);
	EXPECT_FALSE(inst->_fetchCalled);
	t.Start();
	while(!inst->_fetchCalled){}
	t.Stop();
	while (!t.IsFinished()){};
	
	EXPECT_TRUE(inst->_fetchCalled);
}

TEST(ThreadManager, GetInstance) {
	const std::shared_ptr<TMTest> inst = std::make_shared<TMTest>();
	ThreadManager<TMTest> t(inst);

	EXPECT_EQ(t.GetInstance(), inst);
}

TEST(ThreadManager, MoveWorks) {
	const std::shared_ptr<TMTest> inst = std::make_shared<TMTest>();
	ThreadManager<TMTest> t(inst);
	ThreadManager<TMTest> t2 = std::move(t);
	t2.Start();
	while (!t2.GetInstance()->_fetchCalled) {}

	t2.Stop();
	while (!t2.IsFinished()) {};
	EXPECT_TRUE(inst->_fetchCalled);
}

TEST(ThreadManager, MoveWorksOnTheSameInstance) {
	const std::shared_ptr<TMTest> inst = std::make_shared<TMTest>();
	ThreadManager<TMTest> t(inst);
	t = std::move(t);
	t.Start();
	while (!t.GetInstance()->_fetchCalled) {}
	t.Stop();
	EXPECT_TRUE(inst->_fetchCalled);
}

TEST(ThreadManager, FetchReturnTrue_TMIsStopped) {
	const std::shared_ptr<TMTest> inst = std::make_shared<TMTest>();
	inst->_result = true;
	ThreadManager<TMTest> t(inst);
	t.Start();
	while (!t.IsFinished()) {};
	EXPECT_TRUE(t.IsFinished());
}