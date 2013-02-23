all:
	gcc i2c-rotation.c -o i2c-rotation
	gcc `pkg-config --cflags --libs gtk+-2.0` tablet_trayicon.c -o tablet_trayicon

clean:
	rm i2c-rotation
	rm tablet_trayicon

install:
	cp i2c-rotation /usr/local/bin/
	cp watchrotation /usr/local/bin/
	cp tabletmode /usr/local/bin/
	cp tablet_trayicon /usr/local/bin/
	cp 99-i2c.rules /etc/udev/rules.d/
	cp -r Xfce-winter-touch-optimized /usr/share/themes/

uninstall:
	rm -f /usr/local/bin/i2c-rotation
	rm -f /usr/local/bin/watchrotation
	rm -f /usr/local/bin/tabletmode
	rm -f /usr/local/bin/tablet_trayicon
	rm -f /etc/udev/rules.d/99-i2c.rules 
	rm -rf /usr/share/themes/Xfce-winter-touch-optimized/
