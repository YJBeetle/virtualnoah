#
# (C) Copyright 2003-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk
include $(TOPDIR)/include/config.mk

LIB	= $(obj)lib$(CPU).a

START	= start.o
COBJS	= jz4730.o jz4740.o jz5730.o jz_lcd.o jz4740_slcd.o jz4750_lcd.o jz_serial.o jz_eth.o jz_i2c.o \
	  asc_serial.o au1x00_serial.o au1x00_eth.o au1x00_usb_ohci.o \
	  cpu.o interrupts.o incaip_clock.o jz_mmc.o jz4730_nand.o jz4740_cpm_test.o \
	  jz5730_nand.o jz4740_nand.o jz4750_nand.o jz_cs8900.o jz4750.o jz4750_mmc.o \
	  jz4810.o jz4810_nand.o jz4810_eth.o

ifeq ($(CONFIG_CPU_TYPE),4760)
COBJS	+= jz4760.o jz4760_nand.o jz4760_eth.o
endif

ifeq ($(CONFIG_USE_MDDR),y)
COBJS	+= jz4760_ddr.o
COBJS	+= jz_mddr_init.o
endif

ifeq ($(CONFIG_USE_DDR1),y)
COBJS	+= jz4760_ddr.o
COBJS	+= jz_ddr1_init.o
endif

ifeq ($(CONFIG_USE_DDR2),y)
COBJS	+= jz4760_ddr.o
COBJS	+= jz_ddr2_init.o
endif

ifeq ($(CONFIG_USE_SDRAM),y)
COBJS	+= jz4760_sdram.o
endif

SOBJS	= incaip_wdt.o cache.o

SRCS	:= $(START:.o=.S) $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))
START	:= $(addprefix $(obj),$(START))

all:	$(obj).depend $(START) $(LIB)

$(LIB):	$(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
