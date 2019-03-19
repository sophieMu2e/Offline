#define _USE_MATH_DEFINES
//S. Middleton, Feb 2019
// C++ includes.
#include <iostream>
#include <string>
#include <cmath>

// Cosmic Tracks:
#include "TrkReco/inc/CosmicTrackFit.hh"
#include "TrkPatRec/inc/CosmicTrackFinder_types.hh"
#include "TrkReco/inc/CosmicTrackFinderData.hh"
#include "Mu2eUtilities/inc/ParametricFit.hh"
//Mu2e Data Prods:
#include "DataProducts/inc/threevec.hh"
#include "RecoDataProducts/inc/StrawHitFlagCollection.hh"
#include "RecoDataProducts/inc/TimeClusterCollection.hh"
#include "RecoDataProducts/inc/StrawHit.hh"
#include "RecoDataProducts/inc/StrawHitFlag.hh"
#include "MCDataProducts/inc/StrawDigiMC.hh"
#include "MCDataProducts/inc/MCRelationship.hh"
#include "RecoDataProducts/inc/ComboHit.hh"
#include "DataProducts/inc/XYZVec.hh"
#include "RecoDataProducts/inc/CosmicTrack.hh"
#include "RecoDataProducts/inc/CosmicTrackSeed.hh"
#include "MCDataProducts/inc/StrawDigiMC.hh"
// MC Utilities
#include "Mu2eUtilities/inc/SimParticleTimeOffset.hh"
#include "TrkDiag/inc/TrkMCTools.hh"
// Mu2e diagnostics
#include "TrkDiag/inc/ComboHitInfo.hh"
#include "GeneralUtilities/inc/ParameterSetHelpers.hh"

// Framework includes.
#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Optional/TFileService.h"
#include "art/Framework/Core/ModuleMacros.h"

// ROOT incldues
#include "TStyle.h"
#include "Rtypes.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TTree.h"
#include "TLatex.h"

using namespace std; 

namespace mu2e 
{
  class CosmicAnalyzer : public art::EDAnalyzer {
    public:
      explicit CosmicAnalyzer(fhicl::ParameterSet const& pset);
      virtual ~CosmicAnalyzer();
      virtual void beginJob();
      virtual void analyze(const art::Event& e);
    private: 
      int  _diag;
      bool _mcdiag;
      
      //TTree Info:
      TTree* _cosmic_analysis;

      //Some Diag histograms:
      TH1F* _chisq_plot;
      TH1F* _chisq_ndf_plot;
      TH1F* _total_residualsX;
      TH1F* _total_pullsX;
      TH1F* _total_residualsY;
      TH1F* _total_pullsY;
      TH1F* _hiterrs;
      TH1F* _fiterrs;
      // add event id
      Int_t _evt; 

      // Event object Tags
      art::InputTag   _chtag;//combo
      art::InputTag   _tctag;//timeclusters
      art::InputTag   _costag;//Striaght tracks
      art::InputTag   _mcdigistag; //MC digis
      // Event data cache
      const ComboHitCollection* _chcol;
      const CosmicTrackSeedCollection* _coscol;
      const TimeClusterCollection* _tccol;
      const StrawDigiMCCollection* _mcdigis;
      //Numbers:
      Int_t _nsh, _nch; // # associated straw hits / event
      Int_t     _ntc; // # clusters/event
      Int_t _nhits, _nused; // # hits used
      Int_t _n_panels; // # panels
      Int_t _n_stations; // # stations
      Int_t _n_planes; // # stations

      Float_t _hit_time, _hit_drift_time, _cluster_time;
	
      //Flags:

      //offsets for MC
      SimParticleTimeOffset _toff;

      Int_t _strawid; // strawid info

      vector<ComboHitInfoMC> _chinfomc;

      bool findData(const art::Event& evt);
    };

    CosmicAnalyzer::CosmicAnalyzer(fhicl::ParameterSet const& pset) :
	art::EDAnalyzer(pset),
	_diag		(pset.get<int>("diagLevel",1)),
	_mcdiag		(pset.get<bool>("mcdiag",true)),
	_chtag		(pset.get<art::InputTag>("ComboHitCollection")),
	_tctag		(pset.get<art::InputTag>("TimeClusterCollection")),
	_costag		(pset.get<art::InputTag>("CosmicTrackSeedCollection")),
	_mcdigistag	(pset.get<art::InputTag>("StrawDigiMCCollection","makeSD")),
	_toff(pset.get<fhicl::ParameterSet>("TimeOffsets"))
        {}

    CosmicAnalyzer::~CosmicAnalyzer(){}

    void CosmicAnalyzer::beginJob() {
      // create diagnostics if requested...
      if(_diag > 0){
	art::ServiceHandle<art::TFileService> tfs;
	//Tree for detailed diagnostics
	_cosmic_analysis=tfs->make<TTree>("cosmic_analysis"," Diagnostics for Cosmic Track Fitting");

        //Create branches:
        _cosmic_analysis->Branch("evt",&_evt,"evt/I");  // add event id
        _cosmic_analysis->Branch("nhits",&_nhits,"nhits/I");
        _cosmic_analysis->Branch("StrawHitsInEvent", &_nsh, "StrawHitsInEvent/I");
	_cosmic_analysis->Branch("ComboHitsInEvent", &_nch, "ComboHitsInEvent/I");
        _cosmic_analysis->Branch("PanelsCrossedInEvent", &_n_panels, "PanelsCrossedInEvent/I");
        _cosmic_analysis->Branch("PlanesCrossedInEvent", &_n_planes, "PlanesCrossedInEvent/I");
        _cosmic_analysis->Branch("StatonsCrossedInEvent", &_n_stations, "StationsCrossedInEvent/I");
        _cosmic_analysis->Branch("TimeClustersInEvent", &_ntc, "TimeClusterInEvent/I");
       
       _cosmic_analysis->Branch("hit_time", &_hit_time, "hit_time/F");
       _cosmic_analysis->Branch("hit_drit_time", &_hit_drift_time, "hit_drift_time/F");
       _cosmic_analysis->Branch("cluster_time", &_cluster_time, "cluster_time/F");
        
        //Extra histograms for Fit Diags:
        _chisq_plot = tfs->make<TH1F>("chisq_plot","chisq_plot" ,100,0, 100);
	_chisq_ndf_plot = tfs->make<TH1F>("chisq_ndf_plot","chisq_ndf_plot" ,20,0, 0.5);
	_chisq_plot->GetXaxis()->SetTitle("#Chi^{2}");
	_chisq_ndf_plot->GetXaxis()->SetTitle("#Chi^{2}/N");
	
        
        _total_residualsX = tfs->make<TH1F>("Residuals X ","Residuals X " ,50,-200,200);
	_total_residualsX->GetXaxis()->SetTitle("Residual X [mm]");
	_total_residualsX->SetStats();
	
	_total_residualsY = tfs->make<TH1F>("Residuals Y","Residuals Y " ,50,-200,200);
	_total_residualsY->GetXaxis()->SetTitle("Residual Y [mm]");
	_total_residualsY->SetStats();

	_total_pullsX = tfs->make<TH1F>("Pull X","Pull X" ,50,-1, 1);
	_total_pullsX->GetXaxis()->SetTitle("Pull X");
	_total_pullsX->SetStats();
	
	_total_pullsY = tfs->make<TH1F>("Pull Y","Pull Y" ,50,-1, 1);
	_total_pullsY->GetXaxis()->SetTitle("Pull Y");
	_total_pullsY->SetStats();
       
        _hiterrs = tfs->make<TH1F>("hiterr","hiterr" ,100,0, 100);
	_hiterrs->GetXaxis()->SetTitle("Hit Error in Track Frame [mm]");
	_hiterrs->SetStats();
	
	_fiterrs = tfs->make<TH1F>("fiterr","fiterr" ,100,0, 100);
	_fiterrs->GetXaxis()->SetTitle("Fit Error [mm]");
	_fiterrs->SetStats();
	
	
        
        }
      }
      void CosmicAnalyzer::analyze(const art::Event& event) {
        
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
        //loop over tracks"
        for(size_t ist = 0;ist < _coscol->size(); ++ist){
        	CosmicTrackSeed sts =(*_coscol)[ist];
		CosmicTrack st = sts._track;
		//TrkFitFlag const& status = sts._status;
		std::vector<int> panels, planes, stations;
                _chisq_plot->Fill(st.get_chisq());
                //-----------Fill Hist Details:----------//
		for(size_t i=0; i< st.get_hit_errorsTotal().size();i++){
		    _hiterrs->Fill(st.get_hit_errorsTotal()[i]);
		}
		for(size_t i=0; i< st.get_fit_residualsX().size();i++){
		    double pullX = st.get_fit_residualsX()[i]/st.get_fit_residual_errorsX()[i];
                    _total_residualsX->Fill(st.get_fit_residualsX()[i]);             
	            _total_pullsX->Fill(pullX);
	        }
	        for(size_t i=0; i< st.get_fit_residualsY().size();i++){
	            double pullY = st.get_fit_residualsY()[i]/st.get_fit_residual_errorsY()[i];
                    _total_residualsY->Fill(st.get_fit_residualsY()[i]);             
	            _total_pullsY->Fill(pullY);
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
			
		    
                //----------------Get panels/planes/stations per track:------------------//
                _n_panels = std::set<double>( panels.begin(), panels.end() ).size();
		_n_planes = std::set<double>( planes.begin(), planes.end() ).size();
		_n_stations = std::set<double>( stations.begin(), stations.end() ).size();
	 
		}//endS ST
	        
      }//end analyze
     
      _cosmic_analysis->Fill();
      }

      bool CosmicAnalyzer::findData(const art::Event& evt){
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
           auto mcdH = evt.getValidHandle<StrawDigiMCCollection>(_mcdigistag);
           _mcdigis = mcdH.product();
	   // update time offsets
           _toff.updateMap(evt);
        }
	return _chcol != 0 && _tccol!=0 && _coscol !=0 && (_mcdigis != 0 || !_mcdiag);
       }



}//End Namespace Mu2e

using mu2e::CosmicAnalyzer;
DEFINE_ART_MODULE(CosmicAnalyzer);
    

    

