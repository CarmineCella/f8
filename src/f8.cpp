// f8.cpp
//

#include "f8.h"
#include <iostream>
#include <stdexcept>

using namespace std;

int main (int argc, char* argv[]) {
	srand (time (NULL));
	try {
		f8::AtomPtr environment = f8::make_env ();
		if (argc == 1) {
			cout << BOLDWHITE << "[f8 \"fate\", ver. " << VERSION  << "]"
				<< RESET << endl << endl;
			cout << "(c) " << COPYRIGHT  << ", www.carminecella.com" << endl << endl;

			f8::repl (environment,  cin, cout);				
		}
		else {
			for (unsigned int i = 1; i < argc; ++i) {
				f8::load (argv[i], environment);
			}
		}
	} catch (f8::AtomPtr& e) {
		cout << RED << "error: "; f8::puts (e, cout) << RESET << std::endl;
	} catch (exception& e) {
		cout << RED << "exception: " << e.what () << RESET << std::endl;
	} catch (...) {
		cout << RED << "fatal unknown error" << RESET << std::endl;
	}
	
	return 0;
}

// eof

