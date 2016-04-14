#include <cstdint>
#include <cstddef>
#include <iostream>

int main(int argc, char** argv) {
	int const sizeInBytes = static_cast<int>(sizeof(void*));
	int const sizeInBits = sizeInBytes * 8;
	
	std::cout << sizeInBits;

	return 0;
}
