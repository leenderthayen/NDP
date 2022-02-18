//single_histo.cpp
//Dec. 19th, 2017
//main() is the standard histogram generator


//Updated list of included headers ---
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include <string>
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include <unistd.h>
using namespace std;
//------------------------------------

#define _XOPEN_SOURCE_EXTENDED 1
#define PI 3.1415926535897932384626

int main(int argc, char *argv[]) {

	const int PLOT=0;
	int i,j,k;
	int length=0;

	int enrg = (int)atof(argv[1]);
	int angle = (int)atof(argv[2]);
	string pname = (argv[3]);

	printf("Processing a particle type: %s, with KE = %d, incident at %d degree\n",pname.c_str(),enrg,angle);

	string line;
	char linec[200];

	double dangle = ((double) (180-angle))/180.0*PI;
	double ypos = sin(dangle);
	double zpos = cos(dangle)+1.0;
	double yhat = (ypos*(-1.0));
	double zhat = (cos(dangle)*(-1.0));

	//Loading data file
	fstream myfile;
	myfile.open("basic_looped.mac", ios::in | ios::out);  //Specifying data file directory
	while(getline(myfile,line))
	{
		//printf("%s\n", line);
		strcpy(linec,line.c_str());
		//printf("%d %d\n", linec[0],linec[1]);
		//  ASCII      #                 S                 t                  o                  p
		if(linec[0] == 35 && linec[1] == 83 && linec[2] == 116 && linec[3] == 111 && linec[4] == 112)
		{
			//printf("%s\n", line.c_str());
			break;
		}
	}
	long pos = myfile.tellp();
	myfile.close();

	truncate("basic_looped.mac", pos);

	myfile.open("basic_looped.mac", ios_base::app);
	myfile << "/gps/particle " << pname << "\n";
	myfile << "/gps/energy " << enrg << " keV\n";
	myfile << "/gps/direction 0 " << yhat << " " << zhat << "\n";
	myfile << "/gps/position 0 " << ypos << " " << zpos << " cm\n";
	myfile << "/gps/source/list\n";
	myfile << "/run/particle/dumpCutValues\n";
	myfile << "/analysis/setFileName " << pname << "_" << enrg << "keV_" << angle <<"inc.root\n";
	myfile << "/run/printProgress 100\n";
	myfile << "/run/beamOn 2500";
	myfile.close();

	return 0;

}
