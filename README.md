# gen_babymaker

Forked from `manuelfs/babymaker` by Adam Dishaw and Manuel Franco Sevilla.

gen_babymaker
==============

CMSSW code to generate babies (small flat ntuples) from RAWSIM

#### Code setup
To set up the code and generate a file named `baby.root`, issue the following commands 
on lxplus:

    cmsrel CMSSW_8_0_5_patch1
    cd CMSSW_8_0_5_patch1/src
    cmsenv
    git clone git@github.com:trandbea/gen_babymaker
    cd gen_babymaker
    ./compile.sh
    ./genbaby <model>

The `compile.sh` script first compiles the `gen_babymaker/bmaker/autobaby` folder, which
automatically generates the tree structure (see below), and then issues `scram b`
in the `gen_babymaker` folder. 


#### Adding new branches

To add new branches to the tree, you first create the new branch in
`gen_babymaker/variables/basic` where the type and name are specified.
The code in `babymaker/bmaker/genfiles/src/generate_baby.cxx` automatically generates
the files 

    babymaker/bmaker/interface/baby_base.hh
    babymaker/bmaker/interface/baby_basic.hh
    babymaker/bmaker/src/baby_base.cc
    babymaker/bmaker/src/baby_basic.cc

which have the c++ code that defines the class `baby_basic` with the tree, all the branches,
automatic vector clearing in the `baby_basic::Fill` method, and other useful functions.

The branches are filled in the `bmaker_basic::analyze` method in 
`gen_babymaker/bmaker/plugins/bmaker_basic.cc`. Helper functions for unctions that define physics quantities,
like isolation or electron ID, are definedtracing generator record are in `gen_babymaker/bmaker/src/mc_tools.cc`.
