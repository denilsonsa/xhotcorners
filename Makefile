all: xhotcorners

.PHONY: all clean

xhotcorners: xhotcorners.c
	gcc -o $@ $< -lxcb -lxcb-icccm -lxcb-ewmh -Wall

clean:
	rm -f xhotcorners
