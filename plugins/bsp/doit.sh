cd $(dirname $0) # join dir of this script

gcc -m32 -fPIC -c bsp.c -o bsp.o
gcc -m32 -shared -o bsp.so bsp.o
cp bsp.so ../../..