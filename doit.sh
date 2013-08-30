#
# help:
# -O1    = removes references to unused strings for printf_hide -> anti reversing | found "-O3 -s" but dunno the advantage
#
# ./doit.sh tar
# ./doit.sh cod1_1_5
# ./doit.sh cod2_1_2
# ./doit.sh cod2_1_3
# ./doit.sh cod4_1_7
# ./doit.sh wrapper
#

mkdir -p bin
mkdir -p objects_normal

#options="-I. -m32 -fPIC -fvisibility=hidden -O1"
options="-I. -m32 -fPIC"

#objects_tcc="build/gsc_tcc.opp /home/kung/tcc/tcc-0.9.25/libtcc.a"
	
tmp="/root/q3rally/q3rallysa/build";
objects_car="$tmp/q_shared.o $tmp/q_math.o $tmp/com_printf.o $tmp/bg_wheel_forces.o $tmp/bg_pmove.o $tmp/bg_physics.o $tmp/bg_misc.o"

mysql_link="-lmysqlclient"
#mysql_link=""

if [ "$1" == "" ] || [ "$1" == "tar" ]; then
	echo "##### TAR LIBCOD #####"
	
	rm libcod.tar
	
	#tar -cf  libcod.tar json
	#tar -rf  libcod.tar jsonrpc
	tar -cf  libcod.tar plugins
	#tar -rf  libcod.tar vendors
	
	tar -rf  libcod.tar *.c
	tar -rf  libcod.tar *.cpp
	tar -rf  libcod.tar *.hpp

	tar -rf  libcod.tar doit.sh
	echo "libcod.tar created: $?"
fi

if [ "$1" == "" ] || [ "$1" == "base" ]; then
	echo "##### COMPILE GSC_ASTAR.CPP #####"
	gcc $options -c gsc_astar.cpp -o objects_normal/gsc_astar.opp
	echo "##### COMPILE GSC_MYSQL.CPP #####"
	gcc $options -c gsc_mysql.cpp -o objects_normal/gsc_mysql.opp -lmysqlclient -L/usr/lib/mysql
	echo "##### COMPILE SERVER.CPP #####"
	gcc $options -c server.c -o objects_normal/server.opp -D SERVER_PORT=8000
	echo "##### COMPILE GSC_MEMORY.CPP #####"
	gcc $options -c gsc_memory.cpp -o objects_normal/gsc_memory.opp
	echo "##### COMPILE cracking.CPP #####"
	gcc $options -c cracking.cpp -o objects_normal/cracking.opp
fi

if [ "$1" == "" ] || [ "$1" == "clear" ]; then
	echo "##### CLEAR OBJECTS #####"
	rm objects_* -r
fi


if [ "$1" == "" ] || [ "$1" == "tcc" ]; then
	mkdir -p objects_tcc

	echo "##### COMPILE gsc_tcc.cpp #####"
	gcc $options -c gsc_tcc.cpp -o objects_tcc/gsc_tcc.opp
fi

if [ "$1" == "" ] || [ "$1" == "car" ]; then
	mkdir -p objects_car

	echo "##### COMPILE GSC_CAR.CPP #####"
	gcc $options -c gsc_car.cpp -o objects_car/gsc_car.opp -I/root/q3rally/q3rallysa/
fi

if [ "$1" == "" ] || [ "$1" == "cod2_1_3" ]; then
	constants="-D COD2_VERSION=COD2_VERSION_1_3 -D COD_VERSION=COD2_1_2"

	mkdir -p objects_$1
	echo "##### COMPILE $1 LIBCOD.CPP #####"
	gcc $options $constants -o objects_$1/libcod.opp -c libcod.cpp
	echo "##### COMPILE $1 GSC.CPP #####"
	gcc $options $constants -o objects_$1/gsc.opp -c gsc.cpp
	echo "##### COMPILE $1 GSC_PLAYER.CPP #####"
	gcc $options $constants -o objects_$1/gsc_player.opp -c gsc_player.cpp
	echo "##### COMPILE $1 GSC_UTILS.CPP #####"
	gcc $options $constants -o objects_$1/gsc_utils.opp -c gsc_utils.cpp

	echo "##### LINK lib$1.so #####"
	objects="$(ls objects_normal/*.opp) $(ls objects_$1/*.opp)"
	gcc -m32 -shared -L/lib32 $mysql_link -L./vendors/lib -o bin/lib$1.so $objects $objects_tcc -Os -s -ldl -Wall
fi
# -Xlinker --defsym -Xlinker stackStart=0x08297500 

if [ "$1" == "" ] || [ "$1" == "cod2_1_2" ]; then
	constants="-D COD2_VERSION=COD2_VERSION_1_2 -D COD_VERSION=COD2_1_2"

	mkdir -p objects_$1
	echo "##### COMPILE $1 LIBCOD.CPP #####"
	gcc $options $constants -o objects_$1/libcod.opp -c libcod.cpp
	echo "##### COMPILE $1 GSC.CPP #####"
	gcc $options $constants -o objects_$1/gsc.opp -c gsc.cpp
	echo "##### COMPILE $1 GSC_PLAYER.CPP #####"
	gcc $options $constants -o objects_$1/gsc_player.opp -c gsc_player.cpp
	echo "##### COMPILE $1 GSC_UTILS.CPP #####"
	gcc $options $constants -o objects_$1/gsc_utils.opp -c gsc_utils.cpp

	echo "##### LINK lib$1.so #####"
	objects="$(ls objects_normal/*.opp) $(ls objects_$1/*.opp)"
	gcc -m32 -shared -L/lib32 $mysql_link -L./vendors/lib -o bin/lib$1.so $objects $objects_tcc -Os -s -ldl -Wall
fi


if [ "$1" == "" ] || [ "$1" == "cod1_1_5" ]; then
	constants="-D COD_VERSION=COD1_1_5"
	
	#echo "##### COMPILE CoD1 1.5 LIBCOD2.CPP #####"
	#gcc $options $constants -c libcod2.cpp -o libcod2.opp
	echo "##### COMPILE CoD1 1.5 GSC.CPP #####"
	gcc $options $constants -c gsc.cpp -o gsc.opp
	#echo "##### COMPILE CoD1 1.5 GSC_PLAYER.CPP #####"
	#gcc $options $constants -c gsc_player.cpp -o gsc_player.opp

	echo "##### LINK libcod1_1_5.so #####"
	gcc -m32 -shared -L/lib32 -lmysqlclient -L./vendors/lib -o bin/libcod1_1_5.so libcod2.opp gsc.opp gsc_player.opp gsc_astar.opp gsc_mysql.opp server.opp gsc_memory.opp cracking.opp $objects_tcc -Os -s -ldl -Wall
fi


if [ "$1" == "" ] || [ "$1" == "cod4_1_7" ]; then
	constants="-D COD_VERSION=COD4_1_7"

	mkdir -p objects_$1
	echo "##### COMPILE $1 LIBCOD.CPP #####"
	gcc $options $constants -o objects_$1/libcod.opp -c libcod.cpp
	echo "##### COMPILE $1 GSC.CPP #####"
	gcc $options $constants -o objects_$1/gsc.opp -c gsc.cpp
	echo "##### COMPILE $1 GSC_PLAYER.CPP #####"
	gcc $options $constants -o objects_$1/gsc_player.opp -c gsc_player.cpp
	echo "##### COMPILE $1 GSC_UTILS.CPP #####"
	gcc $options $constants -o objects_$1/gsc_utils.opp -c gsc_utils.cpp

	echo "##### LINK lib$1.so #####"
	objects="$(ls objects_normal/*.opp) $(ls objects_$1/*.opp)"
	gcc -m32 -shared -L/lib32 $mysql_link -L./vendors/lib -o bin/lib$1.so $objects $objects_tcc -Os -s -ldl -Wall
fi



if [ "$1" == "wrapper" ]; then
	echo "##### WRAPPER: COMPILE wrapper_libcod2.cpp #####"
	cp wrapper_libcod2.cpp a.cpp # just for name hiding in the .opp/.so -.-
	gcc -m32 -fPIC -c a.cpp -o wrapper_libcod2.opp

	#strip wrapper_libcod2.opp

	# make the shared lib for the wrapper
	echo "##### WRAPPER: LINK wrapper_libcod2.so #####"
	gcc -m32 -shared -L/lib32 -o wrapper_libcod2.so wrapper_libcod2.opp


	cp wrapper_libcod2.so bin/libcod2.so
	#cp wrapper_libcod2.so /root/helper/game_cod2/libs/libcod2.so # other then "mv", "ld" can use it so (not just from game-libs-folder)
fi