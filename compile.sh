#! /bin/bash

## Compiling autobaby which generates baby_base and associated babies
cd bmaker/autobaby
exit_code=0;
if [ $# -ne 0 ] && [ "$1" == "clean" ]
then
    rm -rf run/*.exe bin/*.o bin/*.a bin/*.d *.exe *.out
    ./run/remove_backups.sh
    exit_code=$?
else
    tmp_file=mktemp
    
    make -j $(getconf _NPROCESSORS_ONLN) -k -r -R 2> >(tee $tmp_file >&2)
    exit_code=$?
    
    echo
    
    if [[ $exit_code != 0 ]] ; then
	echo "ERRORS AND WARNINGS:"
	cat $tmp_file >&2
    else
	echo "Generated baby header and source code files :o)"
	echo
    fi
    
    rm -rf $tmp_file
fi

## Scramming CMSSW code
cd ../../..  ## Going back to the base babymaker folder

if [ $# -ne 0 ] && [ "$1" == "clean" ]
then
    scram b clean
else
    scram b -j$(getconf _NPROCESSORS_ONLN)
fi

cd gen_babymaker

exit $exit_code
