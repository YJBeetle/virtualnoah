#
# Makefile for the linux YAFFS2 filesystem routines.
#

obj-y := yaffs2.o	
obj-$(CONFIG_YAFFS_FS) := yaffs_mtdif.o yaffs_mtdif2.o
obj-$(CONFIG_YAFFS_FS) += yaffs_ecc.o yaffs_fs.o yaffs_guts.o
obj-$(CONFIG_YAFFS_FS) += yaffs_packedtags2.o yaffs_qsort.o
obj-$(CONFIG_YAFFS_FS) += yaffs_tagscompat.o yaffs_tagsvalidity.o
obj-$(CONFIG_YAFFS_FS) += yaffs_checkptrw.o yaffs_nand.o
