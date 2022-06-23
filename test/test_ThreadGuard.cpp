#include "../src/ThreadGuard.h"
#include<iostream>

int count[10];
void run(int i) {
	if (i < 0 || i > 9) {
		return;
	}
	int m = 1;
	for (int k = 0; k < i; k++) {
		m *= (k+1);
	}
	count[i] = m;
}
int main() {
	ThreadGuard tg[10];
	for (int i = 0; i < 10; i++) {
		tg[i].start();
	}
	for (int i = 0; i < 10; i++) {
		tg[i].setTask([i]() {run(i); });
	}
	for (int i = 0; i < 10; i++) {
		tg[i].waitFinished();
		std::cout << count[i] << std::endl;
	}
	return 0;
}