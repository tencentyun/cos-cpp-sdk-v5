#!/bin/sh

workspace=`pwd`
EXTRACT="${workspace}/*/op/* ${workspace}/*/util/* ${workspace}/*/request/* ${workspace}/*/response/* ${workspace}/*/trsf/* ${workspace}/include/* ${workspace}/src/*"
# clear
rm UTReport -rf
rm UTResport.tar

mkdir -p build
cd build
cmake -DENABLE_COVERAGE=ON -DBUILD_UNITTEST=ON ..
gmake all -j 4

# init
cd ..
lcov -d build -z
lcov -d build -b . --no-external --initial -c -o sevenyou_init.info

# run
cd build/bin
./all-test
#./async-op-test
#./bucket-op-test
#./live-channel-test
#./object-op-test
#./object-request-test
#./object-response-test
#./util-test

# second
cd ../..
lcov -d build -b . --no-external -c -o sevenyou.info

# filt
lcov --extract sevenyou_init.info ${EXTRACT} -o sevenyou_init_filted.info
lcov --extract sevenyou.info ${EXTRACT} -o sevenyou_filted.info

lcov --remove sevenyou.info "${workspace}/third_party/*" "${workspace}/unittest/*" -o sevenyou_rm_third_party.info
rm sevenyou.info
mv sevenyou_rm_third_party.info sevenyou.info

# genhtml and zip
genhtml -o UTReport --prefix=`pwd` sevenyou_init_filted.info sevenyou_filted.info
tar -cvf UTReport.tar UTReport

# rm sevenyou_init.info
# rm sevenyou_init_filted.info
# rm sevenyou.info
# rm sevenyou_filted.info
rm UTReport -rf
