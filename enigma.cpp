#include <iostream>
#include <string>
#include "enigma.h"
#include "errors.h"

using namespace std;

/* METHODS FOR THE CLASS ENIGMAMACHINE */

/* FUNCTION TO CHECK AND INITIALISE ALL THE SETTINGS FOR THE COMPONENTS
 * IN ENIGMA MACHINE i.e ROTOR, PLUGBOARD AND REFLECTOR */
int EnigmaMachine::initialise() {		
	/* Engima Machine contains a setup_files member which has the following:
	 * setup_files[1] - Plugboard configuration files
	 * setup_files[2] - Reflector configuration files
	 * setup_files[-1] - Rotor start position files
	 * setup_files[3 to N] - Rotor configuration files (can be multiple) */
	
	/* Check Plugboard configurations; if error, error code returned to Main */
	int pbErr = plugboard.checkSetupFile(setup_files[1]);
	if (pbErr) return pbErr;
	
	/* Setup Plugboard configurations */
	plugboard.setup(setup_files[1]);
	
	/* Check Reflector configurations; if error, error code returned to main */
	int rfErr = reflector.checkSetupFile(setup_files[2]);
	if (rfErr) return rfErr;
	
	/* Setup Reflector configurations */
	reflector.setup(setup_files[2]);

	if (files_count - 3 > 0) {
		/* Store rotor configuration files into an array for checking */
		char** rotor_config_files = new (nothrow) char* [files_count - 3];
		if (rotor_config_files == nullptr) heapErr("rotor configuration files");
		
		/* Initialise rotors component, allocating sufficient heap memory to
		 * store the rotors configurations. Note: At this point, the configurations
		 * for rotors are yet to be checked and loaded into the component */
		int heapErr = rotors.addRotors(files_count - 3);
		if (heapErr) return heapErr; // Function return heap error to main if there
									 // is insufficient memory to store config
		
		for (int i = 0; i < rotors.rotors_count; i++) {
			rotor_config_files[i] = setup_files[3 + i];
		}
		
		/* Check rotors configurations; if error, error code returned to main */
		int rotorErr = rotors.checkSetupFile(setup_files[files_count], rotor_config_files);
		if (rotorErr) return rotorErr;

		/* Setup Rotors configurations */
		heapErr = rotors.setup(setup_files[files_count], rotor_config_files);
		if (heapErr) return heapErr; // Function returns heap error code to main if there
									 // is insufficient memory to store config

		/* Deallocate heap memory */
		delete[] rotor_config_files;
	}

	/* Return to main with no error */
	return 0;
}

/* FUNCTION TO ENCODE / DECODE INPUT CHARACTER */
char EnigmaMachine::encodeChar(char input_character) {
	/* Convert input character to integer with value between 0 and 25 */
	int input_signal = input_character - 'A';
	int output_signal; 
	char output_character;

	/*  Transfer input signal to Plugboard for encoding */
	output_signal = plugboard.getOutSignal(input_signal);	
		
	if (rotors.rotors_count) {
		/* Rotate right most rotor before tramission of signal */
		rotors.rotateRotor(rotors.rotors_count - 1);

		/* Transfer signal from Plugboard to Reflector, passing through 
		   the rotors while being encoded along the way */
		for (int i = rotors.rotors_count - 1; i >= 0; i--) {
			output_signal = rotors.getOutSignal(output_signal, i); 
		}
	}

	/* Re-routing the signal from / to the rotors for encoding */
	output_signal = reflector.getOutSignal(output_signal);

	/* Transfer signal from Reflector to Plugboard, passing through
	 * the rotors while being decoded along the way */
	if (rotors.rotors_count) {
		for (int i = 0; i < rotors.rotors_count; i++) {
			output_signal = rotors.getOutSignal(output_signal, i, false); 
		}
	}

	/* Trasnfer signal to plugboard for encoding and output */
	output_signal = plugboard.getOutSignal(output_signal);

	/* Convert output signal to character */
	output_character = output_signal + 'A';	
	return output_character;
}
