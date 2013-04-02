#!/bin/sh


LOCAL_PATH=`dirname $0`
LOCAL_PATH=`cd $LOCAL_PATH && pwd`
NDK_DEBUG=1
#ln -sf libsdl-1.2.so $LOCAL_PATH/../../../obj/local/armeabi/libSDL.so


LNG="ENGLISH"
../setEnvironment.sh sh -c "LNG=\"ENGLISH\" make" && cp -f ja2 libapplication.so
