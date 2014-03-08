cd $(dirname $0) # join dir of this script

gcc -m32 -fPIC -c print.c -o print.o
gcc -m32 -shared -o print.so print.o
mv print.so ../../..