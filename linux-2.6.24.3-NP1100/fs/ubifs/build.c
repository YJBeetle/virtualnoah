/*
 * This file is part of UBIFS.
 *
 * Copyright (C) 2006-2008 Nokia Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Authors: Artem Bityutskiy (Битюцкий Артём)
 *          Adrian Hunter
 */

/*
 * This file implements UBIFS initialization, mount and un-mount. Some
 * initialization stuff which is rather large and complex is placed at
 * corresponding subsystems, but most of it is here.
 */

#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/ctype.h>
#include <linux/random.h>
#include <linux/kthread.h>
#include <linux/parser.h>
#include "ubifs.h"

/* Slab cache for UBIFS inodes */
struct kmem_cache *ubifs_inode_slab;

/* TODO: remove compatibility stuff as late as possible */
#ifndef UBIFS_COMPAT_NO_SHRINKER
/* UBIFS TNC shrinker description */
static struct shrinker ubifs_shrinker_info = {
	.shrink = ubifs_shrinker,
	.seeks = DEFAULT_SEEKS,
};
#endif

/**
 * init_constants_early - initialize UBIFS constants.
 * @c: UBIFS file-system description object
 *
 * This function initialize UBIFS constants which do not need the superblock to
 * be read. It also checks that the UBI volume satisfies basic UBIFS
 * requirements. Returns zero in case of success and a negative error code in
 * case of failure.
 */
static int init_constants_early(struct ubifs_info *c)
{
	if (c->vi.corrupted) {
		ubifs_warn("UBI volume is corrupted - read-only mode");
		c->ro_media = 1;
	}

	if (c->di.ro_mode) {
		ubifs_msg("read-only UBI device");
		c->ro_media = 1;
	}

	if (c->vi.vol_type == UBI_STATIC_VOLUME) {
		ubifs_msg("static UBI volume - read-only mode");
		c->ro_media = 1;
	}

	c->leb_cnt = c->vi.size;
	c->leb_size = c->vi.usable_leb_size;
	c->half_leb_size = c->leb_size / 2;
	c->min_io_size = c->di.min_io_size;
	c->min_io_shift = fls(c->min_io_size) - 1;

	if (c->leb_size < UBIFS_MIN_LEB_SZ) {
		ubifs_err("too small LEBs (%d bytes), min. is %d bytes",
			  c->leb_size, UBIFS_MIN_LEB_SZ);
		return -EINVAL;
	}

	if (c->leb_cnt < UBIFS_MIN_LEB_CNT) {
		ubifs_err("too few LEBs (%d), min. is %d",
			  c->leb_cnt, UBIFS_MIN_LEB_CNT);
		return -EINVAL;
	}

	if (!is_power_of_2(c->min_io_size)) {
		ubifs_err("bad min. I/O size %d", c->min_io_size);
		return -EINVAL;
	}

	/*
	 * UBIFS aligns all node to 8-byte boundary, so to make function in
	 * io.c simpler, assume minimum I/O unit size to be 8 bytes if it is
	 * less than 8.
	 */
	if (c->min_io_size < 8) {
		c->min_io_size = 8;
		c->min_io_shift = 3;
	}

	c->ref_node_alsz = ALIGN(UBIFS_REF_NODE_SZ, c->min_io_size);
	c->mst_node_alsz = ALIGN(UBIFS_MST_NODE_SZ, c->min_io_size);

	/*
	 * Initialize node length ranges which are mostly needed for node
	 * length validation.
	 */
	c->ranges[UBIFS_PAD_NODE].len  = UBIFS_PAD_NODE_SZ;
	c->ranges[UBIFS_SB_NODE].len   = UBIFS_SB_NODE_SZ;
	c->ranges[UBIFS_MST_NODE].len  = UBIFS_MST_NODE_SZ;
	c->ranges[UBIFS_REF_NODE].len  = UBIFS_REF_NODE_SZ;
	c->ranges[UBIFS_TRUN_NODE].len = UBIFS_TRUN_NODE_SZ;
	c->ranges[UBIFS_CS_NODE].len   = UBIFS_CS_NODE_SZ;

	c->ranges[UBIFS_INO_NODE].min_len  = UBIFS_INO_NODE_SZ;
	c->ranges[UBIFS_INO_NODE].max_len  = UBIFS_MAX_INO_NODE_SZ;
	c->ranges[UBIFS_ORPH_NODE].min_len =
				UBIFS_ORPH_NODE_SZ + sizeof(__le64);
	c->ranges[UBIFS_ORPH_NODE].max_len = c->leb_size;
	c->ranges[UBIFS_DENT_NODE].min_len = UBIFS_DENT_NODE_SZ;
	c->ranges[UBIFS_DENT_NODE].max_len = UBIFS_MAX_DENT_NODE_SZ;
	c->ranges[UBIFS_XENT_NODE].min_len = UBIFS_XENT_NODE_SZ;
	c->ranges[UBIFS_XENT_NODE].max_len = UBIFS_MAX_XENT_NODE_SZ;
	c->ranges[UBIFS_DATA_NODE].min_len = UBIFS_DATA_NODE_SZ;
	c->ranges[UBIFS_DATA_NODE].max_len = UBIFS_MAX_DATA_NODE_SZ;
	/*
	 * Minimum indexing node size is amended later when superblock is
	 * read and the key length is known.
	 */
	c->ranges[UBIFS_IDX_NODE].min_len = UBIFS_IDX_NODE_SZ + UBIFS_BRANCH_SZ;
	/*
	 * Maximum indexing node size is amended later when superblock is
	 * read and the fanout is known.
	 */
	c->ranges[UBIFS_IDX_NODE].max_len = INT_MAX;

	/*
	 * Initialize dead and dark LEB space watermarks.
	 *
	 * Dead space is the space which cannot be used. Its watermark is
	 * equivalent to min. I/O unit or minimum node size if it is greater
	 * then min. I/O unit.
	 *
	 * Dark space is the space which might be used, or might not, depending
	 * on which node should be written to the LEB. Its watermark is
	 * equivalent to maximum UBIFS node size.
	 */
	c->dead_wm = ALIGN(MIN_WRITE_SZ, c->min_io_size);
	c->dark_wm = ALIGN(UBIFS_MAX_NODE_SZ, c->min_io_size);

	return 0;
}

/**
 * bud_wbuf_callback - bud LEB write-buffer synchronization call-back.
 * @c: UBIFS file-system description object
 * @lnum: LEB the write-buffer was synchronized to
 * @free: how many free bytes left in this LEB
 * @pad: how many bytes were padded
 *
 * This is a callback function which is called by the I/O unit when the
 * write-buffer is synchronized. We need this to correctly maintain space
 * accounting in bud logical eraseblocks. This function returns zero in case of
 * success and a negative error code in case of failure.
 *
 * This function actually belongs to the journal, but we keep it here because
 * we want to keep it static.
 */
static int bud_wbuf_callback(struct ubifs_info *c, int lnum, int free, int pad)
{
	return ubifs_update_one_lp(c, lnum, free, pad, 0, 0);
}

/*
 * init_constants_late - initialize UBIFS constants.
 * @c: UBIFS file-system description object
 *
 * This is a helper function which initializes various UBIFS constants after
 * the superblock has been read. It also checks various UBIFS parameters and
 * makes sure they are all right. Returns zero in case of success and a
 * negative error code in case of failure.
 */
static int init_constants_late(struct ubifs_info *c)
{
	int tmp, err;
	uint64_t tmp64;

	c->main_bytes = c->main_lebs * c->leb_size;

	c->max_znode_sz = sizeof(struct ubifs_znode) +
				c->fanout * sizeof(struct ubifs_zbranch);

	tmp = ubifs_idx_node_sz(c, 1);
	c->ranges[UBIFS_IDX_NODE].min_len = tmp;
	c->min_idx_node_sz = ALIGN(tmp, 8);

	tmp = ubifs_idx_node_sz(c, c->fanout);
	c->ranges[UBIFS_IDX_NODE].max_len = tmp;
	c->max_idx_node_sz = ALIGN(tmp, 8);

	/* Make sure LEB size is large enough to fit full commit */
	tmp = UBIFS_CS_NODE_SZ + UBIFS_REF_NODE_SZ * c->jhead_cnt;
	tmp = ALIGN(tmp, c->min_io_size);
	if (tmp > c->leb_size) {
		dbg_err("too small LEB size %d, at least %d needed",
			c->leb_size, tmp);
		return -EINVAL;
	}

	/*
	 * Make sure that the log is large enough to fit reference nodes for
	 * all buds plus one reserved LEB.
	 */
	tmp64 = c->max_bud_bytes;
	tmp = do_div(tmp64, c->leb_size);
	c->max_bud_cnt = tmp64 + !!tmp;
	tmp = (c->ref_node_alsz * c->max_bud_cnt + c->leb_size - 1);
	tmp /= c->leb_size;
	tmp += 1;
	if (c->log_lebs < tmp) {
		dbg_err("too small log %d LEBs, required min. %d LEBs",
			c->log_lebs, tmp);
		return -EINVAL;
	}

	/*
	 * When budgeting we assume worst-case scenarios when the pages are not
	 * be compressed and direntries are of the maximum size.
	 *
	 * Note, data, which may be stored in inodes is budgeted separately, so
	 * it is not included into 'c->inode_budget'.
	 *
	 * c->page_budget is PAGE_CACHE_SIZE + UBIFS_CH_SZ * blocks_per_page
	 */
	c->page_budget = PAGE_CACHE_SIZE + UBIFS_CH_SZ;
	c->inode_budget = UBIFS_INO_NODE_SZ;
	c->dent_budget = UBIFS_MAX_DENT_NODE_SZ;

	/*
	 * When the amount of flash space used by buds becomes
	 * 'c->max_bud_bytes', UBIFS just blocks all writers and starts commit.
	 * The writers are unblocked when the commit is finished. To avoid
	 * writers to be blocked UBIFS initiates background commit in advance,
	 * when number of bud bytes becomes above the limit defined below.
	 */
	c->bg_bud_bytes = (c->max_bud_bytes * 13) >> 4;

	/*
	 * Ensure minimum journal size. All the bytes in the journal heads are
	 * considered to be used, when calculating the current journal usage.
	 * Consequently, if the journal is too small, UBIFS will treat it as
	 * always full.
	 */
	tmp64 = (uint64_t)(c->jhead_cnt + 1) * c->leb_size + 1;
	if (c->bg_bud_bytes < tmp64)
		c->bg_bud_bytes = tmp64;
	if (c->max_bud_bytes < tmp64 + c->leb_size)
		c->max_bud_bytes = tmp64 + c->leb_size;

	err = ubifs_calc_lpt_geom(c);
	if (err)
		return err;

	c->min_idx_lebs = ubifs_calc_min_idx_lebs(c);

	/*
	 * Calculate total amount of FS blocks. This number is not used
	 * internally because it does not make much sense for UBIFS, but it is
	 * necessary to report something for the 'statfs()' call.
	 *
	 * Subtract the LEB reserved for GC and the LEB which is reserved for
	 * deletions.
	 *
	 * Review 'ubifs_calc_available()' if changing this calculation.
	 */
	tmp64 = c->main_lebs - 2;
	tmp64 *= c->leb_size - c->dark_wm;
	tmp64 = ubifs_reported_space(c, tmp64);
	c->block_cnt = tmp64 >> UBIFS_BLOCK_SHIFT;

	return 0;
}

/**
 * care_about_gc_lnum - take care about reserved GC LEB.
 * @c: UBIFS file-system description object
 *
 * This function ensures that the LEB reserved for garbage collection is
 * unmapped and is marked as "taken" in lprops. We also have to set free space
 * to LEB size and dirty space to zero, because lprops may contain out-of-date
 * information if the file-system was un-mounted before it has been committed.
 * This function returns zero in case of success and a negative error code in
 * case of failure.
 */
static int care_about_gc_lnum(struct ubifs_info *c)
{
	int err;

	if (c->gc_lnum == -1) {
		ubifs_err("no LEB for GC");
		return -EINVAL;
	}

	err = ubifs_leb_unmap(c, c->gc_lnum);
	if (err)
		return err;

	/* And we have to tell lprops that this LEB is taken */
	err = ubifs_change_one_lp(c, c->gc_lnum, c->leb_size, 0,
				  LPROPS_TAKEN, 0, 0);
	return err;
}

/**
 * alloc_wbufs - allocate write-buffers.
 * @c: UBIFS file-system description object
 *
 * This helper function allocates and initializes UBIFS write-buffers. Returns
 * zero in case of success and %-ENOMEM in case of failure.
 */
static int alloc_wbufs(struct ubifs_info *c)
{
	int i, err;

	c->jheads = kzalloc(c->jhead_cnt * sizeof(struct ubifs_jhead),
			   GFP_KERNEL);
	if (!c->jheads)
		return -ENOMEM;

	/* Initialize journal heads */
	for (i = 0; i < c->jhead_cnt; i++) {
		INIT_LIST_HEAD(&c->jheads[i].buds_list);
		err = ubifs_wbuf_init(c, &c->jheads[i].wbuf);
		if (err)
			return err;

		c->jheads[i].wbuf.sync_callback = &bud_wbuf_callback;
		c->jheads[i].wbuf.jhead = i;
	}

	c->jheads[BASEHD].wbuf.dtype = UBI_SHORTTERM;
	/*
	 * Garbage Collector head likely contains long-term data and
	 * does not need to be synchronized by timer.
	 */
	c->jheads[GCHD].wbuf.dtype = UBI_LONGTERM;
	c->jheads[GCHD].wbuf.timeout = 0;

	sprintf(c->bgt_name, "%s%d_%d", SYNCER_BG_NAME,
		c->vi.ubi_num, c->vi.vol_id);

	return 0;
}

/**
 * free_wbufs - free write-buffers.
 * @c: UBIFS file-system description object
 */
static void free_wbufs(struct ubifs_info *c)
{
	int i;

	if (c->jheads) {
		for (i = 0; i < c->jhead_cnt; i++) {
			kfree(c->jheads[i].wbuf.buf);
			kfree(c->jheads[i].wbuf.inodes);
		}
		kfree(c->jheads);
		c->jheads = NULL;
	}
}

/**
 * free_orphans - free orphans.
 * @c: UBIFS file-system description object
 */
static void free_orphans(struct ubifs_info *c)
{
	struct ubifs_orphan *orph;

	while (c->orph_dnext) {
		orph = c->orph_dnext;
		c->orph_dnext = orph->dnext;
		list_del(&orph->list);
		kfree(orph);
	}

	while (!list_empty(&c->orph_list)) {
		orph = list_entry(c->orph_list.next, struct ubifs_orphan, list);
		list_del(&orph->list);
		kfree(orph);
		dbg_err("orphan list not empty at unmount");
	}

#if defined(CONFIG_MTD_NAND_DMA) && !defined(CONFIG_MTD_NAND_DMABUF)
	kfree(c->orph_buf);
#else
	vfree(c->orph_buf);
#endif
	c->orph_buf = NULL;
}

/**
 * free_buds - free per-bud objects.
 * @c: UBIFS file-system description object
 */
static void free_buds(struct ubifs_info *c)
{
	struct rb_node *this = c->buds.rb_node;
	struct ubifs_bud *bud;

	while (this) {
		if (this->rb_left)
			this = this->rb_left;
		else if (this->rb_right)
			this = this->rb_right;
		else {
			bud = rb_entry(this, struct ubifs_bud, rb);
			this = rb_parent(this);
			if (this) {
				if (this->rb_left == &bud->rb)
					this->rb_left = NULL;
				else
					this->rb_right = NULL;
			}
			kfree(bud);
		}
	}
}

/**
 * check_volume_empty - check if the UBI volume is empty.
 * @c: UBIFS file-system description object
 *
 * This function checks if the UBIFS volume is empty by looking if its LEBs are
 * mapped or not. The result of checking is stored in the @c->empty variable.
 * Returns zero in case of success and a negative error code in case of
 * failure.
 */
static int check_volume_empty(struct ubifs_info *c)
{
	int lnum, err;

	c->empty = 1;
	for (lnum = 0; lnum < c->leb_cnt; lnum++) {
		err = ubi_is_mapped(c->ubi, lnum);
		if (unlikely(err < 0))
			return err;
		if (err == 1) {
			c->empty = 0;
			break;
		}

		cond_resched();
	}

	return 0;
}

/**
 * mount_ubifs - mount UBIFS file-system.
 * @c: UBIFS file-system description object
 *
 * This function mounts UBIFS file system. Returns zero in case of success and
 * a negative error code in case of failure.
 *
 * Note, the function does not de-allocate resources it it fails half way
 * through, and the caller has to do this instead.
 */
static int mount_ubifs(struct ubifs_info *c)
{
	struct super_block *sb = c->vfs_sb;
	int err, mounted_read_only = (sb->s_flags & MS_RDONLY);
	unsigned long long x;
	size_t sz;

	err = init_constants_early(c);
	if (err)
		return err;

#ifdef CONFIG_UBIFS_FS_DEBUG
#if defined(CONFIG_MTD_NAND_DMA) && !defined(CONFIG_MTD_NAND_DMABUF)
	c->dbg_buf = kmalloc(c->leb_size, GFP_KERNEL);
#else
	c->dbg_buf = vmalloc(c->leb_size);
#endif
	if (!c->dbg_buf)
		return -ENOMEM;
#endif

	err = check_volume_empty(c);
	if (err)
		return err;

	if (c->empty && (mounted_read_only || c->ro_media)) {
		/*
		 * This UBI volume is empty, and read-only, or the file system
		 * is mounted read-only - we cannot format it.
		 */
		ubifs_err("can't format empty UBI volume: read-only %s",
			  c->ro_media ? "UBI volume" : "mount");
		return -EROFS;
	}

	if (c->ro_media && !mounted_read_only) {
		ubifs_err("cannot mount read-write - read-only media");
		return -EROFS;
	}

	/*
	 * The requirement for the buffer is that it should fit indexing B-tree
	 * height amount of integers. We assume the height if the TNC tree will
	 * never exceed 64.
	 */
	c->bottom_up_buf = kmalloc(BOTTOM_UP_HEIGHT * sizeof(int), GFP_KERNEL);
	if (!c->bottom_up_buf)
		return -ENOMEM;

#if defined(CONFIG_MTD_NAND_DMA) && !defined(CONFIG_MTD_NAND_DMABUF)
	c->sbuf = kmalloc(c->leb_size, GFP_KERNEL);
#else
	c->sbuf = vmalloc(c->leb_size);
#endif
	if (!c->sbuf)
		return -ENOMEM;

	if (!mounted_read_only) {
#if defined(CONFIG_MTD_NAND_DMA) && !defined(CONFIG_MTD_NAND_DMABUF)
		c->ileb_buf = kmalloc(c->leb_size, GFP_KERNEL);
#else
		c->ileb_buf = vmalloc(c->leb_size);
#endif
		if (!c->ileb_buf)
			return -ENOMEM;
	}

	err = ubifs_read_superblock(c);
	if (err)
		return err;

	/*
	 * Make sure the compressor which is set as the default on in the
	 * superblock was actually compiled in.
	 */
	if (!ubifs_compr_present(c->default_compr)) {
		ubifs_warn("'%s' compressor is set by superblock, but not "
			   "compiled in", ubifs_compr_name(c->default_compr));
		c->default_compr = UBIFS_COMPR_NONE;
	}

	dbg_failure_mode_registration(c);

	err = init_constants_late(c);
	if (err)
		return err;

	sz = ALIGN(c->max_idx_node_sz, c->min_io_size);
	sz = ALIGN(sz + c->max_idx_node_sz, c->min_io_size);
	c->cbuf = kmalloc(sz, GFP_NOFS);
	if (!c->cbuf)
		return -ENOMEM;

	if (!mounted_read_only) {
		err = alloc_wbufs(c);
		if (err)
			return err;

		/* Create background thread */
		c->bgt = kthread_create(ubifs_bg_thread, c, c->bgt_name);
		if (!c->bgt)
			c->bgt = ERR_PTR(-EINVAL);
		if (IS_ERR(c->bgt)) {
			err = PTR_ERR(c->bgt);
			c->bgt = NULL;
			ubifs_err("cannot spawn \"%s\", error %d",
				  c->bgt_name, err);
			return err;
		}
	}

	err = ubifs_read_master(c);
	if (err)
		return err;

	if ((c->mst_node->flags & cpu_to_le32(UBIFS_MST_DIRTY)) != 0) {
		ubifs_msg("recovery needed");
		c->need_recovery = 1;
		if (!mounted_read_only) {
			err = ubifs_recover_inl_heads(c, c->sbuf);
			if (err)
				return err;
		}
	} else if (!mounted_read_only) {
		/*
		 * Set the "dirty" flag so that if we reboot uncleanly we
		 * will notice this immediately on the next mount.
		 */
		c->mst_node->flags |= cpu_to_le32(UBIFS_MST_DIRTY);
		err = ubifs_write_master(c);
		if (err)
			return err;
	}

	err = ubifs_lpt_init(c, 1, !mounted_read_only);
	if (err)
		return err;

	err = dbg_check_idx_size(c, c->old_idx_sz);
	if (err)
		return err;

	err = ubifs_replay_journal(c);
	if (err)
		return err;

	if (!mounted_read_only) {
		int lnum;

		if (c->need_recovery)
			err = ubifs_recover_gc_lnum(c);
		else
			err = care_about_gc_lnum(c);
		if (err)
			return err;
		err = ubifs_mount_orphans(c, c->need_recovery);
		if (err)
			return err;

		/* Check for enough log space */
		lnum = c->lhead_lnum + 1;
		if (lnum >= UBIFS_LOG_LNUM + c->log_lebs)
			lnum = UBIFS_LOG_LNUM;
		if (lnum == c->ltail_lnum) {
			err = ubifs_consolidate_log(c);
			if (err)
				return err;
		}

		/* Check for enough free space */
		if (ubifs_calc_available(c) <= 0) {
			ubifs_err("insufficient available space");
			return -EINVAL;
		}

		err = dbg_check_lprops(c);
		if (err)
			return err;
	}

	if (c->need_recovery) {
		err = ubifs_recover_size(c);
		if (err)
			return err;
	}

	spin_lock(&ubifs_infos_lock);
	list_add_tail(&c->infos_list, &ubifs_infos);
	spin_unlock(&ubifs_infos_lock);

	if (c->need_recovery) {
		if (mounted_read_only)
			ubifs_msg("recovery deferred");
		else {
			c->need_recovery = 0;
			ubifs_msg("recovery completed");
		}
	}

	ubifs_msg("mounted UBI device %d, volume %d", c->vi.ubi_num,
		  c->vi.vol_id);
	if (mounted_read_only)
		ubifs_msg("mounted read-only");
	ubifs_msg("minimal I/O unit size:   %d bytes", c->min_io_size);
	ubifs_msg("logical eraseblock size: %d bytes (%d KiB)",
		  c->leb_size, c->leb_size / 1024);
	x = (unsigned long long)c->main_lebs * c->leb_size;
	ubifs_msg("file system size:        %lld bytes (%lld KiB, %lld MiB, "
		  "%d LEBs)", x, x >> 10, x >> 20, c->main_lebs);
	x = (unsigned long long)c->log_lebs * c->leb_size + c->max_bud_bytes;
	ubifs_msg("journal size:            %lld bytes (%lld KiB, %lld MiB, "
		  "%d LEBs)", x, x >> 10, x >> 20,
		  c->log_lebs + c->max_bud_cnt);
	ubifs_msg("data journal heads:      %d",
		  c->jhead_cnt - NONDATA_JHEADS_CNT);
	ubifs_msg("default compressor:      %s",
		  ubifs_compr_name(c->default_compr));

	dbg_msg("compiled on:            " __DATE__ " at " __TIME__);
	dbg_msg("fast unmount:           %d", c->fast_unmount);
	dbg_msg("big_lpt                 %d", c->big_lpt);
	dbg_msg("log LEBs:               %d (%d - %d)",
		c->log_lebs, UBIFS_LOG_LNUM, c->log_last);
	dbg_msg("LPT area LEBs:          %d (%d - %d)",
		c->lpt_lebs, c->lpt_first, c->lpt_last);
	dbg_msg("orphan area LEBs:       %d (%d - %d)",
		c->orph_lebs, c->orph_first, c->orph_last);
	dbg_msg("main area LEBs:         %d (%d - %d)",
		c->main_lebs, c->main_first, c->leb_cnt - 1);
	dbg_msg("index LEBs:             %d", c->lst.idx_lebs);
	dbg_msg("total index bytes:      %lld (%lld KiB, %lld MiB)",
		c->old_idx_sz, c->old_idx_sz >> 10, c->old_idx_sz >> 20);
	dbg_msg("key hash type:          %d", c->key_hash_type);
	dbg_msg("tree fanout:            %d", c->fanout);
	dbg_msg("reserved GC LEB:        %d", c->gc_lnum);
	dbg_msg("first main LEB:         %d", c->main_first);
	dbg_msg("dead watermark:         %d", c->dead_wm);
	dbg_msg("dark watermark:         %d", c->dark_wm);
	x = c->main_lebs * c->dark_wm;
	dbg_msg("max. dark space:        %lld (%lld KiB, %lld MiB)",
		x, x >> 10, x >> 20);
	dbg_msg("maximum bud bytes:      %lld (%lld KiB, %lld MiB)",
		c->max_bud_bytes, c->max_bud_bytes >> 10,
		c->max_bud_bytes >> 20);
	dbg_msg("BG commit bud bytes:    %lld (%lld KiB, %lld MiB)",
		c->bg_bud_bytes, c->bg_bud_bytes >> 10,
		c->bg_bud_bytes >> 20);
	dbg_msg("current bud bytes       %lld (%lld KiB, %lld MiB)",
		c->bud_bytes, c->bud_bytes >> 10, c->bud_bytes >> 20);
	dbg_msg("max. seq. number:       %llu", c->max_sqnum);
	dbg_msg("commit number:          %llu", c->cmt_no);

	return 0;
}

/**
 * ubifs_umount - un-mount UBIFS file-system.
 * @c: UBIFS file-system description object
 *
 * Note, this function is called to free allocated resourced when un-mounting,
 * as well as free resources when an error occurred while we were half way
 * through mounting (error path cleanup function). So it has to make sure the
 * resource was actually allocated before freeing it.
 */
void ubifs_umount(struct ubifs_info *c)
{
	dbg_gen("un-mounting UBI device %d, volume %d", c->vi.ubi_num,
		c->vi.vol_id);

	ubifs_destroy_size_tree(c);

	if (c->bgt)
		kthread_stop(c->bgt);

	free_buds(c);
	ubifs_destroy_idx_gc(c);
	ubifs_tnc_close(c);

	free_wbufs(c);
	free_orphans(c);
	ubifs_lpt_free(c, 0);

	while (!list_empty(&c->unclean_leb_list)) {
		struct ubifs_unclean_leb *ucleb;

		ucleb = list_entry(c->unclean_leb_list.next,
				   struct ubifs_unclean_leb, list);
		list_del(&ucleb->list);
		kfree(ucleb);
	}

	while (!list_empty(&c->old_buds)) {
		struct ubifs_bud *bud;

		bud = list_entry(c->old_buds.next, struct ubifs_bud, list);
		list_del(&bud->list);
		kfree(bud);
	}

	kfree(c->rcvrd_mst_node);
	kfree(c->mst_node);
	kfree(c->bottom_up_buf);
#if defined(CONFIG_MTD_NAND_DMA) && !defined(CONFIG_MTD_NAND_DMABUF)
	kfree(c->sbuf);
	UBIFS_DBG(kfree(c->dbg_buf));
	kfree(c->ileb_buf);
#else
	vfree(c->sbuf);
	UBIFS_DBG(vfree(c->dbg_buf));
	vfree(c->ileb_buf);
#endif
	dbg_failure_mode_deregistration(c);
}

/**
 * ubifs_remount_rw - re-mount in read-write mode.
 * @c: UBIFS file-system description object
 *
 * UBIFS avoids allocating many unnecessary resources when mounted in read-only
 * mode. This function allocates the needed resources and re-mounts UBIFS in
 * read-write mode.
 */
int ubifs_remount_rw(struct ubifs_info *c)
{
	int err, lnum;

	if (c->ro_media)
		return -EINVAL;

	mutex_lock(&c->umount_mutex);
	c->remounting_rw = 1;

	/* Check for enough free space */
	if (ubifs_calc_available(c) <= 0) {
		ubifs_err("insufficient available space");
		err = -EINVAL;
		goto out;
	}

	if (c->old_leb_cnt != c->leb_cnt) {
		struct ubifs_sb_node *sup;

		sup = ubifs_read_sb_node(c);
		if (IS_ERR(sup)) {
			err = PTR_ERR(sup);
			goto out;
		}
		sup->leb_cnt = cpu_to_le32(c->leb_cnt);
		err = ubifs_write_sb_node(c, sup);
		if (err)
			goto out;
	}

	if (c->need_recovery) {
		ubifs_msg("completing deferred recovery");
		err = ubifs_write_rcvrd_mst_node(c);
		if (err)
			goto out;
		err = ubifs_recover_size(c);
		if (err)
			goto out;
		err = ubifs_clean_lebs(c, c->sbuf);
		if (err)
			goto out;
		err = ubifs_recover_inl_heads(c, c->sbuf);
		if (err)
			goto out;
	}

	if (!(c->mst_node->flags & cpu_to_le32(UBIFS_MST_DIRTY))) {
		c->mst_node->flags |= cpu_to_le32(UBIFS_MST_DIRTY);
		err = ubifs_write_master(c);
		if (err)
			goto out;
	}

#if defined(CONFIG_MTD_NAND_DMA) && !defined(CONFIG_MTD_NAND_DMABUF)
	c->ileb_buf = kmalloc(c->leb_size, GFP_KERNEL);
#else
	c->ileb_buf = vmalloc(c->leb_size);
#endif
	if (!c->ileb_buf) {
		err = -ENOMEM;
		goto out;
	}

	err = ubifs_lpt_init(c, 0, 1);
	if (err)
		goto out;

	err = alloc_wbufs(c);
	if (err)
		goto out;

	ubifs_create_buds_lists(c);

	/* Create background thread */
	c->bgt = kthread_create(ubifs_bg_thread, c, c->bgt_name);
	if (!c->bgt)
		c->bgt = ERR_PTR(-EINVAL);
	if (IS_ERR(c->bgt)) {
		err = PTR_ERR(c->bgt);
		c->bgt = NULL;
		ubifs_err("cannot spawn \"%s\", error %d",
			  c->bgt_name, err);
		return err;
	}

	if (c->need_recovery)
		err = ubifs_recover_gc_lnum(c);
	else
		err = care_about_gc_lnum(c);
	if (err)
		goto out;

	err = ubifs_mount_orphans(c, c->need_recovery);
	if (err)
		goto out;
	/* Check for enough log space */
	lnum = c->lhead_lnum + 1;
	if (lnum >= UBIFS_LOG_LNUM + c->log_lebs)
		lnum = UBIFS_LOG_LNUM;
	if (lnum == c->ltail_lnum) {
		err = ubifs_consolidate_log(c);
		if (err)
			goto out;
	}

	if (c->need_recovery) {
		c->need_recovery = 0;
		ubifs_msg("deferred recovery completed");
	}

	dbg_gen("re-mounted read-write");
	c->vfs_sb->s_flags &= ~MS_RDONLY;
	c->remounting_rw = 0;
	mutex_unlock(&c->umount_mutex);
	return 0;

out:
	free_orphans(c);
	if (c->bgt) {
		kthread_stop(c->bgt);
		c->bgt = NULL;
	}
	free_wbufs(c);
#if defined(CONFIG_MTD_NAND_DMA) && !defined(CONFIG_MTD_NAND_DMABUF)
	kfree(c->ileb_buf);
#else
	vfree(c->ileb_buf);
#endif
	c->ileb_buf = NULL;
	ubifs_lpt_free(c, 1);
	c->remounting_rw = 0;
	mutex_unlock(&c->umount_mutex);
	return err;
}

/**
 * commit_on_unmount - commit the journal when un-mounting.
 * @c: UBIFS file-system description object
 *
 * This function is called during un-mounting and it commits the journal unless
 * the "fast unmount" mode is enabled. It also avoids committing the journal if
 * it contains too few data.
 *
 * Sometimes recovery requires the journal to be committed at least once, and
 * this function takes care about this.
 */
static void commit_on_unmount(struct ubifs_info *c)
{
	if (!c->fast_unmount) {
		long long bud_bytes;

		spin_lock(&c->buds_lock);
		bud_bytes = c->bud_bytes;
		spin_unlock(&c->buds_lock);
		if (bud_bytes > c->leb_size)
			ubifs_run_commit(c);
	}

	if (c->recovery_needs_commit)
		ubifs_recovery_commit(c);
}

/**
 * ubifs_remount_ro - re-mount in read-only mode.
 * @c: UBIFS file-system description object
 *
 * We rely on VFS to have stopped writing. Possibly the background thread could
 * be running a commit, however kthread_stop will wait in that case.
 */
void ubifs_remount_ro(struct ubifs_info *c)
{
	int i, err;

	ubifs_assert(!c->need_recovery);

	commit_on_unmount(c);

	mutex_lock(&c->umount_mutex);
	if (c->bgt) {
		kthread_stop(c->bgt);
		c->bgt = NULL;
	}

	for (i = 0; i < c->jhead_cnt; i++) {
		ubifs_wbuf_sync(&c->jheads[i].wbuf);
		del_timer_sync(&c->jheads[i].wbuf.timer);
	}

	if (!c->ro_media) {
		c->mst_node->flags &= ~cpu_to_le32(UBIFS_MST_DIRTY);
		c->mst_node->flags |= cpu_to_le32(UBIFS_MST_NO_ORPHS);
		c->mst_node->gc_lnum = cpu_to_le32(c->gc_lnum);
		err = ubifs_write_master(c);
		if (err)
			ubifs_ro_mode(c, err);
	}

	ubifs_destroy_idx_gc(c);
	free_wbufs(c);
	free_orphans(c);
#if defined(CONFIG_MTD_NAND_DMA) && !defined(CONFIG_MTD_NAND_DMABUF)
	kfree(c->ileb_buf);
#else
	vfree(c->ileb_buf);
#endif
	c->ileb_buf = NULL;
	ubifs_lpt_free(c, 1);
	mutex_unlock(&c->umount_mutex);
}

/**
 * open_ubi - parse UBI device name string and open the UBI device.
 * @c: UBIFS file-system description object
 * @name: UBI volume name
 * @mode: UBI volume open mode
 *
 * There are several ways to specify UBI volumes when mounting UBIFS:
 * o ubiX_Y    - UBI device number X, volume Y;
 * o ubiY      - UBI device number 0, volume Y;
 * o ubiX:NAME - mount UBI device X, volume with name NAME;
 * o ubi:NAME  - mount UBI device 0, volume with name NAME.
 *
 * Alternative '!' separator may be used instead of ':' (because some shells
 * like busybox may interpret ':' as an NFS host name separator). This function
 * returns zero in case of success and a negative error code in case of
 * failure.
 */
static int open_ubi(struct ubifs_info *c, const char *name, int mode)
{
	int dev, vol;
	char *endptr;

	if (name[0] != 'u' || name[1] != 'b' || name[2] != 'i')
		return -EINVAL;

	if ((name[3] == ':' || name[3] == '!') && name[4] != '\0') {
		/* ubi:NAME method */
		c->ubi = ubi_open_volume_nm(0, name + 4, mode);
		if (IS_ERR(c->ubi))
			return PTR_ERR(c->ubi);
	} else if (isdigit(name[3])) {
		dev = simple_strtoul(name + 3, &endptr, 0);
		if (*endptr == '\0') {
			/* ubiY method */
			c->ubi = ubi_open_volume(0, dev, mode);
			if (IS_ERR(c->ubi))
				return PTR_ERR(c->ubi);
		} else if (*endptr == '_' && isdigit(endptr[1])) {
			/* ubiX_Y method */
			vol = simple_strtoul(endptr + 1, &endptr, 0);
			if (*endptr != '\0')
				return -EINVAL;
			c->ubi = ubi_open_volume(dev, vol, mode);
			if (IS_ERR(c->ubi))
				return PTR_ERR(c->ubi);
		} else if ((*endptr == ':' || *endptr == '!') &&
			   endptr[1] != '\0') {
			/* ubiX:NAME method */
			c->ubi = ubi_open_volume_nm(dev, ++endptr, mode);
			if (IS_ERR(c->ubi))
				return PTR_ERR(c->ubi);
		}
	}

	if (!c->ubi)
		return -EINVAL;

	ubi_get_volume_info(c->ubi, &c->vi);
	ubi_get_device_info(c->vi.ubi_num, &c->di);
	return 0;
}

static int sb_test(struct super_block *sb, void *data)
{
	dev_t *dev = data;

	return sb->s_dev == *dev;
}

static int sb_set(struct super_block *sb, void *data)
{
	return 0;
}

/*
 * UBIFS mount options.
 *
 * Opt_fast_unmount: do not run a journal commit before un-mounting
 * Opt_norm_unmount: run a journal commit before un-mounting
 * Opt_err: just end of array marker
 */
enum {
	Opt_fast_unmount,
	Opt_norm_unmount,
	Opt_err,
};

static match_table_t tokens = {
	{Opt_fast_unmount, "fast_unmount"},
	{Opt_norm_unmount, "norm_unmount"},
	{Opt_err, NULL},
};

/**
 * ubifs_parse_options - parse mount parameters.
 * @c: UBIFS file-system description object
 * @options: parameters to parse
 * @is_remount: non-zero if this is FS re-mount
 *
 * This function parses UBIFS mount options and returns zero in case success
 * and a negative error code in case of failure.
 */
int ubifs_parse_options(struct ubifs_info *c, char *options, int is_remount)
{
	char *p;
	substring_t args[MAX_OPT_ARGS];

	if (!options)
		return 0;

	while ((p = strsep(&options, ",")) != NULL) {
		int token;

		if (!*p)
			continue;

		token = match_token(p, tokens, args);
		switch (token) {
		case Opt_fast_unmount:
			c->mount_opts.unmount_mode = 2;
			c->fast_unmount = 1;
			break;
		case Opt_norm_unmount:
			c->mount_opts.unmount_mode = 1;
			c->fast_unmount = 0;
			break;
		default:
			ubifs_err("unrecognized mount option \"%s\" "
				  "or missing value", p);
			return -EINVAL;
		}
	}

	return 0;
}

static int ubifs_get_sb(struct file_system_type *fs_type, int flags,
			const char *name, void *data, struct vfsmount *mnt)
{
	int err;
	struct super_block *sb;
	struct ubifs_info *c;
	struct inode *root;

	dbg_gen("name %s, flags %#x", name, flags);

	c = kzalloc(sizeof(struct ubifs_info), GFP_KERNEL);
	if (!c)
		return -ENOMEM;

	spin_lock_init(&c->cnt_lock);
	spin_lock_init(&c->cs_lock);
	spin_lock_init(&c->buds_lock);
	spin_lock_init(&c->space_lock);
	spin_lock_init(&c->orphan_lock);
	init_rwsem(&c->commit_sem);
	mutex_init(&c->lp_mutex);
	mutex_init(&c->tnc_mutex);
	mutex_init(&c->log_mutex);
	mutex_init(&c->mst_mutex);
	mutex_init(&c->umount_mutex);
	init_waitqueue_head(&c->cmt_wq);
	c->buds = RB_ROOT;
	c->old_idx = RB_ROOT;
	c->size_tree = RB_ROOT;
	c->orph_tree = RB_ROOT;
	INIT_LIST_HEAD(&c->infos_list);
	INIT_LIST_HEAD(&c->idx_gc);
	INIT_LIST_HEAD(&c->replay_list);
	INIT_LIST_HEAD(&c->replay_buds);
	INIT_LIST_HEAD(&c->uncat_list);
	INIT_LIST_HEAD(&c->empty_list);
	INIT_LIST_HEAD(&c->freeable_list);
	INIT_LIST_HEAD(&c->frdi_idx_list);
	INIT_LIST_HEAD(&c->unclean_leb_list);
	INIT_LIST_HEAD(&c->old_buds);
	INIT_LIST_HEAD(&c->orph_list);
	INIT_LIST_HEAD(&c->orph_new);

	c->highest_inum = UBIFS_FIRST_INO;
	get_random_bytes(&c->vfs_gen, sizeof(int));
	c->lhead_lnum = c->ltail_lnum = UBIFS_LOG_LNUM;

	err = ubifs_parse_options(c, data, 0);
	if (err)
		goto out_free;

	/*
	 * Get UBI device number and volume ID. Mount it read-only so far
	 * because this might be a new mount point, and UBI allows only one
	 * read-write user at a time.
	 */
	err = open_ubi(c, name, UBI_READONLY);
	if (err) {
		ubifs_err("cannot open \"%s\", error %d", name, err);
		goto out_free;
	}

	dbg_gen("opened ubi%d_%d", c->vi.ubi_num, c->vi.vol_id);

	sb = sget(fs_type, &sb_test, &sb_set, &c->vi.cdev);
	if (IS_ERR(sb)) {
		err = PTR_ERR(sb);
		goto out_close;
	}

	if (sb->s_root) {
		/* A new mount point for already mounted UBIFS */
		dbg_gen("this ubi volume is already mounted");
		err = simple_set_mnt(mnt, sb);
		goto out_close;
	}

	/* Re-open the UBI device in read-write mode */
	ubi_close_volume(c->ubi);
	c->ubi = ubi_open_volume(c->vi.ubi_num, c->vi.vol_id, UBI_READWRITE);
	if (IS_ERR(c->ubi)) {
		err = PTR_ERR(c->ubi);
		goto out_free;
	}

	c->vfs_sb = sb;
	sb->s_fs_info = c;
	sb->s_magic = UBIFS_SUPER_MAGIC;
	sb->s_blocksize = UBIFS_BLOCK_SIZE;
	sb->s_blocksize_bits = UBIFS_BLOCK_SHIFT;
	sb->s_dev = c->vi.cdev;
	sb->s_maxbytes = c->max_inode_sz =
			min_t(uint64_t, MAX_LFS_FILESIZE, UBIFS_MAX_INODE_SZ);
	sb->s_op = &ubifs_super_operations;
	sb->s_flags = flags;

	mutex_lock(&c->umount_mutex);
	err = mount_ubifs(c);
	if (err) {
		ubifs_assert(err < 0);
		goto out_umount;
	}

	/* Read the root inode */
	root = ubifs_iget(sb, UBIFS_ROOT_INO);
	if (IS_ERR(root)) {
		err = PTR_ERR(root);
		goto out_umount;
	}

	sb->s_root = d_alloc_root(root);
	if (!sb->s_root)
		goto out_iput;

	mutex_unlock(&c->umount_mutex);

	/* We do not support atime */
	sb->s_flags |= MS_ACTIVE | MS_NOATIME;
	return simple_set_mnt(mnt, sb);

out_iput:
	iput(root);
out_umount:
	spin_lock(&ubifs_infos_lock);
	if (c->infos_list.next)
		list_del(&c->infos_list);
	spin_unlock(&ubifs_infos_lock);
	ubifs_umount(c);
	mutex_unlock(&c->umount_mutex);
	up_write(&sb->s_umount);
	sb->s_root = NULL;
	deactivate_super(sb);
out_close:
	ubi_close_volume(c->ubi);
out_free:
	kfree(c);
	return err;
}

static void ubifs_kill_sb(struct super_block *sb)
{
	struct ubifs_info *c = sb->s_fs_info;

	if (sb->s_root != NULL && !(sb->s_flags & MS_RDONLY))
		commit_on_unmount(c);
	/* The un-mount routine is actually done in put_super() */
	generic_shutdown_super(sb);
}

static struct file_system_type ubifs_fs_type = {
	.name    = "ubifs",
	.owner   = THIS_MODULE,
	.get_sb  = ubifs_get_sb,
	.kill_sb = ubifs_kill_sb
};

/*
 * Inode slab cache constructor.
 *
 * TODO: remove backward compatibility as late as possible
 */
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24))
static void inode_slab_ctor(void *obj, struct kmem_cache *cachep,
			    unsigned long flags)
#else
static void inode_slab_ctor(struct kmem_cache *cachep, void *obj)
#endif
{
	struct ubifs_inode *inode = obj;
	inode_init_once(&inode->vfs_inode);
}

static int __init ubifs_init(void)
{
	int err;

	BUILD_BUG_ON(sizeof(struct ubifs_ch) != 24);

	/* Make sure node sizes are 8-byte aligned */
	BUILD_BUG_ON(UBIFS_CH_SZ        & 7);
	BUILD_BUG_ON(UBIFS_INO_NODE_SZ  & 7);
	BUILD_BUG_ON(UBIFS_DENT_NODE_SZ & 7);
	BUILD_BUG_ON(UBIFS_XENT_NODE_SZ & 7);
	BUILD_BUG_ON(UBIFS_DATA_NODE_SZ & 7);
	BUILD_BUG_ON(UBIFS_TRUN_NODE_SZ & 7);
	BUILD_BUG_ON(UBIFS_SB_NODE_SZ   & 7);
	BUILD_BUG_ON(UBIFS_MST_NODE_SZ  & 7);
	BUILD_BUG_ON(UBIFS_REF_NODE_SZ  & 7);
	BUILD_BUG_ON(UBIFS_CS_NODE_SZ   & 7);
	BUILD_BUG_ON(UBIFS_ORPH_NODE_SZ & 7);

	BUILD_BUG_ON(UBIFS_MAX_DENT_NODE_SZ & 7);
	BUILD_BUG_ON(UBIFS_MAX_XENT_NODE_SZ & 7);
	BUILD_BUG_ON(UBIFS_MAX_DATA_NODE_SZ & 7);
	BUILD_BUG_ON(UBIFS_MAX_INO_NODE_SZ  & 7);
	BUILD_BUG_ON(UBIFS_MAX_NODE_SZ      & 7);
	BUILD_BUG_ON(MIN_WRITE_SZ           & 7);

	/* Check min. node size */
	BUILD_BUG_ON(UBIFS_INO_NODE_SZ  < MIN_WRITE_SZ);
	BUILD_BUG_ON(UBIFS_DENT_NODE_SZ < MIN_WRITE_SZ);
	BUILD_BUG_ON(UBIFS_XENT_NODE_SZ < MIN_WRITE_SZ);
	BUILD_BUG_ON(UBIFS_TRUN_NODE_SZ < MIN_WRITE_SZ);

	BUILD_BUG_ON(UBIFS_MAX_DENT_NODE_SZ > UBIFS_MAX_NODE_SZ);
	BUILD_BUG_ON(UBIFS_MAX_XENT_NODE_SZ > UBIFS_MAX_NODE_SZ);
	BUILD_BUG_ON(UBIFS_MAX_DATA_NODE_SZ > UBIFS_MAX_NODE_SZ);
	BUILD_BUG_ON(UBIFS_MAX_INO_NODE_SZ  > UBIFS_MAX_NODE_SZ);

	/* We do not support multiple pages per block ATM */
	BUILD_BUG_ON(UBIFS_BLOCK_SIZE != PAGE_CACHE_SIZE);

	/* Defined node sizes */
	BUILD_BUG_ON(UBIFS_SB_NODE_SZ  != 4096);
	BUILD_BUG_ON(UBIFS_MST_NODE_SZ != 512);
	BUILD_BUG_ON(UBIFS_INO_NODE_SZ != 160);
	BUILD_BUG_ON(UBIFS_REF_NODE_SZ != 64);

	err  = bdi_init(&ubifs_backing_dev_info);
	if (err)
		return err;

	err = register_filesystem(&ubifs_fs_type);
	if (err) {
		ubifs_err("cannot register file system, error %d", err);
		goto out;
	}

	err = -ENOMEM;
	ubifs_inode_slab = kmem_cache_create("ubifs_inode_slab",
				sizeof(struct ubifs_inode), 0,
				SLAB_MEM_SPREAD | SLAB_RECLAIM_ACCOUNT,
				&inode_slab_ctor UBIFSCOMPATNULL);
	if (!ubifs_inode_slab)
		goto out_reg;

	register_shrinker(&ubifs_shrinker_info);
	dbg_mempressure_init();

	err = ubifs_compressors_init();
	if (err)
		goto out_compr;

	return 0;

out_compr:
	dbg_mempressure_exit();
	unregister_shrinker(&ubifs_shrinker_info);
	kmem_cache_destroy(ubifs_inode_slab);
out_reg:
	unregister_filesystem(&ubifs_fs_type);
out:
	bdi_destroy(&ubifs_backing_dev_info);
	return err;
}
/* late_initcall to let compressors initialize first */
late_initcall(ubifs_init);

static void __exit ubifs_exit(void)
{
	ubifs_assert(list_empty(&ubifs_infos));
	ubifs_assert(atomic_long_read(&ubifs_clean_zn_cnt) == 0);

	ubifs_compressors_exit();
	dbg_mempressure_exit();
	unregister_shrinker(&ubifs_shrinker_info);
	kmem_cache_destroy(ubifs_inode_slab);
	unregister_filesystem(&ubifs_fs_type);
	bdi_destroy(&ubifs_backing_dev_info);
	dbg_leak_report();
}
module_exit(ubifs_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION(__stringify(UBIFS_VERSION));
MODULE_AUTHOR("Artem Bityutskiy, Adrian Hunter");
MODULE_DESCRIPTION("UBIFS - UBI File System");
