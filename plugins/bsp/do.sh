cd `dirname $0`
gcc -m32 -fPIC -c bsp.c -o bsp.o
gcc -m32 -shared -o bsp.so bsp.o

cp bsp.so /home/kung/20000/libs