#pragma once
#include <thread>
#include <string>
#include <sstream>
#include "Logger/Logger.h"
#include <utility>
#include <atomic>
#include <memory>
#include <mutex>
namespace Xaba {
	using namespace std::chrono_literals;
	using namespace std::string_literals;
	template<class T> class ThreadManager {
	private:
		std::thread _thread{};
		std::shared_ptr<T> _instance{ nullptr };
		std::mutex _lock;
		bool _finished{ false };
		bool _stopped{ false };

	public:
		ThreadManager<T>(const ThreadManager<T>& from) = delete;
		ThreadManager<T>& operator=(const ThreadManager<T>& from) = delete;

		ThreadManager<T>(ThreadManager<T>&& from) noexcept {
			if (_instance == from._instance) return;
			_thread = std::move(from._thread);
			_instance = std::move(from._instance);
			_finished = from._finished;
			_stopped = from._stopped;
		};
		ThreadManager<T>& operator=(ThreadManager<T>&& from) noexcept {
			if (_instance == from._instance) return *this;

			_thread = std::move(from._thread);
			_instance = std::move(from._instance);
			_finished = from._finished ;
			_stopped = from._stopped ;

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
			ss << std::this_thread::get_id();

			Logger::Info("Start Thread from "s + ss.str());
			_thread = std::thread([this]() {
				try {
					std::stringstream ss{};
					ss << std::this_thread::get_id();
					{
						std::lock_guard l(_lock);
						_finished = false;
					}
					
					Logger::Info("New Thread started: " + ss.str());
					
					while (true){ 
						{
							std::lock_guard l(_lock);
							if (_stopped) break;
						}
						if (_instance->Fetch()) {
							break;
						};
					}
					Logger::Info(" Thread stopped" + ss.str());
				}
				catch (const std::exception& ex) {
					Logger::Error("Error during thread running :"s + ex.what());
				}
				{
					std::lock_guard l(_lock);
					_finished = true;
				}
				
			});
		};

		void Stop() {
			std::lock_guard l(_lock);
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
			std::lock_guard l(_lock);
			return _finished;
		}
	};
}
