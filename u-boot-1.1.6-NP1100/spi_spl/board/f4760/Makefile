#
# (C) Copyright 2009 Ingenic Semiconductor Inc.
# Author: Regen Huang <lhhuang@ingenic.cn>
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
include $(TOPDIR)/spi_spl/board/$(BOARDDIR)/config.mk

LDSCRIPT= $(TOPDIR)/spi_spl/board/$(BOARDDIR)/u-boot.lds
LDFLAGS	= -Bstatic -T $(LDSCRIPT) -Ttext $(TEXT_BASE)
AFLAGS	+= -DCONFIG_SPI_SPL
CFLAGS	+= -DCONFIG_SPI_SPL

SOBJS	= start.o
COBJS	= spi_boot.o cpu.o jz4760.o jz_serial.o

SRCS	:= $(addprefix $(obj),$(SOBJS:.o=.S) $(COBJS:.o=.c))
OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))
__OBJS	:= $(SOBJS) $(COBJS)
LNDIR	:= $(OBJTREE)/spi_spl/board/$(BOARDDIR)

spiobj	:= $(OBJTREE)/spi_spl/

ALL	= $(spiobj)u-boot-spl $(spiobj)u-boot-spl.bin $(spiobj)u-boot-spl-pad.bin
all:	$(obj).depend $(ALL)

$(spiobj)u-boot-spl-pad.bin: $(spiobj)u-boot-spl.bin
	dd bs=1024 count=8 if=/dev/zero of=$(spiobj)junk1
	cat $< $(spiobj)junk1 > $(spiobj)junk2
	dd bs=1024 count=8 if=$(spiobj)junk2 of=$@
	rm -f $(spiobj)junk*

$(spiobj)u-boot-spl.bin:	$(spiobj)u-boot-spl
	$(OBJCOPY) ${OBJCFLAGS} -O binary $< $@

$(spiobj)u-boot-spl:	$(OBJS)
	cd $(LNDIR) && $(LD) $(LDFLAGS) $$UNDEF_SYM $(__OBJS) \
		-Map $(spiobj)u-boot-spl.map \
		-o $(spiobj)u-boot-spl

# create symbolic links for common files

# from cpu directory
$(obj)start.S:
	@rm -f $(obj)start.S
	ln -s $(SRCTREE)/cpu/mips/start.S $(obj)start.S

$(obj)cpu.c:
	@rm -f $(obj)cpu.c
	ln -s $(SRCTREE)/cpu/mips/cpu.c $(obj)cpu.c

$(obj)jz4760.c:
	@rm -f $(obj)jz4760.c
	ln -s $(SRCTREE)/cpu/mips/jz4760.c $(obj)jz4760.c

$(obj)jz_serial.c:
	@rm -f $(obj)jz_serial.c
	ln -s $(SRCTREE)/cpu/mips/jz_serial.c $(obj)jz_serial.c

# from spi_spl directory
$(obj)spi_boot.c:
	@rm -f $(obj)spi_boot.c
	ln -s $(SRCTREE)/spi_spl/spi_boot.c $(obj)spi_boot.c

#########################################################################

$(obj)%.o:	$(obj)%.S
	$(CC) $(AFLAGS) -c -o $@ $<

$(obj)%.o:	$(obj)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
