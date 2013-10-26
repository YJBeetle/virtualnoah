#Makefile for mkyaffs
#
# NB this is not yet suitable for putting into the kernel tree.
# YAFFS: Yet another Flash File System. A NAND-flash specific file system. 
#
# Copyright (C) 2002 Aleph One Ltd.
#   for Toby Churchill Ltd and Brightstar Engineering
#
# Created by Charles Manning <charles@aleph1.co.uk>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.

## Change or override  KERNELDIR to your kernel
#KERNELDIR = /usr/src/kernel-headers-2.4.18
#CFLAGS =   -I$(KERNELDIR)/include -I.. -O2 -Wall -DCONFIG_YAFFS_UTIL

include ../../../.config

CFLAGS =   -I../../../include -I.. -O2 -Wall -DCONFIG_YAFFS_UTIL
CFLAGS+=   -Wshadow -Wpointer-arith -Wwrite-strings -Wstrict-prototypes -Wmissing-declarations
CFLAGS+=   -Wmissing-prototypes -Wredundant-decls -Wnested-externs -Winline

ifeq ($(CONFIG_YAFFS_ECC_RS), y)
CFLAGS+= -DCONFIG_YAFFS_ECC_RS
endif

ifeq ($(CONFIG_YAFFS_ECC_HAMMING), y)
CFLAGS+= -DCONFIG_YAFFS_ECC_HAMMING
endif

ifeq ($(CONFIG_MTD_HW_BCH_ECC), y)
CFLAGS+= -DCONFIG_MTD_HW_BCH_ECC
endif


## Change if you are using a cross-compiler
MAKETOOLS = #mipsel-linux-

CC=$(MAKETOOLS)gcc

COMMONLINKS = yaffs_ecc.c
COMMONOBJS = $(COMMONLINKS:.c=.o)

ifeq ($(CONFIG_YAFFS_ECC_RS), y)
COMMONOBJS += ssfdc_rs_ecc.o
endif

MKYAFFSSOURCES = mkyaffsimage.c
MKYAFFSIMAGEOBJS = $(MKYAFFSSOURCES:.c=.o)

MKYAFFS2SOURCES = mkyaffs2image.c
MKYAFFS2LINKS = yaffs_packedtags2.c yaffs_tagsvalidity.c
MKYAFFS2IMAGEOBJS = $(MKYAFFS2SOURCES:.c=.o) $(MKYAFFS2LINKS:.c=.o)

all: mkyaffsimage mkyaffs2image

$(COMMONLINKS) $(MKYAFFSLINKS) $(MKYAFFS2LINKS):
	ln -s ../$@ $@

$(COMMONOBJS) $(MKYAFFSIMAGEOBJS) $(MKYAFFS2IMAGEOBJS) : %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

mkyaffsimage: $(COMMONOBJS) $(MKYAFFSIMAGEOBJS)
	$(CC) -o $@ $(COMMONOBJS) $(MKYAFFSIMAGEOBJS)

mkyaffs2image: $(COMMONOBJS) $(MKYAFFS2IMAGEOBJS)
	$(CC) -o $@ $(COMMONOBJS) $(MKYAFFS2IMAGEOBJS)


clean:
	rm -f $(COMMONOBJS) $(MKYAFFSIMAGEOBJS) $(MKYAFFS2IMAGEOBJS) $(COMMONLINKS) $(MKYAFFSLINKS) $(MKYAFFS2LINKS) mkyaffsimage mkyaffs2image core
