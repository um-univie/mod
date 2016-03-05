#include <mod/VM/VM.h>

#include <iostream>
#include <sstream>

int main(int argc, char **argv) {
	std::stringstream baseData;
	baseData << mod::VM::VM::getStdPrelude();
	baseData << mod::VM::VM::argsToCode(argc, argv);
	if(argc > 1) {
		for(int i = 1; i < argc; i++)
			std::cout << "Argument " << i << " ('" << argv[i] << "') can not be recognized." << std::endl;
		return 1;
	}

	mod::VM::VM vm;
	bool res = vm.execute(baseData.str());
	if(!res) return 1;
	return 0;
}
