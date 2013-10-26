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
 * Authors: Adrian Hunter
 *          Artem Bityutskiy (Битюцкий Артём)
 */

/*
 * This file contains miscelanious TNC-related functions shared betweend
 * different files. This file does not form any logically separate TNC
 * sub-system. The file was created because there is a lot of TNC code and
 * putting it all in one file would make that file too big and unreadable.
 * Nonetheless, this contains more or less "generic" code which may be used for
 * more then one specific task.
 */

#include "ubifs.h"

/**
 * ubifs_tnc_levelorder_next - next TNC tree element in levelorder traversal.
 * @zr: root of the subtree to traverse
 * @znode: previous znode
 *
 * This function implements levelorder TNC traversal. The LNC is ignored.
 * Returns the next element or %NULL if @znode is already the last one.
 */
struct ubifs_znode *ubifs_tnc_levelorder_next(struct ubifs_znode *zr,
					      struct ubifs_znode *znode)
{
	int level, iip, level_search = 0;
	struct ubifs_znode *zn;

	ubifs_assert(zr);

	if (unlikely(!znode))
		return zr;

	if (unlikely(znode == zr)) {
		if (znode->level == 0)
			return NULL;
		return ubifs_tnc_find_child(zr, 0);
	}

	level = znode->level;

	iip = znode->iip;
	while (1) {
		ubifs_assert(znode->level <= zr->level);

		/*
		 * First walk up until there is a znode with next branch to
		 * look at.
		 */
		while (znode->parent != zr && iip >= znode->parent->child_cnt) {
			znode = znode->parent;
			iip = znode->iip;
		}

		if (unlikely(znode->parent == zr &&
			     iip >= znode->parent->child_cnt)) {
			/* This level is done, switch to the lower one */
			level -= 1;
			if (level_search || level < 0)
				/*
				 * We were already looking for znode at lower
				 * level ('level_search'). As we are here
				 * again, it just does not exist. Or all levels
				 * were finished ('level < 0').
				 */
				return NULL;

			level_search = 1;
			iip = -1;
			znode = ubifs_tnc_find_child(zr, 0);
			ubifs_assert(znode);
		}

		/* Switch to the next index */
		zn = ubifs_tnc_find_child(znode->parent, iip + 1);
		if (!zn) {
			/* No more children to look at, we have walk up */
			iip = znode->parent->child_cnt;
			continue;
		}

		/* Walk back down to the level we came from ('level') */
		while (zn->level != level) {
			znode = zn;
			zn = ubifs_tnc_find_child(zn, 0);
			if (!zn) {
				/*
				 * This path is not too deep so it does not
				 * reach 'level'. Try next path.
				 */
				iip = znode->iip;
				break;
			}
		}

		if (zn) {
			ubifs_assert(zn->level >= 0);
			return zn;
		}
	}
}

/**
 * ubifs_search_zbranch - search znode branch.
 * @c: UBIFS file-system description object
 * @znode: znode to search in
 * @key: key to search for
 * @n: znode branch slot number is returned here
 *
 * This is a helper function which search branch with key @key in @znode using
 * binary search. The result of the search may be:
 *   o exact match, then %1 is returned, and the slot number of the branch is
 *     stored in @n;
 *   o no exact match, then %0 is returned and the slot number of the left
 *     closest branch is returned in @n; the slot if all keys in this znode are
 *     greater than @key, then %-1 is returned in @n.
 */
int ubifs_search_zbranch(const struct ubifs_info *c,
			 const struct ubifs_znode *znode,
			 const union ubifs_key *key, int *n)
{
	int beg = 0, end = znode->child_cnt, uninitialized_var(mid);
	int uninitialized_var(cmp);
	const struct ubifs_zbranch *zbr = &znode->zbranch[0];

	ubifs_assert(end > beg);

	while (end > beg) {
		mid = (beg + end) >> 1;
		cmp = keys_cmp(c, key, &zbr[mid].key);
		if (cmp > 0)
			beg = mid + 1;
		else if (cmp < 0)
			end = mid;
		else {
			*n = mid;
			return 1;
		}
	}

	*n = end - 1;

	/* The insert point is after *n */
	ubifs_assert(*n >= -1 && *n < znode->child_cnt);
	if (*n == -1)
		ubifs_assert(keys_cmp(c, key, &zbr[0].key) < 0);
	else
		ubifs_assert(keys_cmp(c, key, &zbr[*n].key) > 0);
	if (*n + 1 < znode->child_cnt)
		ubifs_assert(keys_cmp(c, key, &zbr[*n + 1].key) < 0);

	return 0;
}

/**
 * ubifs_tnc_postorder_first - find first znode to do postorder tree traversal.
 * @znode: znode to start at (root of the sub-tree to traverse)
 *
 * Find the lowest leftmost znode in a subtree of the TNC tree. The LNC is
 * ignored.
 */
struct ubifs_znode *ubifs_tnc_postorder_first(struct ubifs_znode *znode)
{
	if (unlikely(!znode))
		return NULL;

	while (znode->level > 0) {
		struct ubifs_znode *child;

		child = ubifs_tnc_find_child(znode, 0);
		if (!child)
			return znode;
		znode = child;
	}

	return znode;
}

/**
 * ubifs_tnc_postorder_next - next TNC tree element in postorder traversal.
 * @znode: previous znode
 *
 * This function implements postorder TNC traversal. The LNC is ignored.
 * Returns the next element or %NULL if @znode is already the last one.
 */
struct ubifs_znode *ubifs_tnc_postorder_next(struct ubifs_znode *znode)
{
	struct ubifs_znode *zn;

	ubifs_assert(znode);
	if (unlikely(!znode->parent))
		return NULL;

	/* Switch to the next index in the parent */
	zn = ubifs_tnc_find_child(znode->parent, znode->iip + 1);
	if (!zn)
		/* This is in fact the last child, return parent */
		return znode->parent;

	/* Go to the first znode in this new subtree */
	return ubifs_tnc_postorder_first(zn);
}

/**
 * ubifs_destroy_tnc_subtree - destroy all znodes connected to a subtree.
 * @znode: znode defining subtree to destroy
 *
 * This function destroys subtree of the TNC tree. Returns number of clean
 * znodes in the subtree.
 */
long ubifs_destroy_tnc_subtree(struct ubifs_znode *znode)
{
	struct ubifs_znode *zn = ubifs_tnc_postorder_first(znode);
	long clean_freed = 0;
	int n;

	ubifs_assert(zn);
	while (1) {
		for (n = 0; n < zn->child_cnt; n++) {
			if (!zn->zbranch[n].znode)
				continue;

			if (zn->level > 0 &&
			    !ubifs_zn_dirty(zn->zbranch[n].znode))
				clean_freed += 1;

			cond_resched();
			kfree(zn->zbranch[n].znode);
		}

		if (zn == znode) {
			if (!ubifs_zn_dirty(zn))
				clean_freed += 1;
			kfree(zn);
			return clean_freed;
		}

		zn = ubifs_tnc_postorder_next(zn);
	}
}
