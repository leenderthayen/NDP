TTree t("t","t");
int Result, Board, Channel, Length, EventID;
unsigned long int Timestamp;
short Waveform[20000];

void fill_tree(TString filein)
{
  t.Branch("Timestamp",&Timestamp,"Timestamp/l");
  t.Branch("EventID",&EventID,"EventID/I");
  t.Branch("Board",&Board,"Board/I");
  t.Branch("Channel",&Channel,"Channel/I");
  t.Branch("Waveform",Waveform,"Waveform[20000]/S");
  ifstream fin(filein.Data(),ios::binary);
  while(fin.read((char*)&Result,1) && fin.read((char*)&EventID,4) && fin.read((char*)&Board,4) && fin.read((char*)&Channel,4) && fin.read((char*)&Timestamp,8) && fin.read((char*)&Length,4) && fin.read((char*)Waveform,40000))
    {
      for(int i=0; i<20000; i++)
	Waveform[i]-=16384*(Waveform[i]/8192);
      t.Fill();
      Result=0;
    }
  fin.close();
  return;
}
