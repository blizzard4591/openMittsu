#include <memory>
#include <cstddef>
#include <cstdint>
#include <iostream>

class Example {
public:
	Example(int i) : _i(i) {}
	
	int get() const { return _i; }
private:
	Example() : _i(-1) {}
	int _i;
};

int main(int argc, char** argv) {
	std::unique_ptr<Example> myUniquePtr = nullptr;
	
	myUniquePtr = std::make_unique<Example>(42);
	
	if (myUniquePtr->get() == 42) {
		return 0;
	} else {
		return 1;
	}
}
