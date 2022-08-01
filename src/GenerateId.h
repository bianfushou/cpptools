#include <deque>
#include <mutex>

class GenerateId {
public:
	GenerateId(int criticalId = 65536):criticalId(criticalId){
	}
	int generateId() {
		std::lock_guard<std::mutex> guard(lock);
		int id = 0;
		if (nextIndex <= criticalId) {
			id = nextIndex;
			++nextIndex;	
		}
		else if (!unusedIdQue.empty()) {
			id = unusedIdQue.front();
			unusedIdQue.pop_front();
		}
		return id;
	}

	void recycleId(int id) {
		std::lock_guard<std::mutex> guard(lock);
		unusedIdQue.push_back(id);
	}

private:
	int nextIndex = 1;
	int criticalId ;
	std::deque<int> unusedIdQue;
	std::mutex lock;
};