#pragma once
#include<thread>
#include <mutex>
#include <condition_variable>
#include<functional>
#include<vector>
#include<atomic>

class ThreadGuard {
public:
	ThreadGuard() {};
	void start() {
		if (threadContainer.size() == 0) {
			threadContainer.emplace_back([this] {run(); });
		}
	}
	void setTask(std::function<void()>&& task) {
		std::unique_lock<std::mutex> lock(mut);
		hasTask = true;
		this->task = std::move(task);
		finished = false;
		cv.notify_all();
	}

	void waitFinished() {
		std::unique_lock<std::mutex> lock(mut);
		cv.wait(lock, [this] {return finished; });
	}
	void stop() {
		if(!isStop.load())
		{
			std::unique_lock<std::mutex> guard(mut);
			hasTask = true;
			finished = false;
			task = [this]() { isStop.store(true); };
			cv.notify_all();
		}
		if (threadContainer.size() > 0) {
			threadContainer[0].join();
			threadContainer.pop_back();
		}
	}
	~ThreadGuard() {
		stop();
	}
private:
	void run() {
		while (!isStop.load()) {
			std::unique_lock<std::mutex> lock(mut);
			if (hasTask) {
				task();
				hasTask = false;
				finished = true;
				cv.notify_all();
			}
			else {
				cv.wait(lock, [this] {return hasTask; });
			}
		}
	}
	bool hasTask = false;
	std::atomic<bool> isStop = false;
	bool finished = true;
	std::vector<std::thread> threadContainer;
	std::function<void()> task;
	std::condition_variable cv;
	std::mutex mut;

};