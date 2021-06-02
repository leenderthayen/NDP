//Nab_PowerSpectrum.cpp
//J. Choi
//May 7th, 2021
// ROOT FFT Power Spectrum Generator


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
#include "TVirtualFFT.h"
using namespace std;

#define nPixel 127
#define FullDAQ 128
//------------------------------------

//            name of Map file | Nab Detector to SSD Contact Map
int ReadSSDMap(string FileIn,       int PixToSSD[nPixel+1]) {

  //Checking to see if the map file exists
  FILE *file;
  if ((file = fopen(FileIn.c_str(), "r")))
  {
    fclose(file);
    printf("Map file exists\n");
  }
  else  //No param of the input name, so give up.
  {
    printf("Map file doesn't exist -> Giving up\n");
    return 0;
  }

  int nLine = 0;
	string line;
  char lett[1];
  char linec[45];
  int iDum,iPixel,iSSD;

  ifstream MFile;
  MFile.open(FileIn.c_str());
  getline(MFile,line);
  while(getline(MFile,line))
	{
    line.erase(line.size() - 1);
    strcpy(linec,line.c_str());
    //printf("Raw line: %s\n",line.c_str() );
    //sscanf(linec, "%d,%s,%d,%s,%d,%d",&iPixel,lett,&iDum,lett,&iDum,&iSSD);
    sscanf(linec, "%d , %s , %d , %s , %d , %d",&iPixel,lett,&iDum,lett,&iDum,&iSSD);
    PixToSSD[iPixel] = iSSD;
    // printf("%d to %d\n", iPixel,PixToSSD[iPixel]);
    nLine++;
  }
  MFile.close();
  if(nLine != nPixel) return 0;

  return 1;
}

//            name of param file | length of single WF | Trigger Offset | Length of the noise WF | Mapping between the Board*8+Channel to the Detector Pixel
int ReadParamFile(string FileIn,     int *sLength,       int *sTrigOff,      int *nLength,                       int BCtoPixel[FullDAQ]) {

  int Pixel = -123;
  int DCChannel = -1;
  int BChannel,PrePixel;

  int *BCReadFlag = new int[FullDAQ];
  for(int ii=0;ii<FullDAQ;ii++) BCReadFlag[ii] = 0;

  string line;
  char cDum[10];
  char linec[200];

  //Checking to see if the param file exists
  FILE *file;
  if ((file = fopen(FileIn.c_str(), "r")))
  {
    fclose(file);
    printf(".param file exists\n");
  }
  else  //No param of the input name, so give up.
  {
    printf(".param file doesn't exist -> Giving up\n");
    return 0;
  }

  fstream ParamFile;
	ParamFile.open(FileIn.c_str(), ios::in);  //Specifying data file directory
	while(getline(ParamFile,line))
	{
    if (line.compare("Board Channel To Pixel Map:\r") == 0)
    {
      printf("Found the Header for the BC to Pixel Map: %s\n", line.c_str());
      getline(ParamFile,line);

      //First Item
      getline(ParamFile,line);
      strcpy(linec,line.c_str());
      sscanf(linec, "%d, %d",&BChannel,&Pixel);
      if(Pixel != -123)
      {
        BCtoPixel[BChannel] = Pixel;
        BCReadFlag[Pixel] = 1;
      }
      else
      {
        BCtoPixel[BChannel] = DCChannel;
        DCChannel--;
      }
      for(int i=1;i<FullDAQ;i++)
      {
        getline(ParamFile,line);
        strcpy(linec,line.c_str());
        PrePixel = Pixel;
        sscanf(linec, "%d, %d",&BChannel,&Pixel);

        if(PrePixel != Pixel)
        {
          if(BCReadFlag[Pixel] == 1)
          {
            printf("Duplicate pixel detected: %d\n", Pixel);
            BCtoPixel[BChannel] = DCChannel;
            DCChannel--;
          }
          else
          {
            BCtoPixel[BChannel] = Pixel;
            BCReadFlag[Pixel] = 1;
          }
        }
        else
        {
          printf("Empty DAQ Channel detected: %d\n", BChannel);
          BCtoPixel[BChannel] = DCChannel;
          DCChannel--;
        }
      }
    }

    if (line.compare("Singles Settings:\r") == 0)
    {
      printf("Found the Header for Singles File Setting: %s\n", line.c_str());
      getline(ParamFile,line);
      strcpy(linec,line.c_str());
      sscanf(linec, "%s %d",cDum,sLength);
      printf("Singles Waveform Length: %d\n",*sLength );
      getline(ParamFile,line);
      strcpy(linec,line.c_str());
      sscanf(linec, "%s %s %d",cDum,cDum,sTrigOff);
      printf("Singles Waveform Trigger Offset: %d\n",*sTrigOff );
    }

    if (line.compare("Long Baseline Settings:\r") == 0)
    {
      printf("Found the Header for Noise File Setting: %s\n", line.c_str());
      getline(ParamFile,line);
      getline(ParamFile,line);
      strcpy(linec,line.c_str());
      sscanf(linec, "%s %s %s %d",cDum,cDum,cDum,nLength);
      printf("Noise Waveform Length: %d\n",*nLength );
    }
	}
	ParamFile.close();

  delete [] BCReadFlag;
  return 1;
}

//           name of the input Run | Type of existing Data
int ReadWFLength(string FileIn,        string *fType) {

  FILE *file;
  //First Looking for .noise file
  string ReadFile = FileIn+".noise";
  if ((file = fopen(ReadFile.c_str(), "r")))
  {
    fclose(file);
    printf(".noise file exists\n");
    *fType = ".noise";
  }
  else
  {
    printf(".noise file doesn't exist -> Looking for .singl file \n");
    printf("** \n");

    ReadFile = FileIn+".singl";
    if ((file = fopen(ReadFile.c_str(), "r")))
    {
      fclose(file);
      *fType = ".singl";
      printf(".singl file exists\n");
    }
    else
    {
      printf(".singl file doesn't exist -> Giving up \n");
      *fType = "FAILED";
      return 0;
    }
  }

  int Length;
  int rFlag = 0;

  char GBs[100];
  unsigned int hlength;
  unsigned long int Timestamp;
  unsigned short pOffset;

  ifstream BinFile(ReadFile.c_str(),ios::binary);
  //Reading a bunch of header to the first record of the Waveform lenght.
  //Assumption here is that all waveforms have same length
  BinFile.read((char*)&hlength,4);
  BinFile.read((char*)&hlength,4);
  BinFile.read((char*)&hlength,4);
  BinFile.read((char*)&GBs,hlength);
  BinFile.read((char*)&hlength,4);
  BinFile.read((char*)&GBs,hlength);
  BinFile.read((char*)&Timestamp,8);
  BinFile.read((char*)&Timestamp,8);
  BinFile.read((char*)&Timestamp,8);
  BinFile.read((char*)&pOffset,2);
  if(BinFile.read((char*)&Length,1)) rFlag = 1;
  if(BinFile.read((char*)&Length,4)) rFlag = 1;
  if(BinFile.read((char*)&Length,4)) rFlag = 1;
  if(BinFile.read((char*)&Length,4)) rFlag = 1;
  if(BinFile.read((char*)&Timestamp,8)) rFlag = 1;
  if(BinFile.read((char*)&Timestamp,8)) rFlag = 1;
  if(BinFile.read((char*)&Length,4)) rFlag = 1;
  BinFile.close();


  if(rFlag == 0) Length = 0;
  printf("File has %d long Waveforms\n",Length);

  return Length;
}


//May. 27th, 2021:
int main(int argc, char *argv[])    //Command-line input:  [Binary data filename] [Run Number] [Sub Run Number]
{
	//Checking the number of arguments
	if(argc<3)
  {
    printf("Valid Command line input: ./Nab_PowerSpectrometer [Run Number] [Sub Run Number]\nNot enough arugments.. -> Quitting\n");
    return 1;
  }

	//Assigning passed Run iD's into strings  --------------------------------
	stringstream RunIDSS;
	RunIDSS.str("");
	RunIDSS << ((int)atof(argv[1]));    // Run ID
	string RunIDS = RunIDSS.str();

  stringstream SubrunIDSS;
  SubrunIDSS.str("");
	SubrunIDSS << ((int)atof(argv[2])); // Sub run ID
	string SubrunIDS = SubrunIDSS.str();

  string DataDir = "../../Data/";
  string OutputDir = "../../Power_Spectrum/";

  string SSDFile = DataDir+"SSD_Pixel_Map.csv";
  string filein = DataDir+"Run"+RunIDS+"_"+SubrunIDS;
  string ParamFile = DataDir+"Run"+RunIDS+"_"+SubrunIDS+".param";
  string fileout = OutputDir+"Run"+RunIDS+"_"+SubrunIDS+".root";
  string HistoOut = OutputDir+"Run"+RunIDS+"_"+SubrunIDS+"_histo.root";

  string dType;

  int hNum = FullDAQ+20;
  double bLow = -20.0;
  double bHigh = ((double)(FullDAQ));

	int *nSet = new int[hNum];
  int *ChtoPx = new int[FullDAQ];
  int *PxtoSSD = new int[nPixel+1];
  int *SSDFillFlag = new int[FullDAQ+10];

  for(int ab = 0;ab<hNum;ab++) nSet[ab] = 0;
  for(int ab = 0;ab<FullDAQ;ab++) ChtoPx[ab] = 0;
  for(int ab = 0;ab<nPixel+1;ab++) PxtoSSD[ab] = 0;
  for(int ab = 0;ab<FullDAQ+10;ab++) SSDFillFlag[ab] = 0;

  int sgWFLen,sgTrigOffset,noiWFLen;

  //Baseline related constants
	int BaselineOffset;
	int RoughBaseROI;  //"Definition" of baseline/ background in a waveform

  // Reading the first line of the binary data for the length of waveform recorded
  int WaveLen = ReadWFLength(filein, &dType);
  if(WaveLen == 0)
  {
    printf("Invalid length of waveform. Suspect the data file does not exist\n");
    return 1;
  }
  printf("%s\n",dType.c_str() );

  // Reading the Parameter file for what the experiment control thought the parameters were
  if( (ReadParamFile(ParamFile, &sgWFLen, &sgTrigOffset, &noiWFLen,ChtoPx)) == 0)
  {
    printf("Couldn't process the Param file\n");
    return 1;
  }

  // Reading the SSD Map file
  if( (ReadSSDMap(SSDFile, PxtoSSD)) == 0)
  {
    printf("Couldn't process the SSD Map file. Possibly it lacks the full map\n");
    return 1;
  }

  //Adapting the Baseline ROI accordingly with the data type
  if(dType.compare(".noise") == 0)
  {
    if(WaveLen != noiWFLen)
    {
      printf("Inconsistency between length of waveform in the data vs. param file. -> Quitting\n");
      return 1;
    }
    RoughBaseROI = WaveLen;
  }
  else if(dType.compare(".singl") == 0)
  {
    if(WaveLen != sgWFLen)
    {
      printf("Inconsistency between length of waveform in the data vs. param file. -> Quitting\n");
      return 1;
    }
    BaselineOffset = ((int)(0.2*WaveLen));
    RoughBaseROI = ((int)(WaveLen/2-BaselineOffset));
  }

	//Script Configuration  ----------------------------------------------------
	//General constants: Definitions & Experimental setups
	const double wTBin = 0.000000004; // [s] Width of a time bin is 4ns = 0.004 us

	//FFT flag:
	const int FFT_Flag = 1;

	//General script Variables  -------------------------------------------------
	int i,j,k,l,a;
	int indexDum,pixelDum,pixelIndex,WaveNum,iLength,pixelBin;

  float BaselineAvg;
	float StoreBinDum,TempBinDum;

	string line;
	//---------------------------------------------------------------------------

  //TTree related Variables  --------------------------------------------------
  int hWaveLen = ((int)(WaveLen/2));
  int PixelID,SSDiD;
  double *Freq = new double[hWaveLen];
  double *Amp = new double[hWaveLen];

  stringstream hWLenSS;
  hWLenSS.str("");
  hWLenSS << hWaveLen;
  string hWLenS = hWLenSS.str();
  string FrArg = "Freq["+hWLenS+"]/D";
  string AmArg = "Amp["+hWLenS+"]/D";

  TTree *TrFFT = new TTree("TrFFT","Tree File with FFT");
  TrFFT->Branch("PixelID",&PixelID,"PixelID/I");
  TrFFT->Branch("SSDiD",&SSDiD,"SSDiD/I");
  TrFFT->Branch("Freq",Freq,FrArg.c_str());
  TrFFT->Branch("Amp",Amp,AmArg.c_str());
  //---------------------------------------------------------------------------

	//Binary data file related Variables-----------------------------------------
	int Result, Board, Channel, Length, EventID;
	unsigned long int Timestamp;
	short dummy;
	int chNum;
	short cut=0;

	short *WaveformAll = new short[WaveLen];

	TH1 *hFFT_Half =0;
	TH1 *hFFT_Full =0;

  TH1F *hDec = new TH1F("hDec","Detector Hits Histogram;Detector Pixel ID;",hNum,bLow,bHigh);

	TH1F *h1 = new TH1F("h1","Sample Waveform: Baseline ROI;Time [s];Amp [ADC ch]",(RoughBaseROI),-((double)(wTBin))/2.0,((double)(wTBin*RoughBaseROI))-((double)(wTBin))/2.0);
  TH1F *h1Scale = new TH1F("h1Scale","Scaled Sample Waveform FFT: Baseline ROI;Frequency [Hz];Amp_{P-P} [ADC ch]", RoughBaseROI/2, 0.0, 0.5/wTBin);
	TH2F *b2D = new TH2F("b2D","Baseline Waveform FFT vs. Pixel ID;Frequency [Hz];Pixel ID;Amp_{P-P} [ADC ch]", RoughBaseROI/2, 0.0, 0.5/wTBin,hNum,bLow,bHigh);
	TH2F *b2DT = new TH2F("b2DT","Baseline Waveform FFT vs. Event ID;Frequency [Hz];Event ID;Amp_{P-P} [ADC ch]", RoughBaseROI/2, 0.0, 0.5/wTBin,2000,0.0,2000.0);

	TH1F *h3 = new TH1F("h3","Sample Waveform: Full Length;Time [s];Amp [ADC ch]",(WaveLen),-((double)(wTBin))/2.0,((double)(wTBin*WaveLen))-((double)(wTBin))/2.0);
  TH1F *h3Scale = new TH1F("h3Scale","Sample Waveform FFT: Full Length;Frequency [Hz];Amp_{P-P} [ADC ch]", hWaveLen, 0.0, 0.5/wTBin);
	TH2F *f2D = new TH2F("f2D","Scaled Full Waveform FFT vs. Pixel ID;Frequency [Hz];Pixel ID;Amp_{P-P} [ADC ch]", hWaveLen, 0.0, 0.5/wTBin,hNum,bLow,bHigh);

  f2D->Reset("ICESM");
  b2D->Reset("ICESM");
  b2DT->Reset("ICESM");

	TVirtualFFT::SetTransform(0);
	//---------------------------------------------------------------------------

	//Displaying File info
  filein = filein+dType;
	printf("Analyzing: %s | Length of waveform: %d\n",filein.c_str(),WaveLen);

	//Loading the waveforms into the array & bare back baseline probe lines here
	WaveNum=0;
	ifstream finRH2(filein.c_str(),ios::binary);

  unsigned int hversion,hlength;
  char GBs;
  char GBh;
  int GBsLength,GBhLength;
  unsigned long int sTime;
  unsigned long int fTime;
  unsigned long int rimestamp;
  unsigned short pOffset;

  finRH2.read((char*)&hversion,4);
  printf("Version number: %d\n",hversion);
  finRH2.read((char*)&hlength,4);
  printf("Header length: %d\n",hlength);
  finRH2.read((char*)&GBsLength,4);
  printf("Length of Git Branch String: %d\n", GBsLength);
  finRH2.read((char*)&GBs,GBsLength);
  printf("String of Git Branch: %s\n", &GBs);
  finRH2.read((char*)&GBhLength,4);
  printf("Length of Git Branch hash String: %d\n", GBhLength);
  finRH2.read((char*)&GBh,GBhLength);
  printf("String of Git hash: %s\n", &GBh);
  finRH2.read((char*)&sTime,8);
  printf("Time: %lu\n", sTime);
  finRH2.read((char*)&sTime,8);
  printf("Time: %lu\n", sTime);
  finRH2.read((char*)&fTime,8);
  printf("FPGA time: %lu\n", fTime);
  finRH2.read((char*)&pOffset,2);

  while(finRH2.read((char*)&Result,1) && finRH2.read((char*)&EventID,4) && finRH2.read((char*)&Board,4) && finRH2.read((char*)&Channel,4) && finRH2.read((char*)&Timestamp,8) && finRH2.read((char*)&rimestamp,8) && finRH2.read((char*)&Length,4) && finRH2.read((char*)WaveformAll,(WaveLen*2)))
  {
    if(Length != WaveLen)
    {
      printf("Change in length of waveform detected. Script not equipped to handle this. Yet. -> Quitting\n");
      break;
    }
		chNum=(Board*8+Channel);
		if((chNum<0) && (chNum>(FullDAQ-1)) )
		{
			printf("Invalid DAQ channel number.. -> Skipping %dth Waveform.\n", WaveNum);
			continue;
		}
		pixelIndex = ChtoPx[chNum];
    pixelBin = hDec->FindBin( ((double) (pixelIndex)) );
    // if((pixelIndex<0) && (pixelIndex>(FullDAQ-1)))
		// {
		// 	printf("Invalid pixel ID.. -> Skipping %dth Waveform.\n", WaveNum);
		// 	continue;
		// }
		nSet[pixelBin] += 1;

		//Shifting the waveform to roll over
    BaselineAvg = 0.0;
		for(i=0;i<WaveLen;i++)
    {
			WaveformAll[i]-=16384*(WaveformAll[i]/8192);

			if(i<RoughBaseROI) BaselineAvg += WaveformAll[i];
		}
    BaselineAvg /= ((float)(RoughBaseROI));

    //Filling the histogram
    h1->Reset("ICESM");
		h3->Reset("ICESM");
    for(i=0;i<WaveLen;i++)
    {
			WaveformAll[i]-=BaselineAvg;

			h3->SetBinContent( (i+1),((float)(WaveformAll[i])) );
			if(i<RoughBaseROI) h1->SetBinContent( (i+1),((float)(WaveformAll[i])) );
		}

		hFFT_Half = h1->FFT(hFFT_Half, "MAG");
    hFFT_Half->SetName("hFT1");
    hFFT_Half->SetTitle("Raw FFT of the Sample Waveform: Baseline ROI; Frequency [4E-9*hLength*Hz];Amplitude_{P-P} [ADC*hLength/4]");

		hFFT_Full = h3->FFT(hFFT_Full, "MAG");
    hFFT_Full->SetName("hFT3");
    hFFT_Full->SetTitle("Raw FFT of the Sample Waveform: Full Length; Frequency [4E-9*fLength*Hz];Amplitude_{P-P} [ADC*fLength/4]");

		for(i=1;i<=hWaveLen;i++)
    {
			StoreBinDum = f2D->GetBinContent(i,pixelBin);
			TempBinDum = hFFT_Full->GetBinContent(i);

      TempBinDum *= (4.0/((double)(WaveLen)));  //Prefactor of 4 comes from the denominator being wavelen/2 and additional factor of 2 to make it p-p amplitude.
			StoreBinDum += TempBinDum;
			f2D->SetBinContent(i,pixelBin,StoreBinDum);
		}

		for(i=1;i<=RoughBaseROI/2;i++)
    {
			StoreBinDum = b2D->GetBinContent(i,pixelBin);
			TempBinDum = hFFT_Half->GetBinContent(i);

      TempBinDum *= (4.0/((double)(RoughBaseROI)));  //Prefactor of 4 comes from the denominator being wavelen/2 and additional factor of 2 to make it p-p amplitude.
			StoreBinDum += TempBinDum;
			b2D->SetBinContent(i,pixelBin,StoreBinDum);
      b2DT->SetBinContent(i,WaveNum,TempBinDum);
		}
		WaveNum++;
	}
	finRH2.close();

  printf("Read Done\n");

  for(i=1;i<=hWaveLen;i++)
  {
    TempBinDum = hFFT_Full->GetBinContent(i);
    TempBinDum *= (4.0/((double)(WaveLen)));
    h3Scale->SetBinContent(i,TempBinDum);
  }
  for(i=1;i<=RoughBaseROI/2;i++)
  {
    TempBinDum = hFFT_Half->GetBinContent(i);
    TempBinDum *= (4.0/((double)(RoughBaseROI)));
    h1Scale->SetBinContent(i,TempBinDum);
  }

  for(i=0;i<FullDAQ;i++)
  {
    // printf("Beginning of the loop\n");
    pixelIndex = ChtoPx[i];
    pixelBin = hDec->FindBin( ((double) (pixelIndex)) );
    if(pixelIndex>0)
    {
      SSDiD = PxtoSSD[pixelIndex];
      // printf("2 SSD Contact: %d | Pixel: %d | DAQ BC: %d\n", SSDiD,pixelIndex,i);
      if(SSDFillFlag[SSDiD] == 1)
      {
        printf("Duplicate SSD contact detected: SSD Contact: %d | Pixel: %d | DAQ BC: %d\n", SSDiD,pixelIndex,i);
      }
      else
      {
        SSDFillFlag[SSDiD] = 1;
      }
      // printf("DAQ BC: %d | Pixel: %d\n", i,ChtoPx[i]);
      // printf("Pixel: %d | Constact: %d\n", pixelIndex,SSDiD);
    }
    // printf("1 SSD Contact: %d | Pixel: %d | DAQ BC: %d\n", SSDiD,pixelIndex,i);

    for(j=1;j<=hWaveLen;j++)
    {
      StoreBinDum = f2D->GetBinContent(j,pixelBin);
      if(nSet[pixelBin]>0) StoreBinDum /= (nSet[pixelBin]);
      f2D->SetBinContent(j,pixelBin,StoreBinDum);

      PixelID = pixelIndex;
      Amp[j-1] = StoreBinDum;
      Freq[j-1] = ((double)(j))*0.5/wTBin/((double)(WaveLen/2));
    }
    if(pixelIndex>0) TrFFT->Fill();
    // printf("End of the loop\n");
  }

  for(i=0;i<FullDAQ;i++)
  {
    pixelIndex = ChtoPx[i];
    pixelBin = hDec->FindBin( ((double) (pixelIndex)) );

    // printf("Pixel %d has %d number of Waveforms\n",pixelIndex,nSet[pixelIndex] );
    for(j=1;j<=RoughBaseROI/2;j++)
    {
      StoreBinDum = b2D->GetBinContent(j,pixelBin);
      if(nSet[pixelBin]>0) StoreBinDum /= (nSet[pixelBin]);
      b2D->SetBinContent(j,pixelBin,StoreBinDum);
    }
  }

  TFile f0(HistoOut.c_str(),"recreate");  //Saving the analysis result
	h1->Write();
  b2D->Write();
  b2DT->Write();
  hFFT_Half->Write();
  h1Scale->Write();
  h3->Write();
  f2D->Write();
  hFFT_Full->Write();
  h3Scale->Write();
	f0.Close();

  TFile f2(fileout.c_str(),"recreate");  //Saving the analysis result
	TrFFT->Write();
	f2.Close();


  delete [] nSet;
  delete [] ChtoPx;
  delete [] PxtoSSD;
  delete [] SSDFillFlag;
	delete [] WaveformAll;/**/

	return 0;
}
