build:
	clang -std=c99 main.c -Wall -Werror -pedantic -o main

run: build
	./main

clean:
	rm main
