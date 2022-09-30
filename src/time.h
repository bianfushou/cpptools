#include <chrono>
#include<iostream>
namespace bath{
#define TICK_TOC(a, b) std::cout << (b) << ": "<<a.count() << std::endl;
class time{
private :
	chrono::time_point<steady_clock> start = std::chrono::steady_clock::now();;
public:
    long long count(){
		auto now = std::chrono::steady_clock::now();
		auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
		return now.count();
	}
};
}