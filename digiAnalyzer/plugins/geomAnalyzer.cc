// -*- C++ -*-
//
// Package:    geomanalyzer
// Class:      geomanalyzer
// 
/**\class geomanalyzer geomanalyzer.cc GEMAnalyzer/geomanalyzer/plugins/geomanalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Geonmo Ryu
//         Created:  Thu, 24 Jul 2014 15:17:45 GMT
// $Id$
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/GEMGeometry/interface/GEMGeometry.h"


#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/GEMDigi/interface/GEMDigiCollection.h"

#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/GEMGeometry/interface/GEMEtaPartitionSpecs.h"

#include <TNtuple.h>
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//
// class declaration
//

class geomanalyzer : public edm::EDAnalyzer {
   public:
      explicit geomanalyzer(const edm::ParameterSet&);
      ~geomanalyzer();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
      //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
      //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

      // ----------member data ---------------------------
      TNtuple* ntuple;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
geomanalyzer::geomanalyzer(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
  edm::Service<TFileService> fs;
  ntuple = fs->make<TNtuple>("strip", "strip","region:station:chamber:layer:strip:phi");
}


geomanalyzer::~geomanalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
geomanalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  edm::ESHandle<GEMGeometry> gem_geo;
  const GEMGeometry* gem_geometry;
  try{
    iSetup.get<MuonGeometryRecord>().get(gem_geo);
    gem_geometry = &*gem_geo;
  }
  catch(edm::eventsetup::NoProxyException<GEMGeometry>& e){
    edm::LogError("MuonGEMDigis") << "+++ Error : GEM geometry is unavailable. +++\n";
    return;
  }
  for( auto region : gem_geometry->regions()) 
    for (auto station : region->stations()) 
      for ( auto ring : station->rings())   
        for ( auto sch : ring->superChambers()) 
          for ( auto ch : sch->chambers()) 
            for ( auto roll : ch->etaPartitions()) { 
	            const StripTopology* topology(&(roll->specificTopology()));
	            auto parameters(roll->specs()->parameters());
	            float nStrips(parameters[3]);
              for( int strip = 0 ; strip <=nStrips ; strip++) {
	              LocalPoint lEdge(topology->localPosition(strip));
                double phi = roll->toGlobal(lEdge).phi().degrees();  
                GEMDetId id( roll->id()) ; 
                //std::cout<<id<<"  "<<"strip : "<<strip<<" phi : "<< phi<<std::endl;
                //printf("%s %f\n", id);
                ntuple->Fill( id.region(), id.station(), id.chamber(), id.layer(), strip, phi);
              }
            }
}


// ------------ method called once each job just before starting event loop  ------------
void 
geomanalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
geomanalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
geomanalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
geomanalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
geomanalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
geomanalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
geomanalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(geomanalyzer);