#ifndef ENIGMA_H
#define ENIGMA_H
#include <string>

const int ALPHA_COUNT = 26; // Alphabet count

/* FORWARD DECLARATION OF THE MAIN COMPONENTS IN ENIGMA MACHINE */
/* -- COMPONENT: PLUGBOARD -- */
class Plugboard {
	int input_count = 0; /* Number of inputs in plugboard configuration file */
	int configurations[ALPHA_COUNT]; /* Plugboard mapping */

	public:
		/* FUNCTION TO CHECK PLUGBOARD CONFIGURATION FILE */
		int checkSetupFile(char* pb_config_file);
		
		/* FUNCTION TO LOAD PLUGBOARD SETUP CONFIGURATIONS FROM 
		 * THE PLUGBOARD CONFIGURATION FILE */
		void setup(char* pb_config_file);
	
		/* FUNCTION TO DECODE SIGNAL PASSING THROUGH THE PLUGBOARD */
		int getOutSignal(int input_signal);
};

/* -- COMPONENT: REFLECTOR -- */
class Reflector {
	int input_count = 0; /* Number of inputs in reflector configuration file */
	int configurations[ALPHA_COUNT]; /* Reflector mapping */
	
	public:
		/* FUNCTION TO CHECK REFLECTOR CONFIGURATION FILE */
		int checkSetupFile(char* rf_config_file);
		
		/* FUNCTION TO LOAD REFLECTOR SETUP CONFIGURATIONS FROM 
		 * THE REFLECTOR CONFIGURATION FILE */
		void setup(char* rf_config_file);
		
		/* FUNCTION TO DECODE SIGNAL PASSING THROUGH THE REFLECTOR */	
		int getOutSignal(int input_signal);
};

/* -- COMPONENT: ROTORS -- */
class Rotors {
	bool isSetupComplete = false;		/* This is to indicate that the rotor setup is complete and
										   succesful. Therefore, the heap memory used for the rotor
										   config will need to be deallocated in the destructor for 
										   the component */
	int* input_count = nullptr; 		/* Number of inputs for each rotor config file */
	int** configurations = nullptr;		/* Pointer to an array where each elm points 
										   seperate rotor mapping (Pb to Rotors) */
	int** inv_configurations = nullptr; /* Pointer to an array where each elm points 
										   to the inverse of the rotor mapping (Rf to Rotors) */
	int* notches_count = nullptr;		/* Pointer to the number of notches for each rotor */
	int** notches = nullptr;			/* Pointer to an array where each elm of the array points
										   a seperate array that stores the notches for each rotor */
	int init_pos_input_count = 0;		/* Number of inputs in the rotor position file */
	int* position = nullptr;			/* Pointer to an array which stores the postion of each rotor */
	int* rotation = nullptr;			/* Pointer to an array which stores the rotation count of each rotor */
	
	/* FUNCTION TO CREATE AN INVERSE MAPPING BASED ON THE ROTOR CONFIGURATION
	 * TO ALLOW MAPPING OF SIGNAL TRAVELLING FROM THE REFLECTOR BACK TO 
	 * THE PLUGBOARD BE DONE IN CONSTANT TIME */
	int invertRotorMapping(); // returns 0 to setup function if no heap error, otherwise return 1;

	public:		
		int rotors_count = 0;
		
		/* FUNCTION TO DECLARE HEAP MEMORY REQUIRED TO STORE THE ROTORS
		 * CONFIGURATIONS 
		 * The function returns Heap Error code to Main if there is insufficient
		 * heap memory to store the configurations */
		int addRotors(int counter);
		
		/* FUNCTION TO CHECK ROTORS CONFIGURATION FILE RETURNS ERROR CODE */
		int checkSetupFile(char* initial_pos_file, char** rotors_config_files);
		
		/* FUNCTION TO LOAD ROTORS SETUP CONFIGURATIONS FROM 
		 * THE ROTORS CONFIGURATION FILE 
		 * The function returns Heap Error Code to Main if there is insufficient 
		 * heap memory to store the configurations */
		int setup(char* intial_pos_file, char** rotors_config_files);
		
		/* FUNCTION TO GET OUTPUT SIGNAL BASED ON ROTOR CONFIGURATIONS 
		 * input: rotor_id defines the rotor being considered 0 being the left most rotor
		 * input: is_Pb_to_Rf defines the direction of the signal is travelling, true indicates
		 * 		  that the signal is travelling from the plugboard to reflector and vice versa */
		int getOutSignal(int input_signal, int rotor_id, bool is_Pb_to_Rf = true); 
	
		/* RECURSIVE FUNCTION THAT UPDATES THE ROTOR POSITIONS, INCREMENT THE ROTATION COUNT
		 * AND POSITION OF THE RIGHT MOST ROTOR BY ONE. THE POSITION OF THE ROTOR TO THE LEFT 
		 * WILL BE UPDATED IF THE CURRENT ROTOR POSITION MATCHES A NOTCH */
		void rotateRotor(int rotor_id);

		/* FUNCTION TO DECONSTRUCT THE ROTORS, DEALLOCATING THE HEAP MEMORY
	 	 * USED TO STORE THE ROTORS CONFIGURATIONS */
		~Rotors();
};

/* -- ENIGMA MACHINE -- */
class EnigmaMachine {
	/* MEMBERS OF ENIGMA MACHINE */
	int files_count;
	char** setup_files;

	/* COMPONENTS IN ENIGMA MACHINE */
	Plugboard plugboard;
	Reflector reflector;
	Rotors rotors;
	
	public:
		/* CONSTRUCTOR FOR ENIGMA MACHINE */
		EnigmaMachine(int files_count, char** setup_files) //
				: files_count(files_count), setup_files(setup_files) {};
		
		/* FUNCTION TO CHECK AND INITIALISE ALL THE SETTINGS FOR THE COMPONENTS
 		 * IN ENIGMA MACHINE i.e ROTOR, PLUGBOARD AND REFLECTOR */
		int initialise();
		
		/* FUNCTION TO ENCODE / DECODE A CHARACTER */
		char encodeChar(char input_character);
};

/* FORWARD DECLARATION FOR HELPER FUNCTIONS */
/* FUNCTION TO CHECK IF A STRING ELEMENT CONTAINS DIGIT */
bool isDigit(std::string a_string);

/* FUNCTION TO GET INDEX OF TARGET ELEMENT IN THE INPUT_ARRAY */
int getIndex(int* input_array, int input_count, int target);

/* FUNCTION TO CHECK REFLECTOR AND PLUGBOARD CONFIGURATION FILES */
/* Input: component_id defines the component being checked. 1 for plugboard, 2 for reflector */
int checkConfigFile(char* config_file, int& input_count, int component_id);

/* FUNCTION TO CHECK ROTOR AND ROTOR POSITION FILES FOR ERROR */
/* Input: isPosFile defines if the configuration file being checked is a position file */
int checkRotorConfigFile(char* config_file, bool isPosFile , int& input_count, int rotor_count /*=0*/);

/* FUNCTION TO EXIT PROGRAMME AND OUTPUT ERROR MESSAGES WHEN THERE IS INSUFFICIENT 
 * HEAP MEMORY FOR VARIABLE */
int heapErr(const char* variable);
#endif
