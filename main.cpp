/* Author: Ng Wei Jian */
#include <iostream>
#include "errors.h"
#include "enigma.h"

using namespace std;

int main(int argc, char** argv) {
	/* argc - Number of parameters passed including ./enigma
	 * argv - Array of c-strings containing the parameters
	 * 		- in this case they are the file path to machine
	 * 		- settings */
	if (argc - 1 < 3) {
		cerr << "usage: enigma plugboard-file reflector-file (<rotor-file>)* rotor-positions" << endl;
		return INSUFFICIENT_NUMBER_OF_PARAMETERS;
	}
	
	/* Create EnigmaMachine */
	EnigmaMachine enigma(argc - 1, argv);
	
	/* Print Settings */
	int err = enigma.initialise();	
	if (err) {
		return err;
	}
	
	char letter;
	while (!(cin >> letter).fail()) {
		if (letter >= 'A' && letter <= 'Z') {
			cout << enigma.encodeChar(letter);
		} else {
			cerr << letter << " is not a valid input character (input characters must be upper case letters A-Z)!" << endl;
			return INVALID_INPUT_CHARACTER;
		}
	}

	return 0;
}
