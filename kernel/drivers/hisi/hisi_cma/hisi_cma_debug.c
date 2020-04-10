/* Copyright (c) Hisilicon Technologies Co., Ltd. 2001-2019. All rights reserved.
 * FileName: hisi_cma_debug.c
 * Description: This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation;
 * either version 2 of the License,
 * or (at your option) any later version.
 * Author:BaoPeng Feng
 * Create:2014-05-15
 */

#define pr_fmt(fmt) "hisi_cma_debug: " fmt
#include <linux/err.h>
#include <linux/mutex.h>
#include <linux/sizes.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/sched/task.h>
#include <linux/cma.h>
#include "cma.h"
#include "internal.h"
#include <linux/hisi/hisi_cma_debug.h>

static DEFINE_MUTEX(cma_list_lock);
static LIST_HEAD(cma_mem_list);

struct cma_alloc_mem_info {
	unsigned long pfn;
	u64 size;
	const char *cma_name;
	char task_name[TASK_COMM_LEN];
	pid_t pid;
	struct list_head list;
};

void dump_cma_mem_info(void)
{
	struct list_head *entry = NULL;
	struct cma_alloc_mem_info *cma_alloc_info = NULL;

	mutex_lock(&cma_list_lock);
	list_for_each(entry, &cma_mem_list) {
		cma_alloc_info = list_entry(entry,
			struct cma_alloc_mem_info, list);
		pr_err("%s[%s-%u](%s,pfn %lx ,size %llx)\n",
			__func__, cma_alloc_info->task_name,
			cma_alloc_info->pid, cma_alloc_info->cma_name,
			cma_alloc_info->pfn, cma_alloc_info->size);
	}
	mutex_unlock(&cma_list_lock);
}

void  record_cma_alloc_info(struct cma *cma,
		unsigned long pfn, size_t count)
{
	struct task_struct *task = NULL;
	struct cma_alloc_mem_info *cma_alloc_info = NULL;

	if (!cma || !cma->count || !count || !pfn_valid(pfn))
		return;

	cma_alloc_info = kzalloc(sizeof(struct cma_alloc_mem_info), GFP_KERNEL);
	if (!cma_alloc_info) {
		pr_err("%s: Failed to allocate cma alloc info mem\n");
		return;
	}

	task = current->group_leader;
	get_task_comm(cma_alloc_info->task_name, task);
	cma_alloc_info->pid = task_pid_nr(task);
	cma_alloc_info->cma_name = cma->name;
	cma_alloc_info->pfn = pfn;
	cma_alloc_info->size = count * PAGE_SIZE;

	mutex_lock(&cma_list_lock);
	list_add_tail(&cma_alloc_info->list, &cma_mem_list);
	mutex_unlock(&cma_list_lock);
}

void  record_cma_release_info(struct cma *cma,
		unsigned long pfn, size_t count)
{
	struct cma_alloc_mem_info *cma_alloc_info = NULL;
	struct list_head *pos = NULL, *next = NULL;

	if (!cma || !cma->count
		|| !count || !pfn_valid(pfn))
		return;

	mutex_lock(&cma_list_lock);
	if (list_empty(&cma_mem_list)) {
		mutex_unlock(&cma_list_lock);
		return;
	}

	list_for_each_safe(pos, next, &cma_mem_list) {
		cma_alloc_info = list_entry(pos,
					struct cma_alloc_mem_info, list);
		if (pfn == cma_alloc_info->pfn
			&& count == cma_alloc_info->size / PAGE_SIZE) {
			list_del(pos);
			kfree(cma_alloc_info);
		}
	}
	mutex_unlock(&cma_list_lock);
}

unsigned int cma_bitmap_used(struct cma *cma)
{
	unsigned int used;

	used = (unsigned int)bitmap_weight(cma->bitmap,
		(unsigned int)cma_bitmap_maxno(cma));

	return used << cma->order_per_bit;
}

unsigned long int cma_bitmap_maxchunk(struct cma *cma)
{
	unsigned long maxchunk = 0;
	unsigned long start, end = 0;
	unsigned long bitmap_maxno = 0;

	bitmap_maxno = cma_bitmap_maxno(cma);

	for (;;) {
		start = find_next_zero_bit(cma->bitmap, bitmap_maxno, end);
		if (start >= cma->count)
			break;
		end = find_next_bit(cma->bitmap, bitmap_maxno, start);
		maxchunk = max(end - start, maxchunk);
	}

	return (maxchunk << cma->order_per_bit);
}

void dump_cma_page_flags(unsigned long pfn)
{
	struct page *page = NULL;

	if (!pfn_valid(pfn))
		return;

	page = pfn_to_page(pfn);

	pr_info("[%s] PFN %lu Block %lu Flags %s%s%s%s%s%s%s%s%s%s%s%s\
		flags 0x%lx page_count(page) %d page_mapcount(page) %d\n",
			__func__,
			pfn,
			pfn >> pageblock_order,
			PageLocked(page)	? "K" : " ",
			PageError(page)		? "E" : " ",
			PageReferenced(page)	? "R" : " ",
			PageUptodate(page)	? "U" : " ",
			PageDirty(page)		? "D" : " ",
			PageLRU(page)		? "L" : " ",
			PageActive(page)	? "A" : " ",
			PageSlab(page)		? "S" : " ",
			PageWriteback(page)	? "W" : " ",
			PageCompound(page)	? "C" : " ",
			PageSwapCache(page)	? "B" : " ",
			PageMappedToDisk(page)	? "M" : " ",
			page->flags,
			page_count(page),
			PageSlab(page) ? 0 : page_mapcount(page));
}

void dump_cma_page(struct cma *cma, size_t count,
			unsigned long mask, unsigned long offset,
			unsigned long bitmap_maxno, unsigned long bitmap_count)
{
	unsigned int used = 0;
	unsigned long maxchunk = 0;
	unsigned long bitmap_no;
	unsigned long start = 0;
	struct page *page = NULL;
	unsigned long pfn, pfn_end;

	if (!cma || !cma->count || !count)
		return;

	mutex_lock(&cma->lock);
	used = cma_bitmap_used(cma);
	maxchunk = cma_bitmap_maxchunk(cma);
	pr_info("total %lu KB mask 0x%lx used %u KB\
		maxchunk %lu KB alloc %lu KB\n",
			cma->count << (PAGE_SHIFT - 10), mask,
			used << (PAGE_SHIFT - 10),
			maxchunk << (PAGE_SHIFT - 10),
			count << (PAGE_SHIFT - 10));

	for (;;) {
		bitmap_no = bitmap_find_next_zero_area_off(cma->bitmap,
			bitmap_maxno, start, bitmap_count, mask, offset);
		if (bitmap_no >= bitmap_maxno)
			break;

		pfn = cma->base_pfn + (bitmap_no << cma->order_per_bit);
		pfn_end = pfn + count;
		while (pfn < pfn_end) {
			if (!pfn_valid_within(pfn)) {
				pfn++;
				continue;
			}
			page = pfn_to_page(pfn);
			if (PageBuddy(page)) {
				pfn += 1 << page_order(page);
			} else {
				dump_cma_page_flags(pfn);
				break;
			}
		}

		/* try again with a bit different memory target */
		start = bitmap_no + mask + 1;
	}
	mutex_unlock(&cma->lock);
}

