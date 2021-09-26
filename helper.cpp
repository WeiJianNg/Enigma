#include <iostream>
#include <fstream>
#include <cctype>
#include "enigma.h"
#include "errors.h"

using namespace std;

bool isDigit(string a_string) {
	int i = 0;
	while (a_string[i] != '\0') {
		if (!isdigit(a_string[i]))
			return false;
		i++;
	}
	return true;
}

/* Function to get index of the element with target value in input array
 * return -1 if target value not in input_array */
int getIndex(int* input_array, int input_count, int target) {
	for (int i = 0; i < input_count; i++) {
		if (input_array[i] == target) return i;
	}
	return -1;
}

/* Function to check Reflector and plugboard file */
int checkConfigFile(char* config_file, int& input_count, int component_id) {
	ifstream file; 
	string str_input;
	string component = component_id == 1 ? "plugboard" : "reflector";
	int input_array[ALPHA_COUNT] {0}; /* maximum number of input to pb is 26 */
	
	/* Check if file can be opened */
	file.open(config_file);
	if (file.fail()) {
		cerr << "Unable to open " << component << "configuration file "
			 << config_file << endl;
		return ERROR_OPENING_CONFIGURATION_FILE;
	}

	input_count = 0; /* Clear value */
	while (file >> str_input) {
		input_count++;
		
		/* Check if input_count is more than 26 */
		if (input_count > ALPHA_COUNT) {
			cerr << "Incorrect ";
			if (component_id == 2) {
				cerr << "(odd) ";
			}
			cerr << "number of parameters in " << component 
				 << " file " << config_file << endl;
			switch (component_id) {
				case 1: return INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS;
				case 2: return INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS;
			}
		}

		/* Check if input is Non-numeric */
		if (!isDigit(str_input)) {
			cerr << "Non-numeric character in " << component << " file "
			 	<< config_file << endl;
			return NON_NUMERIC_CHARACTER;
		}
		
		/* Check if input is valid index between 0 and 25 */
		if (!(stoi(str_input) < ALPHA_COUNT && stoi(str_input) >= 0)) {
			cerr << "Invalid index in " << component << " file "
				 << config_file << endl;
			return INVALID_INDEX;
		}
		
		/* Check if input is repeated in the config file */
		for (int i = input_count - 2; i >= 0; i--) {
			if (stoi(str_input) == input_array[i]) {
				cerr << "Invalid mapping of input " << str_input 
					 << " in " << component << " file " << config_file
					 << ". Unable to form two connections to same input " << endl;
				switch (component_id) {
					case 1: return IMPOSSIBLE_PLUGBOARD_CONFIGURATION;
					case 2: return INVALID_REFLECTOR_MAPPING;
				}
			}
		}

		/* All checks passed, assign value to input_array */
		input_array[input_count - 1] = stoi(str_input); 
	}
	file.close();

	/* Check if input count is more than min_input_count 
	 * Explicit check fo reflector */
	if (input_count < ALPHA_COUNT) {
		if (component_id == 2) {
			if (input_count % 2 == 0) {
				cerr << "Insufficient number of mappings in " << component
				     << " file: " << config_file << endl;
			} else {
				cerr << "Incorrect (odd) number of parameters in " << component
				     << " file " << config_file << endl;
			}
			return INCORRECT_NUMBER_OF_REFLECTOR_PARAMETERS;
		} else {
			if (input_count % 2 != 0) {
				cerr << "Incorrect number of parameters in " << component
					 << " file " << config_file << endl;
				return INCORRECT_NUMBER_OF_PLUGBOARD_PARAMETERS;
			}
		}
	}
	return 0;
}

int checkRotorConfigFile(char* config_file, bool isPosFile /*False*/, 
						 int& input_count, int rotor_count /*=0*/) {
	ifstream file; 
	string str_input;
	string component = isPosFile ? "rotor positions" : "rotor";
	int input_array[2 * ALPHA_COUNT]; // Max input to rotor (26 for mapping and 26 for notches)
	
	input_count = 0;

	file.open(config_file);
	if (file.fail()) {
		cerr << "Unable to open " << component << "configuration file "
			 << config_file << endl;
		return ERROR_OPENING_CONFIGURATION_FILE;
	}
	
	while (file >> str_input) {
		input_count++;
		/* Check if input array exceeds input limit */
		if (isPosFile) {
			if (input_count > rotor_count) {
			cerr << "Number of parameters in " << component << " file "
				 << config_file << " exceeds rotor count." << endl;
			return 13;
			}
		} else {
			if (input_count > 2 * ALPHA_COUNT) {
			cerr << "Incorrect number of parameters in " << component
				 << " file " << config_file << ". Input exceeds the 52 limit"
				 " (26 for mapping and 26 for notches)" << endl;
			return INCORRECT_NUMBER_OF_ROTORS_INPUT;
			}
		}

		/* Check if input is Non-numeric */
		if (!isDigit(str_input)) {
			cerr << "Non-numeric character ";
			if (!isPosFile) {
				cerr << "for mapping ";
			}
			cerr << "in "  << component << " file " << config_file << endl;
			return NON_NUMERIC_CHARACTER;
		}
		
		/* Check if input is valid index between 0 and 25 */
		if (!(stoi(str_input) < ALPHA_COUNT && stoi(str_input) >= 0)) {
			cerr << "Invalid index for mapping in " << component << " file "
				 << config_file << endl;
			return INVALID_INDEX;
		}	
	
		/* Check for repetition in rotor config file, this check
		 * is not required for rotor pos file */
		if (!isPosFile) {
			if (input_count <= ALPHA_COUNT) {
			/* Check repetition for mapping */
				for (int j = input_count - 2; j >= 0; j--) {
					if (input_array[j] == stoi(str_input)) {
						cerr << "Invalid mapping of input " << input_count - 1 << " to output "
						     << str_input << " (output " << str_input << " is already mapped "
							 "to from input " << j << ") in rotor file: " << config_file << endl; 
						return INVALID_ROTOR_MAPPING;
					}
				}
			} else {
			/* Check for repetition of notches */
				for (int i = input_count - 2; i >= ALPHA_COUNT; i--) {
					if (input_array[i] == stoi(str_input)) {
						cerr << "Invalid Notch Input. Notch input " << str_input
							 << " repeated in rotor file: " << config_file << endl;
						return 14; // Self defined error code when notches are repeated
								   // Error code not defined in specification
					}
				}
			}
		}
		input_array[input_count - 1] = stoi(str_input);
	}

	/* Check if input count is sufficient */
	if (isPosFile) {
		if (input_count < rotor_count) {
		/* Error if input positions provided is more than rotor count */
			cerr << "No starting position for rotor ";
			for (int i = 0; i < rotor_count - input_count; i++) {
				cerr << i << " ";
			}
			cerr << "in rotor position file: " << config_file << endl;
			return NO_ROTOR_STARTING_POSITION;
		}
	} else {
		if (input_count == ALPHA_COUNT) {
		/* Error if Notch position not provided in rotor file */
			cerr << "Notch position not provided in rotor file: "
				 << config_file << endl;
			return 15; // Self defined error code for no rotor position
					   // Error code not defined in specification
		} else if (input_count < ALPHA_COUNT) {
		/* Error if not all rotor mapping is provided */
			cerr << "Not all inputs mapped in rotor file: "
				 << config_file << endl;
			return INVALID_ROTOR_MAPPING;
		}
	}
	file.close();
	return 0;
}

int heapErr(const char* variable) {
	cerr << "Unable to allocate sufficient memory in heap for "
	     << variable << endl;
	return 16; // Self defined error code for insufficient heap
			   // memory to store the configurations 
}
		
