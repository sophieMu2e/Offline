#define _USE_MATH_DEFINES
#include <iostream>
#include <string>
#include <cmath>

// Cosmic Tracks:
#include "CosmicReco/inc/CosmicTrackFit.hh"
#include "CosmicReco/inc/CosmicTrackFinderData.hh"
#include "RecoDataProducts/inc/CosmicTrack.hh"
#include "RecoDataProducts/inc/CosmicTrackSeed.hh"
#include "CosmicReco/inc/CosmicTrackMCInfo.hh"

//Mu2e Data Prods:
#include "RecoDataProducts/inc/StrawHitFlagCollection.hh"
#include "RecoDataProducts/inc/StrawHit.hh"
#include "RecoDataProducts/inc/StrawHitFlag.hh"
#include "MCDataProducts/inc/StrawDigiMC.hh"
#include "MCDataProducts/inc/MCRelationship.hh"
#include "MCDataProducts/inc/SimParticleCollection.hh"
#include "MCDataProducts/inc/StepPointMCCollection.hh"
#include "RecoDataProducts/inc/ComboHit.hh"
#include "DataProducts/inc/XYZVec.hh"

//Utilities
#include "Mu2eUtilities/inc/SimParticleTimeOffset.hh"
#include "TrkDiag/inc/TrkMCTools.hh"
#include "CosmicReco/inc/DriftFitUtils.hh"
#include "Mu2eUtilities/inc/ParametricFit.hh"

// Mu2e diagnostics
#include "TrkDiag/inc/ComboHitInfo.hh"
#include "GeneralUtilities/inc/ParameterSetHelpers.hh"
#include "GeometryService/inc/GeomHandle.hh"
#include "GeometryService/inc/DetectorSystem.hh"
// Framework includes.
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art_root_io/TFileService.h"


// ROOT incldues
#include "TStyle.h"
#include "Rtypes.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TTree.h"
#include "TLatex.h"
#include "TGraph.h"
#include "TProfile.h"
using namespace std; 

namespace mu2e 
{
  class CosmicTrackDetails : public art::EDAnalyzer {
    public:
	struct Config{
	      using Name=fhicl::Name;
	      using Comment=fhicl::Comment;
	      fhicl::Atom<int> diag{Name("diagLevel"), Comment("set to 1 for info"),1};
	      fhicl::Atom<bool> mcdiag{Name("mcdiag"), Comment("set on for MC info"),true};
	      fhicl::Atom<art::InputTag> chtag{Name("ComboHitCollection"),Comment("tag for combo hit collection")};
	      fhicl::Atom<art::InputTag> tctag{Name("TimeClusterCollection"),Comment("tag for time cluster collection")};
	      fhicl::Atom<art::InputTag> costag{Name("CosmicTrackSeedCollection"),Comment("tag for cosmci track seed collection")};
	      fhicl::Atom<art::InputTag> mcdigistag{Name("StrawDigiMCCollection"),Comment("StrawDigi collection tag"),"makeSD"};
	      fhicl::Table<SimParticleTimeOffset::Config> toff{Name("TimeOffsets"), Comment("Sim particle time offset ")};
	    };
      typedef art::EDAnalyzer::Table<Config> Parameters;

      explicit CosmicTrackDetails(const Parameters& conf);
      virtual ~CosmicTrackDetails();
      virtual void beginJob();
      virtual void analyze(const art::Event& e) override;
      virtual void endJob();
    private: 
      
      Config _conf;

      int  _diag;
      bool _mcdiag;
      std::ofstream outputfile;
      art::InputTag   _chtag;//combo
      art::InputTag   _tctag;//timeclusters
      art::InputTag   _costag;//Striaght tracks
      art::InputTag   _mcdigistag; //MC digis
      SimParticleTimeOffset _toff;
      const ComboHitCollection* _chcol;
      const CosmicTrackSeedCollection* _coscol;
      const TimeClusterCollection* _tccol;
      const StrawDigiMCCollection* _mcdigis;
      CosmicTrackMCInfo trueinfo;

      	//TTree Info:
      TTree* _cosmic_tree;
      /*
      	//Seed Info:
      double _seed_a1;
      double _seed_b1;
      double _seed_a0;
      double _seed_b0;

      	//Seed Info in Det Frame:
      double _seed_a1XYZ;
      double _seed_b1XYZ;
      double _seed_a0XYZ;
      double _seed_b0XYZ;
      double _niters;
*/
    //True MC paramets in global coords:
      double _TrueA1;
      double _TrueB1;
      double _TrueA0;
      double _TrueB0;
      
      //Angles Info:
      double _mc_phi_angle;
      double _reco_phi_angle;
      double _mc_theta_angle;
      double _reco_theta_angle;
/*
     //Look at distributions of covarience of each seed parameter:
      double _Seed_Cov_A0;
      double _Seed_Cov_A1;
      double _Seed_Cov_B0;
      double _Seed_Cov_B1;
      double _Seed_Cov_B0B1;
      double _Seed_Cov_A0A1;

      //Seed Diagnostics:
      double _total_residualsX_init;
      double _total_pullsX_init;
      double _total_residualsY_init;
      double _total_pullsY_init;
      double _chisq_ndf_plot_final;
      double _chisq_ndf_plot_finalX;
      double _chisq_ndf_plot_finalY;
      double _chisq_ndf_plot_initX;
      double _chisq_ndf_plot_initY;
      double _chisq_ndf_plot_init;
      double _change_chisq_ndf_plot_X;
      double _change_chisq_ndf_plot_Y;
      double _change_chisq_ndf_plot_Total;
      double _total_residualsX_final;
      double _total_pullsX_final;
      double _total_residualsY_final;
      double _total_pullsY_final;
      double _FinalErrX;
      double _FinalErrY;
      double _InitErrX;
      double _InitErrY;
      double _FinalErrTot;
      double _InitErrTot;

      	//Drift Parameters
      double _total_residualsX_Minuit;
      double _total_residualsY_Minuit;

	*/
	//Track Parameters from end of minuit minimzation rotuine:
      double _MinuitA0;
      double _MinuitA1;
      double _MinuitB1;
      double _MinuitB0;
	/*
      	//Drift diags:
      double _FullFitEndDOCAs;
      double _TrueDOCAs;
      double _FullFitEndTimeResiduals;
      double _StartTimeResiduals;
      double _TrueTimeResiduals;
      double _StartDOCAs;
      double _GaussianEndDOCAs;
      double _GaussianEndTimeResiduals;
      double _minuit_pullsX_final;
      double _minuit_pullsY_final;
      double _NLL;
      double _AMBIG;
      */
      // add event id
      Int_t _evt; 

      
      //Numbers:
      Int_t _nsh, _nch; // # associated straw hits / event
      Int_t _ntc; // # clusters/event
      Int_t _nhits, _nused; // # hits used
      Int_t _n_panels; // # panels
      Int_t _n_stations; // # stations
      Int_t _n_planes; // # stations
      int n_analyze =0;
      Float_t _hit_time, _hit_drift_time, _cluster_time, _dt;
	
      //Flags:
	Bool_t _StraightTrackInit, _StraightTrackConverged, _StraightTrackOK, _hitsOK;
      Int_t _strawid; 
      vector<ComboHitInfoMC> _chinfomc;
      CosmicTrackMCInfo FitMC(const StrawDigiMCCollection*& _mcdigis);
      CosmicTrackMCInfo FillDriftMC(Straw const& straw, double reco_ambig, CosmicTrackMCInfo info);
      bool findData(const art::Event& evt);
    };

    CosmicTrackDetails::CosmicTrackDetails(const Parameters& conf) :
	art::EDAnalyzer(conf),
	_diag (conf().diag()),
	_mcdiag (conf().mcdiag()),
	_chtag (conf().chtag()),
	_tctag (conf().tctag()),
	_costag (conf().costag()),
	_mcdigistag (conf().mcdigistag()),
	_toff (conf().toff())
    {
      
	if(_mcdiag){
	      for (auto const& tag : conf().toff().inputs()) {
		consumes<SimParticleTimeMap>(tag);
	      }
	}
    }

    CosmicTrackDetails::~CosmicTrackDetails(){}

    void CosmicTrackDetails::beginJob() {
      // create diagnostics if requested...
      if(_diag > 0){
	if(_mcdiag) { 
		
		outputfile.open("doca.csv");
	}
	art::ServiceHandle<art::TFileService> tfs;
	//Tree for detailed diagnostics
	_cosmic_tree=tfs->make<TTree>("cosmic_tree"," Diagnostics for Cosmic Track Fitting");

        //Create branches:
        _cosmic_tree->Branch("evt",&_evt,"evt/I");  // add event id
        _cosmic_tree->Branch("nhits",&_nhits,"nhits/I");
        _cosmic_tree->Branch("StrawHitsInEvent", &_nsh, "StrawHitsInEvent/I");
	_cosmic_tree->Branch("ComboHitsInEvent", &_nch, "ComboHitsInEvent/I");
        _cosmic_tree->Branch("PanelsCrossedInEvent", &_n_panels, "PanelsCrossedInEvent/I");
        _cosmic_tree->Branch("PlanesCrossedInEvent", &_n_planes, "PlanesCrossedInEvent/I");
        _cosmic_tree->Branch("StatonsCrossedInEvent", &_n_stations, "StationsCrossedInEvent/I");
        _cosmic_tree->Branch("TimeClustersInEvent", &_ntc, "TimeClusterInEvent/I"); 
        _cosmic_tree->Branch("hit_time", &_hit_time, "hit_time/F");
        _cosmic_tree->Branch("hit_drit_time", &_hit_drift_time, "hit_drift_time/F");
        _cosmic_tree->Branch("cluster_time", &_cluster_time, "cluster_time/F");
        _cosmic_tree->Branch("dt", &_dt, "dt/F");
        _cosmic_tree->Branch("hitsOK",&_hitsOK,"hitsOK/B");
        _cosmic_tree->Branch("StraightTrackInit",&_StraightTrackInit,"StraightTrackInit/B");
        _cosmic_tree->Branch("StraightTrackOK",&_StraightTrackOK,"StraightTrackOK/B");
        _cosmic_tree->Branch("StraightTrackConverged",&_StraightTrackConverged,"StraightTrackConverged/B");
        
	_cosmic_tree->Branch("MinuitA0",&_MinuitA0,"MinuitA0/F");
        _cosmic_tree->Branch("MinuitA1",&_MinuitA1,"MinuitA1/F");
	_cosmic_tree->Branch("MinuitB0",&_MinuitA0,"MinuitB0/F");
        _cosmic_tree->Branch("MinuitB1",&_MinuitA1,"MinuitB1/F");

       	_cosmic_tree->Branch("RecoPhi",&_reco_phi_angle, "RecoPhi/F");
	_cosmic_tree->Branch("RecoTheta",&_reco_theta_angle, "RecoTheta/F");
      double _mc_theta_angle;
     
	//--------------------------------Truth----------------------------------------//
	if(_mcdiag ){
	_cosmic_tree->Branch("TrueA0",&_TrueA0,"TrueA0/F");
        _cosmic_tree->Branch("TrueA1",&_TrueA1,"TrueA1/F");
	_cosmic_tree->Branch("TrueB0",&_TrueA0,"TrueB0/F");
        _cosmic_tree->Branch("TrueB1",&_TrueA1,"TrueB1/F");

	_cosmic_tree->Branch("TruePhi",&_mc_phi_angle, "TruePhi/F");
	_cosmic_tree->Branch("TrueTheta",&_mc_theta_angle, "TrueTheta/F");
	}
	
	
	}
      }
      void CosmicTrackDetails::analyze(const art::Event& event) {
       
        _evt = event.id().event();  // add event id
        if(!findData(event)) // find data
      		throw cet::exception("RECO")<<"No Time Clusters in event"<< endl; 
       
        //find time clusters:
    	_ntc = _tccol->size();
        _nch = _chcol->size();
        
        for(size_t itc=0; itc<_tccol->size();++itc){
		TimeCluster tc = (*_tccol)[itc];
        	_cluster_time =  tc._t0._t0;
                //terr  = tc._t0._t0err;
	}
	
        //loop over tracks
        for(size_t ist = 0;ist < _coscol->size(); ++ist){
        	n_analyze+=1;
        	
        	CosmicTrackSeed sts =(*_coscol)[ist];
		CosmicTrack st = sts._track;
		TrkFitFlag const& status = sts._status;
        	if (!status.hasAllProperties(TrkFitFlag::helixOK) ){continue;}
		if(st.converged == false or st.minuit_converged  == false) { continue;}
		std::vector<int> panels, planes, stations;

		_reco_phi_angle=(st.get_fit_phi()); 
		//_niters=(st.get_iter()); 
		/*
                _seed_a1=(st.FitParams.A1);
                _seed_b1=(st.FitParams.B1);
                _seed_a0=(st.FitParams.A0);
                _seed_b0=(st.FitParams.B0);

		_seed_a1XYZ=(st.FitEquationXYZ.Dir.X());
                _seed_b1XYZ=(st.FitEquationXYZ.Dir.Y());
                _seed_a0XYZ=(st.FitEquationXYZ.Pos.X());
                _seed_b0XYZ=(st.FitEquationXYZ.Pos.Y());

                _Seed_Cov_A0=(st.FitParams.Covarience.sigA0);
                _Seed_Cov_A1=(st.FitParams.Covarience.sigA1);
                _Seed_Cov_B0=(st.FitParams.Covarience.sigB0);
                _Seed_Cov_B1=(st.FitParams.Covarience.sigB1);
                _Seed_Cov_B0B1=(st.FitParams.Covarience.sigB1B0);
		_Seed_Cov_A0A1=(st.FitParams.Covarience.sigA1A0);
	*/
	        _MinuitA0=(st.MinuitFitParams.A0);
	        _MinuitA1=(st.MinuitFitParams.A1);
	        _MinuitB1=(st.MinuitFitParams.B1);
		_MinuitB0=(st.MinuitFitParams.B1);
	       if(_mcdiag){
			
			trueinfo = FitMC(_mcdigis);
			
			_mc_phi_angle=(trueinfo.TruePhi);
	                _mc_theta_angle=(trueinfo.TrueTheta);                  
	                
			_TrueA1=(trueinfo.TrueFitEquation.Dir.X());
		        _TrueB1=(trueinfo.TrueFitEquation.Dir.Y());
		        _TrueA0=(trueinfo.TrueFitEquation.Pos.X());
		        _TrueB0=(trueinfo.TrueFitEquation.Pos.Y());

			
	    }
		/*
		_chisq_ndf_plot_init=(st.Diag.InitialChiTot);
                _chisq_ndf_plot_final=(st.Diag.FinalChiTot);
                
                _chisq_ndf_plot_finalX=(st.Diag.FinalChiX);
                _chisq_ndf_plot_finalY=(st.Diag.FinalChiY);
               
                _chisq_ndf_plot_initX=(st.Diag.InitialChiX);
                _chisq_ndf_plot_initY=(st.Diag.InitialChiY);
               
                _change_chisq_ndf_plot_X=(st.Diag.InitialChiX-st.Diag.FinalChiX);
                _change_chisq_ndf_plot_Y=(st.Diag.InitialChiY-st.Diag.FinalChiY);
                _change_chisq_ndf_plot_Total=(st.Diag.InitialChiTot-st.Diag.FinalChiTot);
		
		_NLL=(st.DriftDiag.NLL);
	        
                for(size_t i=0; i< st.Diag.InitErrTot.size();i++){
		    _InitErrTot=(st.Diag.InitErrTot[i]); 
                    _total_residualsX_init=(st.Diag.InitialResidualsX[i]);        
	            _total_pullsX_init=(st.Diag.InitialResidualsX[i]/st.Diag.InitErrX[i]);
	            _InitErrX=(st.Diag.InitErrX[i]);     
                    _total_residualsY_init=(st.Diag.InitialResidualsY[i]);            
	            _total_pullsY_init=(st.Diag.InitialResidualsY[i]/st.Diag.InitErrY[i]);
	            _InitErrY=(st.Diag.InitErrY[i]); 
	        }

		for(size_t i=0; i< st.Diag.FinalErrTot.size();i++){
		    _FinalErrTot=(st.Diag.FinalErrTot[i]);
                    _total_residualsX_final=(st.Diag.FinalResidualsX[i]);          
	            _total_pullsX_final=(st.Diag.FinalResidualsX[i]/st.Diag.FinalErrX[i]);
	            _FinalErrX=(st.Diag.FinalErrX[i]);
                    _total_residualsY_final=(st.Diag.FinalResidualsY[i]);            
	            _total_pullsY_final=(st.Diag.FinalResidualsY[i]/st.Diag.FinalErrY[i]);
	            _FinalErrY=(st.Diag.FinalErrTot[i]);
	            
	        }   
                for(size_t i=0; i<st.DriftDiag.GaussianEndDOCAs.size();i++){
		    _GaussianEndDOCAs=(st.DriftDiag.GaussianEndDOCAs[i]);
		    _GaussianEndTimeResiduals=(st.DriftDiag.GaussianEndTimeResiduals[i]);
			
		}
		
		for(size_t i=0; i<st.DriftDiag.FullFitEndDOCAs.size();i++){
	            _StartDOCAs=(st.DriftDiag.StartDOCAs[i]);
	            _StartTimeResiduals=(st.DriftDiag.StartTimeResiduals[i]);
	            _FullFitEndDOCAs=(st.DriftDiag.FullFitEndDOCAs[i]);
		    _FullFitEndTimeResiduals=(st.DriftDiag.FullFitEndTimeResiduals[i]);
		   	if(_mcdiag){
				trueinfo = FitMC(_mcdigis);
				trueinfo = FillDriftMC(sts._straws[i], st.DriftDiag.RecoAmbigs[i], trueinfo);
			if(DriftFitUtils::GetTestDOCA(sts._straws[i], trueinfo.TrueFitEquation.Pos.X(), trueinfo.TrueFitEquation.Dir.X(), trueinfo.TrueFitEquation.Pos.Y(),trueinfo.TrueFitEquation.Dir.Y())<2.5 and st.DriftDiag.FullFitEndDOCAs[i]<2.5 and abs(trueinfo.TrueFitEquation.Pos.X() ) < 5000 and abs(trueinfo.TrueFitEquation.Pos.Y())<5000 and abs(trueinfo.TrueFitEquation.Dir.X())<5 and abs(trueinfo.TrueFitEquation.Dir.Y())<5){
			outputfile<<DriftFitUtils::GetTestDOCA(sts._straws[i], trueinfo.TrueFitEquation.Pos.X(), trueinfo.TrueFitEquation.Dir.X(), trueinfo.TrueFitEquation.Pos.Y(),trueinfo.TrueFitEquation.Dir.Y())<<","<<st.DriftDiag.FullFitEndDOCAs[i]<<","<<st.DriftDiag.RecoAmbigs[i]/DriftFitUtils::GetAmbig(sts._straws[i], trueinfo.TrueFitEquation.Pos.X(), trueinfo.TrueFitEquation.Dir.X(), trueinfo.TrueFitEquation.Pos.Y(),trueinfo.TrueFitEquation.Dir.Y())<<","<<trueinfo.TrueTheta<<","<<st.MinuitFitParams.A0<<","<<st.MinuitFitParams.A1
<<","<<st.MinuitFitParams.B0<<","<<st.MinuitFitParams.B1<<","<<trueinfo.TrueFitEquation.Pos.X()<<","<<trueinfo.TrueFitEquation.Dir.X()
<<","<<trueinfo.TrueFitEquation.Pos.Y()<<","<<trueinfo.TrueFitEquation.Dir.Y()<<endl;
			}      
			}
	        }

		for(size_t i=0; i< st.DriftDiag.FullFitEndDOCAs.size();i++){
                    _total_residualsX_Minuit=(st.DriftDiag.FinalResidualsX[i]);          
                    _total_residualsY_Minuit=(st.DriftDiag.FinalResidualsY[i]);   
		    _minuit_pullsX_final=(st.DriftDiag.FinalResidualsX[i]/st.DriftDiag.FinalErrX[i]);          
                    _minuit_pullsY_final=(st.DriftDiag.FinalResidualsY[i]/st.DriftDiag.FinalErrY[i]);           
   		}   
     */
	        for(auto const& tseed : *_coscol) {   
                	TrkFitFlag const& status = tseed._status;
                	_hitsOK = status.hasAllProperties(TrkFitFlag::hitsOK);
                	_StraightTrackOK = status.hasAllProperties(TrkFitFlag::helixOK);
                	_StraightTrackConverged = status.hasAllProperties(TrkFitFlag::helixConverged);
                	_StraightTrackInit = status.hasAllProperties(TrkFitFlag::circleInit);
        	}

		for(size_t ich = 0;ich < _chcol->size(); ++ich){
                        ComboHit const& chit =(*_chcol)[ich];
			
                //-----------Fill diag details:----------//
                        _nhits = chit.nStrawHits();
                        _nsh = chit.nStrawHits(); 
                        panels.push_back(chit.strawId().panel());
		        planes.push_back(chit.strawId().plane());
			stations.push_back(chit.strawId().station());
		//-----------Hit details:---------------//
		        _hit_time = chit.time();
			_hit_drift_time = chit.driftTime();
                        _dt =  _hit_time - _cluster_time;
			}
                //----------------Get panels/planes/stations per track:------------------//
                _n_panels = std::set<float>( panels.begin(), panels.end() ).size();
		_n_planes = std::set<float>( planes.begin(), planes.end() ).size();
		_n_stations = std::set<float>( stations.begin(), stations.end() ).size();
	 
      }//end analyze
      _cosmic_tree->Fill();
      
	
     }

void CosmicTrackDetails::endJob() {
	if(_diag and _mcdiag) outputfile.close();
}

CosmicTrackMCInfo CosmicTrackDetails::FitMC(const StrawDigiMCCollection*& _mcdigis){	
	::BuildLinearFitMatrixSums S; 
        CosmicTrackMCInfo TrackTrueInfo;
    	
        StrawDigiMC hitP1; 
	StrawDigiMC first = (*_mcdigis)[0];

        //Get StepPointMC:
	art::Ptr<StepPointMC> const& spmcp0= first.stepPointMC(StrawEnd::cal);
        XYZVec pos0(spmcp0->position().x(), spmcp0->position().y(), spmcp0->position().z());
        XYZVec dir0(spmcp0->momentum().x(), spmcp0->momentum().y(), spmcp0->momentum().z());
	
        for(size_t ich = 0;ich < _mcdigis->size(); ++ich){
            hitP1 = (*_mcdigis)[ich];
	    
            //Get StepPointMC:
	    art::Ptr<StepPointMC> const& spmcp = hitP1.stepPointMC(StrawEnd::cal);
            XYZVec posN(spmcp->position().x(), spmcp->position().y(), spmcp->position().z());
            
            //Use Step Point MC direction as the True Axes:
            XYZVec ZPrime = Geom::toXYZVec(spmcp->momentum().unit());
            
            //Store True Track details:
            TrackAxes TrueAxes = ParametricFit::GetTrackAxes(ZPrime);
            TrackTrueInfo.TrueTrackCoordSystem = (TrueAxes);
	    
            //Apply routine to the True Tracks (for validation):
            XYZVec point(posN.x(), posN.y(), posN.z());
            XYZVec X(1,0,0);
            XYZVec Y(0,1,0);
            XYZVec Z(0,0,1);
            S.addPoint( point, X,Y,Z, 1,1);
            
        }   
    
     TrackParams RawTrueParams(S.GetAlphaX()[0][0], S.GetAlphaX()[1][0], S.GetAlphaY()[0][0], S.GetAlphaY()[1][0]);
     
     XYZVec TruePos(S.GetAlphaX()[0][0], S.GetAlphaY()[0][0], 0);
     
     XYZVec TrueDir(S.GetAlphaX()[1][0], S.GetAlphaY()[1][0], 1);
     TrueDir = TrueDir.Unit();
     TrueDir = TrueDir/TrueDir.Z();
     
     pos0.SetX(pos0.X()-(dir0.X()*pos0.Z()/dir0.Z()));
     pos0.SetY(pos0.Y()-(dir0.Y()*pos0.Z()/dir0.Z()));
     pos0.SetZ(pos0.Z()-(dir0.Z()*pos0.Z()/dir0.Z()));
     dir0 = dir0/dir0.Z();
  
     TrackEquation TrueTrack(pos0, dir0);
     
     TrackTrueInfo.TrueFitEquation = (TrueTrack);
     TrackTrueInfo.TruePhi =(atan(TrueDir.y()/TrueDir.x()));
     TrackTrueInfo.TrueTheta = (acos(TrueDir.x()/sqrt(TrueDir.Mag2())));
     
     return TrackTrueInfo;
     }

CosmicTrackMCInfo CosmicTrackDetails::FillDriftMC(Straw const& straw, double RecoAmbig, CosmicTrackMCInfo info){

     double true_doca = DriftFitUtils::GetTestDOCA(straw, info.TrueFitEquation.Pos.X(), info.TrueFitEquation.Dir.X(), info.TrueFitEquation.Pos.Y(),info.TrueFitEquation.Dir.Y());
     double trueambig = DriftFitUtils::GetAmbig(straw, info.TrueFitEquation.Pos.X(), info.TrueFitEquation.Dir.X(), info.TrueFitEquation.Pos.Y(),info.TrueFitEquation.Dir.Y());
     double true_time_residual = true_doca/0.0625;
     info.Ambig.push_back(trueambig);
     info.TrueDOCA.push_back(true_doca);
     info.TrueTimeResiduals.push_back(true_time_residual);
     //_TrueDOCAs=(true_doca);
     //_TrueTimeResiduals=(true_time_residual);
    
     return info;
}

bool CosmicTrackDetails::findData(const art::Event& evt){
	_chcol = 0; 
        _tccol = 0;
        _coscol = 0; 
	auto chH = evt.getValidHandle<ComboHitCollection>(_chtag);
	_chcol = chH.product();
	auto tcH = evt.getValidHandle<TimeClusterCollection>(_tctag);
	_tccol =tcH.product();
	auto stH = evt.getValidHandle<CosmicTrackSeedCollection>(_costag);
	_coscol =stH.product();
        if(_mcdiag){
	    
	   _mcdigis=0;
           auto mcdH = evt.getValidHandle<StrawDigiMCCollection>(_mcdigistag);
           _mcdigis = mcdH.product();
           _toff.updateMap(evt);
        }
	return _chcol != 0 && _tccol!=0 && _coscol !=0 && (_mcdigis != 0 || !_mcdiag);
       }

}

using mu2e::CosmicTrackDetails;
DEFINE_ART_MODULE(CosmicTrackDetails);
