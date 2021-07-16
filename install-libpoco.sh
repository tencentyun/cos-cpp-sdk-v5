#!/bin/sh
cp third_party/lib/linux/poco/libPocoCrypto.so /usr/lib64/libPocoCrypto.so.64
cp third_party/lib/linux/poco/libPocoFoundation.so /usr/lib64/libPocoFoundation.so.64
cp third_party/lib/linux/poco/libPocoJSON.so /usr/lib64/libPocoJSON.so.64
cp third_party/lib/linux/poco/libPocoNet.so /usr/lib64/libPocoNet.so.64
cp third_party/lib/linux/poco/libPocoNetSSL.so /usr/lib64/libPocoNetSSL.so.64
cp third_party/lib/linux/poco/libPocoUtil.so /usr/lib64/libPocoUtil.so.64
cp third_party/lib/linux/poco/libPocoXML.so /usr/lib64/libPocoXML.so.64
cd /usr/lib64/
ln -s libPocoCrypto.so.64 libPocoCrypto.s0
ln -s libPocoFoundation.so.64 libPocoFoundation.so
ln -s libPocoJSON.so.64 libPocoJSON.so
ln -s libPocoNet.so.64 libPocoNet.so
ln -s libPocoNetSSL.so.64 libPocoNetSSL.so
ln -s libPocoUtil.so.64 libPocoUtil.so
ln -s libPocoXML.so.64 libPocoXML.so