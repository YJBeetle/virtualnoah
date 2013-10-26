#
# (C) Copyright 2006
# Stefan Roese, DENX Software Engineering, sr@denx.de.
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
include $(TOPDIR)/msc_spl/board/$(BOARDDIR)/config.mk

LDSCRIPT= $(TOPDIR)/msc_spl/board/$(BOARDDIR)/u-boot.lds
LDFLAGS	= -Bstatic -T $(LDSCRIPT) -Ttext $(TEXT_BASE)
AFLAGS	+= -DCONFIG_NAND_SPL -DCONFIG_MSC_SPL
CFLAGS	+= -DCONFIG_NAND_SPL -DCONFIG_MSC_SPL

SOBJS	= start.o
COBJS	= msc_boot_jz4750.o cpu.o jz4750.o jz_serial.o

SRCS	:= $(addprefix $(obj),$(SOBJS:.o=.S) $(COBJS:.o=.c))
OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))
__OBJS	:= $(SOBJS) $(COBJS) 
LNDIR	:= $(OBJTREE)/msc_spl/board/$(BOARDDIR)

mscobj	:= $(OBJTREE)/msc_spl/

ALL	= $(mscobj)u-boot-spl $(mscobj)u-boot-spl.bin $(mscobj)u-boot-spl-pad.bin
all:	$(obj).depend $(ALL)

$(mscobj)u-boot-spl-pad.bin: $(mscobj)u-boot-spl.bin
	dd bs=1024 count=8 if=/dev/zero of=$(mscobj)junk1
	cat $< $(mscobj)junk1 > $(mscobj)junk2
	dd bs=1024 count=8 if=$(mscobj)junk2 of=$(mscobj)junk3
	cat $(mscobj)junk3 $(mscobj)junk3 > $@
	rm -f $(mscobj)junk*

$(mscobj)u-boot-spl.bin:	$(mscobj)u-boot-spl
	$(OBJCOPY) ${OBJCFLAGS} -O binary $< $@

$(mscobj)u-boot-spl:	$(OBJS)
	cd $(LNDIR) && $(LD) $(LDFLAGS) $$UNDEF_SYM $(__OBJS) \
		-Map $(mscobj)u-boot-spl.map \
		-o $(mscobj)u-boot-spl

# create symbolic links for common files

# from cpu directory
$(obj)start.S:
	@rm -f $(obj)start.S
	ln -s $(SRCTREE)/cpu/mips/start.S $(obj)start.S

$(obj)cpu.c:
	@rm -f $(obj)cpu.c
	ln -s $(SRCTREE)/cpu/mips/cpu.c $(obj)cpu.c

$(obj)jz4750.c:
	@rm -f $(obj)jz4750.c
	ln -s $(SRCTREE)/cpu/mips/jz4750.c $(obj)jz4750.c

$(obj)jz_serial.c:
	@rm -f $(obj)jz_serial.c
	ln -s $(SRCTREE)/cpu/mips/jz_serial.c $(obj)jz_serial.c

# from nand_spl directory
$(obj)msc_boot_jz4750.c:
	@rm -f $(obj)msc_boot_jz4750.c
	ln -s $(SRCTREE)/msc_spl/msc_boot_jz4750.c $(obj)msc_boot_jz4750.c

#########################################################################

$(obj)%.o:	$(obj)%.S
	$(CC) $(AFLAGS) -c -o $@ $<

$(obj)%.o:	$(obj)%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
