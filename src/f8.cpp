// f8.cpp
//

#include "f8.h"
#include <iostream>
#include <stdexcept>
#include <csignal>

#define VER 0.2
#define COPYRIGHT 2024

using namespace std;

f8::AtomPtr environment = f8::make_env (); // global to support interrupt

const std::string currentDateTime () {
	time_t     now = time (0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime (&now);
	strftime (buf, sizeof(buf), "%Y-%m-%d, %X", &tstruct);
    return buf;
}

void sighandler (int sig) {
    cout << endl << endl;
    cout << "*** interrupted by the user ***" << endl << endl;
	cout << "session restarted at: " << currentDateTime () << endl << endl;
	environment = f8::make_env ();
	cout << ">> "; cout.flush ();
}

int main (int argc, char* argv[]) {
	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGINT, &sighandler);

	try {
		if (argc == 1) {
			cout << BOLDWHITE << "[f8 \"fate\", ver. " << VER  << "]"
				<< RESET << endl << endl;
			cout << "(c) " << COPYRIGHT  << ", www.carminecella.com" << endl << endl;
			while (true) {
				repl (environment,  cin, cout);	
			}
			
		}
		else {
			for (unsigned int i = 1; i < argc; ++i) {
				load (argv[i], environment);
			}
		}
	} catch (f8::AtomPtr& e) {
		cout << RED << "exception: "; f8::print (e, cout) << RESET << std::endl;
	} catch (exception& e) {
		cout << RED << "error: " << e.what () << RESET << std::endl;
	} catch (...) {
		cout << RED << "fatal unknown error" << RESET << std::endl;
	}
	
	return 0;
}

// eof

