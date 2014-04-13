DESTDIR=/

all:
	gcc  -std=c99 -ansi -pedantic -fstack-protector-all -g -Wall -o g500-control g500-control.c
install:
	install -m 755 g500-control $(DESTDIR)/usr/sbin/
uninstall:
	rm -f $(DESTDIR)/usr/sbin/g500-control
clean:
	rm -f g500-control
