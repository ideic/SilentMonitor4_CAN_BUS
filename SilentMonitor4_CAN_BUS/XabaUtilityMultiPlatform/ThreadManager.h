#pragma once
#include <chrono>
#include <thread>
#include <string>
#include <sstream>
#include "Logger/Logger.h"
#include <utility>

namespace Xaba {
	using namespace std::chrono_literals;
	using namespace std::string_literals;
	template<class T> class ThreadManager {
	private:
		std::thread _thread{};
		std::shared_ptr<T> _instance{ nullptr };
		std::atomic<bool> _finished{ false };
		std::atomic<bool> _stopped{ false };

	public:
		ThreadManager<T>(const ThreadManager<T>& from) = delete;
		ThreadManager<T>& operator=(const ThreadManager<T>& from) = delete;

		ThreadManager<T>(ThreadManager<T>&& from) noexcept {
			if (_instance == from._instance) return;
			_thread = std::move(from._thread);
			_instance = std::move(from._instance);
			_finished = from._finished ? true : false;
			_stopped = from._stopped ? true : false;
		};
		ThreadManager<T>& operator=(ThreadManager<T>&& from) noexcept {
			if (_instance == from._instance) return *this;

			_thread = std::move(from._thread);
			_instance = std::move(from._instance);
			_finished = from._finished ? true : false;
			_stopped = from._stopped ? true : false;

			return *this;
		};

		ThreadManager(std::shared_ptr<T> instance): _instance(std::move(instance)) {
		}

		~ThreadManager() {
			if (_thread.joinable()) {
				Stop();
				_thread.join();
			}
		}
		void Start() {
			std::stringstream ss{};
			ss << _thread.get_id();

			Logger::Info("Start Thread from "s + ss.str());
			_thread = std::thread([this]() {
				try {
					std::stringstream ss{};
					ss << _thread.get_id();

					_finished = false;
					Logger::Info("New Thread started: " + ss.str());

					while (!_stopped) {
						if (_instance->Fetch()) {
							_stopped = true;
						};
					}
					Logger::Info(" Thread stopped" + ss.str());
				}
				catch (const std::exception& ex) {
					Logger::Error("Error during thread running :"s + ex.what());
				}
				_finished = true;
			});
			Logger::Info("std thread id " + ss.str() + "");
		};

		void Stop() {
			_stopped = true;
		};

		void Join() {
			if ( _thread.joinable()) {
				_thread.join();
			}
		}

		const std::shared_ptr<T>& GetInstance() {
			return _instance;
		}

		bool IsFinished() {
			return _finished;
		}
	};
}
