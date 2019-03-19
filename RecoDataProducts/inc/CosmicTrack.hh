#ifndef RecoDataProducts_CosmicTrack_hh
#define RecoDataProducts_CosmicTrack_hh
////S. Middleton, Feb 2019 - Cosmic track class, will store info of fit result in terms of m and c and errors.
#include "TMath.h"
#include "TMatrixD.h"
#include "DataProducts/inc/XYZVec.hh"
#include<vector>
#include<bitset>
/*

Cosmic track is parameterised by a paramteric equatrion r(t) were t is a vector of arbituary parameters.

e.g. r(t) = 	   x0	      a
		[  y0 ] + t*[ b ]    assuming 3D with 4 parameters then: 
		   z0         c	      
(first part is point on line, second is direction)
Assume all pass through vertical (assumption for cosmics ...not true)
r(t) = 	   x0	      a
	[  1 ] +  t*[ 0 ]    assuming 3D with 4 parameters then: 
	   z0 	      b     

our parameters are then x0, z0, a and b
*/
using namespace std;

namespace mu2e {
  
  class CosmicTrack{
  public:
   
    // Constructors
    CosmicTrack();
    
    CosmicTrack(double par_1, double par_2, double par_3, double par_4) : _a0(par_1), _a1(par_2), _b0(par_3), _b1(par_4){};

    CosmicTrack(std::vector<double> track_parameters) : _track_parameters(track_parameters){};

    
    // Destructor
    ~CosmicTrack();


    std::vector<std::vector<double>> Initialize_Cov_Mat(int i, int j, int sizei, int sizej);

 //---------------Accessors:--------------//
    int get_N() const { return _Nhits;}
    double get_chisq() const { return _chisq; }
    double get_chisq_dof() const { return _chisq_dof; }
    
    std::vector<double> get_track_parameters() const { return _track_parameters; } 
    
    double get_parameter(unsigned para_ID) const { 
	if(_track_parameters.size() >=para_ID){
		return _track_parameters.at(para_ID);
	}throw "Error: parameter list not long enough";
     }
     
    XYZVec get_track_equation() const{return _track_equation;}
    XYZVec get_track_direction() const{return _track_direction;}
    XYZVec getXPrime() const { return _XPrime;}
    XYZVec getYPrime() const { return _YPrime;}
    XYZVec getZPrime() const { return _ZPrime;}
    
    double GetSagitta() const {return _Sagitta;}
    
    std::vector<double> get_fit_residualsX() const { return _fit_residualsX; }
    std::vector<double> get_fit_residual_errorsX() const { return _fit_residual_errorsX; }
    std::vector<double> get_fit_residualsY() const { return _fit_residualsY; }
    std::vector<double> get_fit_residual_errorsY() const { return _fit_residual_errorsY; }
    //hit errors info
    std::vector<double> get_hit_errorsX() const{ return _hit_errorsX;}
    std::vector<double> get_hit_errorsY() const{ return _hit_errorsY;}
    std::vector<double> get_hit_errorsTotal() const{ return _hit_errorsTotal;}
//---------------------------------------//  
    
     void clear();
     
     
//-------------Modiffiers---------------//
   
    void set_N(int N){_Nhits = N;}
    void set_parameters(double a0,double a1, double b0, double b1){ 
	_a0=a0;
	_a1=a1;
	_b0=b0;
	_b1=b1;
        _track_parameters.push_back(a0);
        _track_parameters.push_back(a1);
	_track_parameters.push_back(b0);
	_track_parameters.push_back(b1);

	}
    void set_parameters(unsigned para_ID, double par){
    	if(_track_parameters.size() >= para_ID){
		_track_parameters.at(para_ID) = par;
	}throw "Error: Parameter list not long enough";
	 
	
    }
    void set_track_direction(XYZVec direction){_track_direction = direction;}
    void setXPrime(XYZVec XPrime){ _XPrime = XPrime;}
    void setYPrime(XYZVec YPrime){ _YPrime = YPrime;}
    void setZPrime(XYZVec ZPrime){_ZPrime = ZPrime;}
    
    void set_chisq(double chisq) { _chisq = chisq; }
    void set_mom(XYZVec mom){_track_mommentum=mom;} 
    void set_fit_residualsX(double residual) { _fit_residualsX.push_back(residual); }
    void set_fit_residual_errorsX(double residual_err) { _fit_residual_errorsX.push_back(residual_err); }
    
    void set_fit_residualsY(double residual) { _fit_residualsY.push_back(residual); }
    void set_fit_residual_errorsY(double residual_err) { _fit_residual_errorsY.push_back(residual_err); }
    
    void set_hit_errorsX(double hit_errorX){_hit_errorsX.push_back(hit_errorX);}
    void set_hit_errorsY(double hit_errorY){_hit_errorsY.push_back(hit_errorY);}
    void set_hit_errorsTotal(double hit_errorTotal){_hit_errorsTotal.push_back(hit_errorTotal);}
    
    void set_cov(int r, int c, double element){ _cov_mat[r][c] = element;}
    
  private:
    
    int _Nhits;

    double _a0; //a0
    double _a1; //a1
    double _b0; //b0
    double _b1; //b1

    std::vector<double> _track_parameters; //FIXME NEED TO BE 4D vector.....

    double _Sagitta;//TODO
    XYZVec _track_mommentum;//TODO
    
    XYZVec _track_equation;//r(t) expression
    XYZVec _track_direction;//the "gradient" term
    XYZVec _track_point0;//the "starting point" in fit line
  
    //Get Track Co-ordinate System:
    XYZVec _XPrime;
    XYZVec _YPrime;
    XYZVec _ZPrime;
    
    double _chisq;
    double _chisq_dof;

    std::vector<double> _fit_residualsX;
    std::vector<double> _fit_residual_errorsX;
    std::vector<double> _fit_residualsY;
    std::vector<double> _fit_residual_errorsY;
    
    std::vector<double> _hit_errorsX;
    std::vector<double> _hit_errorsY;
    std::vector<double> _hit_errorsTotal;
    
    std::vector<std::vector<double> > _cov_mat;

    bool set_chosenOne;
   
  };
}

#endif
