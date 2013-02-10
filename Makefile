all:
	gcc i2c-rotation.c -o i2c-rotation

clean:
	rm i2c-rotation

install:
	cp i2c-rotation /usr/local/bin/
	cp watchrotation /usr/local/bin/
	cp tabletmode /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/i2c-rotation
	rm -f /usr/local/bin/watchrotation
	rm -f /usr/local/bin/tabletmode
