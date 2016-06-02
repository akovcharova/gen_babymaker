###########################################################
### Configuration file to make basic babies from miniAOD
###########################################################
import math, sys
from   os import environ
from   os.path import exists, join, basename

###### Input parameters parsing 
import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
options = VarParsing ('analysis')
options.register('nEventsSample',
                 100,
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.int,
                 "Total number of events in dataset for event weight calculation.")
options.register('nEvents',
                 100,
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.int,
                 "Number of events to run over.")
options.register('condorSubTime',
                 '000000_000000',
                 VarParsing.multiplicity.singleton,
                 VarParsing.varType.string,
                 "Timestamp from condor submission")
options.parseArguments()
outName = options.outputFile
if outName == "output.root": # output filename not set
    rootfile = basename(options.inputFiles[0])
    outName = "baby_"+rootfile

###### Defining Baby process, input and output files 
process = cms.Process("Baby")
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring(options.inputFiles)
)

process.baby_basic = cms.EDAnalyzer('bmaker_basic',
                                    condor_subtime = cms.string(options.condorSubTime),
                                    outputFile = cms.string(outName),
                                    inputFiles = cms.vstring(options.inputFiles),
                                    nEventsSample = cms.uint32(options.nEventsSample),
                                    debugMode = cms.bool(False),
                                    mgp_wf = cms.bool(True), # if True read  GenLumiInfoHeader, to get GEN config
                                    # genPtclTag = cms.InputTag("prunedGenParticles"),
                                    genPtclTag = cms.InputTag("genParticles"),
                                    genMETTag = cms.InputTag("genMetTrue"),
                                     # lheInfoTag = cms.InputTag("externalLHEProducer"),
                                    lheInfoTag = cms.InputTag("source"),
                                    genInfoTag = cms.InputTag("generator")
)

###### Setting up number of events, and reporing frequency 
process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.nEvents) )
process.MessageLogger.cerr.FwkReport.reportEvery = 100000


process.p = cms.Path(process.baby_basic)
