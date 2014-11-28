// -*- C++ -*-
//
// Package:    geomAnalyzer
// Class:      geomAnalyzer
// 
/**\class geomAnalyzer geomAnalyzer.cc GEMAnalyzer/geomAnalyzer/plugins/geomAnalyzer.cc

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

class geomAnalyzer : public edm::EDAnalyzer {
   public:
      explicit geomAnalyzer(const edm::ParameterSet&);
      ~geomAnalyzer();

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
geomAnalyzer::geomAnalyzer(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
  edm::Service<TFileService> fs;
  ntuple = fs->make<TNtuple>("strip", "strip","region:station:chamber:layer:roll:strip:x:y:z:eta:phi");
}


geomAnalyzer::~geomAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
geomAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
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
                double x = roll->toGlobal(lEdge).x();  
                double y = roll->toGlobal(lEdge).y();  
                double z = roll->toGlobal(lEdge).z();  
                double eta = roll->toGlobal(lEdge).eta();  
                double phi = roll->toGlobal(lEdge).phi().degrees();  
                GEMDetId id( roll->id()) ; 
                //std::cout<<id<<"  "<<"strip : "<<strip<<" phi : "<< phi<<std::endl;
                //printf("%s %f\n", id);
                ntuple->Fill( id.region(), id.station(), id.chamber(), id.layer(), id.roll(), strip, x,y,z,eta,phi);
              }
            }
}


// ------------ method called once each job just before starting event loop  ------------
void 
geomAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
geomAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
geomAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
geomAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
geomAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
geomAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
geomAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(geomAnalyzer);
