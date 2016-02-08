// -*- C++ -*-
//
// Package:    Npv/Npv
// Class:      Npv
// 
/**\class Npv Npv.cc Npv/Npv/plugins/Npv.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Andrea Carlo Marini
//         Created:  Fri, 05 Feb 2016 14:01:12 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//Vertex
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
//Gen
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
//PU
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

// TFile Service
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"

#include "TTree.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TMath.h"
#include "TLorentzVector.h"


//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class Npv : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit Npv(const edm::ParameterSet&);
      ~Npv();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      edm::Service<TFileService> fileService_;
          // --- Handle
        edm::Handle<reco::VertexCollection> vtx_h;
	edm::Handle< std::vector<PileupSummaryInfo> > pu_h;
        // --- Token
        edm::EDGetTokenT<reco::VertexCollection> vtx_t;
	edm::EDGetTokenT< std::vector<PileupSummaryInfo> > pu_t;

	TTree *tree_;
	int npv,npu;
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
Npv::Npv(const edm::ParameterSet& iConfig)

{
   //now do what ever initialization is needed
   usesResource("TFileService");
   tree_ = fileService_ -> make<TTree>("events", "events");
   tree_ -> Branch( "npv",&npv,"npv/I");
   tree_ -> Branch( "npu",&npu,"npu/I");

   vtx_t = consumes<reco::VertexCollection>(edm::InputTag("offlinePrimaryVertices"));
   pu_t = consumes<std::vector<PileupSummaryInfo> >(edm::InputTag("addPileupInfo"));

}


Npv::~Npv()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
Npv::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   iEvent.getByToken(vtx_t,vtx_h);
   iEvent.getByToken(pu_t,pu_h);

    npv=0;
    for (reco::VertexCollection::const_iterator vtx = vtx_h->begin();  vtx != vtx_h->end(); ++vtx) {
        bool isFake = (vtx->chi2()==0 && vtx->ndof()==0);
		if ( !isFake
                && vtx->ndof()>=4. && vtx->position().Rho()<=2.0
                && fabs(vtx->position().Z())<=24.0) {
            	++npv;
		}
            //break;
        }

    npu=0;

  for(const auto & pu : *pu_h)
    {
        //Intime
        if (pu.getBunchCrossing() == 0)
            npu += pu.getTrueNumInteractions();
    }

   tree_->Fill();
}


// ------------ method called once each job just before starting event loop  ------------
void 
Npv::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
Npv::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
Npv::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Npv);
