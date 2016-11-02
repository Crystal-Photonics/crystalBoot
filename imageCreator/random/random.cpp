#include <random>
#include <iostream>

char to_hex(int i) {
	if (i < 10) {
		return i + '0';
	}
	return i - 10 + 'A';
}

int main() {
	std::random_device rd;
	if (rd.entropy() > 0.) {
		std::uniform_int_distribution<int> dist(0, 15);
		for (int i = 0; i < 2 * 16; i++) {
			std::cout << to_hex(dist(rd));
		}
	}
}

