#!/bin/sh
#cd src/autocode/
#./vnpktcoder ../protocols/prelogging.pkt
cd `dirname $0`

if [ "$1" == "release" ];then
	BUILD_TYPE="Release"
	cd build/release
else
	BUILD_TYPE="Debug"
	cd build/debug
	
fi

cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ../../prj/cmake

echo $?

if [ $? == 0 ];then
	make
	if [ $? == 0 ]; then
		kill -9 `ps x|grep spider|grep -v "grep"|awk '{print $2}'`
		cp spider ../../run/bin
		echo "success!"
	fi
fi

