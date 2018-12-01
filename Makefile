test:
	gcc combination.c -o /dev/null

run: build exec clean

build:
	gcc combination.c

exec: a.out
	./a.out

clean: a.out
	rm ./a.out
