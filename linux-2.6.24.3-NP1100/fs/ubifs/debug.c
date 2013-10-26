/*
 * This file is part of UBIFS.
 *
 * Copyright (C) 2006-2008 Nokia Corporation
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
 * This file implements most of the debugging stuff which is compiled in only
 * when it is enabled. But some debugging check functions are implemented in
 * corresponding subsystem, just because they are closely related and utilize
 * various local functions of those subsystems.
 */

#define UBIFS_DBG_PRESERVE_UBI

#include "ubifs.h"
#include <linux/module.h>
#include <linux/moduleparam.h>

#ifdef CONFIG_UBIFS_FS_DEBUG

DEFINE_SPINLOCK(dbg_lock);

static char dbg_key_buf0[128];
static char dbg_key_buf1[128];

unsigned int ubifs_msg_flags = UBIFS_MSG_FLAGS_DEFAULT;
unsigned int ubifs_chk_flags = UBIFS_CHK_FLAGS_DEFAULT;
unsigned int ubifs_tst_flags;

module_param_named(debug_msgs, ubifs_msg_flags, uint, S_IRUGO | S_IWUSR);
module_param_named(debug_chks, ubifs_chk_flags, uint, S_IRUGO | S_IWUSR);
module_param_named(debug_tsts, ubifs_tst_flags, uint, S_IRUGO | S_IWUSR);

MODULE_PARM_DESC(debug_msgs, "Debug message type flags");
MODULE_PARM_DESC(debug_chks, "Debug check flags");
MODULE_PARM_DESC(debug_tsts, "Debug special test flags");

static const char *get_key_fmt(int fmt)
{
	switch (fmt) {
	case UBIFS_SIMPLE_KEY_FMT:
		return "simple";
	default:
		return "unknown/invalid format";
	}
}

static const char *get_key_hash(int hash)
{
	switch (hash) {
	case UBIFS_KEY_HASH_R5:
		return "R5";
	case UBIFS_KEY_HASH_TEST:
		return "test";
	default:
		return "unknown/invalid name hash";
	}
}

static const char *get_key_type(int type)
{
	switch (type) {
	case UBIFS_INO_KEY:
		return "inode";
	case UBIFS_DENT_KEY:
		return "direntry";
	case UBIFS_XENT_KEY:
		return "xentry";
	case UBIFS_DATA_KEY:
		return "data";
	case UBIFS_TRUN_KEY:
		return "truncate";
	default:
		return "unknown/invalid key";
	}
}

static void sprintf_key(const struct ubifs_info *c, const union ubifs_key *key,
			char *buffer)
{
	char *p = buffer;
	int type = key_type(c, key);

	if (c->key_fmt == UBIFS_SIMPLE_KEY_FMT) {
		switch (type) {
		case UBIFS_INO_KEY:
			sprintf(p, "(%lu, %s)", key_ino(c, key),
			       get_key_type(type));
			break;
		case UBIFS_DENT_KEY:
		case UBIFS_XENT_KEY:
			sprintf(p, "(%lu, %s, %#08x)", key_ino(c, key),
				get_key_type(type), key_hash(c, key));
			break;
		case UBIFS_DATA_KEY:
			sprintf(p, "(%lu, %s, %u)", key_ino(c, key),
				get_key_type(type), key_block(c, key));
			break;
		case UBIFS_TRUN_KEY:
			sprintf(p, "(%lu, %s)",
				key_ino(c, key), get_key_type(type));
			break;
		default:
			sprintf(p, "(bad key type: %#08x, %#08x)",
				key->u32[0], key->u32[1]);
		}
	} else
		sprintf(p, "bad key format %d", c->key_fmt);
}

const char *dbg_key_str0(const struct ubifs_info *c, const union ubifs_key *key)
{
	/* dbg_lock must be held */
	sprintf_key(c, key, dbg_key_buf0);
	return dbg_key_buf0;
}

const char *dbg_key_str1(const struct ubifs_info *c, const union ubifs_key *key)
{
	/* dbg_lock must be held */
	sprintf_key(c, key, dbg_key_buf1);
	return dbg_key_buf1;
}

const char *dbg_ntype(int type)
{
	switch (type) {
	case UBIFS_PAD_NODE:
		return "padding node";
	case UBIFS_SB_NODE:
		return "superblock node";
	case UBIFS_MST_NODE:
		return "master node";
	case UBIFS_REF_NODE:
		return "reference node";
	case UBIFS_INO_NODE:
		return "inode node";
	case UBIFS_DENT_NODE:
		return "direntry node";
	case UBIFS_XENT_NODE:
		return "xentry node";
	case UBIFS_DATA_NODE:
		return "data node";
	case UBIFS_TRUN_NODE:
		return "truncate node";
	case UBIFS_IDX_NODE:
		return "indexing node";
	case UBIFS_CS_NODE:
		return "commit start node";
	case UBIFS_ORPH_NODE:
		return "orphan node";
	default:
		return "unknown node";
	}
}

static const char *dbg_gtype(int type)
{
	switch (type) {
	case UBIFS_NO_NODE_GROUP:
		return "no node group";
	case UBIFS_IN_NODE_GROUP:
		return "in node group";
	case UBIFS_LAST_OF_NODE_GROUP:
		return "last of node group";
	default:
		return "unknown";
	}
}

const char *dbg_cstate(int cmt_state)
{
	switch (cmt_state) {
	case COMMIT_RESTING:
		return "commit resting";
	case COMMIT_BACKGROUND:
		return "background commit requested";
	case COMMIT_REQUIRED:
		return "commit required";
	case COMMIT_RUNNING_BACKGROUND:
		return "BACKGROUND commit running";
	case COMMIT_RUNNING_REQUIRED:
		return "commit running and required";
	case COMMIT_BROKEN:
		return "broken commit";
	default:
		return "unknown commit state";
	}
}

static void dump_ch(const struct ubifs_ch *ch)
{
	printk(KERN_DEBUG "\tmagic          %#x\n", le32_to_cpu(ch->magic));
	printk(KERN_DEBUG "\tcrc            %#x\n", le32_to_cpu(ch->crc));
	printk(KERN_DEBUG "\tnode_type      %d (%s)\n", ch->node_type,
	       dbg_ntype(ch->node_type));
	printk(KERN_DEBUG "\tgroup_type     %d (%s)\n", ch->group_type,
	       dbg_gtype(ch->group_type));
	printk(KERN_DEBUG "\tsqnum          %llu\n",
	       (unsigned long long)le64_to_cpu(ch->sqnum));
	printk(KERN_DEBUG "\tlen            %u\n", le32_to_cpu(ch->len));
}

void dbg_dump_inode(const struct ubifs_info *c, const struct inode *inode)
{
	const struct ubifs_inode *ui = ubifs_inode(inode);

	printk(KERN_DEBUG "inode      %lu\n", inode->i_ino);
	printk(KERN_DEBUG "size       %llu\n",
	       (unsigned long long)i_size_read(inode));
	printk(KERN_DEBUG "nlink      %u\n", inode->i_nlink);
	printk(KERN_DEBUG "uid        %u\n", (unsigned int)inode->i_uid);
	printk(KERN_DEBUG "gid        %u\n", (unsigned int)inode->i_gid);
	printk(KERN_DEBUG "atime      %u.%u\n",
	       (unsigned int)inode->i_atime.tv_sec,
	       (unsigned int)inode->i_atime.tv_nsec);
	printk(KERN_DEBUG "mtime      %u.%u\n",
	       (unsigned int)inode->i_mtime.tv_sec,
	       (unsigned int)inode->i_mtime.tv_nsec);
	printk(KERN_DEBUG "ctime       %u.%u\n",
	       (unsigned int)inode->i_ctime.tv_sec,
	       (unsigned int)inode->i_ctime.tv_nsec);
	printk(KERN_DEBUG "creat_sqnum %llu\n", ui->creat_sqnum);
	printk(KERN_DEBUG "xattr_size  %lld\n", ui->xattr_size);
	printk(KERN_DEBUG "xattr_cnt   %d\n", ui->xattr_cnt);
	printk(KERN_DEBUG "xattr_names %d\n", ui->xattr_names);
	printk(KERN_DEBUG "dirty       %u\n", ui->dirty);
	printk(KERN_DEBUG "xattr       %u\n", ui->xattr);
	printk(KERN_DEBUG "flags       %d\n", ui->flags);
	printk(KERN_DEBUG "compr_type  %d\n", ui->compr_type);
	printk(KERN_DEBUG "data_len    %d\n", ui->data_len);
}

void dbg_dump_node(const struct ubifs_info *c, const void *node)
{
	int i, n;
	union ubifs_key key;
	const struct ubifs_ch *ch = node;

	if (dbg_failure_mode)
		return;

	/* If the magic is incorrect, just hexdump the first bytes */
	if (le32_to_cpu(ch->magic) != UBIFS_NODE_MAGIC) {
		printk(KERN_DEBUG "Not a node, first %zu bytes:", UBIFS_CH_SZ);
		print_hex_dump(KERN_DEBUG, "", DUMP_PREFIX_OFFSET, 32, 1,
			       (void *)node, UBIFS_CH_SZ, 1);
		return;
	}

	spin_lock(&dbg_lock);
	dump_ch(node);

	switch (ch->node_type) {
	case UBIFS_PAD_NODE:
	{
		const struct ubifs_pad_node *pad = node;

		printk(KERN_DEBUG "\tpad_len        %u\n",
		       le32_to_cpu(pad->pad_len));
		break;
	}
	case UBIFS_SB_NODE:
	{
		const struct ubifs_sb_node *sup = node;
		unsigned int sup_flags = le32_to_cpu(sup->flags);

		printk(KERN_DEBUG "\tkey_hash       %d (%s)\n",
		       (int)sup->key_hash, get_key_hash(sup->key_hash));
		printk(KERN_DEBUG "\tkey_fmt        %d (%s)\n",
		       (int)sup->key_fmt, get_key_fmt(sup->key_fmt));
		printk(KERN_DEBUG "\tflags          %#x\n", sup_flags);
		printk(KERN_DEBUG "\t  big_lpt      %u\n",
		       !!(sup_flags & UBIFS_FLG_BIGLPT));
		printk(KERN_DEBUG "\tmin_io_size    %u\n",
		       le32_to_cpu(sup->min_io_size));
		printk(KERN_DEBUG "\tleb_size       %u\n",
		       le32_to_cpu(sup->leb_size));
		printk(KERN_DEBUG "\tleb_cnt        %u\n",
		       le32_to_cpu(sup->leb_cnt));
		printk(KERN_DEBUG "\tmax_leb_cnt    %u\n",
		       le32_to_cpu(sup->max_leb_cnt));
		printk(KERN_DEBUG "\tmax_bud_bytes  %llu\n",
		       (unsigned long long)le64_to_cpu(sup->max_bud_bytes));
		printk(KERN_DEBUG "\tlog_lebs       %u\n",
		       le32_to_cpu(sup->log_lebs));
		printk(KERN_DEBUG "\tlpt_lebs       %u\n",
		       le32_to_cpu(sup->lpt_lebs));
		printk(KERN_DEBUG "\torph_lebs      %u\n",
		       le32_to_cpu(sup->orph_lebs));
		printk(KERN_DEBUG "\tjhead_cnt      %u\n",
		       le32_to_cpu(sup->jhead_cnt));
		printk(KERN_DEBUG "\tfanout         %u\n",
		       le32_to_cpu(sup->fanout));
		printk(KERN_DEBUG "\tlsave_cnt      %u\n",
		       le32_to_cpu(sup->lsave_cnt));
		printk(KERN_DEBUG "\tdefault_compr  %u\n",
		       (int)le16_to_cpu(sup->default_compr));
		printk(KERN_DEBUG "\trp_size        %llu\n",
		       (unsigned long long)le64_to_cpu(sup->rp_size));
		printk(KERN_DEBUG "\trp_uid         %u\n",
		       le32_to_cpu(sup->rp_uid));
		printk(KERN_DEBUG "\trp_gid         %u\n",
		       le32_to_cpu(sup->rp_gid));
		printk(KERN_DEBUG "\tfmt_version    %u\n",
		       le32_to_cpu(sup->fmt_version));
		printk(KERN_DEBUG "\ttime_gran      %u\n",
		       le32_to_cpu(sup->time_gran));
		break;
	}
	case UBIFS_MST_NODE:
	{
		const struct ubifs_mst_node *mst = node;

		printk(KERN_DEBUG "\thighest_inum   %llu\n",
		       (unsigned long long)le64_to_cpu(mst->highest_inum));
		printk(KERN_DEBUG "\tcommit number  %llu\n",
		       (unsigned long long)le64_to_cpu(mst->cmt_no));
		printk(KERN_DEBUG "\tflags          %#x\n",
		       le32_to_cpu(mst->flags));
		printk(KERN_DEBUG "\tlog_lnum       %u\n",
		       le32_to_cpu(mst->log_lnum));
		printk(KERN_DEBUG "\troot_lnum      %u\n",
		       le32_to_cpu(mst->root_lnum));
		printk(KERN_DEBUG "\troot_offs      %u\n",
		       le32_to_cpu(mst->root_offs));
		printk(KERN_DEBUG "\troot_len       %u\n",
		       le32_to_cpu(mst->root_len));
		printk(KERN_DEBUG "\tgc_lnum        %u\n",
		       le32_to_cpu(mst->gc_lnum));
		printk(KERN_DEBUG "\tihead_lnum     %u\n",
		       le32_to_cpu(mst->ihead_lnum));
		printk(KERN_DEBUG "\tihead_offs     %u\n",
		       le32_to_cpu(mst->ihead_offs));
		printk(KERN_DEBUG "\tindex_size     %u\n",
		       le32_to_cpu(mst->index_size));
		printk(KERN_DEBUG "\tlpt_lnum       %u\n",
		       le32_to_cpu(mst->lpt_lnum));
		printk(KERN_DEBUG "\tlpt_offs       %u\n",
		       le32_to_cpu(mst->lpt_offs));
		printk(KERN_DEBUG "\tnhead_lnum     %u\n",
		       le32_to_cpu(mst->nhead_lnum));
		printk(KERN_DEBUG "\tnhead_offs     %u\n",
		       le32_to_cpu(mst->nhead_offs));
		printk(KERN_DEBUG "\tltab_lnum      %u\n",
		       le32_to_cpu(mst->ltab_lnum));
		printk(KERN_DEBUG "\tltab_offs      %u\n",
		       le32_to_cpu(mst->ltab_offs));
		printk(KERN_DEBUG "\tlsave_lnum     %u\n",
		       le32_to_cpu(mst->lsave_lnum));
		printk(KERN_DEBUG "\tlsave_offs     %u\n",
		       le32_to_cpu(mst->lsave_offs));
		printk(KERN_DEBUG "\tlscan_lnum     %u\n",
		       le32_to_cpu(mst->lscan_lnum));
		printk(KERN_DEBUG "\tleb_cnt        %u\n",
		       le32_to_cpu(mst->leb_cnt));
		printk(KERN_DEBUG "\tempty_lebs     %u\n",
		       le32_to_cpu(mst->empty_lebs));
		printk(KERN_DEBUG "\tidx_lebs       %u\n",
		       le32_to_cpu(mst->idx_lebs));
		printk(KERN_DEBUG "\ttotal_free     %llu\n",
		       (unsigned long long)le64_to_cpu(mst->total_free));
		printk(KERN_DEBUG "\ttotal_dirty    %llu\n",
		       (unsigned long long)le64_to_cpu(mst->total_dirty));
		printk(KERN_DEBUG "\ttotal_used     %llu\n",
		       (unsigned long long)le64_to_cpu(mst->total_used));
		printk(KERN_DEBUG "\ttotal_dead     %llu\n",
		       (unsigned long long)le64_to_cpu(mst->total_dead));
		printk(KERN_DEBUG "\ttotal_dark     %llu\n",
		       (unsigned long long)le64_to_cpu(mst->total_dark));
		break;
	}
	case UBIFS_REF_NODE:
	{
		const struct ubifs_ref_node *ref = node;

		printk(KERN_DEBUG "\tlnum           %u\n",
		       le32_to_cpu(ref->lnum));
		printk(KERN_DEBUG "\toffs           %u\n",
		       le32_to_cpu(ref->offs));
		printk(KERN_DEBUG "\tjhead          %u\n",
		       le32_to_cpu(ref->jhead));
		break;
	}
	case UBIFS_INO_NODE:
	{
		const struct ubifs_ino_node *ino = node;

		key_read(c, &ino->key, &key);
		printk(KERN_DEBUG "\tkey            %s\n", DBGKEY(&key));
		printk(KERN_DEBUG "\tsize           %llu\n",
		       (unsigned long long)le64_to_cpu(ino->size));
		printk(KERN_DEBUG "\tnlink          %u\n",
		       le32_to_cpu(ino->nlink));
		printk(KERN_DEBUG "\tatime          %lld.%u\n",
		       (long long)le64_to_cpu(ino->atime_sec),
		       le32_to_cpu(ino->atime_nsec));
		printk(KERN_DEBUG "\tmtime          %lld.%u\n",
		       (long long)le64_to_cpu(ino->mtime_sec),
		       le32_to_cpu(ino->mtime_nsec));
		printk(KERN_DEBUG "\tctime          %lld.%u\n",
		       (long long)le64_to_cpu(ino->ctime_sec),
		       le32_to_cpu(ino->ctime_nsec));
		printk(KERN_DEBUG "\tuid            %u\n",
		       le32_to_cpu(ino->uid));
		printk(KERN_DEBUG "\tgid            %u\n",
		       le32_to_cpu(ino->gid));
		printk(KERN_DEBUG "\tmode           %u\n",
		       le32_to_cpu(ino->mode));
		printk(KERN_DEBUG "\tflags          %#x\n",
		       le32_to_cpu(ino->flags));
		printk(KERN_DEBUG "\txattr_cnt      %u\n",
		       le32_to_cpu(ino->xattr_cnt));
		printk(KERN_DEBUG "\txattr_size     %llu\n",
		       (unsigned long long)le64_to_cpu(ino->xattr_size));
		printk(KERN_DEBUG "\txattr_names    %u\n",
		       le32_to_cpu(ino->xattr_names));
		printk(KERN_DEBUG "\tcompr_type     %#x\n",
		       (int)le16_to_cpu(ino->compr_type));
		printk(KERN_DEBUG "\tdata len       %u\n",
		       le32_to_cpu(ino->data_len));
		break;
	}
	case UBIFS_DENT_NODE:
	case UBIFS_XENT_NODE:
	{
		const struct ubifs_dent_node *dent = node;
		int nlen = le16_to_cpu(dent->nlen);

		key_read(c, &dent->key, &key);
		printk(KERN_DEBUG "\tkey            %s\n", DBGKEY(&key));
		printk(KERN_DEBUG "\tinum           %llu\n",
		       (unsigned long long)le64_to_cpu(dent->inum));
		printk(KERN_DEBUG "\ttype           %d\n", (int)dent->type);
		printk(KERN_DEBUG "\tnlen           %d\n", nlen);
		printk(KERN_DEBUG "\tname           ");

		if (nlen > UBIFS_MAX_NLEN) {
			nlen = UBIFS_MAX_NLEN;
			printk(KERN_DEBUG "\tWarning! Node is corrupted\n");
		}

		for (i = 0; i < nlen && dent->name[i]; i++)
			printk("%c", dent->name[i]);
		printk("\n");

		break;
	}
	case UBIFS_DATA_NODE:
	{
		const struct ubifs_data_node *dn = node;
		int dlen = le32_to_cpu(ch->len) - UBIFS_DATA_NODE_SZ;

		key_read(c, &dn->key, &key);
		printk(KERN_DEBUG "\tkey            %s\n", DBGKEY(&key));
		printk(KERN_DEBUG "\tsize           %u\n",
		       le32_to_cpu(dn->size));
		printk(KERN_DEBUG "\tcompr_typ      %d\n",
		       (int)le16_to_cpu(dn->compr_type));
		printk(KERN_DEBUG "\tdata size      %d\n",
		       dlen);
		printk(KERN_DEBUG "\tdata:\n");
		print_hex_dump(KERN_DEBUG, "\t", DUMP_PREFIX_OFFSET, 32, 1,
			       (void *)&dn->data, dlen, 0);
		break;
	}
	case UBIFS_TRUN_NODE:
	{
		const struct ubifs_trun_node *trun = node;

		key_read(c, &trun->key, &key);
		printk(KERN_DEBUG "\tkey            %s\n", DBGKEY(&key));
		printk(KERN_DEBUG "\told_size       %llu\n",
		       (unsigned long long)le64_to_cpu(trun->old_size));
		printk(KERN_DEBUG "\tnew_size       %llu\n",
		       (unsigned long long)le64_to_cpu(trun->new_size));
		break;
	}
	case UBIFS_IDX_NODE:
	{
		const struct ubifs_idx_node *idx = node;

		n = le16_to_cpu(idx->child_cnt);
		printk(KERN_DEBUG "\tchild_cnt      %d\n", n);
		printk(KERN_DEBUG "\tlevel          %d\n",
		       (int)le16_to_cpu(idx->level));
		printk(KERN_DEBUG "\tBranches:\n");

		for (i = 0; i < n && i < c->fanout - 1; i++) {
			const struct ubifs_branch *br;

			br = ubifs_idx_branch(c, idx, i);
			key_read(c, &br->key, &key);
			printk(KERN_DEBUG "\t%d: LEB %d:%d len %d key %s\n",
			       i, le32_to_cpu(br->lnum), le32_to_cpu(br->offs),
			       le32_to_cpu(br->len), DBGKEY(&key));
		}
		break;
	}
	case UBIFS_CS_NODE:
		break;
	case UBIFS_ORPH_NODE:
	{
		const struct ubifs_orph_node *orph = node;

		printk(KERN_DEBUG "\tcommit number  %llu\n",
		       (unsigned long long)
				le64_to_cpu(orph->cmt_no) & LLONG_MAX);
		printk(KERN_DEBUG "\tlast node flag %llu\n",
		       (unsigned long long)(le64_to_cpu(orph->cmt_no)) >> 63);
		n = (le32_to_cpu(ch->len) - UBIFS_ORPH_NODE_SZ) >> 3;
		printk(KERN_DEBUG "\t%d orphan inode numbers:\n", n);
		for (i = 0; i < n; i++)
			printk(KERN_DEBUG "\t  ino %llu\n",
			       le64_to_cpu(orph->inos[i]));
		break;
	}
	default:
		printk(KERN_DEBUG "node type %d was not recognized\n",
		       (int)ch->node_type);
	}
	spin_unlock(&dbg_lock);
}

void dbg_dump_budget_req(const struct ubifs_budget_req *req)
{
	spin_lock(&dbg_lock);
	printk(KERN_DEBUG "Budgeting request: new_ino %d, dirtied_ino %d\n",
	       req->new_ino, req->dirtied_ino);
	printk(KERN_DEBUG "\tnew_ino_d  %d, dirtied_ino_d %d\n",
	       req->new_ino_d, req->dirtied_ino_d);
	printk(KERN_DEBUG "\tnew_page    %d, dirtied_page %d\n",
	       req->new_page, req->dirtied_page);
	printk(KERN_DEBUG "\tnew_dent    %d, mod_dent     %d\n",
	       req->new_dent, req->mod_dent);
/* TODO: remove compatibility stuff as late as possible */
#ifdef UBIFS_COMPAT_USE_OLD_PREPARE_WRITE
	printk(KERN_DEBUG "\tlocked_pg   %d idx_growth    %d\n",
	       req->locked_pg, req->idx_growth);
#else
	printk(KERN_DEBUG "\tidx_growth  %d\n", req->idx_growth);
#endif
	printk(KERN_DEBUG "\tdata_growth %d dd_growth     %d\n",
	       req->data_growth, req->dd_growth);
	spin_unlock(&dbg_lock);
}

void dbg_dump_lstats(const struct ubifs_lp_stats *lst)
{
	spin_lock(&dbg_lock);
	printk(KERN_DEBUG "Lprops statistics: empty_lebs %d, idx_lebs  %d\n",
	       lst->empty_lebs, lst->idx_lebs);
	printk(KERN_DEBUG "\ttaken_empty_lebs %d, total_free %lld, "
	       "total_dirty %lld\n", lst->taken_empty_lebs, lst->total_free,
	       lst->total_dirty);
	printk(KERN_DEBUG "\ttotal_used %lld, total_dark %lld, "
	       "total_dead %lld\n", lst->total_used, lst->total_dark,
	       lst->total_dead);
	spin_unlock(&dbg_lock);
}

void dbg_dump_budg(struct ubifs_info *c)
{
	int i;
	struct rb_node *rb;
	struct ubifs_bud *bud;
	struct ubifs_gced_idx_leb *idx_gc;

	spin_lock(&dbg_lock);
	printk(KERN_DEBUG "Budgeting info: budg_data_growth %lld, "
	       "budg_dd_growth %lld, budg_idx_growth %lld\n",
	       c->budg_data_growth, c->budg_dd_growth, c->budg_idx_growth);
	printk(KERN_DEBUG "\tdata budget sum %lld, total budget sum %lld, "
	       "freeable_cnt %d\n", c->budg_data_growth + c->budg_dd_growth,
	       c->budg_data_growth + c->budg_dd_growth + c->budg_idx_growth,
	       c->freeable_cnt);
	printk(KERN_DEBUG "\tmin_idx_lebs %d, old_idx_sz %lld, "
	       "calc_idx_sz %lld, idx_gc_cnt %d\n", c->min_idx_lebs,
	       c->old_idx_sz, c->calc_idx_sz, c->idx_gc_cnt);
	printk(KERN_DEBUG "\tdirty_pg_cnt %ld, dirty_ino_cnt %ld, "
	       "dirty_zn_cnt %ld, clean_zn_cnt %ld\n",
	       atomic_long_read(&c->dirty_pg_cnt),
	       atomic_long_read(&c->dirty_ino_cnt),
	       atomic_long_read(&c->dirty_zn_cnt),
	       atomic_long_read(&c->clean_zn_cnt));
	printk(KERN_DEBUG "\tdark_wm %d, dead_wm %d, max_idx_node_sz %d\n",
	       c->dark_wm, c->dead_wm, c->max_idx_node_sz);
	printk(KERN_DEBUG "\tgc_lnum %d, ihead_lnum %d\n",
	       c->gc_lnum, c->ihead_lnum);
	for (i = 0; i < c->jhead_cnt; i++)
		printk(KERN_DEBUG "\tjhead %d\t LEB %d\n",
		       c->jheads[i].wbuf.jhead, c->jheads[i].wbuf.lnum);
	for (rb = rb_first(&c->buds); rb; rb = rb_next(rb)) {
		bud = rb_entry(rb, struct ubifs_bud, rb);
		printk(KERN_DEBUG "\tbud LEB %d\n", bud->lnum);
	}
	list_for_each_entry(bud, &c->old_buds, list)
		printk(KERN_DEBUG "\told bud LEB %d\n", bud->lnum);
	list_for_each_entry(idx_gc, &c->idx_gc, list)
		printk(KERN_DEBUG "\tGC'ed idx LEB %d unmap %d\n",
		       idx_gc->lnum, idx_gc->unmap);
	printk(KERN_DEBUG "\tcommit state %d\n", c->cmt_state);
	spin_unlock(&dbg_lock);
}

void dbg_dump_lprop(const struct ubifs_info *c, const struct ubifs_lprops *lp)
{
	printk(KERN_DEBUG "LEB %d lprops: free %d, dirty %d (used %d), "
	       "flags %#x\n", lp->lnum, lp->free, lp->dirty,
	       c->leb_size - lp->free - lp->dirty, lp->flags);
}

void dbg_dump_lprops(struct ubifs_info *c)
{
	int lnum, err;
	struct ubifs_lprops lp;
	struct ubifs_lp_stats lst;

	printk(KERN_DEBUG "Dumping LEB properties\n");
	ubifs_get_lp_stats(c, &lst);
	dbg_dump_lstats(&lst);

	for (lnum = c->main_first; lnum < c->leb_cnt; lnum++) {
		err = ubifs_read_one_lp(c, lnum, &lp);
		if (err)
			ubifs_err("cannot read lprops for LEB %d", lnum);

		dbg_dump_lprop(c, &lp);
	}
}

void dbg_dump_leb(const struct ubifs_info *c, int lnum)
{
	struct ubifs_scan_leb *sleb;
	struct ubifs_scan_node *snod;

	if (dbg_failure_mode)
		return;

	printk(KERN_DEBUG "Dumping LEB %d\n", lnum);

	sleb = ubifs_scan(c, lnum, 0, c->dbg_buf);
	if (IS_ERR(sleb)) {
		ubifs_err("scan error %d", (int)PTR_ERR(sleb));
		return;
	}

	printk(KERN_DEBUG "LEB %d has %d nodes ending at %d\n", lnum,
	       sleb->nodes_cnt, sleb->endpt);

	list_for_each_entry(snod, &sleb->nodes, list) {
		cond_resched();
		printk(KERN_DEBUG "Dumping node at LEB %d:%d len %d\n", lnum,
		       snod->offs, snod->len);
		dbg_dump_node(c, snod->node);
	}

	ubifs_scan_destroy(sleb);
	return;
}

void dbg_dump_znode(const struct ubifs_info *c,
		    const struct ubifs_znode *znode)
{
	int n;
	const struct ubifs_zbranch *zbr;

	spin_lock(&dbg_lock);
	if (znode->parent)
		zbr = &znode->parent->zbranch[znode->iip];
	else
		zbr = &c->zroot;

	printk(KERN_DEBUG "znode %p, LEB %d:%d len %d parent %p iip %d level %d"
	       " child_cnt %d flags %lx\n", znode, zbr->lnum, zbr->offs,
	       zbr->len, znode->parent, znode->iip, znode->level,
	       znode->child_cnt, znode->flags);

	if (znode->child_cnt <= 0 || znode->child_cnt > c->fanout) {
		spin_unlock(&dbg_lock);
		return;
	}

	printk(KERN_DEBUG "zbranches:\n");
	for (n = 0; n < znode->child_cnt; n++) {
		cond_resched();

		zbr = &znode->zbranch[n];
		if (znode->level > 0)
			printk(KERN_DEBUG "\t%d: znode %p LEB %d:%d len %d key "
					  "%s\n", n, zbr->znode, zbr->lnum,
					  zbr->offs, zbr->len,
					  DBGKEY(&zbr->key));
		else
			printk(KERN_DEBUG "\t%d: LNC %p LEB %d:%d len %d key "
					  "%s\n", n, zbr->znode, zbr->lnum,
					  zbr->offs, zbr->len,
					  DBGKEY(&zbr->key));
	}
	spin_unlock(&dbg_lock);
}

void dbg_dump_heap(struct ubifs_info *c, struct ubifs_lpt_heap *heap, int cat)
{
	int i;

	printk(KERN_DEBUG "Dumping heap cat %d (%d elements)\n",
	       cat, heap->cnt);
	for (i = 0; i < heap->cnt; i++) {
		struct ubifs_lprops *lprops = heap->arr[i];

		printk(KERN_DEBUG "\t%d. LEB %d hpos %d free %d dirty %d "
		       "flags %d\n", i, lprops->lnum, lprops->hpos,
		       lprops->free, lprops->dirty, lprops->flags);
	}
}

void dbg_dump_pnode(struct ubifs_info *c, struct ubifs_pnode *pnode,
		    struct ubifs_nnode *parent, int iip)
{
	int i;

	printk(KERN_DEBUG "Dumping pnode:\n");
	printk(KERN_DEBUG "\taddress %zx parent %zx cnext %zx\n",
	       (size_t)pnode, (size_t)parent, (size_t)pnode->cnext);
	printk(KERN_DEBUG "\tflags %lu iip %d level %d num %d\n",
	       pnode->flags, iip, pnode->level, pnode->num);
	for (i = 0; i < UBIFS_LPT_FANOUT; i++) {
		struct ubifs_lprops *lp = &pnode->lprops[i];

		printk(KERN_DEBUG "\t%d: free %d dirty %d flags %d lnum %d\n",
		       i, lp->free, lp->dirty, lp->flags, lp->lnum);
	}
}

void dbg_dump_tnc(struct ubifs_info *c)
{
	struct ubifs_znode *znode;
	int level;

	printk(KERN_DEBUG "\n");
	printk(KERN_DEBUG "Dumping the TNC tree\n");
	znode = ubifs_tnc_levelorder_next(c->zroot.znode, NULL);
	level = znode->level;
	printk(KERN_DEBUG "== Level %d ==\n", level);
	while (znode) {
		if (level != znode->level) {
			level = znode->level;
			printk(KERN_DEBUG "== Level %d ==\n", level);
		}
		dbg_dump_znode(c, znode);
		znode = ubifs_tnc_levelorder_next(c->zroot.znode, znode);
	}

	printk(KERN_DEBUG "\n");
}

/*
 * dbg_check_dir - check directory inode size.
 * @c: UBIFS file-system description object
 * @dir: the directory to calculate size for
 * @size: the result is returned here
 *
 * This function makes sure that directory size is correct. Returns zero
 * in case of success and a negative error code in case of failure.
 *
 * Note, it is good idea to make sure the @dir->i_mutex is locked before
 * calling this function.
 */
int dbg_check_dir_size(struct ubifs_info *c, const struct inode *dir)
{
	union ubifs_key key;
	struct ubifs_dent_node *dent, *pdent = NULL;
	struct qstr nm = { .name = NULL };
	loff_t size = UBIFS_INO_NODE_SZ;

	if (!(ubifs_chk_flags & UBIFS_CHK_GEN))
		return 0;

	if (!S_ISDIR(dir->i_mode))
		return 0;

	lowest_dent_key(c, &key, dir->i_ino);
	while (1) {
		int err;

		dent = ubifs_tnc_next_ent(c, &key, &nm);
		if (IS_ERR(dent)) {
			err = PTR_ERR(dent);
			if (err == -ENOENT)
				break;
			return err;
		}

		nm.name = dent->name;
		nm.len = le16_to_cpu(dent->nlen);
		size += CALC_DENT_SIZE(nm.len);
		kfree(pdent);
		pdent = dent;
		key_read(c, &dent->key, &key);
	}

	kfree(pdent);

	if (i_size_read(dir) != size) {
		ubifs_err("bad directory dir %lu size %llu, "
			  "calculated %llu", dir->i_ino,
			  (unsigned long long)i_size_read(dir),
			  (unsigned long long)size);
		dump_stack();
		return -EINVAL;
	}

	return 0;
}

/**
 * dbg_check_key_order - make sure that colliding keys are properly ordered.
 * @c: UBIFS file-system description object
 * @zbr1: first zbranch
 * @zbr1: following zbranch
 *
 * In UBIFS indexing B-tree colliding keys has to be sorted in binary order of
 * names of the direntries/xentries which are referred by the keys. This
 * function reads direntries/xentries referred by @zbr1 and @zbr2 and makes
 * sure the name of direntry/xentry referred by @zbr1 is less than
 * direntry/xentry referred by @zbr2. Returns zero if this is true, %1 if not,
 * and a negative error code in case of failure.
 */
static int dbg_check_key_order(struct ubifs_info *c, struct ubifs_zbranch *zbr1,
			       struct ubifs_zbranch *zbr2)
{
	int err, nlen1, nlen2, cmp;
	struct ubifs_dent_node *dent1, *dent2;
	union ubifs_key key;

	ubifs_assert(!keys_cmp(c, &zbr1->key, &zbr2->key));
	dent1 = kmalloc(UBIFS_MAX_DENT_NODE_SZ, GFP_NOFS);
	if (!dent1)
		return -ENOMEM;
	dent2 = kmalloc(UBIFS_MAX_DENT_NODE_SZ, GFP_NOFS);
	if (!dent2) {
		err = -ENOMEM;
		goto out_free;
	}

	err = dbg_read_leaf_nolock(c, zbr1, dent1);
	if (err)
		goto out_free;
	err = ubifs_validate_entry(c, dent1);
	if (err)
		goto out_free;

	err = dbg_read_leaf_nolock(c, zbr2, dent2);
	if (err)
		goto out_free;
	err = ubifs_validate_entry(c, dent2);
	if (err)
		goto out_free;

	/* Make sure node keys are the same as in zbranch */
	err = 1;
	key_read(c, &dent1->key, &key);
	if (keys_cmp(c, &zbr1->key, &key)) {
		dbg_err("1st entry at %d:%d has key %s", zbr1->lnum,
			zbr1->offs, DBGKEY(&key));
		dbg_err("but it should have key %s according to tnc",
			DBGKEY(&zbr1->key));
			dbg_dump_node(c, dent1);
			goto out_free;
	}

	key_read(c, &dent2->key, &key);
	if (keys_cmp(c, &zbr2->key, &key)) {
		dbg_err("2nd entry at %d:%d has key %s", zbr1->lnum,
			zbr1->offs, DBGKEY(&key));
		dbg_err("but it should have key %s according to tnc",
			DBGKEY(&zbr2->key));
			dbg_dump_node(c, dent2);
			goto out_free;
	}

	nlen1 = le16_to_cpu(dent1->nlen);
	nlen2 = le16_to_cpu(dent2->nlen);

	cmp = memcmp(dent1->name, dent2->name, min_t(int, nlen1, nlen2));
	if (cmp < 0 || (cmp == 0 && nlen1 < nlen2)) {
		err = 0;
		goto out_free;
	}
	if (cmp == 0 && nlen1 == nlen2)
		dbg_err("2 xent/dent nodes with the same name");
	else
		dbg_err("bad order of colliding key %s",
			DBGKEY(&key));

	dbg_msg("first node at %d:%d\n", zbr1->lnum, zbr1->offs);
	dbg_dump_node(c, dent1);
	dbg_msg("second node at %d:%d\n", zbr2->lnum, zbr2->offs);
	dbg_dump_node(c, dent2);

out_free:
	kfree(dent2);
	kfree(dent1);
	return err;
}

/**
 * dbg_check_znode - check if znode is all right.
 * @c: UBIFS file-system description object
 * @zbr: zbranch which points to this znode
 *
 * This function makes sure that znode referred to by @zbr is all right.
 * Returns zero if it is, and %-EINVAL if it is not.
 */
static int dbg_check_znode(struct ubifs_info *c, struct ubifs_zbranch *zbr)
{
	struct ubifs_znode *znode = zbr->znode;
	struct ubifs_znode *zp = znode->parent;
	int n, err, cmp;

	if (znode->child_cnt <= 0 || znode->child_cnt > c->fanout) {
		err = 1;
		goto out;
	}
	if (znode->level < 0) {
		err = 2;
		goto out;
	}
	if (znode->iip < 0 || znode->iip >= c->fanout) {
		err = 3;
		goto out;
	}

	if (zbr->len == 0)
		/* Only dirty zbranch may have no on-flash nodes */
		if (!ubifs_zn_dirty(znode)) {
			err = 4;
			goto out;
		}

	if (ubifs_zn_dirty(znode)) {
		/*
		 * If znode is dirty, its parent has to be dirty as well. The
		 * order of the operation is important, so we have to have
		 * memory barriers.
		 */
		smp_mb();
		if (zp && !ubifs_zn_dirty(zp)) {
			/*
			 * The dirty flag is atomic and is cleared outside the
			 * TNC mutex, so znode's dirty flag may now have
			 * been cleared. The child is always cleared before the
			 * parent, so we just need to check again.
			 */
			smp_mb();
			if (ubifs_zn_dirty(znode)) {
				err = 5;
				goto out;
			}
		}
	}

	if (zp) {
		const union ubifs_key *min, *max;

		if (znode->level != zp->level - 1) {
			err = 6;
			goto out;
		}

		/* Make sure the 'parent' pointer in our znode is correct */
		err = ubifs_search_zbranch(c, zp, &zbr->key, &n);
		if (!err) {
			/* This zbranch does not exist in the parent */
			err = 7;
			goto out;
		}

		if (znode->iip >= zp->child_cnt) {
			err = 8;
			goto out;
		}

		if (znode->iip != n) {
			/* This may happen only in case of collisions */
			if (keys_cmp(c, &zp->zbranch[n].key,
				     &zp->zbranch[znode->iip].key)) {
				err = 9;
				goto out;
			}
			n = znode->iip;
		}

		/*
		 * Make sure that the first key in our znode is greater than or
		 * equal to the key in the pointing zbranch.
		 */
		min = &zbr->key;
		cmp = keys_cmp(c, min, &znode->zbranch[0].key);
		if (cmp == 1) {
			err = 10;
			goto out;
		}

		if (n + 1 < zp->child_cnt) {
			max = &zp->zbranch[n + 1].key;

			/*
			 * Make sure the last key in our znode is less or
			 * equivalent than the the key in zbranch which goes
			 * after our pointing zbranch.
			 */
			cmp = keys_cmp(c, max,
				&znode->zbranch[znode->child_cnt - 1].key);
			if (cmp == -1) {
				err = 11;
				goto out;
			}
		}
	} else {
		/* This may only be root znode */
		if (zbr != &c->zroot) {
			err = 12;
			goto out;
		}
	}

	/*
	 * Make sure that next key is greater or equivalent then the previous
	 * one.
	 */
	for (n = 1; n < znode->child_cnt; n++) {
		cmp = keys_cmp(c, &znode->zbranch[n - 1].key,
			       &znode->zbranch[n].key);
		if (cmp > 0) {
			err = 13;
			goto out;
		}
		if (cmp == 0) {
			/* This can only be keys with colliding hash */
			if (!is_hash_key(c, &znode->zbranch[n].key)) {
				err = 14;
				goto out;
			}

			if (znode->level != 0 || c->replaying)
				continue;

			/*
			 * Colliding keys should follow binary order of
			 * corresponding xentry/dentry names.
			 */
			err = dbg_check_key_order(c, &znode->zbranch[n - 1],
						  &znode->zbranch[n]);
			if (err < 0)
				return err;
			if (err) {
				err = 15;
				goto out;
			}
		}
	}

	for (n = 0; n < znode->child_cnt; n++) {
		if (!znode->zbranch[n].znode &&
		    (znode->zbranch[n].lnum == 0 ||
		     znode->zbranch[n].len == 0)) {
			err = 16;
			goto out;
		}

		if (znode->zbranch[n].lnum != 0 &&
		    znode->zbranch[n].len == 0) {
			err = 17;
			goto out;
		}

		if (znode->zbranch[n].lnum == 0 &&
		    znode->zbranch[n].len != 0) {
			err = 18;
			goto out;
		}

		if (znode->zbranch[n].lnum == 0 &&
		    znode->zbranch[n].offs != 0) {
			err = 19;
			goto out;
		}

		if (znode->level != 0 && znode->zbranch[n].znode)
			if (znode->zbranch[n].znode->parent != znode) {
				err = 20;
				goto out;
			}
	}

	return 0;

out:
	ubifs_err("failed, error %d", err);
	ubifs_msg("dump of the znode");
	dbg_dump_znode(c, znode);
	if (zp) {
		ubifs_msg("dump of the parent znode");
		dbg_dump_znode(c, zp);
	}
	dump_stack();
	return -EINVAL;
}

/**
 * dbg_check_tnc - check TNC tree.
 * @c: UBIFS file-system description object
 * @extra: do extra checks that are possible at start commit
 *
 * This function traverses whole TNC tree and checks every znode. Returns zero
 * if everything is all right and %-EINVAL if something is wrong with TNC.
 */
int dbg_check_tnc(struct ubifs_info *c, int extra)
{
	struct ubifs_znode *znode;
	long clean_cnt = 0, dirty_cnt = 0;
	int err, last;

	if (!(ubifs_chk_flags & UBIFS_CHK_TNC))
		return 0;

	ubifs_assert(mutex_is_locked(&c->tnc_mutex));
	if (!c->zroot.znode)
		return 0;

	znode = ubifs_tnc_postorder_first(c->zroot.znode);
	while (1) {
		struct ubifs_znode *prev;
		struct ubifs_zbranch *zbr;

		if (!znode->parent)
			zbr = &c->zroot;
		else
			zbr = &znode->parent->zbranch[znode->iip];

		err = dbg_check_znode(c, zbr);
		if (err)
			return err;

		if (extra) {
			if (ubifs_zn_dirty(znode))
				dirty_cnt += 1;
			else
				clean_cnt += 1;
		}

		prev = znode;
		znode = ubifs_tnc_postorder_next(znode);
		if (!znode)
			break;

		/*
		 * If the last key of this znode is equivalent to the first key
		 * of the next znode (collision), then check order of the keys.
		 */
		last = prev->child_cnt - 1;
		if (prev->level == 0 && znode->level == 0 && !c->replaying &&
		    !keys_cmp(c, &prev->zbranch[last].key,
			      &znode->zbranch[0].key)) {
			err = dbg_check_key_order(c, &prev->zbranch[last],
						  &znode->zbranch[0]);
			if (err < 0)
				return err;
			if (err) {
				ubifs_msg("first znode");
				dbg_dump_znode(c, prev);
				ubifs_msg("second znode");
				dbg_dump_znode(c, znode);
				return -EINVAL;
			}
		}
	}

	if (extra) {
		if (clean_cnt != atomic_long_read(&c->clean_zn_cnt)) {
			ubifs_err("incorrect clean_zn_cnt %ld, calculated %ld",
				  atomic_long_read(&c->clean_zn_cnt),
				  clean_cnt);
			return -EINVAL;
		}
		if (dirty_cnt != atomic_long_read(&c->dirty_zn_cnt)) {
			ubifs_err("incorrect dirty_zn_cnt %ld, calculated %ld",
				  atomic_long_read(&c->dirty_zn_cnt),
				  dirty_cnt);
			return -EINVAL;
		}
	}

	return 0;
}

static int dbg_add_size(struct ubifs_info *c, struct ubifs_znode *znode,
			void *priv)
{
	long long *idx_size = priv;
	int add;

	add = ubifs_idx_node_sz(c, znode->child_cnt);
	add = ALIGN(add, 8);
	*idx_size += add;
	return 0;
}

int dbg_check_idx_size(struct ubifs_info *c, long long idx_size)
{
	int err;
	long long calc = 0;

	if (!(ubifs_chk_flags & UBIFS_CHK_IDX_SZ))
		return 0;

	err = dbg_walk_index(c, NULL, dbg_add_size, &calc);
	if (err) {
		ubifs_err("error %d while walking the index", err);
		return err;
	}

	if (calc != idx_size) {
		ubifs_err("index size check failed");
		ubifs_err("calculated size is %lld, should be %lld",
			  calc, idx_size);
		dump_stack();
		return -EINVAL;
	}

	return 0;
}

static int invocation_cnt;

int dbg_force_in_the_gaps(void)
{
	if (!dbg_force_in_the_gaps_enabled)
		return 0;
	/* Force in-the-gaps every 8th commit */
	return !((invocation_cnt++) & 0x7);
}

#endif /* CONFIG_UBIFS_FS_DEBUG */

/* Failure mode for recovery testing */

#define chance(n, d) (simple_rand() <= (n) * 32768LL / (d))

struct failure_mode_info {
	struct list_head list;
	struct ubifs_info *c;
};

static LIST_HEAD(fmi_list);
static DEFINE_SPINLOCK(fmi_lock);

static unsigned int next;

static int simple_rand(void)
{
	if (next == 0)
		next = current->pid;
	next = next * 1103515245 + 12345;
	return (next >> 16) & 32767;
}

void dbg_failure_mode_registration(struct ubifs_info *c)
{
	struct failure_mode_info *fmi;

	fmi = kmalloc(sizeof(struct failure_mode_info), GFP_NOFS);
	if (!fmi) {
		dbg_err("Failed to register failure mode - no memory");
		return;
	}
	fmi->c = c;
	spin_lock(&fmi_lock);
	list_add_tail(&fmi->list, &fmi_list);
	spin_unlock(&fmi_lock);
}

void dbg_failure_mode_deregistration(struct ubifs_info *c)
{
	struct failure_mode_info *fmi, *tmp;

	spin_lock(&fmi_lock);
	list_for_each_entry_safe(fmi, tmp, &fmi_list, list)
		if (fmi->c == c) {
			list_del(&fmi->list);
			kfree(fmi);
		}
	spin_unlock(&fmi_lock);
}

static struct ubifs_info *dbg_find_info(struct ubi_volume_desc *desc)
{
	struct failure_mode_info *fmi;

	spin_lock(&fmi_lock);
	list_for_each_entry(fmi, &fmi_list, list)
		if (fmi->c->ubi == desc) {
			struct ubifs_info *c = fmi->c;

			spin_unlock(&fmi_lock);
			return c;
		}
	spin_unlock(&fmi_lock);
	return NULL;
}

static int in_failure_mode(struct ubi_volume_desc *desc)
{
	struct ubifs_info *c = dbg_find_info(desc);

	if (c)
		return c->failure_mode;
	return 0;
}

static int do_fail(struct ubi_volume_desc *desc, int lnum, int write)
{
	struct ubifs_info *c = dbg_find_info(desc);

	if (!c || !dbg_failure_mode)
		return 0;
	if (c->failure_mode)
		return 1;
	if (!c->fail_cnt) {
		/* First call - decide delay to failure */
		if (chance(1, 2)) {
			unsigned int delay = 1 << (simple_rand() >> 11);

			if (chance(1, 2)) {
				c->fail_delay = 1;
				c->fail_timeout = jiffies +
						  msecs_to_jiffies(delay);
				dbg_rcvry("failing after %ums", delay);
			} else {
				c->fail_delay = 2;
				c->fail_cnt_max = delay;
				dbg_rcvry("failing after %u calls", delay);
			}
		}
		c->fail_cnt += 1;
	}
	/* Determine if failure delay has expired */
	if (c->fail_delay == 1) {
		if (time_before(jiffies, c->fail_timeout))
			return 0;
	} else if (c->fail_delay == 2)
		if (c->fail_cnt++ < c->fail_cnt_max)
			return 0;
	if (lnum == UBIFS_SB_LNUM) {
		if (write) {
			if (chance(1, 2))
				return 0;
		} else if (chance(19, 20))
			return 0;
		dbg_rcvry("failing in super block LEB %d", lnum);
	} else if (lnum == UBIFS_MST_LNUM || lnum == UBIFS_MST_LNUM + 1) {
		if (chance(19, 20))
			return 0;
		dbg_rcvry("failing in master LEB %d", lnum);
	} else if (lnum >= UBIFS_LOG_LNUM && lnum <= c->log_last) {
		if (write) {
			if (chance(99, 100))
				return 0;
		} else if (chance(399, 400))
			return 0;
		dbg_rcvry("failing in log LEB %d", lnum);
	} else if (lnum >= c->lpt_first && lnum <= c->lpt_last) {
		if (write) {
			if (chance(7, 8))
				return 0;
		} else if (chance(19, 20))
			return 0;
		dbg_rcvry("failing in LPT LEB %d", lnum);
	} else if (lnum >= c->orph_first && lnum <= c->orph_last) {
		if (write) {
			if (chance(1, 2))
				return 0;
		} else if (chance(9, 10))
			return 0;
		dbg_rcvry("failing in orphan LEB %d", lnum);
	} else if (lnum == c->ihead_lnum) {
		if (chance(99, 100))
			return 0;
		dbg_rcvry("failing in index head LEB %d", lnum);
	} else if (c->jheads && lnum == c->jheads[GCHD].wbuf.lnum) {
		if (chance(9, 10))
			return 0;
		dbg_rcvry("failing in GC head LEB %d", lnum);
	} else if (write && !RB_EMPTY_ROOT(&c->buds) &&
		   !ubifs_search_bud(c, lnum)) {
		if (chance(19, 20))
			return 0;
		dbg_rcvry("failing in non-bud LEB %d", lnum);
	} else if (c->cmt_state == COMMIT_RUNNING_BACKGROUND ||
		   c->cmt_state == COMMIT_RUNNING_REQUIRED) {
		if (chance(999, 1000))
			return 0;
		dbg_rcvry("failing in bud LEB %d commit running", lnum);
	} else {
		if (chance(9999, 10000))
			return 0;
		dbg_rcvry("failing in bud LEB %d commit not running", lnum);
	}
	ubifs_err("*** SETTING FAILURE MODE ON (LEB %d) ***", lnum);
	c->failure_mode = 1;
	dump_stack();
	return 1;
}

static void cut_data(const void *buf, int len)
{
	int flen, i;
	unsigned char *p = (void *)buf;

	flen = (len * (long long)simple_rand()) >> 15;
	for (i = flen; i < len; i++)
		p[i] = 0xff;
}

int dbg_leb_read(struct ubi_volume_desc *desc, int lnum, char *buf, int offset,
		 int len, int check)
{
	if (in_failure_mode(desc))
		return -EIO;
	return ubi_leb_read(desc, lnum, buf, offset, len, check);
}

int dbg_leb_write(struct ubi_volume_desc *desc, int lnum, const void *buf,
		  int offset, int len, int dtype)
{
	int err;

	if (in_failure_mode(desc))
		return -EIO;
	if (do_fail(desc, lnum, 1))
		cut_data(buf, len);
	err = ubi_leb_write(desc, lnum, buf, offset, len, dtype);
	if (err)
		return err;
	if (in_failure_mode(desc))
		return -EIO;
	return 0;
}

int dbg_leb_change(struct ubi_volume_desc *desc, int lnum, const void *buf,
		   int len, int dtype)
{
	int err;

	if (do_fail(desc, lnum, 1))
		return -EIO;
	err = ubi_leb_change(desc, lnum, buf, len, dtype);
	if (err)
		return err;
	if (do_fail(desc, lnum, 1))
		return -EIO;
	return 0;
}

int dbg_leb_erase(struct ubi_volume_desc *desc, int lnum)
{
	int err;

	if (do_fail(desc, lnum, 0))
		return -EIO;
	err = ubi_leb_erase(desc, lnum);
	if (err)
		return err;
	if (do_fail(desc, lnum, 0))
		return -EIO;
	return 0;
}

int dbg_leb_unmap(struct ubi_volume_desc *desc, int lnum)
{
	int err;

	if (do_fail(desc, lnum, 0))
		return -EIO;
	err = ubi_leb_unmap(desc, lnum);
	if (err)
		return err;
	if (do_fail(desc, lnum, 0))
		return -EIO;
	return 0;
}

int dbg_is_mapped(struct ubi_volume_desc *desc, int lnum)
{
	if (in_failure_mode(desc))
		return -EIO;
	return ubi_is_mapped(desc, lnum);
}
