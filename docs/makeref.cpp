// makeref.cpp

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

using namespace std;

std::string getExtension (const std::string& in) {
	size_t pos = std::string::npos;
	pos = in.find_last_of (".");
	if (pos != std::string::npos) {
		return in.substr (pos, in.size () - pos);
	} else return "";
}

string next(istream& in) {
    string t;
    in >> t;
    return t;
}
void parse (istream& in, ostream& out) {
    while (!in.eof ()) {
        string t = next (in);
        if (t == "///~") {  
            string l;
            getline (in, l);
            out << l << endl;
        }
    }
}

int main (int argc, char* argv[]) {
    cout << "[makeref, ver. " << VERSION  << "]" << endl << endl;
    cout << "(c) " << COPYRIGHT  << ", www.carminecella.com" << endl << endl;

	try {
        if (argc < 2) {
            throw runtime_error ("syntax is 'makedoc out.md file1 file2 ...");
        }

        ofstream out (argv[1]);
        if (!out.good ()) throw runtime_error ("cannot create output file");

        cout << "documenting code..."; cout.flush ();
        for (unsigned int i = 2; i < argc; ++i) {
            ifstream in (argv[i]);
            if (!in.good ()) {
                cout << "cannot open " << argv[1] << "; skipping..." << endl;
            } else {
                parse (in, out);
            }
        }

        cout << "saved to " << argv[1] << endl;
	} catch (exception& e) {
		cout << "error: " << e.what () << std::endl;
	} catch (...) {
		cout << "fatal unknown error" << std::endl;
	}
	
	return 0;
}

// eof

