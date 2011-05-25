#ifndef Mu2eReflection_Mu2eReflection_hh
#define Mu2eReflection_Mu2eReflection_hh

//
// derived class that defines a reflection process in G4; reverses
// particle momentum and charge, used for debugging magnetic field
// code

//
// Original author R. Bernstein

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4VDiscreteProcess.hh"
#include "G4VParticleChange.hh"
#include "G4VPhysicalVolume.hh"
#include "Mu2eUtilities/inc/safeSqrt.hh"
#include "cetlib/pow.h"
#include <cmath>

namespace mu2e{

  class Mu2eReflection : public G4VDiscreteProcess{
  public:
    Mu2eReflection(
                   const G4String& startingVolume, //need to do the string; G4 may not have
                   const G4String& endingVolume,   //made the volume yet
                   const double toleranceForQuitting = 0.001*meter):
      G4VDiscreteProcess("Mu2eReflection",fUserDefined),
      _startingVolume(startingVolume),
      _endingVolume(endingVolume),
      _toleranceForQuitting(toleranceForQuitting),
      alreadyReflected(false)
    {
      //tell the user what I'm doing; could iterate over all volumes and check names exist
      G4cout << "Reflecting through following volumes"
             << G4endl
             << "starting Volume was: " <<startingVolume
             << "\n"
             << "ending Volume was:   " <<endingVolume
             << G4endl;
      pParticleChange = &fMu2eParticleChangeForReflection;

    }
    ~Mu2eReflection(){}

  public:
    const G4String _startingVolume;
    const G4String _endingVolume;
    double _toleranceForQuitting;
    G4bool IsApplicable(const G4ParticleDefinition& aParticleType);
    G4ParticleTable* particleTable;


  protected:
    // GetMeanFreePath returns ctau*beta*gamma for decay in flight
    virtual G4double GetMeanFreePath(const G4Track& aTrack,
                                     G4double   previousStepSize,
                                     G4ForceCondition* condition
                                     );

  private:

    G4ParticleChange fMu2eParticleChangeForReflection;

    G4VParticleChange* PostStepDoIt(const G4Track& aTrack, const G4Step& aStep);

    G4VParticleChange* ReflectIt (const G4Track& aTrack, const G4Step& aStep);
    G4VParticleChange* KillIt    (const G4Track& aTrack, const G4Step& aStep);
    G4VParticleChange* DoNothing (const G4Track& aTrack, const G4Step& aStep);

    Mu2eReflection( const Mu2eReflection& right);
    Mu2eReflection & operator=(const Mu2eReflection &right);

    bool alreadyReflected;

    G4ThreeVector startingVertex;

    double Distance(const G4ThreeVector a, const G4ThreeVector b)
    {
      return std::sqrt( cet::sum_of_squares( a.x() - b.x(),
                                             a.y() - b.y(),
                                             a.z() - b.z() ) );
    }

  };


} //end namespace mu2e

#endif /* Mu2eReflection_Mu2eReflection_hh */
