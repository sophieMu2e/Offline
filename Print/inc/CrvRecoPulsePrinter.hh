//
//  Utility class to print CrvRecoPulse
// 
#ifndef Print_inc_CrvRecoPulsePrinter_hh
#define Print_inc_CrvRecoPulsePrinter_hh

#include <cstring>
#include <iostream>

#include "Print/inc/ProductPrinter.hh"
#include "RecoDataProducts/inc/CrvRecoPulseCollection.hh"
#include "art/Framework/Principal/Handle.h"
#include "canvas/Persistency/Common/Ptr.h"

namespace mu2e {

  class CrvRecoPulsePrinter : public ProductPrinter {
  public:

    typedef std::vector<std::string> vecstr;

    CrvRecoPulsePrinter() { set( fhicl::ParameterSet() ); }
    CrvRecoPulsePrinter(const fhicl::ParameterSet& pset) { set(pset); }

    // tags to select which product instances to process
    void setTags(const vecstr& tags) { _tags = tags; }

    // pset should contain a table called CrvRecoPulsePrinter
    void set(const fhicl::ParameterSet& pset);

    // the vector<string> list of inputTags
    const vecstr& tags() const {return _tags; }

    // all the ways to request a printout
    void Print(art::Event const& event,
	       std::ostream& os = std::cout) override;
    void Print(const art::Handle<CrvRecoPulseCollection>& handle, 
	       std::ostream& os = std::cout);
    void Print(const art::ValidHandle<CrvRecoPulseCollection>& handle, 
	       std::ostream& os = std::cout);
    void Print(const CrvRecoPulseCollection& coll, 
	       std::ostream& os = std::cout);
    void Print(const art::Ptr<CrvRecoPulse>& ptr, 
	       int ind = -1, std::ostream& os = std::cout);
    void Print(const mu2e::CrvRecoPulse& obj, 
	       int ind = -1, std::ostream& os = std::cout);

    void PrintHeader(const std::string& tag, 
		     std::ostream& os = std::cout);
    void PrintListHeader(std::ostream& os = std::cout);

  private:

    vecstr _tags;

  };

}
#endif
