// f8.cpp
//

#include "f8.h"
#include <iostream>
#include <stdexcept>
#include <valarray>

using namespace std;

int main (int argc, char* argv[]) {
	std::valarray<double> e ({1, 2, 3, 4, 5, 6, 7, 8, 9, 10});
	std::valarray<double> s1 = e[std::slice(0, 20, 2)];
	std::valarray<double> s2 = e[std::slice(1, 20, 2)];
	
	for (double &x: s1) cout << x << " ";
	cout << endl;
	for (double &x: s2) cout << x << " ";
	cout << endl;
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
		cout << RED << "error: "; f8::print (e, cout) << RESET << std::endl;
	} catch (exception& e) {
		cout << RED << "exception: " << e.what () << RESET << std::endl;
	} catch (...) {
		cout << RED << "fatal unknown error" << RESET << std::endl;
	}
	
	return 0;
}

// eof

