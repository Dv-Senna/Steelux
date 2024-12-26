#include "test.hpp"

#include <iostream>


namespace sl {
	void sayHello() {
		#ifdef SL_WINDOWS
			std::cout << "Hello from Windows testlib !" << std::endl;
		#elifdef SL_LINUX
			std::cout << "Hello from Linux testlib !" << std::endl;
		#elifdef SL_APPLE
			std::cout << "Hello from Apple testlib !" << std::endl;
		#endif
	}
}
