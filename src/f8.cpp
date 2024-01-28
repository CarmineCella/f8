// f8.cpp
//

#include "f8.h"
#include "system.h"
#include "numeric.h"

#include <iostream>
#include <stdexcept>

#define VER 0.2
#define COPYRIGHT 2024

using namespace std;

int main (int argc, char* argv[]) {
	AtomPtr environment = add_system (add_core (make_env ()));
	try {
		if (argc == 1) {
			cout << BOLDWHITE << "[f8 (\"fate\"), ver. " << VER  << "]"
				<< RESET << endl << endl;
			cout << "(c) " << COPYRIGHT  << ", www.carminecella.com" << endl << endl;
			repl (environment,  cin, cout);	
		}
		else {
			for (unsigned int i = 1; i < argc; ++i) {
				load (argv[i], environment);
			}
		}
	} catch (AtomPtr& e) {
		cout << RED << "exception: "; print (e, cout) << RESET << std::endl;
	} catch (exception& e) {
		cout << RED << "error: " << e.what () << RESET << std::endl;
	} catch (...) {
		cout << RED << "fatal unknown error" << RESET << std::endl;
	}
	
	return 0;
}

// eof

