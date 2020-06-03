#ifndef NDDPixelReadOut_h
#define NDDPixelReadOut_h 1

#include "G4VUserParallelWorld.hh"

class NDDPixelReadOut : public G4VUserParallelWorld {
public:
  NDDPixelReadOut(G4String&);
  virtual ~NDDPixelReadOut();

protected:
  virtual void Construct();
  virtual void ConstructSD();
};

#endif
