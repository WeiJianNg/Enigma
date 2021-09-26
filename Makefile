all: enigma

enigma: main.o enigma.o plugboard.o reflector.o rotors.o helper.o
	g++ -g -Wall main.o enigma.o plugboard.o reflector.o helper.o rotors.o -o enigma

main.o: main.cpp errors.h enigma.h
	g++ -g -Wall -c main.cpp

enigma.o: enigma.cpp errors.h enigma.h
	g++ -g -Wall -c enigma.cpp

plugboard.o: plugboard.cpp errors.h enigma.h
	g++ -g -Wall -c plugboard.cpp

reflector.o: reflector.cpp errors.h enigma.h
	g++ -g -Wall -c reflector.cpp

rotors.o: rotors.cpp errors.h enigma.h
	g++ -g -Wall -c rotors.cpp

helper.o: helper.cpp errors.h enigma.h
	g++ -g -Wall -c helper.cpp

clean:
	rm -f *.o
