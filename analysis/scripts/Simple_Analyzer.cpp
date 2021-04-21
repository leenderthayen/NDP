TTree t("t","t");
int Result, Board, Channel, Length, EventID,GBsLength,GBhLength;
unsigned int hversion,hlength;
char GBs;
char GBh;
unsigned long int rimestamp;
unsigned long int Timestamp;
unsigned long int dTimestamp;
unsigned long int oTimestamp;
unsigned long int fTime;
unsigned short pOffset;
unsigned long int sTime;
short *Waveform;
float *avgWaveform;
short dummy;
float *xval;
int chNum;
float hourTime;
float WFDiff;
float WFDMax;
int risingEdge;
float baselineVal;
float overshootVal;
float midVal;
float lastVal;
short cut=0;
int WaveIndex;

void Simple_Analyzer(TString filein,int waveLen)
{
  int i;
  int wavenum=0;
  Waveform = new short[waveLen];

  avgWaveform = new float[waveLen];
  xval = new float[waveLen];

  for(i=0; i<waveLen; i++)
  {
    //printf("%d\n",i );
    avgWaveform[i]=0.0;
    xval[i]=((float)(i));
    //fprintf(fp,"%d\n",Waveform[i]);
  }

  //t.Branch("Timestamp",&Timestamp,"Timestamp/l");
  t.Branch("Time",&hourTime,"hourTime/F");
  //t.Branch("EventID",&EventID,"EventID/I");
  //t.Branch("Board",&Board,"Board/I");
  t.Branch("Channel",&chNum,"chNum/I");
  t.Branch("risingEdge",&risingEdge,"risingEdge/I");
  t.Branch("baseline",&baselineVal,"baselineVal/F");
  t.Branch("overshoot",&overshootVal,"overshootVal/F");
  t.Branch("mid",&midVal,"midVal/F");
  t.Branch("last",&lastVal,"lastVal/F");
  t.Branch("WaveIndex",&WaveIndex,"WaveIndex/I");
  t.Branch("dTimestamp",&dTimestamp,"dTimestamp/l");
  stringstream leng;
  stringstream ChLeng;

  leng.str("");
  leng << waveLen;

  string waveLenString = leng.str();
  string fileArg;
  string argument = "Waveform["+waveLenString+"]/S";

  //t.Branch("Waveform",Waveform,argument.c_str());

  argument = "xval["+waveLenString+"]/I";
  //t.Branch("xval",xval,argument.c_str());

  FILE* fp;
  ifstream fin(filein.Data(),ios::binary);


  fin.read((char*)&hversion,4);
  printf("Version number: %d\n",hversion);
  fin.read((char*)&hlength,4);
  printf("Header length: %d\n",hlength);
  fin.read((char*)&GBsLength,4);
  printf("Length of Git Branch String: %d\n", GBsLength);
  fin.read((char*)&GBs,GBsLength);
  printf("String of Git Branch: %s\n", &GBs);
  fin.read((char*)&GBhLength,4);
  printf("Length of Git Branch hash String: %d\n", GBhLength);
  fin.read((char*)&GBh,GBhLength);
  printf("String of Git hash: %s\n", &GBh);
  fin.read((char*)&sTime,8);
  printf("Time: %lu\n", sTime);
  fin.read((char*)&sTime,8);
  printf("Time: %lu\n", sTime);
  fin.read((char*)&fTime,8);
  printf("FPGA time: %lu\n", fTime);
  fin.read((char*)&pOffset,2);

  oTimestamp=0;
  while(fin.read((char*)&Result,1) && fin.read((char*)&EventID,4) && fin.read((char*)&Board,4) && fin.read((char*)&Channel,4) && fin.read((char*)&Timestamp,8) && fin.read((char*)&rimestamp,8) && fin.read((char*)&Length,4) && fin.read((char*)Waveform,(waveLen*2)))
  {
    hourTime=Timestamp*0.000000004/3600.0;

    WaveIndex=(wavenum%600);
    dTimestamp=Timestamp-oTimestamp;

    oTimestamp=Timestamp;


    //printf("Length %d\n", Length);
    chNum=(Board*8+Channel);
    for(i=0; i<waveLen; i++)
    {
      //printf("%d\n",i );
  		Waveform[i]-=16384*(Waveform[i]/8192);
      //fprintf(fp,"%d\n",Waveform[i]);
    }

    WFDMax=0.0;
    for(i=1; i<waveLen; i++)
    {
      WFDiff=(Waveform[i]-Waveform[i-1]);
      if(WFDMax<WFDiff)
      {
        WFDMax=WFDiff;
        risingEdge=i;
      }
    }

    baselineVal=0.0;
    for(i=500; i<3000; i++)
    {
      baselineVal+=(Waveform[i]);
    }
    baselineVal/=((double)(3000-500));

    for(i=0; i<waveLen; i++)
    {
      avgWaveform[i]+=(((float)(Waveform[i]))-baselineVal);
    }


    overshootVal=0.0;
    for(i=3750; i<4750; i++)
    {
      overshootVal+=(Waveform[i]);
    }
    overshootVal/=((double)(4750-3750));
    overshootVal-=baselineVal;

    midVal=0.0;
    for(i=31250; i<37500; i++)
    {
      midVal+=(Waveform[i]);
    }
    midVal/=((double)(37500-31250));
    midVal-=baselineVal;

    lastVal=0.0;
    for(i=57500; i<65000; i++)
    {
      lastVal+=(Waveform[i]);
    }
    lastVal/=((double)(65000-57500));
    lastVal-=baselineVal;

    if(wavenum>0) t.Fill();
    //Result=0;
    wavenum++;
    //if(wavenum>1) break;
    //fclose(fp);
  }

  float grx[16]={500.0,500.0,3000.0,3000.0,3750.0,3750.0,4750.0,4750.0,31250.0,31250.0,37500.0,37500.0,57500.0,57500.0,65000.0,65000.0};
  float gry[16]={-100.0,7000.0,7000.0,-100.0,-100.0,7000.0,7000.0,-100.0,-100.0,7000.0,7000.0,-100.0,-100.0,7000.0,7000.0,-100.0};

  for(i=0; i<waveLen; i++)
  {
    avgWaveform[i]/=((float)(wavenum));
    xval[i]*=0.004;
  }
  for(i=0; i<16; i++)
  {
    grx[i]*=0.004;;
  }

  TCanvas *canvas = new TCanvas("canvas", "Single photon pulse height distribution of uPMT", 200, 10, 700, 500);
  canvas->SetGrid();
  TGraph *gr = new TGraph(waveLen,xval,avgWaveform);
  TGraph *grCur = new TGraph(16,grx,gry);
  gr->Draw("A*");
  //grCur->Draw("* SAME");
  canvas->Update();
  fin.close();

  TString fileout = filein+".root";       // Name of output root file
	TFile f0(fileout,"recreate");
  t.Write();
  gr->Write();
	f0.Close();

  return;
}
