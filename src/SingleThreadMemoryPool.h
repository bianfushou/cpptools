#pragma once

#include<vector>
#include<list>
#include<mutex>

namespace DST {
	struct MemoryBlock {
		static MemoryBlock createMemoryBlock(size_t size) {
			
			void* ptr = malloc(size);
			if (ptr == nullptr) {
				abort();
			}
			return MemoryBlock(ptr, size);
		}

		static void freeMemoryBlock(MemoryBlock& block) {
			if (block.ptr != nullptr) {
				free(block.ptr);
				block.ptr = nullptr;
			}
			block.pos = 0;
			block.size = 0;
		}
		MemoryBlock(void* ptr, size_t size):ptr(ptr), pos(0),size(size){}

		~MemoryBlock() {
		}

		void* ptr;
		size_t pos = 0;
		size_t size;
	};
	class MemoryPool {
	public:
		MemoryPool() {
		}

		void init() {
			if (!isInit) {
				freePool();
				blockList.push_back(MemoryBlock::createMemoryBlock(perBlockSize));
				blockIter = blockList.begin();
				isInit = true;
			}
		}
		void* alloc(size_t size) {
			return memAlloc(size, blockIter, blockList);
		}

		void* allocTempMemory(size_t size) {
			if (tempblockList.empty()) {
				tempblockList.push_back(MemoryBlock::createMemoryBlock(perBlockSize));
				tempBlockIter = tempblockList.begin();
			}
			return memAlloc(size, tempBlockIter, tempblockList);
		}

		void freePool() {
			for (auto iter = blockList.begin(); iter != blockList.end(); iter++) {
				MemoryBlock::freeMemoryBlock(*iter);
			}
			blockList.clear();

			freeTempPool();

			isInit = false;
		}

		void resetPool() {
			for (auto iter = blockList.begin(); iter != blockList.end(); iter++) {
				iter->pos = 0;
			}
			blockIter = blockList.begin();

			freeTempPool();
		}

		void freeTempPool() {
			for (auto iter = tempblockList.begin(); iter != tempblockList.end(); iter++) {
				MemoryBlock::freeMemoryBlock(*iter);
			}
			tempblockList.clear();
		}

		void resetTempPool() {
			for (auto iter = tempblockList.begin(); iter != tempblockList.end(); iter++) {
				iter->pos = 0;
			}
			tempBlockIter = tempblockList.begin();
		}

		~MemoryPool() {
			freePool();
		}

		size_t getPerBlockSize() {
			return perBlockSize;
		}
	private:
		void* memAlloc(int size, std::list<MemoryBlock>::iterator& memIter, std::list<MemoryBlock>& memBlockList) {
			if (size > perBlockSize) {
				return nullptr;
			}
			MemoryBlock& block = *memIter;
			void* ptr = nullptr;
			if (block.size - block.pos < size) {
				memIter++;
				if (memIter == memBlockList.end()) {
					MemoryBlock nBlock = MemoryBlock::createMemoryBlock(perBlockSize);
					ptr = nBlock.ptr;
					nBlock.pos = size;
					memBlockList.push_back(nBlock);
					memIter = memBlockList.end();
					memIter--;
				}
				else {
					MemoryBlock& nBlock = *memIter;
					ptr = nBlock.ptr;
					nBlock.pos = size;
				}
			}
			else {
				ptr = (void *)((size_t)block.ptr + block.pos);
				block.pos += size;
			}
			return ptr;
		}

		std::list<MemoryBlock> blockList;
		std::list<MemoryBlock> tempblockList;

		std::list<MemoryBlock>::iterator blockIter;
		std::list<MemoryBlock>::iterator tempBlockIter;
		bool isInit = false;
		size_t perBlockSize = 1024*64;
	};