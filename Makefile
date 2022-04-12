all: semprimes msgprimes

semprimes: semprimes.c
	gcc semprimes.c -o semprimes -lm
	

msgprimes: msgprimes.c
	gcc msgprimes.c -o msgprimes

clean:
	rm semprimes msgprimes