all:
	cp ./c_src/*.c ./
	gcc -o server_x86 *.c -lsqlite3 -ljpeg -lpthread
	arm-none-linux-gnueabi-gcc -o server_a9 *.c -lsqlite3 -ljpeg -lpthread
	rm *.c
x86:
	cp ./c_src/*.c ./
	gcc -o server_x86 *.c -lsqlite3 -ljpeg -lpthread
	rm *.c
a9: 
	cp ./c_src/*.c ./
	arm-none-linux-gnueabi-gcc -o server_a9 *.c -lsqlite3 -ljpeg -lpthread
	rm *.c
clean:
	rm  server_x86 server_a9 *.c
