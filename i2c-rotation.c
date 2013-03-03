/*
   HTC Shift G-Sensor Reader
   Reads data from i2c interface, chip STMicroelectronics LIS3LV02DL

   Copyright (C) 2008       Esteve Espuna <esteve@eslack.org>
   Pau Oliva <pof@eslack.org>

   Based on i2cget.c:
   Copyright (C) 2005       Jean Delvare <khali@linux-fr.org>

   Based on i2cset.c, i2cbusses.c:
   Copyright (C) 2001-2003  Frodo Looijaard <frodol@dds.nl>, and
   Mark D. Studebaker <mdsxyz123@yahoo.com>
   Copyright (C) 2004-2005  Jean Delvare <khali@linux-fr.org>

   Modified by Arobase Chac (arobase40) and Imarune for the Acer 1825PT(Z).
   May work with some Dell machine, Samsung NB30, Acer 1410, 1810TZ, 1810T and some Packard Bell clone of the Acer tablet pc laptops...

   Modified by Thomas Mayer (Residuum).

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
   MA 02110-1301 USA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include "i2c-dev.h"

#define WHO_AM_I    0x3b

typedef enum {
	undefined,
	normal,
	left,
	right,
	inverted
} t_rotation;

char *rotation_names[] = {
	"",
	"normal",
	"left",
	"right",
	"inverted"
};

int open_i2c_dev(const int i2cbus, char *filename, const int quiet)
{
	int file;

	sprintf(filename, "/dev/i2c/%d", i2cbus);
	file = open(filename, O_RDWR);

	if (file < 0 && errno == ENOENT) {
		sprintf(filename, "/dev/i2c-%d", i2cbus);
		file = open(filename, O_RDWR);
	}

	if (file < 0 && !quiet) {
		if (errno == ENOENT) {
			fprintf(stderr, "Error: Could not open file "
					"`/dev/i2c-%d' or `/dev/i2c/%d': %s\n",
					i2cbus, i2cbus, strerror(ENOENT));
		} else {
			fprintf(stderr, "Error: Could not open file "
					"`%s': %s\n", filename, strerror(errno));
			if (errno == EACCES)
				fprintf(stderr, "Run as root?\n");
		}
	}

	return file;
}

int main(int argc, char *argv[])
{
	int res, i2cbus = 0, address, file, noloop = 0;
	t_rotation dir = undefined;
	t_rotation newdir = normal;
	int xl,xh;
	int yl,yh;
	int zl,zh;
	short x,y,z;
	int magic;
	char filename[20];

	if ( argc >= 2 ) {
		i2cbus = atoi(argv[1]);
		if (argc == 3 && strcmp(argv[2], "noloop") == 0)
			noloop = 1;
	}    
	else {
		fprintf(stderr, "Usage : %s i2cbus [noloop]\n", argv[0]);
		exit (1);
	}


	address = 0x1d;
	file = open_i2c_dev(i2cbus, filename , 0);

	ioctl(file, I2C_SLAVE, address);
	i2c_smbus_write_byte_data(file, 0x20, 0x47);
	i2c_smbus_write_byte_data(file, 0x21, 0x63);

	magic = i2c_smbus_read_byte_data(file, 0xf);
	if (magic != WHO_AM_I ) {
		printf("Accelerometer not found\n");
		exit(-1);
	}
	while (1)
	{
		xh = i2c_smbus_read_byte_data(file, 0x28);
		xl = i2c_smbus_read_byte_data(file, 0x29);
		x = (xh<<8) | ( xl );
		yh = i2c_smbus_read_byte_data(file, 0x2a);
		yl = i2c_smbus_read_byte_data(file, 0x2b);
		y = (yh<<8) | ( yl );
		zh = i2c_smbus_read_byte_data(file, 0x2c);
		zl = i2c_smbus_read_byte_data(file, 0x2d);
		z = (zh<<8) | ( zl );
		/*fprintf(stdout, "x = %d y = %d z = %d\n", x, y, z);
		fflush(stdout);*/
		if ((x < 20 || x > 240) 
				&& (y < 20 ||y > 240) 
				&& z > 40) {
			newdir = undefined; /* flat */
			/*fprintf(stdout, "flat\n");
			fflush(stdout);*/
		}
		else if (x > 128 
				&& y > 128) {
			newdir=normal;
		}
		else if (x < 20 
				&& y < 128 
				&& z < 40) {
			newdir=right;
		}
		else if (x < 20 
				&& y > 128 
				&& z < 40) {
			newdir=left;
		}
		else if (x > 40 
				&& y > 200 
				&& z > 20) {
			newdir=inverted;
		}
		if (noloop) {
			fprintf(stdout, "%s\n", rotation_names[newdir]);
			fflush(stdout);
			break;
		} else if (newdir != undefined && newdir != dir) {
			dir = newdir;
			fprintf(stdout, "%s\n", rotation_names[dir]);
			fflush(stdout);
		}
		sleep(2);
	}
	close(file);
	exit(0);
}
