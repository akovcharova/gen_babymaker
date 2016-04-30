#!/usr/bin/env python

import os

test = "wfa"

wf = {}
wf["wfa"] = "file:/net/cms2/cms2r0/aovcharova/data/RelValSusySignalTest1_PUpmx25ns_80X_mcRun2_asymptotic_v12_FastSim-v1_84B40B3B-1308-E611-9783-0025905A60DA.root"#,file:/net/cms2/cms2r0/aovcharova/data/RelValSusySignalTest1_PUpmx25ns_80X_mcRun2_asymptotic_v12_FastSim-v1_D685BD24-1308-E611-A68D-0CC47A4D7616.root"
wf["wfb"] = "file:/net/cms2/cms2r0/aovcharova/data/RelValSusySignalTest2_PUpmx25ns_80X_mcRun2_asymptotic_v12_FastSim-v1_8E9051AC-4D08-E611-9C2E-0025905B85A2.root,file:/net/cms2/cms2r0/aovcharova/data/RelValSusySignalTest2_PUpmx25ns_80X_mcRun2_asymptotic_v12_FastSim-v1_C2155CF4-4B08-E611-A8D8-0025905A60A6.root"
wf["wfc"] = "file:/net/cms2/cms2r0/aovcharova/data/RelValSusySignalTest3_PUpmx25ns_80X_mcRun2_asymptotic_v12_FastSim-v1_B4061001-2B08-E611-A485-0025905A6136.root,file:/net/cms2/cms2r0/aovcharova/data/RelValSusySignalTest3_PUpmx25ns_80X_mcRun2_asymptotic_v12_FastSim-v1_E0B78FF4-2A08-E611-AD8D-0CC47A78A436.root"


cmd = "cmsRun bmaker/python/bmaker_basic_cfg.py"
cmd += " inputFiles="+wf[test]
cmd += " outputFile="+test+".root"
cmd += " nEvents=10"

print cmd
os.system(cmd)