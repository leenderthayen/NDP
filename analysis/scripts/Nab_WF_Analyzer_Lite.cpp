//Nab_WF_Analyzer_VarShaping.cpp
//J. Choi
//Jan. 10th, 2020 -> Updated Jan. 22nd, 2020 -> Jan. 28th, 2020: It still looks fine.
//Feb. 19th, 2020: Fixed some typos
//Feb. 19th, 2020: Branched off from Feb. 7th, 2020 version to vary structure of the Trapezoidal filter
//Varies width of each wings of the Trapezoidal filter separately: needs to specify width of the filter for both sides.
//General version
//It got super lean on April 9th 2020


//Purpose:
//Analysis script to extract information out of the waveforms from the Nab system's DAQ.
//Currently it will look for output channel 0 and channel 2. It's just where there data were fed.
//It will roughly evaluate value of the baseline, and judge the necessity to flip the sign of the waveform, locate the peak, and rigorously evaluate the value of the baseline and "noisiness" of the baseline

//Execution argument:  [Binary data filename] [Length of waveform] [Script mode] [Output waveform? 1 for yes] [Skip Ch#]
//Binary data filename and length of waveform should be self-explanatory.
//Different data types can be accomodated via the script mode. ScriptMode == 0 is for the Detector/ Electronics "tail" pulse | ScriptMode == 1 is for the square wave from the PB-5 pulser
//Output waveform will pack all of the waveforms into the same TTree files
//Skip Ch#: option to skip a "tag" channel from analysis. This logic and few other make this version rather specific for the linearity study

//Analysis fundamentals:
//Moving average filter to eliminate any high frequency noise
//Derivative to locate the peak of the square wave
//Average and standard deviation to classify baseline of waveform to "clean" vs. "contaminated" vs. "noisy" <-NEEDS TO BE DONE
//Background subtraction from the baseline average
//TGraphErrors & a fitting function to extract the fall time of the Tail pulse

//Algorithm outline:
//1.) Read the binary data files to determine the number of "proper" waveforms; "proper" just means waveforms from the channel that's not being skipped.
//Re-reading the data files to load all waveforms into the memory
//2.) Rougly calculate the baseline defined by the 100th time bin to 199th time bin
//3.) Calculate the integral of the waveform and subtract the baseline. If the net integral is less than 0, flip the waveform
//4.) Locate the peak. This process involves the moving average filter, and for the "Tail" pulses, peak is simply the highest value point. For the square pulse, peak is where the derivative has decreased to "0" after the peak.
//5.) Calculate the averge and standard deviation of the baseline

//Details on the Trapezoidal filter. Feb. 20th, 2020:
//1.) The rising edge and the falling edge have different shaping time.
//2.) Gap time is provided in the script as well.


//Updated list of included headers ---
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "math.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TVirtualFFT.h"
using namespace std;
//------------------------------------

void SmoothArraySF(short *Input, float *Output, int currentDim, int sWidth, int lWidth)
{
	int ii,jj;
	float FilterDum;

	for(ii=0;ii<(currentDim-sWidth-lWidth+2);ii++)  //Generating a histogram of the baseline values
	{
		FilterDum=0.0;
		for(jj=(sWidth-1);jj<lWidth;jj++)
		{
			FilterDum+=((float)(sWidth*Input[ii+jj]));
		}
		for(jj=1;jj<sWidth;jj++)
		{
			FilterDum+=((float)(jj*Input[ii+jj-1]));
			FilterDum+=((float)(jj*Input[ii+lWidth+sWidth-jj-1]));
		}
		Output[ii]=FilterDum/((float)(sWidth*lWidth));
	}
}

void SmoothArrayFF(float *Input, float *Output, int currentDim, int sWidth, int lWidth)
{
	int ii,jj;
	float FilterDum;

	for(ii=0;ii<(currentDim-sWidth-lWidth+2);ii++)  //Generating a histogram of the baseline values
	{
		FilterDum=0.0;
		for(jj=(sWidth-1);jj<lWidth;jj++)
		{
			FilterDum+=(((float)(sWidth))*Input[ii+jj]);
		}
		for(jj=1;jj<sWidth;jj++)
		{
			FilterDum+=(((float)(jj))*Input[ii+jj-1]);
			FilterDum+=(((float)(jj))*Input[ii+lWidth+sWidth-jj-1]);
		}
		Output[ii]=FilterDum/((float)(sWidth*lWidth));
	}
}

void RunTFilter(float *Input, float *Output, int currentDim, int Rise, int Fall, int hgap)
{
	int ii,jj;
	float riseval,fallval;
	float rweight = ((float)(Rise))/2.0;
	float fweight = ((float)(Fall))/2.0;
	int lFilter = (Rise+hgap*2+1+Fall);

	for(ii=0;ii<(currentDim-lFilter+1);ii++)  //Generating a histogram of the baseline values
	{
		riseval=0.0;
		for(jj=0;jj<Rise;jj++)
		{
			riseval+=((Input[ii+jj])*(((float)(jj))+0.5)/((float)(Rise)));
		}
		riseval/=rweight;

		fallval=0.0;
		for(jj=0;jj<Fall;jj++)
		{
			fallval+=((Input[ii+lFilter-Fall+jj])*(1.0-(((double)(jj))+0.5)/((double)(Fall))));
		}
		fallval/=fweight;
		Output[ii]=((float)(fallval-riseval));
	}
}

//Apr. 2nd, 2020:
//---------- the exact phrases "int Nab" and "int main" can not be part of first 18 characters in a line. There must be 2 empty spaces in front of the "int main", or the translator will not work right
//---------- Same with phrases "int Nab" and such, the exact phrahse "TFile f0(" cannnot be part of first 19 characters of a line
//int Nab_WF_Analyzer(string filein, int WaveLen, int ScriptMode, int Waveform_IO, int SkipCh, int RiseWidth, int FallWidth, int hFilter_Gap, float inputV)    //Command-line input:  [Binary data filename] [Length of waveform] [Script mode, ie. Signal shape] [Output waveform? 1 for yes] [Skip Ch#] [Risetime Width] [Falltime width] [Half-Gap width]
  int main(int argc, char *argv[])    //Command-line input:  [Binary data filename] [Length of waveform] [Script mode, ie. Signal shape] [Output waveform? 1 for yes] [Skip Ch#] [Risetime Width] [Falltime width] [Half-Gap width]
{

	//Assigning passed arguments into varaibles  --------------------------------
	  string filein = argv[1];               // Name of input file
	const int WaveLen = (int)atof(argv[2]);      // Length of a waveform

	string fileout = filein+"_All.root";       // Name of output root file
	//---------------------------------------------------------------------------


	const int MAFilter_LengthHi = 2;  //High frequency filter
	const int MAFilter_LengthLo = 3; //Low frequency filter

	//---------------------------------------------------------------------------

	//General script Variables  -------------------------------------------------
	int i,j,k,l,a;
	int WaveNum;
	int peakloc;
	float peakval;
	//int Risetime;
	float Risetime;
	float BaseMean;    //Baseline oberservable related
	//---------------------------------------------------------------------------

	//Binary data file related Variables-----------------------------------------
	int Result, Board, Channel, Length, EventID;
	unsigned long int Timestamp;

	short *WaveformAll = new short[WaveLen];
	short *dWaveform = new short[WaveLen-1];
	float *sdWaveform = new float[WaveLen-MAFilter_LengthHi-MAFilter_LengthLo+1];
	float *WaveformF = new float[WaveLen];
	float *sWaveform = new float[WaveLen-3-100+2];

	short dummy;
	int chNum;
	short cut=0;

	float cPeak;
	float v10,v90;
	unsigned long int tStep;
	int NewDim;
	int LowFlag, HighFlag;
	int LowLoc, HighLoc;

	TTree *TRAux = new TTree("TRAux","Tree File with Auxillary info");    // TTree - Auxillary
	TRAux->Branch("WaveID",&WaveNum,"WaveNum/I");
	TRAux->Branch("Channel",&chNum,"chNum/I");
	TRAux->Branch("BaseMean",&BaseMean,"BaseMean/F");
	//TRAux->Branch("sPeakVal",&peakval,"peakval/F");
	TRAux->Branch("PeakVal",&cPeak,"cPeak/F");
	TRAux->Branch("PeakLoc",&peakloc,"peakloc/I");
	TRAux->Branch("Risetime",&Risetime,"Risetime/F");
	//TRAux->Branch("Risetime",&Risetime,"Risetime/I");
	TRAux->Branch("HighLoc",&HighLoc,"HighLoc/I");
	TRAux->Branch("LowLoc",&LowLoc,"LowLoc/I");
	TRAux->Branch("eTime",&tStep,"tStep/l");


	//---------------------------------------------------------------------------

	printf("Analyzing: %s | Length of waveform: %d\n",filein.c_str(),WaveLen);


	//Loading the waveforms into the array & bare back baseline probe lines here
	WaveNum=0;
	ifstream finRH2(filein.c_str(),ios::binary);
	while(finRH2.read((char*)&Result,1) && finRH2.read((char*)&EventID,4) && finRH2.read((char*)&Board,4) && finRH2.read((char*)&Channel,4) && finRH2.read((char*)&Timestamp,8) && finRH2.read((char*)&Length,4) && finRH2.read((char*)WaveformAll,(WaveLen*2)))
  {
		chNum=(Board*8+Channel);

		//if(WaveNum%100 == 0) printf("%d\n",WaveNum);
		tStep = Timestamp;

		//Shifting the waveform to roll over
		for(i=0;i<WaveLen;i++)
    {
			WaveformAll[i]-=16384*(WaveformAll[i]/8192);
		}

		//"Derivative" calculation
		for(i=0;i<WaveLen-1;i++)
    {
			WaveformF[i] = ((float)(WaveformAll[i]));
			dWaveform[i] = WaveformAll[i+1] - WaveformAll[i];

		}
		WaveformF[WaveLen-1] = ((float)(WaveformAll[WaveLen-1]));


		SmoothArraySF(dWaveform,sdWaveform,WaveLen-1,MAFilter_LengthHi,MAFilter_LengthLo);
		NewDim = WaveLen-MAFilter_LengthHi-MAFilter_LengthLo+1;

		cPeak = -1000000.0;
		peakloc = 0;
		for(i=0;i<NewDim;i++)
		{
			if(cPeak < sdWaveform[i])
			{
				cPeak = sdWaveform[i];
				peakloc=i;
			}
		}
		peakloc+=((WaveLen-NewDim)/2);


		//Integrating the baseline, defiend to end 1/2 the Trapezoidal filter away from peakloc
		BaseMean=0.0;
		for(i=0;i<(peakloc-1500);i++)
    {
			BaseMean+= WaveformF[i];
		}
		BaseMean/=((float)(peakloc-1500));

		for(i=0;i<WaveLen;i++)
    {
			WaveformF[i] -= BaseMean;
		}

		cPeak = -1000000.0;
		for(i=peakloc;i<WaveLen;i++)
    {
			if(cPeak < WaveformF[i])
			{
				cPeak = WaveformF[i];
			}
			//if(WaveformF[i] < WaveformF[i-1] && WaveformF[i] < WaveformF[i-2] && WaveformF[i] < WaveformF[i-3] && WaveformF[i] < WaveformF[i-4]) break;
			if(i>WaveLen-4) break;
			if(WaveformF[i] > WaveformF[i+1] ) break;
		}

		v10 = cPeak*0.1;
		v90 = cPeak*0.9;

		HighFlag = 1;
		for(i=peakloc;i<WaveLen;i++)
    {
			if(HighFlag == 1)
			{
				if(WaveformF[i] >= v90)
				{
					HighFlag = 0;
					HighLoc = i;
				}
			}
		}

		LowFlag = 1;
		for(i=peakloc;i>=0;i--)
    {
			if(LowFlag == 1)
			{
				if(WaveformF[i] <= v10)
				{
					LowFlag = 0;
					HighFlag = 1;
					LowLoc = i;
				}
			}
		}
		Risetime = ((float)(HighLoc-LowLoc))*4.0; // [us] Width of a time bin is 4ns = 0.004 us
		//Risetime = (HighLoc-LowLoc);
		//Correcting for the 1st order DC offset && logging the waveform


		//printf("%f , %f\n",WFTime[WaveNum],TFilter[WaveNum]);

		if(peakloc == 0)
		{
			BaseMean = -1000.0;
			cPeak = -1000.0;
			Risetime = -1000.0;
			peakloc = -1000;
		}

		TRAux->Fill();

		WaveNum++;
	}
	finRH2.close();


	printf("Done with analysis | i: %d\n",i);

	//---------- Same with "int Nab" and such, the exact phrahse "TFile f0(" cannnot be part of first 19 characters of a line
	//ROOT Version will have /* in front of "File f0(", skipping the write process and the deleting process
	  TFile f0(fileout.c_str(),"recreate");  //Saving the analysis result


	TRAux->Write();
	f0.Close();

	printf("Write done\n");

	delete [] WaveformAll;
	delete [] WaveformF;
	delete [] sWaveform;

	printf("Deleting objects done\n");

	return 0;
}
