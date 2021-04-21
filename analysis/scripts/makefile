LIBS=$(shell root-config --glibs) 
CFLAG=$(shell root-config --cflags) 

Nab_WF_Analyzer_LT: Nab_WF_Analyzer_LT

Nab_WF_Analyzer_LT: Nab_WF_Analyzer_LT.o 
	g++ -O3 Nab_WF_Analyzer_LT.o $(LIBS) -o Nab_WF_Analyzer_LT

Nab_WF_Analyzer_LT.o:
	g++ -O3 -fPIC -pthread $(CFLAG) -c Nab_WF_Analyzer_Lite.cpp -o Nab_WF_Analyzer_LT.o

	
clean:
	rm  Nab_WF_Analyzer_LT.o



