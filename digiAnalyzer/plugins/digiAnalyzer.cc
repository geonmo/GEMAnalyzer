// -*- C++ -*-
//
// Package:    digiAnalyzer
// Class:      digiAnalyzer
// 
/**\class digiAnalyzer digiAnalyzer.cc GEMAnalyzer/digiAnalyzer/plugins/digiAnalyzer.cc

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

#include <TNtuple.h>
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

//
// class declaration
//

class digiAnalyzer : public edm::EDAnalyzer {
   public:
      explicit digiAnalyzer(const edm::ParameterSet&);
      ~digiAnalyzer();

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
      edm::EDGetToken stripToken_;
      int  minBX_;
      int  maxBX_;
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
digiAnalyzer::digiAnalyzer(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
  stripToken_ = consumes<GEMDigiCollection>(iConfig.getParameter<edm::InputTag>("stripLabel"));
  minBX_ = iConfig.getParameter<int>("minBX");
  maxBX_ = iConfig.getParameter<int>("maxBX");
  edm::Service<TFileService> fs;
  ntuple = fs->make<TNtuple>("strip", "strip","region:station:chamber:layer:strip:bx:eta:phi:r:x:y:z");
}


digiAnalyzer::~digiAnalyzer()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
digiAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
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

  edm::Handle<GEMDigiCollection> gem_digis;
  iEvent.getByToken( stripToken_, gem_digis);
  if (!gem_digis.isValid()) {
    edm::LogError("GEMStripDigiValidation") << "Cannot get strips by Token stripToken.\n";
    return ;
  }
 
  for( auto cItr = gem_digis->begin() ; cItr!= gem_digis->end(); cItr++ ) {
    GEMDetId id = (*cItr).first;

    const GeomDet* gdet = gem_geometry->idToDet(id);  
    if ( gdet == nullptr) {
      std::cout<<"Error! Need to check geometry."<<std::endl; 
      continue;
    }
    const BoundPlane & surface = gdet->surface();
    const GEMEtaPartition * roll = gem_geometry->etaPartition(id);
    Short_t region  = id.region();
    Short_t station = id.station();
    Short_t chamber = id.chamber();
    Short_t layer   = id.layer();

    for ( auto digiItr = (*cItr).second.first ; digiItr != (*cItr).second.second ; digiItr++) {
      Short_t strip = (Short_t) digiItr->strip();
      Short_t bx = (Short_t) digiItr->bx();
      if ( bx < minBX_ or bx>maxBX_ ) continue;

      LocalPoint lp = roll->centreOfStrip(digiItr->strip());
      GlobalPoint gp = surface.toGlobal(lp);
      Float_t g_eta = (Float_t) gp.eta();
      Float_t g_phi = (Float_t) gp.phi();

      Float_t g_r = (Float_t) gp.perp();
      Float_t g_x = (Float_t) gp.x();
      Float_t g_y = (Float_t) gp.y();
      Float_t g_z = (Float_t) gp.z();
      ntuple->Fill( region, station, chamber, layer, strip, bx, g_eta, g_phi,g_r, g_x,g_y,g_z);

    } 
  } 
}


// ------------ method called once each job just before starting event loop  ------------
void 
digiAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
digiAnalyzer::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
/*
void 
digiAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
digiAnalyzer::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
digiAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
digiAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
digiAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(digiAnalyzer);
