#pragma once
#include<functional>
#include<string>
#include<map>
#include<list>
#include<mutex>
#include<condition_variable>
#include<chrono>
#include<memory>
namespace abs {
	struct Event {
		int code;
		std::string name;
	};
	class Bus;
	class Subscriber :std::enable_shared_from_this<Subscriber>{
	private:

		std::condition_variable cv;
		std::mutex mut;
		std::shared_ptr<Event> ev;
		std::shared_ptr<Bus> bus;
		std::function<void(Event*)> handle;
		int code;
	public:
		Subscriber(std::shared_ptr<Bus> bus, int code, bool is_once = true):bus(bus), code(code), is_once(is_once){
		}
		~Subscriber() {
		}
		const bool is_once;
		void wait_for(long long s){
			std::unique_lock<std::mutex> uq(mut);
			if (!ev) {
				cv.wait_for(uq, std::chrono::milliseconds(s));
			}
			if(ev)
			{
				handle(ev.get());
				ev = nullptr;
			}
			
		}
		void notify(std::shared_ptr<Event>& ev) {
			{
				std::lock_guard<std::mutex> guard(mut);
				this->ev = ev;
			}
			cv.notify_one();
		}

		void subscribe() {
			bus->addSubscriber(code, this->shared_from_this());
		}
	};

	class Bus {
	private:
		std::map<int, std::list<std::weak_ptr<Subscriber>>> subscribersMap;
		std::mutex mut;
	public:
		void emit(std::shared_ptr<Event>& ev) {
			std::lock_guard<std::mutex> guard(mut);
			auto it = subscribersMap.find(ev->code);
			if (it != subscribersMap.end()) {
				for (auto subit = it->second.begin(); subit != it->second.end(); ) {
					auto subscriber = subit->lock();
					if (subscriber) {
						subscriber->notify(ev);
					}
					subit = it->second.erase(subit);
				}
			}
		}
		void addSubscriber(int code, std::shared_ptr<Subscriber> subscriber) {
			std::lock_guard<std::mutex> guard(mut);
			auto it = subscribersMap.find(code);
			if (it == subscribersMap.end()) {
				subscribersMap.emplace(code, std::list<std::weak_ptr<Subscriber>>());
			}
			subscribersMap[code].push_back(subscriber);
		}
	};
}
