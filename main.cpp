#include "minimizer.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
	// validate command line usage
	if (argc != 2) {
		cerr << "Usage: ./minimize [n-bit string]" << endl;
		return 1;
	}

    try {
        Minimizer m = Minimizer(argv[1]); 
        cout << m.minimize() << endl;
    } catch (const runtime_error& err) {
        cout << err.what() << endl;
        return 1;
    }

    return 0;
}
