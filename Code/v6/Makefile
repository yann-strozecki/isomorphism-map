CC= gcc
CFLAGS= -O3 -Wall -std=c11 -c -g 
POSTFLAGS= -lm 

kekule: main.o input.o concat.o fold.o signature.o chrono.o hash.o 
	$(CC) -o kekule main.o input.o concat.o fold.o signature.o chrono.o hash.o $(POSTFLAGS)

main.o: main.c structure.h input.h concat.h hash.h
	$(CC) $(CFLAGS) main.c 

input.o: input.c input.h structure.h
	$(CC) $(CFLAGS) input.c

concat.o: concat.c concat.h structure.h input.h fold.h 
	$(CC) $(CFLAGS) concat.c 

fold.o: fold.c fold.h structure.h concat.h signature.h 
	$(CC) $(CFLAGS) fold.c

signature.o: signature.c signature.h structure.h
	$(CC) $(CFLAGS) signature.c 

hash.o: hash.c hash.h signature.h structure.h 
	$(CC) $(CFLAGS) hash.c 

chrono.o: chrono.c chrono.h
	$(CC) $(CFLAGS) chrono.c 

clean:
	rm -f *.o
	rm -f kekule
	rm -rf Kekule kekule.tgz

mrpropre: clean
	rm -f callgrind.out.*
	rm -f debug.log

YI: clean kekule
	./kekule ../Examples/testYI.mot 10
	./kekule ../Examples/testYI.mot 15
	./kekule ../Examples/testYI.mot 20
	./kekule ../Examples/testYI.mot 25	

YVV: clean kekule
	./kekule ../Examples/testYVV.mot 8
	./kekule ../Examples/testYVV.mot 16
	./kekule ../Examples/testYVV.mot 24

XVI: clean kekule
	./kekule ../Examples/testXVI.mot 4
	./kekule ../Examples/testXVI.mot 8
	./kekule ../Examples/testXVI.mot 12
	./kekule ../Examples/testXVI.mot 16
	./kekule ../Examples/testXVI.mot 20

IVV: clean kekule
	./kekule ../Examples/testIVV.mot 6
	./kekule ../Examples/testIVV.mot 9
	./kekule ../Examples/testIVV.mot 12
	./kekule ../Examples/testIVV.mot 15
	./kekule ../Examples/testIVV.mot 18
	./kekule ../Examples/testIVV.mot 24

JYV: clean kekule
	./kekule ../Examples/testJYV.mot 4
	./kekule ../Examples/testJYV.mot 8
	./kekule ../Examples/testJYV.mot 12
	./kekule ../Examples/testJYV.mot 16
	./kekule ../Examples/testJYV.mot 20

YVI: clean kekule
	./kekule ../Examples/testYVI.mot 13

Y4: clean kekule
	./kekule ../Examples/testY4.mot 4
	./kekule ../Examples/testY4.mot 6
	./kekule ../Examples/testY4.mot 8
	./kekule ../Examples/testY4.mot 10

XY: clean kekule
	./kekule ../Examples/testXY.mot 7
	./kekule ../Examples/testXY.mot 14
	./kekule ../Examples/testXY.mot 21

IVK: clean kekule
	./kekule ../Examples/testIVK.mot 6
	./kekule ../Examples/testIVK.mot 12
	./kekule ../Examples/testIVK.mot 18


XI: clean kekule
	./kekule ../Examples/testXI.mot 6
	./kekule ../Examples/testXI.mot 9
	./kekule ../Examples/testXI.mot 12
	./kekule ../Examples/testXI.mot 15
	./kekule ../Examples/testXI.mot 18

JVV: clean kekule
	./kekule ../Examples/testJVV.mot 6
	./kekule ../Examples/testJVV.mot 9	
	./kekule ../Examples/testJVV.mot 12
	./kekule ../Examples/testJVV.mot 15
	./kekule ../Examples/testJVV.mot 18

debug: clean kekule
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./kekule ../Examples/testIVV.mot 3

profile: clean kekule
	valgrind --tool=callgrind ./kekule ../Examples/testIVV.mot 18
	kcachegrind

git: clean
	git pull

tar: clean
	mkdir Kekule
	cp *.c *.h Kekule
	cp Makefile Kekule
	cp -r ../Examples Kekule
	tar cvfz kekule.tgz Kekule
	rm -rf Kekule
	ls -sh kekule.tgz
