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
 * This file implements VFS file and inode operations of regular files, device
 * nodes and symlinks as well as address space operations.
 *
 * UBIFS uses 2 page flags: PG_private and PG_checked. PG_private is set if the
 * page is dirty and is used for budgeting purposes - dirty pages should not be
 * budgeted. The PG_checked flag is set if full budgeting is required for the
 * page e.g., when it corresponds to a file hole or it is just beyond the file
 * size. The budgeting is done in 'ubifs_write_begin()', because it is OK to
 * fail in this function, and the budget is released in 'ubifs_write_end()'. So
 * the PG_private and PG_checked flags carry the information about how the page
 * was budgeted, to make it possible to release the budget properly.
 *
 * A thing to keep in mind: inode's 'i_mutex' is locked in most VFS operations
 * we implement. However, this is not true for '->writepage()', which might be
 * called with 'i_mutex' unlocked. For example, when pdflush is performing
 * write-back, it calls 'writepage()' with unlocked 'i_mutex', although the
 * inode has 'I_LOCK' flag in this case. At "normal" work-paths 'i_mutex' is
 * locked in '->writepage', e.g. in "sys_write -> alloc_pages -> direct reclaim
 * path'. So, in '->writepage()' we are only guaranteed that the page is
 * locked.
 *
 * Similarly, 'i_mutex' does not have to be locked in readpage(), e.g.,
 * readahead path does not have it locked ("sys_read -> generic_file_aio_read
 * -> ondemand_readahead -> readpage"). In case of readahead, 'I_LOCK' flag is
 * not set as well.
 *
 * This, for example means that there might be 2 concurrent '->writepage()'
 * calls for the same inode, but different inode dirty pages.
 */

#include "ubifs.h"
#include <linux/mount.h>

static int read_block(struct inode *inode, void *addr, unsigned int block,
		      struct ubifs_data_node *dn)
{
	struct ubifs_info *c = inode->i_sb->s_fs_info;
	int err, len, out_len;
	union ubifs_key key;
	unsigned int dlen;

	data_key_init(c, &key, inode->i_ino, block);
	err = ubifs_tnc_lookup(c, &key, dn);
	if (err) {
		if (err == -ENOENT)
			/* Not found, so it must be a hole */
			memset(addr, 0, UBIFS_BLOCK_SIZE);
		return err;
	}

	ubifs_assert(dn->ch.sqnum > ubifs_inode(inode)->creat_sqnum);

	len = le32_to_cpu(dn->size);
	if (len <= 0 || len > UBIFS_BLOCK_SIZE)
		goto dump;

	dlen = le32_to_cpu(dn->ch.len) - UBIFS_DATA_NODE_SZ;
	out_len = UBIFS_BLOCK_SIZE;
	err = ubifs_decompress(&dn->data, dlen, addr, &out_len,
			       le16_to_cpu(dn->compr_type));
	if (err || len != out_len)
		goto dump;

	/*
	 * Data length can be less than a full block, even for blocks that are
	 * not the last in the file (e.g., as a result of making a hole and
	 * appending data). Ensure that the remainder is zeroed out.
	 */
	if (len < UBIFS_BLOCK_SIZE)
		memset(addr + len, 0, UBIFS_BLOCK_SIZE - len);

	return 0;

dump:
	ubifs_err("bad data node (block %u, inode %lu)",
		  block, inode->i_ino);
	dbg_dump_node(c, dn);
	return -EINVAL;
}

/* TODO: remove compatibility stuff as late as possible */
#ifdef UBIFS_COMPAT_USE_OLD_PREPARE_WRITE
int ubifs_do_readpage(struct page *page)
#else
static int do_readpage(struct page *page)
#endif
{
	void *addr;
	int err = 0, i;
	unsigned int block, beyond;
	struct ubifs_data_node *dn;
	struct inode *inode = page->mapping->host;
	loff_t i_size =  i_size_read(inode);

	dbg_gen("ino %lu, pg %lu, i_size %lld, flags %#lx",
		inode->i_ino, page->index, i_size, page->flags);
	ubifs_assert(!PageChecked(page));
	ubifs_assert(!PagePrivate(page));

	addr = kmap(page);

	block = page->index << UBIFS_BLOCKS_PER_PAGE_SHIFT;
	beyond = (i_size + UBIFS_BLOCK_SIZE - 1) >> UBIFS_BLOCK_SHIFT;
	if (block >= beyond) {
		/* Reading beyond inode */
		SetPageChecked(page);
		memset(addr, 0, PAGE_CACHE_SIZE);
		goto out;
	}

	dn = kmalloc(UBIFS_MAX_DATA_NODE_SZ, GFP_NOFS);
	if (!dn) {
		err = -ENOMEM;
		goto error;
	}

	i = 0;
	while (1) {
		int ret;

		if (block >= beyond) {
			/* Reading beyond inode */
			err = -ENOENT;
			memset(addr, 0, UBIFS_BLOCK_SIZE);
		} else {
			ret = read_block(inode, addr, block, dn);
			if (ret) {
				err = ret;
				if (err != -ENOENT)
					break;
			}
		}
		if (++i >= UBIFS_BLOCKS_PER_PAGE)
			break;
		block += 1;
		addr += UBIFS_BLOCK_SIZE;
	}
	if (err) {
		if (err == -ENOENT) {
			/* Not found, so it must be a hole */
			SetPageChecked(page);
			dbg_gen("hole");
			goto out_free;
		}
		ubifs_err("cannot read page %lu of inode %lu, error %d",
			  page->index, inode->i_ino, err);
		goto error;
	}

out_free:
	kfree(dn);
out:
	SetPageUptodate(page);
	ClearPageError(page);
	flush_dcache_page(page);
	kunmap(page);
	return 0;

error:
	kfree(dn);
	ClearPageUptodate(page);
	SetPageError(page);
	flush_dcache_page(page);
	kunmap(page);
	return err;
}

/* TODO: remove compatibility stuff as late as possible */
#ifndef UBIFS_COMPAT_USE_OLD_PREPARE_WRITE

static int ubifs_write_begin(struct file *file, struct address_space *mapping,
			     loff_t pos, unsigned len, unsigned flags,
			     struct page **pagep, void **fsdata)
{
	struct inode *inode = mapping->host;
	struct ubifs_info *c = inode->i_sb->s_fs_info;
	pgoff_t index = pos >> PAGE_CACHE_SHIFT;
	struct ubifs_budget_req req = { .new_page = 1 };
	loff_t i_size =  i_size_read(inode);
	int uninitialized_var(err);
	struct page *page;

	ubifs_assert(!(inode->i_sb->s_flags & MS_RDONLY));

	if (unlikely(c->ro_media))
		return -EROFS;

	/*
	 * We are about to have a page of data written and we have to budget for
	 * this. The very important point here is that we have to budget before
	 * locking the page, because budgeting may force write-back, which
	 * would wait on locked pages and deadlock if we had the page locked.
	 *
	 * At this point we do not know anything about the page of data we are
	 * going to change, so assume the biggest budget (i.e., assume that
	 * this is a new page of data and it does not override an older page of
	 * data in the inode). Later the budget will be amended if this is not
	 * true.
	 */
	if (pos + len > i_size)
		/*
		 * We are writing beyond the file which means we are going to
		 * change inode size and make the inode dirty. And in turn,
		 * this means we have to budget for making the inode dirty.
		 *
		 * Note, if the inode is already dirty,
		 * 'ubifs_budget_inode_op()' will not allocate any budget,
		 * but will just lock the @budg_mutex of the inode to prevent
		 * it from becoming clean before we have changed its size,
		 * which is going to happen in 'ubifs_write_end()'.
		 */
		err = ubifs_budget_inode_op(c, inode, &req);
	else
		/*
		 * The inode is not going to be marked as dirty by this write
		 * operation, do not budget for this.
		 */
		err = ubifs_budget_space(c, &req);
	if (unlikely(err))
		return err;

	page = __grab_cache_page(mapping, index);
	if (unlikely(!page)) {
		err = -ENOMEM;
		goto out_release;
	}

	if (!PageUptodate(page)) {
		/*
		 * The page is not loaded from the flash and has to be loaded
		 * unless we are writing all of it.
		 */
		if (!(pos & PAGE_CACHE_MASK) && len == PAGE_CACHE_SIZE)
			/*
			 * Set the PG_checked flag to make the further code
			 * assume the page is new.
			 */
			SetPageChecked(page);
		else {
			err = do_readpage(page);
			if (err)
				goto out_unlock;
		}

		SetPageUptodate(page);
		ClearPageError(page);
	}

	if (PagePrivate(page))
		/*
		 * The page is dirty, which means it was budgeted twice:
		 *   o first time the budget was allocated by the task which
		 *     made the page dirty and set the PG_private flag;
		 *   o and then we budgeted for it for the second time at the
		 *     very beginning of this function.
		 *
		 * So what we have to do is to release the page budget we
		 * allocated.
		 *
		 * Note, the page write operation may change the inode length,
		 * which makes it dirty and means the budget should be
		 * allocated. This was done above in the "pos + len > i_size"
		 * case. If this was done, we do not free the the inode budget,
		 * because we cannot as we are really going to mark it dirty in
		 * the 'ubifs_write_end()' function.
		 */
		ubifs_release_new_page_budget(c);
	else if (!PageChecked(page))
		/*
		 * The page is not new, which means we are changing the page
		 * which already exists on the media. This means that changing
		 * the page does not make the amount of indexing information
		 * larger, and this part of the budget which we have already
		 * acquired may be released.
		 */
		ubifs_convert_page_budget(c);

	*pagep = page;
	return 0;

out_unlock:
	unlock_page(page);
	page_cache_release(page);
out_release:
	if (pos + len > i_size)
		ubifs_cancel_ino_op(c, inode, &req);
	else
		ubifs_release_budget(c, &req);
	return err;
}

static int ubifs_write_end(struct file *file, struct address_space *mapping,
			   loff_t pos, unsigned len, unsigned copied,
			   struct page *page, void *fsdata)
{
	struct inode *inode = mapping->host;
	struct ubifs_inode *ui = ubifs_inode(inode);
	struct ubifs_info *c = inode->i_sb->s_fs_info;
	loff_t i_size =  i_size_read(inode);

	dbg_gen("ino %lu, pos %llu, pg %lu, len %u, copied %d, i_size %lld",
		inode->i_ino, pos, page->index, len, copied, i_size);

	if (unlikely(copied < len && len == PAGE_CACHE_SIZE)) {
		/*
		 * VFS copied less data to the page that it intended and
		 * declared in its '->write_begin()' call via the @len
		 * argument. If the page was not up-to-date, and @len was
		 * @PAGE_CACHE_SIZE, the 'ubifs_write_begin()' function did
		 * not load it from the media (for optimization reasons). This
		 * means that part of the page contains garbage. So read the
		 * page now.
		 */
		dbg_gen("copied %d instead of %d, read page and repeat",
			copied, len);

		if (pos + len > i_size)
			/* See a comment below about this hacky unlock */
			mutex_unlock(&ui->budg_mutex);

		copied = do_readpage(page);

		/*
		 * Return 0 to force VFS to repeat the whole operation, or the
		 * error code if 'do_readpage()' failed.
		 */
		goto out;
	}

	if (!PagePrivate(page)) {
		SetPagePrivate(page);
		atomic_long_inc(&c->dirty_pg_cnt);
		__set_page_dirty_nobuffers(page);
	}

	if (pos + len > i_size) {
		i_size_write(inode, pos + len);

		/*
		 * Note, we do not set @I_DIRTY_PAGES (which means that the
		 * inode has dirty pages), this has been done in
		 * '__set_page_dirty_nobuffers()'.
		 */
		mark_inode_dirty_sync(inode);

		/*
		 * The inode has been marked dirty, unlock it. This is a bit
		 * hacky because normally we would have to call
		 * 'ubifs_release_ino_dirty()'. But we know there is nothing
		 * to release because page's budget will be released in
		 * 'ubifs_write_page()' and inode's budget will be released in
		 * 'ubifs_write_inode()', so just unlock the inode here for
		 * optimization.
		 */
		mutex_unlock(&ui->budg_mutex);
	}

out:
	unlock_page(page);
	page_cache_release(page);
	return copied;
}

#endif /* UBIFS_COMPAT_USE_OLD_PREPARE_WRITE */

static int ubifs_readpage(struct file *file, struct page *page)
{
	do_readpage(page);
	unlock_page(page);
	return 0;
}

/**
 * release_existing_page_budget - release budget of an existing page.
 * @c: UBIFS file-system description object
 *
 * This is a helper function which releases budget corresponding to the budget
 * of changing one one page of data which already exists on the flash media.
 *
 * This function was not moved to "budget.c" because there is only one user.
 */
static void release_existing_page_budget(struct ubifs_info *c)
{
	struct ubifs_budget_req req = { .dd_growth = c->page_budget};

	ubifs_release_budget(c, &req);
}

static int do_writepage(struct page *page, int len)
{
	int err = 0, i, blen;
	unsigned int block;
	void *addr;
	union ubifs_key key;
	struct inode *inode = page->mapping->host;
	struct ubifs_info *c = inode->i_sb->s_fs_info;

	/* Update radix tree tags */
	set_page_writeback(page);

	addr = kmap(page);

	block = page->index << UBIFS_BLOCKS_PER_PAGE_SHIFT;
	i = 0;
	while (len) {
		blen = min_t(int, len, UBIFS_BLOCK_SIZE);
		data_key_init(c, &key, inode->i_ino, block);
		err = ubifs_jnl_write_data(c, inode, &key, addr, blen);
		if (err)
			break;
		if (++i >= UBIFS_BLOCKS_PER_PAGE)
			break;
		block += 1;
		addr += blen;
		len -= blen;
	}
	if (err) {
		SetPageError(page);
		ubifs_err("cannot write page %lu of inode %lu, error %d",
			  page->index, inode->i_ino, err);
		ubifs_ro_mode(c, err);
	}

	ubifs_assert(PagePrivate(page));
	if (PageChecked(page))
		ubifs_release_new_page_budget(c);
	else
		release_existing_page_budget(c);

	atomic_long_dec(&c->dirty_pg_cnt);
	ClearPagePrivate(page);
	ClearPageChecked(page);

	kunmap(page);
	unlock_page(page);
	end_page_writeback(page);

	return err;
}

static int ubifs_writepage(struct page *page, struct writeback_control *wbc)
{
	struct inode *inode = page->mapping->host;
	loff_t i_size =  i_size_read(inode);
	pgoff_t end_index = i_size >> PAGE_CACHE_SHIFT;
	int len;
	void *kaddr;

	dbg_gen("ino %lu, pg %lu, pg flags %#lx",
		inode->i_ino, page->index, page->flags);
	ubifs_assert(PagePrivate(page));

	/* Is the page fully inside i_size? */
	if (page->index < end_index)
		return do_writepage(page, PAGE_CACHE_SIZE);

	/* Is the page fully outside i_size? (truncate in progress) */
	len = i_size & (PAGE_CACHE_SIZE - 1);
	if (page->index >= end_index + 1 || !len) {
		unlock_page(page);
		return 0;
	}

	/*
	 * The page straddles i_size. It must be zeroed out on each and every
	 * writepage invocation because it may be mmapped. "A file is mapped
	 * in multiples of the page size. For a file that is not a multiple of
	 * the page size, the remaining memory is zeroed when mapped, and
	 * writes to that region are not written out to the file."
	 */
	kaddr = kmap_atomic(page, KM_USER0);
	memset(kaddr + len, 0, PAGE_CACHE_SIZE - len);
	flush_dcache_page(page);
	kunmap_atomic(kaddr, KM_USER0);

	return do_writepage(page, len);
}

static int ubifs_trunc(struct inode *inode, loff_t new_size)
{
	loff_t old_size;
	int err;

	dbg_gen("ino %lu, size %lld -> %lld",
		inode->i_ino, inode->i_size, new_size);
	old_size = inode->i_size;

	err = vmtruncate(inode, new_size);
	if (err)
		return err;

	if (new_size < old_size) {
		struct ubifs_info *c = inode->i_sb->s_fs_info;
		int offset = new_size & (UBIFS_BLOCK_SIZE - 1);

		if (offset) {
			pgoff_t index = new_size >> PAGE_CACHE_SHIFT;
			struct page *page;

			page = find_lock_page(inode->i_mapping, index);
			if (page) {
				if (PageDirty(page)) {
					ubifs_assert(PagePrivate(page));

					clear_page_dirty_for_io(page);
					if (UBIFS_BLOCKS_PER_PAGE_SHIFT)
						offset = new_size &
							 (PAGE_CACHE_SIZE - 1);
					err = do_writepage(page, offset);
					page_cache_release(page);
					if (err)
						return err;
					/*
					 * We could now tell ubifs_jnl_truncate
					 * not to read the last block.
					 */
				} else {
					/*
					 * We could 'kmap()' the page and
					 * pass the data to ubifs_jnl_truncate
					 * to save it from having to read it.
					 */
					unlock_page(page);
					page_cache_release(page);
				}
			}
		}
		err = ubifs_jnl_truncate(c, inode->i_ino, old_size, new_size);
		if (err)
			return err;
	}

	return 0;
}

int ubifs_setattr(struct dentry *dentry, struct iattr *attr)
{
	unsigned int ia_valid = attr->ia_valid;
	struct inode *inode = dentry->d_inode;
	struct ubifs_info *c = inode->i_sb->s_fs_info;
	struct ubifs_budget_req req;
	int truncation, err = 0;

	dbg_gen("ino %lu, ia_valid %#x", inode->i_ino, ia_valid);
	err = inode_change_ok(inode, attr);
	if (err)
		return err;

	memset(&req, 0, sizeof(struct ubifs_budget_req));

	/*
	 * If this is truncation, and we do not truncate on a block boundary,
	 * budget for changing one data block, because the last block will be
	 * re-written.
	 */
	truncation = (ia_valid & ATTR_SIZE) && attr->ia_size != inode->i_size;
	if (truncation && attr->ia_size < inode->i_size &&
	    (attr->ia_size & (UBIFS_BLOCK_SIZE - 1)))
		req.dirtied_page = 1;

	err = ubifs_budget_inode_op(c, inode, &req);
	if (err)
		return err;

	if (truncation) {
		err = ubifs_trunc(inode, attr->ia_size);
		if (err) {
			ubifs_cancel_ino_op(c, inode, &req);
			return err;
		}

		inode->i_mtime = inode->i_ctime = ubifs_current_time(inode);
	}

	if (ia_valid & ATTR_UID)
		inode->i_uid = attr->ia_uid;
	if (ia_valid & ATTR_GID)
		inode->i_gid = attr->ia_gid;
	if (ia_valid & ATTR_ATIME)
		inode->i_atime = timespec_trunc(attr->ia_atime,
						inode->i_sb->s_time_gran);
	if (ia_valid & ATTR_MTIME)
		inode->i_mtime = timespec_trunc(attr->ia_mtime,
						inode->i_sb->s_time_gran);
	if (ia_valid & ATTR_CTIME)
		inode->i_ctime = timespec_trunc(attr->ia_ctime,
						inode->i_sb->s_time_gran);
	if (ia_valid & ATTR_MODE) {
		umode_t mode = attr->ia_mode;

		if (!in_group_p(inode->i_gid) && !capable(CAP_FSETID))
			mode &= ~S_ISGID;
		inode->i_mode = mode;
	}

	mark_inode_dirty_sync(inode);
	ubifs_release_ino_dirty(c, inode, &req);

	if (req.dirtied_page) {
		/*
		 * Truncation code does not make the reenacted page dirty, it
		 * just changes it on journal level, so we have to release page
		 * change budget.
		 */
		memset(&req, 0, sizeof(struct ubifs_budget_req));
		req.dd_growth = c->page_budget;
		ubifs_release_budget(c, &req);
	}

	if (IS_SYNC(inode))
		err = write_inode_now(inode, 1);

	return err;
}

static void ubifs_invalidatepage(struct page *page, unsigned long offset)
{
	struct inode *inode = page->mapping->host;
	struct ubifs_info *c = inode->i_sb->s_fs_info;
	struct ubifs_budget_req req;

	ubifs_assert(PagePrivate(page));
	if (offset)
		/* Partial page remains dirty */
		return;

	memset(&req, 0, sizeof(struct ubifs_budget_req));
	if (PageChecked(page)) {
		req.new_page = 1;
		req.idx_growth = -1;
		req.data_growth = c->page_budget;
	} else
		req.dd_growth = c->page_budget;
	ubifs_release_budget(c, &req);

	atomic_long_dec(&c->dirty_pg_cnt);
	ClearPagePrivate(page);
	ClearPageChecked(page);
}

static void *ubifs_follow_link(struct dentry *dentry, struct nameidata *nd)
{
	struct ubifs_inode *ui = ubifs_inode(dentry->d_inode);

	nd_set_link(nd, ui->data);
	return NULL;
}

int ubifs_fsync(struct file *file, struct dentry *dentry, int datasync)
{
	struct inode *inode = dentry->d_inode;
	struct ubifs_info *c = inode->i_sb->s_fs_info;
	int err;

	dbg_gen("syncing inode %lu", inode->i_ino);

	/* Synchronize the inode and dirty pages */
	err = write_inode_now(inode, 1);
	if (err)
		return err;

	/*
	 * Some data related to this inode may still sit in a write-buffer.
	 * Flush them.
	 */
	err = ubifs_sync_wbufs_by_inodes(c, &inode, 1);
	if (err)
		return err;

	return 0;
}

/**
 * mctime_update_needed - check if mtime or ctime update is needed.
 * @inode: the inode to do the check for
 * @now: current time
 *
 * This helper function checks if the inode mtime/ctime should be updated or
 * not. If current values of the time-stamps are within the UBIFS inode time
 * granularity, they are not updated. This is an optimization.
 */
static inline int mctime_update_needed(struct inode *inode,
				       struct timespec *now)
{
	if (!timespec_equal(&inode->i_mtime, now) ||
	    !timespec_equal(&inode->i_ctime, now))
		return 1;
	return 0;
}

/**
 * update_ctime - update mtime and ctime of an inode.
 * @c: UBIFS file-system description object
 * @inode: inode to update
 *
 * This function updates mtime and ctime of the inode if it is not equivalent to
 * current time. Returns zero in case of success and a negative error code in
 * case of failure.
 */
static int update_mctime(struct ubifs_info *c, struct inode *inode)
{
	struct timespec now = ubifs_current_time(inode);

	if (mctime_update_needed(inode, &now)) {
		struct ubifs_budget_req req;
		int err;

		memset(&req, 0, sizeof(struct ubifs_budget_req));
		err = ubifs_budget_inode_op(c, inode, &req);
		if (err)
			return err;

		inode->i_mtime = inode->i_ctime = now;
		mark_inode_dirty_sync(inode);
		mutex_unlock(&ubifs_inode(inode)->budg_mutex);
	}

	return 0;
}

static ssize_t ubifs_write(struct file *file, const char __user *buf,
			   size_t len, loff_t *ppos)
{
	int err;
	ssize_t ret;
	struct inode *inode = file->f_mapping->host;
	struct ubifs_info *c = inode->i_sb->s_fs_info;

	err = update_mctime(c, inode);
	if (err)
		return err;

	ret = do_sync_write(file, buf, len, ppos);
	if (ret < 0)
		return ret;

	if (ret > 0 && IS_SYNC(inode)) {
		err = ubifs_sync_wbufs_by_inodes(c, &inode, 1);
		if (err)
			return err;
	}

	return ret;
}

static ssize_t ubifs_aio_write(struct kiocb *iocb, const struct iovec *iov,
			       unsigned long nr_segs, loff_t pos)
{
	int err;
	ssize_t ret;
	struct inode *inode = iocb->ki_filp->f_mapping->host;
	struct ubifs_info *c = inode->i_sb->s_fs_info;

	err = update_mctime(c, inode);
	if (err)
		return err;

	ret = generic_file_aio_write(iocb, iov, nr_segs, pos);
	if (ret < 0)
		return ret;

	if (ret > 0 && IS_SYNC(inode)) {
		err = ubifs_sync_wbufs_by_inodes(c, &inode, 1);
		if (err)
			return err;
	}

	return ret;
}

static int ubifs_set_page_dirty(struct page *page)
{
	int ret;

	ret = __set_page_dirty_nobuffers(page);
	/*
	 * An attempt to dirty a page without budgeting for it - should not
	 * happen.
	 */
	ubifs_assert(ret == 0);
	return ret;
}

static int ubifs_releasepage(struct page *page, gfp_t unused_gfp_flags)
{
	/*
	 * An attempt to release a dirty page without budgeting for it - should
	 * not happen.
	 */
	if (PageWriteback(page))
		return 0;
	ubifs_assert(PagePrivate(page));
	ubifs_assert(0);
	ClearPagePrivate(page);
	ClearPageChecked(page);
	return 1;
}

#ifndef UBIFS_COMPAT_NO_SHARED_MMAP

/*
 * mmap()d file has taken write protection fault and is being made
 * writable. UBIFS must ensure page is budgeted for.
 */
static int ubifs_vm_page_mkwrite(struct vm_area_struct *vma, struct page *page)
{
	struct inode *inode = vma->vm_file->f_path.dentry->d_inode;
	struct ubifs_info *c = inode->i_sb->s_fs_info;
	struct timespec now = ubifs_current_time(inode);
	struct ubifs_budget_req req = { .new_page = 1 };
	int err, update_time;

	dbg_gen("ino %lu, pg %lu, i_size %lld",	inode->i_ino, page->index,
		i_size_read(inode));
	ubifs_assert(!(inode->i_sb->s_flags & MS_RDONLY));

	if (unlikely(c->ro_media))
		return -EROFS;

	/*
	 * We have not locked @page so far so we may budget for changing the
	 * page. Note, we cannot do this after we locked the page, because
	 * budgeting may cause write-back which would cause deadlock.
	 *
	 * At the moment we do not know whether the page is dirty or not, so we
	 * assume that it is not and budget for a new page. We could look at
	 * the @PG_private flag and figure this out, but we may race with write
	 * back and the page state may change by the time we lock it, so this
	 * would need additional care. We do not bother with this at the
	 * moment, although it might be good idea to do. Instead, we allocate
	 * budget for a new page and amend it later on if the page was in fact
	 * dirty.
	 *
	 * The budgeting-related logic of this function is similar to what we
	 * do in 'ubifs_write_begin()' and 'ubifs_write_end()'. Glance there
	 * for more comments.
	 */
	if (mctime_update_needed(inode, &now)) {
		/*
		 * We have to change inode time stamp which requires extra
		 * budgeting.
		 */
		update_time = 1;
		err = ubifs_budget_inode_op(c, inode, &req);
	} else {
		update_time = 0;
		err = ubifs_budget_space(c, &req);
	}
	if (unlikely(err)) {
		if (err == -ENOSPC)
			ubifs_warn("out of space for mmapped file "
				   "(inode number %lu)", inode->i_ino);
		return err;
	}

	lock_page(page);
	if (unlikely(page->mapping != inode->i_mapping ||
		     page_offset(page) > i_size_read(inode))) {
		/* Page got truncated out from underneath us */
		err = -EINVAL;
		goto out_unlock;
	}

	if (PagePrivate(page))
		ubifs_release_new_page_budget(c);
	else {
		if (!PageChecked(page))
			ubifs_convert_page_budget(c);
		SetPagePrivate(page);
		atomic_long_inc(&c->dirty_pg_cnt);
		__set_page_dirty_nobuffers(page);
	}

	if (update_time) {
		inode->i_mtime = inode->i_ctime = now;
		mark_inode_dirty_sync(inode);
		mutex_unlock(&ubifs_inode(inode)->budg_mutex);
	}

	unlock_page(page);
	return 0;

out_unlock:
	unlock_page(page);
	if (update_time)
		ubifs_cancel_ino_op(c, inode, &req);
	else
		ubifs_release_budget(c, &req);
	return err;
}

struct vm_operations_struct ubifs_file_vm_ops = {
#if (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,22))
	.nopage       = filemap_nopage,
#else
	.fault        = filemap_fault,
#endif
	.page_mkwrite = ubifs_vm_page_mkwrite,
};

static int ubifs_file_mmap(struct file *file, struct vm_area_struct *vma)
{
	int err;

	/* 'generic_file_mmap()' takes care of NOMMU case */
	err = generic_file_mmap(file, vma);
	if (err)
		return err;
	vma->vm_ops = &ubifs_file_vm_ops;
	return 0;
}
#endif

struct address_space_operations ubifs_file_address_operations = {
	.readpage       = ubifs_readpage,
	.writepage      = ubifs_writepage,
/* TODO: remove compatibility stuff as late as possible */
#ifdef UBIFS_COMPAT_USE_OLD_PREPARE_WRITE
	.prepare_write  = ubifs_prepare_write,
	.commit_write   = ubifs_commit_write,
#else
	.write_begin    = ubifs_write_begin,
	.write_end      = ubifs_write_end,
#endif
	.invalidatepage = ubifs_invalidatepage,
	.set_page_dirty = ubifs_set_page_dirty,
	.releasepage    = ubifs_releasepage,
};

struct inode_operations ubifs_file_inode_operations = {
	.setattr     = ubifs_setattr,
	.getattr     = ubifs_getattr,
#ifdef CONFIG_UBIFS_FS_XATTR
	.setxattr    = ubifs_setxattr,
	.getxattr    = ubifs_getxattr,
	.listxattr   = ubifs_listxattr,
	.removexattr = ubifs_removexattr,
#endif
};

struct inode_operations ubifs_symlink_inode_operations = {
	.readlink    = generic_readlink,
	.follow_link = ubifs_follow_link,
	.setattr     = ubifs_setattr,
	.getattr     = ubifs_getattr,
};

struct file_operations ubifs_file_operations = {
	.llseek         = generic_file_llseek,
	.read           = do_sync_read,
	.write          = ubifs_write,
	.aio_read       = generic_file_aio_read,
	.aio_write      = ubifs_aio_write,
	.mmap           = ubifs_file_mmap,
	.fsync          = ubifs_fsync,
	.unlocked_ioctl = ubifs_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl   = ubifs_compat_ioctl,
#endif
};
