// Creates baby tree with basic branches

#ifndef H_BMAKER_BASIC
#define H_BMAKER_BASIC

// System include files
#include <memory>
#include <vector>
#include <ctime>

// FW include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// FW physics include files
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

// ROOT include files
#include "TTree.h"
#include "TString.h"

// User include files
#include "gen_babymaker/bmaker/interface/baby_basic.hh"
#include "gen_babymaker/bmaker/interface/mc_tools.hh"
#include "gen_babymaker/bmaker/interface/utilities.hh"

typedef float& (baby_base::*baby_float)();
typedef std::vector<float>& (baby_base::*baby_vfloat)();
typedef std::vector<bool>& (baby_base::*baby_vbool)();

// Class declaration
class bmaker_basic : public edm::EDAnalyzer {
public:
  explicit bmaker_basic(const edm::ParameterSet&);
  ~bmaker_basic();

  TFile *outfile;
  baby_basic baby;
  time_t startTime;

  //object classes
  mc_tools *mcTool;

  // Functions that do the branch writing
  void writeGenInfo(edm::Handle<LHEEventProduct> lhe_info);
  void writeMC(edm::Handle<reco::GenParticleCollection> genParticles);

  void reportTime(const edm::Event& iEvent);


  std::vector<TString> trig_name;

  // Input parameters
  edm::EDGetTokenT<reco::GenParticleCollection> genPtclToken;
  edm::EDGetTokenT<reco::GenJetCollection> genJetsToken;
  edm::EDGetTokenT<LHEEventProduct> lheInfoToken;
  edm::EDGetTokenT<GenEventInfoProduct> genEventProdToken;
  edm::EDGetTokenT<GenLumiInfoHeader> genLumiHeaderToken;
  TString outname;
  std::vector<std::string> inputfiles;
  TString condor_subtime;
  unsigned int nevents_sample;
  unsigned int nevents;

  bool debug;
  bool mgp_wf;

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  int gen_cfgid_;
  int mprod_;
  int mlsp_;

  // virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  // virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

  // ----------member data ---------------------------
};

#endif
