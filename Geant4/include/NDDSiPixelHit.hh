/// \file NDDSiPixelHit.hh
/// \brief Definition of the NDDSiPixelHit class

#ifndef NDDSiPixelHit_h
#define NDDSiPixelHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

class NDDSiPixelHit : public G4VHit {
 public:
  NDDSiPixelHit();
  NDDSiPixelHit(const NDDSiPixelHit&);
  virtual ~NDDSiPixelHit();

  // operators
  const NDDSiPixelHit& operator=(const NDDSiPixelHit&);
  G4int operator==(const NDDSiPixelHit&) const;

  inline void* operator new(size_t);
  inline void operator delete(void*);

  // methods from base class
  virtual void Draw();
  virtual void Print();

  // Set methods
  void SetTrackID(G4int track) { trackID = track; };
  void SetEnDep(G4double de) { enDep = de; };
  void SetPos(G4ThreeVector xyz) { pos = xyz; };
  void SetTime(G4double t) { time = t; };
  void SetMomentum(G4ThreeVector mom) { momentum = mom; };
  void SetField(G4ThreeVector f) { field = f; };
  void SetParticleCode(G4int pc) { particleCode = pc; };
  void SetPixelNumber(G4int pn) { pixelNumber= pn; };
  void SetPixelName(G4String pn) { pixelName = pn; };

  // Get methods
  G4int GetTrackID() const { return trackID; };
  G4double GetEnDep() const { return enDep; };
  G4ThreeVector GetPos() const { return pos; };
  G4double GetTime() const { return time; };
  G4ThreeVector GetMomentum() const { return momentum; };
  G4ThreeVector GetField() const { return field; };
  G4int GetParticleCode() const { return particleCode; };
  G4int GetPixelNumber() const { return pixelNumber; };
  G4String GetPixelName() const { return pixelName; };

 private:
  G4int trackID;
  G4double enDep;
  G4ThreeVector pos;
  G4ThreeVector momentum;
  G4ThreeVector field;
  G4double time;
  G4int particleCode;
  G4int pixelNumber;
  G4String pixelName;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<NDDSiPixelHit> NDDSiPixelHitsCollection;

extern G4ThreadLocal G4Allocator<NDDSiPixelHit>* NDDSiPixelHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* NDDSiPixelHit::operator new(size_t) {
  if (!NDDSiPixelHitAllocator) NDDSiPixelHitAllocator = new G4Allocator<NDDSiPixelHit>;
  return (void*)NDDSiPixelHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void NDDSiPixelHit::operator delete(void* hit) {
  NDDSiPixelHitAllocator->FreeSingle((NDDSiPixelHit*)hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
