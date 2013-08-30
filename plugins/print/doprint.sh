	gcc -m32 -fPIC -c print.c -o print.o
	gcc -m32 -shared -o print.so print.o
	cp print.so /home/kung/20000/libs