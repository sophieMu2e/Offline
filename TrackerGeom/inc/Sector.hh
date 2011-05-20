#ifndef TrackerGeom_Sector_hh
#define TrackerGeom_Sector_hh
//
// Holds information about one sector in a tracker.
//

//
// $Id: Sector.hh,v 1.9 2011/05/20 19:18:44 wb Exp $
// $Author: wb $
// $Date: 2011/05/20 19:18:44 $
//
// Original author Rob Kutschke
//

#include <algorithm>
#include <vector>

#include "TrackerGeom/inc/Layer.hh"
#include "TrackerGeom/inc/SectorId.hh"

#include "CLHEP/Vector/ThreeVector.h"
#ifndef __CINT__
#include "boost/bind.hpp"
#endif


namespace mu2e {

  class Tracker;

  class Sector{


    friend class Device;
    friend class LTracker;
    friend class TTracker;
    friend class LTrackerMaker;
    friend class TTrackerMaker;

  public:

    Sector():_id(SectorId(-1,-1)){};
    Sector( const SectorId& id ):_id(id){};

    // Accept the compiler generated destructor, copy constructor and assignment operators

    const SectorId& Id() const { return _id;}

    const std::vector<Layer>& getLayers() const{
      return _layers;
    }

    int nLayers() const{
      return _layers.size();
    }

    const Layer& getLayer ( int n ) const {
      return _layers.at(n);
    }

    const Layer& getLayer ( const LayerId& lid) const {
      return _layers.at(lid.getLayer());
    }

    const Straw& getStraw ( const StrawId& sid ) const{
      return _layers.at(sid.getLayer()).getStraw(sid);
    }

    // Formatted string embedding the id of the sector.
    std::string name( std::string const& base ) const;

    const std::vector<double>& boxHalfLengths() const { return _boxHalfLengths; }

    const double         boxRxAngle()     const { return _boxRxAngle;     }
    const double         boxRyAngle()     const { return _boxRyAngle;     }
    const double         boxRzAngle()     const { return _boxRzAngle;     }
    const CLHEP::Hep3Vector&    boxOffset()      const { return _boxOffset;      }

    std::vector<CLHEP::Hep3Vector> const& getBasePosition() const{
      return _basePosition;
    }

    CLHEP::Hep3Vector const& getBaseDelta() const{
      return _baseDelta;
    }

    // On readback from persistency, recursively recompute mutable members.
    void fillPointers ( const Tracker& tracker ) const;

#ifndef __CINT__

    template <class F>
    void for_each_layer( F f) const{
      std::for_each ( _layers.begin(),
                      _layers.end(),
                      f);
    }

    template <class F>
    void for_each_straw( F f) const {
      for_each_layer( boost::bind( Layer::for_each<F>, _1, f));
    }

    // Loop over all straws and call F.
    // F can be a class with an operator() or a free function.
    template <class F>
    inline void forAllStraws ( F& f) const{
      for ( std::vector<Layer>::const_iterator i=_layers.begin(), e=_layers.end();
            i !=e; ++i){
        i->forAllStraws(f);
      }
    }

    template <class F>
    inline void forAllLayers ( F& f) const{
      for ( std::vector<Layer>::const_iterator i=_layers.begin(), e=_layers.end();
            i !=e; ++i){
        f(*i);
      }
    }


#endif

  protected:

    SectorId _id;
    std::vector<Layer> _layers;

    // Vertices of enclosing polygon.
    std::vector<CLHEP::Hep3Vector> corners;

    // Properties of the enclosing logical volume (box).

    // Half lengths of the logical box.
    std::vector<double> _boxHalfLengths;

    std::vector<CLHEP::Hep3Vector> _basePosition;
    CLHEP::Hep3Vector _baseDelta;

    // Rotations and offsets to place the logical box.
    // placedshape = ( offset + RZ*RX*RY*shape );
    //
    double _boxRxAngle;
    double _boxRyAngle;
    double _boxRzAngle;
    CLHEP::Hep3Vector _boxOffset;

  };

}  //namespace mu2e
#endif /* TrackerGeom_Sector_hh */
