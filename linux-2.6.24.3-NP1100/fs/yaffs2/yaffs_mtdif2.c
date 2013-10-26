/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/* mtd interface for YAFFS2 */

const char *yaffs_mtdif2_c_version =
    "$Id: yaffs_mtdif2.c,v 1.1.1.1 2008-03-28 04:29:21 jlwei Exp $";

#include "yportenv.h"
#include "yaffs_mtdif2.h"
#include "linux/mtd/mtd.h"
#include "linux/types.h"
#include "linux/time.h"

#include "yaffs_packedtags2.h"

#define PT2_BYTES		25

void nandmtd2_pt2buf(yaffs_Device *dev, yaffs_PackedTags2 *pt, int is_raw)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->genericDevice);
	int	i, j = 0, k, n;
	__u8	pt2_byte_buf[PT2_BYTES];
	
	/* Pack buffer with 0xff */
	for (i = 0; i < mtd->oobsize; i++)
		dev->spareBuffer[i] = 0xff;
		
	if (!is_raw) {
		*((unsigned int *) &dev->spareBuffer[0]) = pt->t.sequenceNumber;
		*((unsigned int *) &dev->spareBuffer[4]) = pt->t.objectId;
		*((unsigned int *) &dev->spareBuffer[8]) = pt->t.chunkId;
		*((unsigned int *) &dev->spareBuffer[12]) = pt->t.byteCount;
		dev->spareBuffer[16] = pt->ecc.colParity;
		dev->spareBuffer[17] = pt->ecc.lineParity & 0xff;
		dev->spareBuffer[18] = (pt->ecc.lineParity >> 8) & 0xff;
		dev->spareBuffer[19] = (pt->ecc.lineParity >> 16) & 0xff;
		dev->spareBuffer[20] = (pt->ecc.lineParity >> 24) & 0xff;
		dev->spareBuffer[21] = pt->ecc.lineParityPrime & 0xff;
		dev->spareBuffer[22] = (pt->ecc.lineParityPrime >> 8) & 0xff;
		dev->spareBuffer[23] = (pt->ecc.lineParityPrime >> 16) & 0xff;
		dev->spareBuffer[24] = (pt->ecc.lineParityPrime >> 24) & 0xff;
	} else {
		*((unsigned int *) &pt2_byte_buf[0]) = pt->t.sequenceNumber;
		*((unsigned int *) &pt2_byte_buf[4]) = pt->t.objectId;
		*((unsigned int *) &pt2_byte_buf[8]) = pt->t.chunkId;
		*((unsigned int *) &pt2_byte_buf[12]) = pt->t.byteCount;
		pt2_byte_buf[16] = pt->ecc.colParity;
		pt2_byte_buf[17] = pt->ecc.lineParity & 0xff;
		pt2_byte_buf[18] = (pt->ecc.lineParity >> 8) & 0xff;
		pt2_byte_buf[19] = (pt->ecc.lineParity >> 16) & 0xff;
		pt2_byte_buf[20] = (pt->ecc.lineParity >> 24) & 0xff;
		pt2_byte_buf[21] = pt->ecc.lineParityPrime & 0xff;
		pt2_byte_buf[22] = (pt->ecc.lineParityPrime >> 8) & 0xff;
		pt2_byte_buf[23] = (pt->ecc.lineParityPrime >> 16) & 0xff;
		pt2_byte_buf[24] = (pt->ecc.lineParityPrime >> 24) & 0xff;

//		k = mtd->oobinfo.oobfree[j][0];
//		n = mtd->oobinfo.oobfree[j][1];

		k = mtd->ecclayout->oobfree[j].offset;
		n = mtd->ecclayout->oobfree[j].length;
		if (n == 0) {
			T(YAFFS_TRACE_ERROR, (TSTR("No OOB space for tags" TENDSTR)));
			YBUG();
		}
                                
		for (i = 0; i < PT2_BYTES; i++) {
			if (n == 0) {
				j++;
//				k = mtd->oobinfo.oobfree[j][0];
//				n = mtd->oobinfo.oobfree[j][1];
				k = mtd->ecclayout->oobfree[j].offset;
				n = mtd->ecclayout->oobfree[j].length;

				if (n == 0) {
					T(YAFFS_TRACE_ERROR, (TSTR("No OOB space for tags" TENDSTR)));
					YBUG();
				}
			}
			dev->spareBuffer[k++] = pt2_byte_buf[i];
			n--;
		}
	}

}

void nandmtd2_buf2pt(yaffs_Device *dev, yaffs_PackedTags2 *pt, int is_raw)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->genericDevice);
	int	i, j = 0, k, n;
	__u8	pt2_byte_buf[PT2_BYTES];
	

	if (!is_raw) {
		pt->t.sequenceNumber = *((unsigned int *) &dev->spareBuffer[0]);
		pt->t.objectId = *((unsigned int *) &dev->spareBuffer[4]);
		pt->t.chunkId = *((unsigned int *) &dev->spareBuffer[8]);
		pt->t.byteCount = *((unsigned int *) &dev->spareBuffer[12]);
		pt->ecc.colParity = dev->spareBuffer[16];
		pt->ecc.lineParity = (dev->spareBuffer[17] & 0x000000ff) |
	 		((dev->spareBuffer[18] << 8) & 0x0000ff00) |
	 		((dev->spareBuffer[19] << 16) & 0x00ff0000) |
	 		((dev->spareBuffer[20] << 24) & 0xff000000);
		pt->ecc.lineParityPrime = (dev->spareBuffer[21] & 0x000000ff) |
	 		((dev->spareBuffer[22] << 8) & 0x0000ff00) |
	 		((dev->spareBuffer[23] << 16) & 0x00ff0000) |
	 		((dev->spareBuffer[24] << 24) & 0xff000000);
	} else {
//		k = mtd->oobinfo.oobfree[j][0];
//		n = mtd->oobinfo.oobfree[j][1];
	
		k = mtd->ecclayout->oobfree[j].offset;
		n = mtd->ecclayout->oobfree[j].length;

		if (n == 0) {
			T(YAFFS_TRACE_ERROR, (TSTR("No space in OOB for tags" TENDSTR)));
			YBUG();
		}
                                
		for (i = 0; i < PT2_BYTES; i++) {
			if (n == 0) {
				j++;
//				k = mtd->oobinfo.oobfree[j][0];
//				n = mtd->oobinfo.oobfree[j][1];
				k = mtd->ecclayout->oobfree[j].offset;
				n = mtd->ecclayout->oobfree[j].length;

				if (n == 0) {
					T(YAFFS_TRACE_ERROR, (TSTR("No space in OOB for tags" TENDSTR)));
					YBUG();
				}
			}
			pt2_byte_buf[i] = dev->spareBuffer[k++];
			n--;
		}
		pt->t.sequenceNumber = *((unsigned int *) &pt2_byte_buf[0]);
		pt->t.objectId = *((unsigned int *) &pt2_byte_buf[4]);
		pt->t.chunkId = *((unsigned int *) &pt2_byte_buf[8]);
		pt->t.byteCount = *((unsigned int *) &pt2_byte_buf[12]);
		pt->ecc.colParity = pt2_byte_buf[16];
		pt->ecc.lineParity = (pt2_byte_buf[17] & 0x000000ff) |
	 		((pt2_byte_buf[18] << 8) & 0x0000ff00) |
	 		((pt2_byte_buf[19] << 16) & 0x00ff0000) |
	 		((pt2_byte_buf[20] << 24) & 0xff000000);
		pt->ecc.lineParityPrime = (pt2_byte_buf[21] & 0x000000ff) |
	 		((pt2_byte_buf[22] << 8) & 0x0000ff00) |
	 		((pt2_byte_buf[23] << 16) & 0x00ff0000) |
	 		((pt2_byte_buf[24] << 24) & 0xff000000);
	}
}

int nandmtd2_WriteChunkWithTagsToNAND(yaffs_Device * dev, int chunkInNAND,
				      const __u8 * data,
				      const yaffs_ExtendedTags * tags)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->genericDevice);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,17))
	struct mtd_oob_ops ops;
#else
	size_t dummy;
#endif
	int retval = 0;
	loff_mtd_t addr = ((loff_mtd_t) chunkInNAND) * dev->nDataBytesPerChunk;
	yaffs_PackedTags2 pt;

	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("nandmtd2_WriteChunkWithTagsToNAND chunk %d data %p tags %p"
	    TENDSTR), chunkInNAND, data, tags));

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,17))
	if (tags)
		yaffs_PackTags2(&pt, tags);
	else
		BUG(); /* both tags and data should always be present */

	if (data) {
     		nandmtd2_pt2buf(dev, &pt, 0);         //modify
		ops.mode = MTD_OOB_AUTO;
		ops.ooblen = sizeof(pt);
		ops.len = dev->nDataBytesPerChunk;
		ops.ooboffs = 0;
		ops.datbuf = (__u8 *)data;
//		ops.oobbuf = (void *)&pt;              //modify
		ops.oobbuf = (void *)dev->spareBuffer; //modify
		retval = mtd->write_oob(mtd, addr, &ops);
	} else
		BUG(); /* both tags and data should always be present */
#else
	if (tags) {
		yaffs_PackTags2(&pt, tags);
	}

	if (data && tags) {
		nandmtd2_pt2buf(dev, &pt, 0);
		if (dev->useNANDECC)
			retval =
			    mtd->write_ecc(mtd, addr, dev->nDataBytesPerChunk,
					   &dummy, data, dev->spareBuffer, NULL);
		else
			retval =
			    mtd->write_ecc(mtd, addr, dev->nDataBytesPerChunk,
					   &dummy, data, dev->spareBuffer, NULL);
	} else {
		if (data) {
			retval =
			    mtd->write(mtd, addr, dev->nDataBytesPerChunk, &dummy,
				       data);
		}
		if (tags) {
			nandmtd2_pt2buf(dev, &pt, 1);
			retval =
			    mtd->write_oob(mtd, addr, mtd->oobsize, &dummy,
					   dev->spareBuffer);
		}
	}
#endif

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

int nandmtd2_ReadChunkWithTagsFromNAND(yaffs_Device * dev, int chunkInNAND,
				       __u8 * data, yaffs_ExtendedTags * tags)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->genericDevice);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,17))
	struct mtd_oob_ops ops;
#endif
	size_mtd_t dummy;
	int retval = 0;
	loff_mtd_t addr = ((loff_mtd_t) chunkInNAND) * dev->nDataBytesPerChunk;
	yaffs_PackedTags2 pt;

	T(YAFFS_TRACE_MTD,
	  (TSTR
	   ("nandmtd2_ReadChunkWithTagsFromNAND chunk %d data %p tags %p"
	    TENDSTR), chunkInNAND, data, tags));

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,17))
	if (data && !tags)
		retval = mtd->read(mtd, addr, dev->nDataBytesPerChunk,
				&dummy, data);
	else if (tags) {
		ops.mode = MTD_OOB_AUTO;
		ops.ooblen = sizeof(pt);
		ops.len = data ? dev->nDataBytesPerChunk : sizeof(pt);
		ops.ooboffs = 0;
		ops.datbuf = data;
		ops.oobbuf = dev->spareBuffer;
		retval = mtd->read_oob(mtd, addr, &ops);
		nandmtd2_buf2pt(dev, &pt, 0);      //modify by yliu
	}
#else
	if (data && tags) {
		if (dev->useNANDECC) {
			retval =
			    mtd->read_ecc(mtd, addr, dev->nDataBytesPerChunk,
					  &dummy, data, dev->spareBuffer,
					  NULL);
		} else {
			retval =
			    mtd->read_ecc(mtd, addr, dev->nDataBytesPerChunk,
					  &dummy, data, dev->spareBuffer,
					  NULL);
		}
		nandmtd2_buf2pt(dev, &pt, 0);
	} else {
		if (data) {
			retval =
			    mtd->read(mtd, addr, dev->nDataBytesPerChunk, &dummy,
				      data);
		}
		if (tags) {
			retval =
			    mtd->read_oob(mtd, addr, mtd->oobsize, &dummy,
					  dev->spareBuffer);
			nandmtd2_buf2pt(dev, &pt, 1);
		}
	}
#endif

	if (tags)
		yaffs_UnpackTags2(tags, &pt);
	
	if(tags && retval == -EBADMSG && tags->eccResult == YAFFS_ECC_RESULT_NO_ERROR)
		tags->eccResult = YAFFS_ECC_RESULT_UNFIXED;

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

int nandmtd2_MarkNANDBlockBad(struct yaffs_DeviceStruct *dev, int blockNo)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->genericDevice);
	int retval;
	T(YAFFS_TRACE_MTD,
	  (TSTR("nandmtd2_MarkNANDBlockBad %d" TENDSTR), blockNo));

	retval =
	    mtd->block_markbad(mtd,
			       (u64)blockNo * dev->nChunksPerBlock *
			       dev->nDataBytesPerChunk);

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;

}

int nandmtd2_QueryNANDBlock(struct yaffs_DeviceStruct *dev, int blockNo,
			    yaffs_BlockState * state, int *sequenceNumber)
{
	struct mtd_info *mtd = (struct mtd_info *)(dev->genericDevice);
	int retval;

	T(YAFFS_TRACE_MTD,
	  (TSTR("nandmtd2_QueryNANDBlock %d" TENDSTR), blockNo));

	retval =
	    mtd->block_isbad(mtd,
			     (u64)blockNo * dev->nChunksPerBlock *
			     dev->nDataBytesPerChunk);
	if (retval) {
		T(YAFFS_TRACE_MTD, (TSTR("block is bad" TENDSTR)));

		*state = YAFFS_BLOCK_STATE_DEAD;
		*sequenceNumber = 0;
	} else {
		yaffs_ExtendedTags t;
		nandmtd2_ReadChunkWithTagsFromNAND(dev,
						   blockNo *
						   dev->nChunksPerBlock, NULL,
						   &t);

		if (t.chunkUsed) {
			*sequenceNumber = t.sequenceNumber;
			*state = YAFFS_BLOCK_STATE_NEEDS_SCANNING;
		} else {
			*sequenceNumber = 0;
			*state = YAFFS_BLOCK_STATE_EMPTY;
		}
	}
	T(YAFFS_TRACE_MTD,
	  (TSTR("block is bad seq %d state %d" TENDSTR), *sequenceNumber,
	   *state));

	if (retval == 0)
		return YAFFS_OK;
	else
		return YAFFS_FAIL;
}

