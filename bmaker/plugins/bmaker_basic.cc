//// BMAKER_BASIC: Creates baby tree with basic branches
//// Function names follow the first-lowercase, following words-uppercase. No underscores

// System include files
#include <cmath>
#include <memory>
#include <iostream>
#include <stdlib.h>
#include <iomanip>  // put_time

// FW include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// FW physics include files
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenLumiInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenLumiInfoHeader.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

// ROOT include files
#include "TFile.h"
#include "TROOT.h"

// User include files
#include "gen_babymaker/bmaker/interface/bmaker_basic.hh"
#include "gen_babymaker/bmaker/interface/baby_basic.hh"

using namespace std;
using namespace utilities;

///////////////////////// analyze: METHOD CALLED EACH EVENT ///////////////////////////
void bmaker_basic::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  nevents++;
  baby.Clear();

  ////////////////////// Event info /////////////////////
  baby.run() = iEvent.id().run();
  baby.event() = iEvent.id().event();
  baby.lumiblock() = iEvent.luminosityBlock();

  ////////////////////// Gen Lumi info //////////////////////
  baby.gen_cfgid() = gen_cfgid_;
  baby.mprod() = mprod_;
  baby.mlsp() = mlsp_;

  ////////////////// MC particles and Truth-matching //////////////////
  if (debug) cout<<"INFO: Writing MC particles..."<<endl;
  edm::Handle<reco::GenParticleCollection> genParticles;
  iEvent.getByToken(genPtclToken, genParticles);
  writeMC(genParticles);

  ///////////////////// MC hard scatter info ///////////////////////
  if (debug) cout<<"INFO: Retrieving hard scatter info..."<<endl;
  if (!lheInfoToken.isUninitialized()) {
    edm::Handle<LHEEventProduct> lhe_info;
    iEvent.getByToken(lheInfoToken, lhe_info);
    writeGenInfo(lhe_info);
  }

  //////////////// Weights and systematics //////////////////
  edm::Handle<GenEventInfoProduct> gen_event_info;
  iEvent.getByToken(genEventProdToken, gen_event_info);
  if (gen_event_info->hasDJRValues()){
    baby.djr1() = gen_event_info->DJRValues()[0];
    baby.djr2() = gen_event_info->DJRValues()[1];
    baby.djr3() = gen_event_info->DJRValues()[2];
    baby.djr4() = gen_event_info->DJRValues()[3];
    baby.nme() = gen_event_info->nMEPartons();
  }

  ////////////////// GEN Jets //////////////////
  edm::Handle<reco::GenJetCollection> genJets;
  iEvent.getByToken(genJetsToken, genJets);
  baby.lead_genjet_pt() = genJets->at(0).pt();
  baby.ngenjets() = 0;
  baby.genht()=0;
  for (size_t ijet(0); ijet< genJets->size(); ijet++){
    baby.genht() += genJets->at(ijet).pt();
    if (genJets->at(ijet).pt()>30.) baby.ngenjets() +=1;
  }

  ////////////////// Filling the tree //////////////////
  baby.Fill();

  reportTime(iEvent);

}


/*
______                      _                     _ _   _             
| ___ \                    | |                   (_) | (_)            
| |_/ /_ __ __ _ _ __   ___| |__   __      ___ __ _| |_ _ _ __   __ _ 
| ___ \ '__/ _` | '_ \ / __| '_ \  \ \ /\ / / '__| | __| | '_ \ / _` |
| |_/ / | | (_| | | | | (__| | | |  \ V  V /| |  | | |_| | | | | (_| |
\____/|_|  \__,_|_| |_|\___|_| |_|   \_/\_/ |_|  |_|\__|_|_| |_|\__, |
                                                                 __/ |
                                                                |___/ 
*/


void bmaker_basic::writeGenInfo(edm::Handle<LHEEventProduct> lhe_info){
  // baby.nisr_me()=0; baby.ht_isr_me()=0.; 
  // for ( unsigned int icount = 0 ; icount < (unsigned int)lhe_info->hepeup().NUP; icount++ ) {
  //   unsigned int pdgid = abs(lhe_info->hepeup().IDUP[icount]);
  //   int status = lhe_info->hepeup().ISTUP[icount];
  //   int mom1id = abs(lhe_info->hepeup().IDUP[lhe_info->hepeup().MOTHUP[icount].first-1]);
  //   int mom2id = abs(lhe_info->hepeup().IDUP[lhe_info->hepeup().MOTHUP[icount].second-1]);
  //   float px = (lhe_info->hepeup().PUP[icount])[0];
  //   float py = (lhe_info->hepeup().PUP[icount])[1];
  //   float pt = sqrt(px*px+py*py);

  //   if(status==1 && (pdgid<6 || pdgid==21) && mom1id!=6 && mom2id!=6 && mom1id!=24 && mom2id!=24 
  //      && mom1id!=23 && mom2id!=23 && mom1id!=25 && mom2id!=25) {
  //      baby.nisr_me()++;
  //      baby.ht_isr_me() += pt;
  //   }

  // }

} // writeGenInfo

void bmaker_basic::writeMC(edm::Handle<reco::GenParticleCollection> genParticles){
  LVector isr_p4;
  baby.ntruleps()=0; baby.ntrumus()=0; baby.ntruels()=0; baby.ntrutaush()=0; baby.ntrutausl()=0;
  const size_t bsmid(1000000);
  for (size_t imc(0); imc < genParticles->size(); imc++) {
    const reco::GenParticle &mc = (*genParticles)[imc];
    size_t id = abs(mc.pdgId());
    bool isLast = mcTool->isLast(mc, id);
    // mcTool->printParticle(mc); // Prints various properties of the MC particle

    const reco::GenParticle *mom = nullptr;
    size_t momid = abs(mcTool->mom(mc, mom));
    bool isTop(id==6);
    bool isNewPhysics(id>=bsmid);
    bool isZ(id==23);
    bool isW(id==24);
    bool bTopOrBSM(id==5 && (momid==6 || momid>=bsmid));
    bool nuFromZ((id==12 || id==14 || id==16) && momid==23);
    bool eFromTopZ(id==11 && (momid==24 || momid==23));
    bool muFromTopZ(id==13 && (momid==24 || momid==23));
    bool tauFromTopZ(id==15 && (momid==24 || momid==23));
    bool fromWOrWTau(mcTool->fromWOrWTau(mc));

    if(isLast){
      //////// Finding p4 of ME ISR system
      if((isTop && outname.Contains("TTJets"))
         || (id==1000021 && (outname.BeginsWith("T1") || outname.BeginsWith("T5")))
	 || (id==1000006 && (outname.Contains("T2tt") || outname.Contains("T2bt")))
         || (isZ && outname.Contains("DY"))) isr_p4 -= mc.p4();

      //////// Saving interesting true particles
      if(isTop || isNewPhysics || isZ
         || isW || bTopOrBSM || eFromTopZ || muFromTopZ 
         || tauFromTopZ || nuFromZ || fromWOrWTau){
        baby.mc_id().push_back(mc.pdgId());
        baby.mc_pt().push_back(mc.pt());
        baby.mc_eta().push_back(mc.eta());
        baby.mc_phi().push_back(mc.phi());
        baby.mc_mass().push_back(mc.mass());
        baby.mc_mom().push_back(mcTool->mom(mc,mom));
	baby.mc_direct_mom().push_back(mc.mother()->pdgId());
      }

      //////// Counting true leptons
      if(muFromTopZ) baby.ntrumus()++;
      if(eFromTopZ)  baby.ntruels()++;
      if(tauFromTopZ){
        const reco::GenParticle *tauDaughter(0);
        if(mcTool->decaysTo(mc, 11, tauDaughter) || mcTool->decaysTo(mc, 13, tauDaughter)){
          baby.mc_id().push_back(tauDaughter->pdgId());
          baby.mc_pt().push_back(tauDaughter->pt());
          baby.mc_eta().push_back(tauDaughter->eta());
          baby.mc_phi().push_back(tauDaughter->phi());
          baby.mc_mom().push_back(mcTool->mom(*tauDaughter,mom));
	  baby.mc_direct_mom().push_back(mc.mother()->pdgId());
          baby.ntrutausl()++;
        } else baby.ntrutaush()++;
      }
    }
  }

  baby.ntruleps() = baby.ntrumus()+baby.ntruels()+baby.ntrutaush()+baby.ntrutausl();
  baby.isr_tru_pt() = isr_p4.pt();
  baby.isr_tru_eta() = isr_p4.eta();
  baby.isr_tru_phi() = isr_p4.phi();

  // baby.met_tru_nuw() = hypot(metw_tru_x, metw_tru_y);
  // baby.met_tru_nuw_phi() = atan2(metw_tru_y, metw_tru_x);

  // baby.mt_tru()     = getMT(baby.met_tru(),     baby.met_tru_phi(),     lep_tru_pt, lep_tru_phi);
  // baby.mt_tru_nuw() = getMT(baby.met_tru_nuw(), baby.met_tru_nuw_phi(), lep_tru_pt, lep_tru_phi);
} // writeMC


/*
 _____                 _                   _                 
/  __ \               | |                 | |                
| /  \/ ___  _ __  ___| |_ _ __ _   _  ___| |_ ___  _ __ ___ 
| |    / _ \| '_ \/ __| __| '__| | | |/ __| __/ _ \| '__/ __|
| \__/\ (_) | | | \__ \ |_| |  | |_| | (__| || (_) | |  \__ \
 \____/\___/|_| |_|___/\__|_|   \__,_|\___|\__\___/|_|  |___/
*/

bmaker_basic::bmaker_basic(const edm::ParameterSet& iConfig):
  genPtclToken(consumes<reco::GenParticleCollection>(iConfig.getParameter<edm::InputTag>("genPtclTag"))),
  genEventProdToken(consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("genInfoTag"))),
  genLumiHeaderToken(consumes<GenLumiInfoHeader,edm::InLumi>(iConfig.getParameter<edm::InputTag>("genInfoTag"))),
  outname(TString(iConfig.getParameter<string>("outputFile"))),
  inputfiles(iConfig.getParameter<vector<string> >("inputFiles")),
  condor_subtime(iConfig.getParameter<string>("condor_subtime")),
  nevents_sample(iConfig.getParameter<unsigned int>("nEventsSample")),
  nevents(0),
  debug(iConfig.getParameter<bool>("debugMode")),
  mgp_wf(iConfig.getParameter<bool>("mgp_wf"))
{
  time(&startTime);

  mcTool = new mc_tools();

  edm::InputTag lheInfoTag = iConfig.getParameter<edm::InputTag>("lheInfoTag");
  if (!mgp_wf && lheInfoTag.label()!="") lheInfoToken = consumes<LHEEventProduct>(lheInfoTag);

  genJetsToken = consumes<reco::GenJetCollection>(edm::InputTag("ak4GenJets"));

  outfile = new TFile(outname, "recreate");
  outfile->cd();
  baby.tree_.SetDirectory(outfile);

}


bmaker_basic::~bmaker_basic(){
  outfile->cd();
  baby.tree_.SetDirectory(outfile);
  baby.Write();

  string commit_s = execute("git rev-parse HEAD");
  while(!commit_s.empty() && commit_s.at(commit_s.length()-1) == '\n') commit_s.erase(commit_s.length()-1);
  TString commit = commit_s;
  TString type = baby.Type();
  TString root_version = gROOT->GetVersion();
  TString root_tutorial_dir = gROOT->GetTutorialsDir();
  TString user(getenv("ORIGIN_USER"));
  if (user=="") user = getenv("USER");
  TString cmssw(getenv("CMSSW_BASE"));
  time_t curTime;
  time(&curTime);
  char time_c[100];
  struct tm * timeinfo = localtime(&curTime);
  strftime(time_c,100,"%Y-%m-%d %H:%M:%S",timeinfo);
  TString date(time_c);
  int seconds(floor(difftime(curTime,startTime)+0.5));

  TTree treeglobal("treeglobal", "treeglobal");
  treeglobal.Branch("nev_sample", &nevents_sample);
  treeglobal.Branch("nev_file", &nevents);
  treeglobal.Branch("runtime_seconds", &seconds);
  treeglobal.Branch("git_commit", &commit);
  treeglobal.Branch("baby_type", &type);
  treeglobal.Branch("root_version", &root_version);
  treeglobal.Branch("root_tutorial_dir", &root_tutorial_dir);
  treeglobal.Branch("user", &user);
  treeglobal.Branch("cmssw", &cmssw);
  treeglobal.Branch("date", &date);
  treeglobal.Branch("inputfiles", &inputfiles);
  treeglobal.Branch("condor_subtime", &condor_subtime);
  treeglobal.Fill();
  treeglobal.SetDirectory(outfile);
  treeglobal.Write();
  
  outfile->Close();

  int minutes((seconds/60)%60), hours(seconds/3600);
  TString runtime("");
  if(hours<10) runtime += "0";
  runtime += hours; runtime += ":";
  if(minutes<10) runtime += "0";
  runtime += minutes; runtime += ":";
  if((seconds%60)<10) runtime += "0";
  runtime += seconds%60; 
  float hertz(nevents); hertz /= seconds;
  cout<<endl<<"BABYMAKER: Written "<<nevents<<" events in "<<outname<<". It took "<<seconds<<" seconds to run ("<<runtime<<"), "
      <<roundNumber(hertz,1)<<" Hz, "<<roundNumber(1000,2,hertz)<<" ms per event"<<endl<<endl;
  cout<<"BABYMAKER: *********** List of input files ***********"<<endl;
  for(size_t ifile(0); ifile < inputfiles.size(); ifile++)
    cout<<"BABYMAKER: "<<inputfiles[ifile].c_str()<<endl;
  cout<<endl;

  delete outfile;

  delete mcTool;
}

void bmaker_basic::reportTime(const edm::Event& iEvent){
  // Time reporting
  if(nevents==1) {
    time_t curTime;
    time(&curTime);
    cout<<endl<<"BABYMAKER: Took "<<roundNumber(difftime(curTime,startTime),1)<<" seconds for set up and run first event"
        <<endl<<endl;
    time(&startTime);
  }
  if(debug || (nevents<100&&nevents%10==0) || (nevents<1000&&nevents%100==0) 
     || (nevents<10000&&nevents%1000==0) || nevents%10000==0) {
    time_t curTime;
    time(&curTime);
    float seconds(difftime(curTime,startTime));
    cout<<"BABYMAKER: Run "<<iEvent.id().run()<<", Event "<< setw(8)<<iEvent.id().event()
        <<", LumiSection "<< setw(5)<< iEvent.luminosityBlock()
        <<". Ran "<<setw(7)<<nevents<<" events in "<<setw(7)<<seconds<<" seconds -> "
        <<setw(5)<<roundNumber(nevents-1,1,seconds)<<" Hz, "
        <<setw(5)<<roundNumber(seconds*1000,2,nevents-1)<<" ms per event"<<endl;
  }
}

// ------------ method called once each job just before starting event loop  ------------
void bmaker_basic::beginJob() {
}

// ------------ method called once each job just after ending the event loop  ------------
void bmaker_basic::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
void 
bmaker_basic::beginRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a run  ------------
/*
void 
bmaker_basic::endRun(edm::Run const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when starting to processes a luminosity block  ------------
void bmaker_basic::beginLuminosityBlock(edm::LuminosityBlock const& iLumi, edm::EventSetup const& iEventSetup){

  edm::Handle<GenLumiInfoHeader> gen_header;
  iLumi.getByToken(genLumiHeaderToken, gen_header);
  
  gen_cfgid_ = gen_header->randomConfigIndex();
  if (mgp_wf) {
    string model = gen_header->configDescription();
    mcTool->getMassPoints(model, mprod_, mlsp_);
  }

}


// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
bmaker_basic::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
bmaker_basic::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(bmaker_basic);
