
if [ "$1" == "" ]
then 
	echo "usage ./do_debug 32 or 64 ...."
	exit 1
fi

if [ "$2" == "" ]
then
        echo "usage ./do_debug 32 or 64 Linux or Windows ...."
        exit 1
fi


#rm -rf Debug$2$1

#mkdir Debug$2$1
#cd Debug$2$1
rm -rf build
make -C templatexml clean all
cmake -DCMAKE_INSTALL_PREFIX=Debug$2$1 -DCMAKE_TOOLCHAIN_FILE=./$1_$2_i686.toolchain.cmake  -DCMAKE_BUILD_TYPE:STRING=Debug -H$PWD -B$PWD/build "-GUnix Makefiles"
make -C build -j8 all install

