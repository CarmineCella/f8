// f8.cpp
//

#include "f8.h"
#include <iostream>
#include <stdexcept>

using namespace std;

int main (int argc, char* argv[]) {
	srand (time (NULL));
	f8::AtomPtr environment = f8::make_env ();
	try {
		bool interactive = false;
		int opt = 0;
		while ((opt = getopt(argc, argv, "i")) != -1) {
		    switch (opt) {
		    case 'i': interactive = true; break;
		    default:
		        std::stringstream msg;
		        msg << "usage is " << argv[0] << " [-i] [file...]";
		        throw runtime_error (msg.str ());
		    }
		}

		if (argc - optind == 0) {
			cout << BOLDBLUE << "[f8 \"fate\", version "
				<< VERSION <<"]" << RESET << endl << endl;

			cout << "scripting language" << endl;
			cout << "(c) " << COPYRIGHT << ", www.carminecella.com" << endl << endl;

			f8::repl (environment, cin, cout);
		} else {
			for (int i = optind; i < argc; ++i) {
				if (!f8::source (argv[i], environment)) {
					std::cout << "warning: cannot open input file " << argv[i] << std::endl;
				}
			}
			if (interactive) f8::repl (environment, cin, cout);
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

