#include <iostream>
#include <fstream>
#include "enigma.h"

using namespace std;

/* FUNCTION TO CHECK SETUP FILE FOR ERRORS. IF ERROR, RETURNS ERROCODE TO MAIN */
/* Function to check setup file */
int Plugboard:: checkSetupFile(char* pb_config_file) {
	/* Utilise helper function to check file,
	 * Plugboard and Reflector share the same function to check for error */
	return checkConfigFile(pb_config_file, input_count, 1 /*Key fo PB*/);
}

/* FUNCTION TO LOAD AND SETUP PLUGBOARD CONFIGURATION USING INPUT FROM THE 
 * PLUGBOARD CONFIGURATION FILE */
void Plugboard::setup(char* pb_config_file) {
	/* Initialise configuration with value of its index */
	/* If there aren't any connection any input signal will be mapped onto
	 * itself e.g. 0 -> 0 */
	for (int j = 0; j < ALPHA_COUNT; j++) {
		configurations[j] = j;
	}
	
	if (input_count) {
		int input;
		ifstream file;
		int input_array[input_count]; 
		int i = 0;
		/* Read config file and populate pb config */
		/* Note the file has already been checked in the check stage and at this 
		 * point all the inputs are valid */
		file.open(pb_config_file);
		
		while (file >> input) {
			input_array[i] = input;	
			i++;
		}
		
		file.close();

		/* Create reflector mapping configuration using the input array */
		for (int k = 0; k < input_count; k++) {
			configurations[input_array[k]] = k % 2 == 0 
										   ? input_array[k + 1]
										   : input_array[k - 1];
		}
	}
}

/* FUNCTION TO DECODE SIGNAL PASSING THROUGH THE PLUGBOARD */
int Plugboard:: getOutSignal(int input_signal) {
	return configurations[input_signal];
}
