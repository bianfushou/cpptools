#include <unordered_map>
#include <thread>
#include <mutex>
#include <vector>

class MemoryPool {
	public:
		void pool_free(void* ptr) {
			std::lock_guard<std::mutex> guard(mut);
			auto it = usedMemMap.find(ptr);
			if (it != usedMemMap.end()) {
				pool[it->second.first].second += it->second.second;
				if (pool[it->second.first].second == pool[it->second.first].first) {
					memList.emplace_back(it->second.first, std::pair<void*, size_t>(it->second.first, pool[it->second.first].second));
				}
				usedMemMap.erase(it);
			}
			else {
				if(ptr != nullptr)
					free(ptr);
			}
		}

		void* pool_malloc(size_t size) {
			if (size > maxLimit || size < minLimit) {
				return malloc(size);
			}
			std::lock_guard<std::mutex> lock(mut);
			void* ptr = nullptr;
			std::vector<std::pair<void *, std::pair<void*, size_t>>> freeMemList;
			if (!memList.empty()) {
				for (int i = memList.size() - 1; i >= 0; i--) {
					if (memList[i].second.second >= size) {
						ptr = memList[i].first;
						memList[i].first = (char*)memList[i].first + size;
						pool[memList[i].second.first].second -= size;
						memList[i].second.second -= size;

						usedMemMap.emplace(ptr, std::pair<void*, size_t>(memList[i].second.first, size));
						if (memList[i].second.second == 0) {
							memList.pop_back();
						}
						break;
					}
					else {
						pool[memList[i].second.first].second += memList[i].second.second;
						if (pool[memList[i].second.first].second == pool[memList[i].second.first].first) {
							freeMemList.emplace_back(memList[i].second.first, std::pair<void*, size_t>(memList[i].second.first, pool[memList[i].second.first].second));
						}
						memList.pop_back();
					}
				}
			}
			if (ptr == nullptr) {
				size_t realSize = 12 * size;
				void* mem = malloc(realSize);
				if (mem != nullptr) {
					pool[mem] = std::pair<size_t, size_t>(realSize, realSize - size);
					memList.emplace_back((char*)mem + size, std::pair<void*, size_t>(mem, realSize - size));
					
					ptr = mem;
					usedMemMap.emplace(ptr, std::pair<void*, size_t>(mem, size));
				}
			}
			if (!freeMemList.empty()) {
				memList.insert(memList.end(), freeMemList.begin(), freeMemList.end());
			}
			return ptr;
			
		}

		~MemoryPool() {
			for (auto ptr: pool) {
				free(ptr.first);
			}
		}
	private:
		std::unordered_map<void*, std::pair<size_t, size_t>> pool;
		std::unordered_map<void*, std::pair<void*, size_t>> usedMemMap;
		std::vector<std::pair<void *,std::pair<void*, size_t>>> memList;
		size_t maxLimit = 1024 * 1024 * 4;
		size_t minLimit = 1000;
		std::mutex mut;
	};