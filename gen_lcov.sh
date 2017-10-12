#!/bin/sh

workspace=`pwd`
EXTRACT="${workspace}/*/op/* ${workspace}/*/util/* ${workspace}/*/request/*  ${workspace}/*/response/* ${workspace}/include"
# clear
rm UTReport -rf
rm UTResport.tar

mkdir -p build
cd build
cmake -DENABLE_COVERAGE=ON ..
gmake all

# init
cd ..
lcov -d build -z
lcov -d build -b . --no-external --initial -c -o sevenyou_init.info

# run
cd build/bin
./auth_tool_test
./object_request_test
./object_op_test
./bucket_op_test

# second
cd ../..
lcov -d build -b . --no-external -c -o sevenyou.info

# filt
lcov --extract sevenyou_init.info ${EXTRACT} -o sevenyou_init_filted.info
lcov --extract sevenyou.info ${EXTRACT} -o sevenyou_filted.info

# genhtml and zip
genhtml -o UTReport --prefix=`pwd` sevenyou_init_filted.info sevenyou_filted.info
tar -cvf UTReport.tar UTReport

rm sevenyou_init.info
rm sevenyou_init_filted.info
rm sevenyou.info
rm sevenyou_filted.info
rm UTReport -rf
