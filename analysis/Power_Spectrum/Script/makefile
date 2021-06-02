LIBS=$(shell root-config --glibs) 
CFLAG=$(shell root-config --cflags) 

Nab_PowerSpectrum: Nab_PowerSpectrum

Nab_PowerSpectrum: Nab_PowerSpectrum.o 
	g++ -O3 Nab_PowerSpectrum.o $(LIBS) -o Nab_PowerSpectrum

Nab_PowerSpectrum.o:
	g++ -O3 -fPIC -pthread $(CFLAG) -c Nab_PowerSpectrum.cpp -o Nab_PowerSpectrum.o

	
clean:
	rm  Nab_PowerSpectrum.o



