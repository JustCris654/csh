##
# See shell
#
# @file
# @version 0.1

build_and_run: main run

main: main.c
	clang main.c -o main

run: main
	./main

clean:
	rm main

# end
