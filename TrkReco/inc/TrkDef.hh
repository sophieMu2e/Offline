// BTrk includes
#include "BTrk/BaBar/BaBar.hh"
#include "BTrk/TrkBase/HelixTraj.hh"
#include "BTrk/TrkBase/CosmicLineTraj.hh"
#include "BTrk/TrkBase/TrkParticle.hh"
#include "BTrk/TrkBase/TrkT0.hh"
// CLHEP
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
class TrkDifPieceTraj;
namespace mu2e 
{
  class TrkDef {
  public:
    TrkDef(TimeCluster const& tcluster, HelixTraj const& helix,
      TrkParticle const& tpart, TrkFitDirection const& fdir);
    TrkDef(TimeCluster const& tcluster, CosmicLineTraj const& helix,
      TrkParticle const& tpart, TrkFitDirection const& fdir);
    TrkDef(const TrkDef&);
    TrkDef& operator = (const TrkDef&);
    ~TrkDef();
  // append a straw hit to this track definition
    void appendHit(size_t index) { _timeCluster._strawHitIdxs.push_back(index); }
  // accessors
    std::vector<StrawHitIndex> const& strawHitIndices() const { return _timeCluster._strawHitIdxs;}
    HelixTraj const& helix() const { return _h0; }
    CosmicLineTraj const& cosmic() const { return _c0;}
    TrkT0 const& t0() const { return _timeCluster._t0; }
    TrkParticle const& particle() const { return _tpart; }
    TrkFitDirection const& fitdir() const { return _fdir; }
    //non-const accessors to allow updates
    std::vector<StrawHitIndex>& strawHitIndices() { return _timeCluster._strawHitIdxs;}
    HelixTraj& helix() { return _h0; }
    CosmicLineTraj& cosmic() { return _c0;}
    TrkT0& t0() { return _timeCluster._t0; }
  private:
    TimeCluster _timeCluster; // t0 and hit indices
    HelixTraj _h0; // helix estimate, valid in the region around z=0
    CosmicLineTraj _c0; //comsic version
    TrkParticle _tpart; // particle type.  Note this defines both the charge and the mass
    TrkFitDirection _fdir; // fit direction
  };
}
#endif
