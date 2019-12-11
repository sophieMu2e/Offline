#include "BTrk/BaBar/BaBar.hh"
#include <assert.h>
#include <math.h>
#include <limits.h>

#include "BTrk/BaBar/Constants.hh"
#include "BTrk/BbrGeom/HepPoint.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "BTrk/TrkBase/HelixTraj.hh"
#include "CosmicReco/inc/CosmicTrkVisitor.hh"
#include "BTrk/difAlgebra/DifNumber.hh"
#include "BTrk/difAlgebra/DifPoint.hh"
#include "BTrk/difAlgebra/DifVector.hh"
#include "BTrk/BbrGeom/BbrAngle.hh"
#include "CosmicReco/inc/CosmicLineParams.hh"
#include "CosmicReco/inc/CosmicLineTraj.hh"
#include "CosmicReco/inc/CosmicTrkMomVisitor.hh"
#include "BTrk/BaBar/ErrLog.hh"
using std::endl;
using std::ostream;
using namespace CLHEP;
#ifndef M_2PI
#define M_2PI 2*M_PI
#endif

//reference point --> is this the center of the tracker system?

CosmicLineTraj::CosmicLineTraj(const HepVector& pvec, const HepSymMatrix& pcov,
                     double lowlim, double hilim, const HepPoint& refpoint) :
  TrkSimpTraj(pvec, pcov, lowlim,hilim,refpoint)
{
  //  Make sure the dimensions of the input matrix and vector are correct

  if( pvec.num_row() != NHLXPRM || pcov.num_row() != NHLXPRM ){
    ErrMsg(fatal) 
      << "CosmicLineTraj: incorrect constructor vector/matrix dimension" << endmsg;
  }

}


CosmicLineTraj::CosmicLineTraj(const CosmicLineParams& inpar,
                     double lowlim, double hilim, const HepPoint& refpoint) :
  TrkSimpTraj(inpar.params(), inpar.covariance(), lowlim,hilim,refpoint)
{
  
}

CosmicLineTraj::CosmicLineTraj(const TrkParams& inpar,
                     double lowlim, double hilim, const HepPoint& refpoint) :
  TrkSimpTraj(inpar, lowlim,hilim,refpoint)
{
  assert(inpar.parameter().num_row()==NHLXPRM);
}

CosmicLineTraj::CosmicLineTraj( const CosmicLineTraj& h )
  : TrkSimpTraj(h.parameters()->parameter(), h.parameters()->covariance(), 
                h.lowRange(),h.hiRange(),h.referencePoint())
{
}

CosmicLineTraj*
CosmicLineTraj::clone() const
{
  return new CosmicLineTraj(*this);
}

CosmicLineTraj&
CosmicLineTraj::operator=(const CosmicLineTraj& h)
{
  if( &h != this ){
    Trajectory::operator=(h);
    _dtparams = *(h.parameters());
    _refpoint = h._refpoint;
  }
  return *this;
}

CosmicLineTraj::~CosmicLineTraj()
{
}

double
CosmicLineTraj::z(const double& f) const //TODO 
{
  return  f + referencePoint().z();
}

double
CosmicLineTraj::zFlight(double zpos, double z0) const { //TODO 
  return (zpos - z0);
}

HepPoint
CosmicLineTraj::position(double POCA_Z) const //TODO f ->>POCA_Z
{
  double sphi0 = sin(phi0());
  double cphi0 = cos(phi0());
 
  double x_pos = -1*d0()*sphi0+referencePoint().x();
  double y_pos = d0()*cphi0+referencePoint().y();
  double z_pos = POCA_Z+referencePoint().z();
  return HepPoint(x_pos, y_pos, z_pos);
}


Hep3Vector
CosmicLineTraj::direction( double f) const 
{

double x_dir = cos(phi());
double y_dir = sin(theta());
double z_dir = cos(theta());
 
 return Hep3Vector (x_dir, y_dir, z_dir);
}

Hep3Vector
CosmicLineTraj::delDirect( double fltLen ) const  //assume cosntant at the moment
{
 return Hep3Vector(0,0, 0.0);
}

double
CosmicLineTraj::distTo1stError(double s, double tol, int pathDir) const 
{
  return 9999;
}

double
CosmicLineTraj::distTo2ndError(double s, double tol, int pathDir) const 
{
  return 9999;
}

void
CosmicLineTraj::getInfo(double fltLen, HepPoint& pos, Hep3Vector& dir) const
{
  pos = position(fltLen);
  dir = direction(fltLen);
}

void
CosmicLineTraj::getInfo(double fltLen, HepPoint& pos, Hep3Vector& dir, 
                   Hep3Vector& delDir) const //delDir? We assume constant vel.
{
  pos = position(fltLen);
  dir = direction(fltLen);
}
/*
HepMatrix
CosmicLineTraj::derivDeflect(double fltlen,deflectDirection idirect) const //TODO
{
//
//  This function computes the column matrix of derrivatives for the change
//  in parameters for a change in the direction of a track at a point along
//  its flight, holding the momentum and position constant.  The effects for
//  changes in 2 perpendicular directions (theta1 = dip and
//  theta2 = phi*cos(dip)) can sometimes be added, as scattering in these
//  are uncorrelated.
//
  HepMatrix ddflct(NHLXPRM,1);
//
//  Compute some common things
//
  //double omeg = omega();
  //double tand = tanDip();
  double arcl = 1;//arc(fltlen);
  double dx = 1;//cos(arcl);
  double dy = 1;//sin(arcl);
  //double cosd = cosDip();
  //double darc = omeg*d0();
//
//  Go through the parameters
//
  switch (idirect) {
  case theta1:
    
    ddflct(thetaIndex+1,1) = 1;//1.0/pow(cosd,2);
    ddflct(d0Index+1,1) = 1;//(1-dx)*tand/omeg;
    ddflct(phi0Index+1,1) =  1;//-dy*tand/(1+darc);
    ddflct(phiIndex+1,1) = 1;//- translen(fltlen) - pow(tand,2)*dy/(omeg*(1+darc));
    break;
  case theta2:
    
    ddflct(thetaIndex+1,1) = 0;
    ddflct(d0Index+1,1) = 1;//-dy/(cosd*omeg);
    ddflct(phi0Index+1,1) = 1;//dx/(cosd*(1+darc));
    ddflct(phiIndex+1,1) = 1;//-tand*(1- dx/(1+darc))/(cosd*omeg);
    break;
  }

  return ddflct;
}


HepMatrix
CosmicLineTraj::derivDisplace(double fltlen,deflectDirection idirect) const //TODO
{
//
//  This function computes the column matrix of derrivatives for the change
//  in parameters for a change in the position of a track at a point along
//  its flight, holding the momentum and direction constant.  The effects for
//  changes in 2 perpendicular directions 'theta1' = (-sin(l)cos(p),-sin(l)sin(p),cos(l)) and
//  'theta2' = (-sin(p),cos(p),0).  These are by definition orthogonal and uncorrelated.
//  these displacements are correlated with the angular change above
//
  HepMatrix ddflct(NHLXPRM,1);
//
//  Compute some common things
//
  double omeg = 1;//omega();
  double tand = 1;//tanDip();
  double arcl = 1;//arc(fltlen);
  double dx = 1;//cos(arcl);
  double dy =1;//in(arcl);
  double cosd = 1;//cosDip();
  double sind = 1;//sinDip();
  double darc_1 = 1;//1.0+omeg*d0();
//
//  Go through the parameters
//
  switch (idirect) {
  case theta1:
   
    ddflct(thetaIndex+1,1) = 1;
    ddflct(d0Index+1,1) = 1;//-sind*dy;
    ddflct(phi0Index+1,1) = 1;//sind*dx*omeg/darc_1;
    ddflct(phiIndex+1,1) = 1;//sind*tand*dx/darc_1 + cosd;
    break;
  case theta2:
    
    ddflct(thetaIndex+1,1) = 1;
    ddflct(d0Index+1,1) = 1;//dx;
    ddflct(phi0Index+1,1) = 1;//dy*omeg/darc_1;
    ddflct(phiIndex+1,1) = 1;//tand*dy/darc_1;
    break;
  }

  return ddflct;
}
*/
void
CosmicLineTraj::getDFInfo(double flt, DifPoint& pos, DifVector& dir,
                     DifVector& delDir) const
{
  //Provides difNum version of information for calculation of derivatives.
  //  All arithmetic operations have been replaced by +=, etc. versions 
  //  for speed.

  // Create difNumber versions of parameters
  
  DifNumber phi0Df(phi0(), phi0Index+1, NHLXPRM);
  phi0Df.setIndepPar( parameters() );
  DifNumber d0Df(d0(), d0Index+1, NHLXPRM);
  d0Df.setIndepPar( parameters() );
  DifNumber thetaDf(theta(), thetaIndex+1, NHLXPRM);
  thetaDf.setIndepPar( parameters() );
  DifNumber phiDf(phi(), phiIndex+1, NHLXPRM);
  phiDf.setIndepPar( parameters() );

  DifNumber dipDf = thetaDf;

  static DifNumber cTheta; //tan(lambda) = 1/tan(theta)
  dipDf.cosAndSin(cTheta, dir.z);//For Z direction	
  static DifNumber sinPhi0, cosPhi0; //For tranverse plane
  phi0Df.cosAndSin(cosPhi0, sinPhi0);

  bool lref = (referencePoint().x() != 0. || referencePoint().y() != 0. ||
               referencePoint().z() != 0.);


  DifNumber alphaDf = cTheta;
  alphaDf *= flt;
  alphaDf += phi0Df;

  alphaDf.mod(-Constants::pi, Constants::pi);
  alphaDf.cosAndSin(dir.x, dir.y);

  DifNumber temp = d0Df;
  temp *= sinPhi0;
  pos.x -= temp;

  temp = d0Df;
  temp *= cosPhi0;
  pos.y += temp;

  pos.z = flt;
  pos.z *= dir.z;
  //pos.z += z0Df;

  if (lref) {
    DifNumber px(referencePoint().x());
    DifNumber py(referencePoint().y());
    DifNumber pz(referencePoint().z());
    pos.x += px;
    pos.y += py;
    pos.z += pz;
  }

}



void CosmicLineTraj::getDFInfo2(double flt, DifPoint& pos, DifVector& dir) const //TODO
{
  //Provides difNum version of information for calculation of derivatives.
  //  All arithmetic operations have been replaced by +=, etc. versions 
  //  for speed.

  DifNumber phi0Df(phi0(), phi0Index+1, NHLXPRM);
  phi0Df.setIndepPar( parameters() );
  DifNumber d0Df(d0(), d0Index+1, NHLXPRM);
  d0Df.setIndepPar( parameters() );
  DifNumber thetaDf(theta(), thetaIndex+1, NHLXPRM);
  thetaDf.setIndepPar( parameters() );
  DifNumber phiDf(phi(), phiIndex+1, NHLXPRM);
  phiDf.setIndepPar( parameters() );

  DifNumber dipDf = thetaDf;

  static DifNumber cTheta; //tan(lambda) = 1/tan(theta)
  dipDf.cosAndSin(cTheta, dir.z);//For Z direction	
  static DifNumber sinPhi0, cosPhi0; //For tranverse plane
  phi0Df.cosAndSin(cosPhi0, sinPhi0);

  bool lref = (referencePoint().x() != 0. || referencePoint().y() != 0. ||
               referencePoint().z() != 0.);

  DifNumber alphaDf = cTheta;
  alphaDf *= flt;
  alphaDf += phi0Df;

  alphaDf.mod(-Constants::pi, Constants::pi);
  alphaDf.cosAndSin(dir.x, dir.y);

  DifNumber temp = d0Df;
  temp *= sinPhi0;
  pos.x -= temp;

  temp = d0Df;
  temp *= cosPhi0;
  pos.y += temp;

  pos.z = flt;
  pos.z *= dir.z;
  //pos.z += z0Df;

  if (lref) {
    DifNumber px(referencePoint().x());
    DifNumber py(referencePoint().y());
    DifNumber pz(referencePoint().z());
    pos.x += px;
    pos.y += py;
    pos.z += pz;
  }


}


double
CosmicLineTraj::curvature(double ) const   //TODO - we assume curvature not present so return 1
{

//  Compute the curvature as the magnitude of the 2nd derivative
//  of the position function with respect to the 3-d flight distance
//
  
return 1;
}

//double
//CosmicLineTraj::angle() const 
//{
//  return BbrAngle(parameters()->parameter()[phi0Index]).rad();
//}

void
CosmicLineTraj::paramFunc(const HepPoint& oldpoint,const HepPoint& newpoint,
                     const HepVector& oldvec,const HepSymMatrix& oldcov,
                     HepVector& newvec,HepSymMatrix& newcov,
                     double fltlen) {

  HepVector parvec(oldvec);
  newvec = parvec;

  double delx = newpoint.x()-oldpoint.x();
  double dely = newpoint.y()-oldpoint.y();
  //double delz = newpoint.z()-oldpoint.z();


  double delta = parvec[d0Index];
  double cos0 = cos(parvec[phi0Index]);
  double sin0 = sin(parvec[phi0Index]);

  double perp = delx*sin0-dely*cos0;
  double para = delx*cos0+dely*sin0;
  double oldphi = parvec[phi0Index];


// delta
  double newdelta2 = delta*delta + delx*delx + dely*dely + 2.0*delta*perp;
// assume delta, newdelta have the same sign
  double newdelta = delta>0 ? sqrt(newdelta2) : -sqrt(newdelta2);
  double invdelta = 1.0/newdelta;
  double invdelta2 = 1.0/newdelta2;

// d0
  newvec[d0Index] = newdelta;//-rad

// phi0; check that we don't get the wrong wrapping. Atan2 has 2Pi ambiguity, not pi
  double newphi = atan2(sin0+delx/delta,cos0-dely/delta);
  while(fabs(newphi - oldphi)>M_PI)
    if(newphi > oldphi)
      newphi -= M_2PI;
    else
      newphi += M_2PI;
  newvec[phi0Index] = newphi;
  //double delphi = newphi-parvec[phi0Index];


// now covariance: first, compute the rotation matrix
// start with 0: lots of terms are zero
  static HepMatrix covrot(NHLXPRM,NHLXPRM,0);

//theta:
  covrot(thetaIndex+1,thetaIndex+1) = 1.0;//TODO

// d0
  covrot(d0Index+1,d0Index+1) = invdelta*(delta + perp);
  covrot(d0Index+1,phi0Index+1) = delta*para*invdelta;

// phi0
  covrot(phi0Index+1,d0Index+1) = -para*invdelta2;
  covrot(phi0Index+1,phi0Index+1) = delta*(delta + perp)*invdelta2;

// phi
  covrot(phiIndex+1,phiIndex+1) = 1.0;//TODO

//  Apply the rotation
  newcov = oldcov.similarity(covrot);

}
//void
//CosmicLineTraj::visitAccept(CosmicTrkVisitor* vis) const
//{
// Visitor access--just use the Visitor class member function
 // vis->trkVisitCosmicTraj(this);
//}



void CosmicLineTraj::invertParams(TrkParams* params, std::vector<bool>& flags) const
{
  // Inverts parameters and returns true if the parameter inversion
  // requires a change in sign of elements in the covariance matrix

  for (unsigned iparam = 0; iparam < NHLXPRM; iparam++) {
    switch ( iparam ) {
    case d0Index:  // changes sign
    
    case phi0Index:  // changes by pi, but covariance matrix shouldn't change
      params->parameter()[iparam] =
        BbrAngle(params->parameter()[iparam] + Constants::pi);
      flags[iparam] = false;
      break;
    case thetaIndex:  //pi change - i think....
      params->parameter()[iparam] =
        BbrAngle(params->parameter()[iparam] + Constants::pi);
      flags[iparam] = false;
    case phiIndex: //pi change - i think ...
      params->parameter()[iparam] =
        BbrAngle(params->parameter()[iparam] + Constants::pi);
      flags[iparam] = false;
    }
  }
  return;
}

double
CosmicLineTraj::angle(const double& f) const //TODO
{
  return BbrAngle(phi0() );//+ arc(f));
}

void
CosmicLineTraj::printAll(ostream& os) const
{
  os  << "CosmicLineTraj with range "
      << lowRange() <<" to " << hiRange() << " and parameters " << endl
      << "d0= " << d0() << " phi0= "
      << phi0() << " theta = "
      << theta() << " phi =  "
      << phi()  << endl;
}

void
CosmicLineTraj::print(ostream& os) const
{
  Trajectory::print(os << "CosmicLineTraj" );
}
