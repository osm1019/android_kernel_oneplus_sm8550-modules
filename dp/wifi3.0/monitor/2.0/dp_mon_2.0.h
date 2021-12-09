/*
 * Copyright (c) 2021 The Linux Foundation. All rights reserved.
 * Copyright (c) 2021-2022 Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _DP_MON_2_0_H_
#define _DP_MON_2_0_H_

#if !defined(DISABLE_MON_CONFIG)
#include <qdf_lock.h>
#include <dp_types.h>
#include <dp_mon.h>
#include <dp_mon_filter.h>
#include <dp_htt.h>
#include <dp_mon.h>
#include <dp_tx_mon_2.0.h>

#define DP_MON_RING_FILL_LEVEL_DEFAULT 2048
#define DP_MON_DATA_BUFFER_SIZE     2048
#define DP_MON_DESC_MAGIC 0xdeadabcd

/**
 * struct dp_mon_filter_be - Monitor TLV filter
 * @rx_tlv_filter: Rx MON TLV filter
 * @tx_tlv_filter: Tx MON TLV filter
 * @tx_valid: enable/disable Tx Mon TLV filter
 */
struct dp_mon_filter_be {
	struct dp_mon_filter rx_tlv_filter;
#ifdef QCA_MONITOR_2_0_SUPPORT
	struct htt_tx_ring_tlv_filter tx_tlv_filter;
#endif
	bool tx_valid;
};

/**
 * struct dp_mon_desc
 *
 * @buf_addr: virtual address
 * @paddr: physical address
 * @in_use: desc is in use
 * @unmapped: used to mark desc an unmapped if the corresponding
 * nbuf is already unmapped
 * @cookie: unique desc identifier
 * @magic: magic number to validate desc data
 */
struct dp_mon_desc {
	uint8_t *buf_addr;
	qdf_dma_addr_t paddr;
	uint8_t in_use:1,
		unmapped:1;
	uint32_t cookie;
	uint32_t magic;
};

/**
 * struct dp_mon_desc_list_elem_t
 * @next: Next pointer to form free list
 * @mon_desc: DP mon descriptor
 */
union dp_mon_desc_list_elem_t {
	union dp_mon_desc_list_elem_t *next;
	struct dp_mon_desc mon_desc;
};

/**
 * struct dp_mon_desc_pool - monitor desc pool
 * @pool_size: number of descriptor in the pool
 * @array: pointer to array of descriptor
 * @freelist: pointer to free descriptor list
 * @lock: Protection for the descriptor pool
 * @owner: owner for nbuf
 * @buf_size: Buffer size
 * @buf_alignment: Buffer alignment
 */
struct dp_mon_desc_pool {
	uint32_t pool_size;
	union dp_mon_desc_list_elem_t *array;
	union dp_mon_desc_list_elem_t *freelist;
	qdf_spinlock_t lock;
	uint8_t owner;
	uint16_t buf_size;
	uint8_t buf_alignment;
};

/**
 * struct dp_mon_pdev_be - BE specific monitor pdev object
 * @mon_pdev: monitor pdev structure
 * @filter_be: filters sent to fw
 * @tx_capture: pointer to tx capture function
 * @tx_stats: tx monitor drop stats
 */
struct dp_mon_pdev_be {
	struct dp_mon_pdev mon_pdev;
	struct dp_mon_filter_be **filter_be;
#ifdef WLAN_TX_PKT_CAPTURE_ENH_BE
	struct dp_pdev_tx_capture_be tx_capture_be;
#endif
	struct dp_tx_monitor_drop_stats tx_stats;
};

/**
 * struct dp_mon_soc_be - BE specific monitor soc
 * @mon_soc: Monitor soc structure
 * @tx_mon_buf_ring: TxMon replenish ring
 * @tx_mon_dst_ring: TxMon Destination ring
 * @tx_desc_mon: descriptor pool for tx mon src ring
 * @rx_desc_mon: descriptor pool for rx mon src ring
 * @rx_mon_ring_fill_level: rx mon ring refill level
 * @tx_mon_ring_fill_level: tx mon ring refill level
 * @tx_low_thresh_intrs: number of tx mon low threshold interrupts received
 * @rx_low_thresh_intrs: number of rx mon low threshold interrupts received
 */
struct dp_mon_soc_be {
	struct dp_mon_soc mon_soc;
	/* Source ring for Tx monitor */
	struct dp_srng tx_mon_buf_ring;
	struct dp_srng tx_mon_dst_ring[MAX_NUM_LMAC_HW];

	/* Sw descriptor pool for tx mon source ring */
	struct dp_mon_desc_pool tx_desc_mon;
	/* Sw descriptor pool for rx mon source ring */
	struct dp_mon_desc_pool rx_desc_mon;

	uint16_t rx_mon_ring_fill_level;
	uint16_t tx_mon_ring_fill_level;
	uint32_t tx_low_thresh_intrs;
	uint32_t rx_low_thresh_intrs;
};
#endif

/**
 * dp_mon_desc_pool_init() - Monitor descriptor pool init
 * @mon_desc_pool: mon desc pool
 * @pool_size
 *
 * Return: non-zero for failure, zero for success
 */
QDF_STATUS
dp_mon_desc_pool_init(struct dp_mon_desc_pool *mon_desc_pool,
		      uint32_t pool_size);

/*
 * dp_mon_desc_pool_deinit()- monitor descriptor pool deinit
 * @mon_desc_pool: mon desc pool
 *
 * Return: None
 *
 */
void dp_mon_desc_pool_deinit(struct dp_mon_desc_pool *mon_desc_pool);

/*
 * dp_mon_desc_pool_free()- monitor descriptor pool free
 * @mon_desc_pool: mon desc pool
 *
 * Return: None
 *
 */
void dp_mon_desc_pool_free(struct dp_mon_desc_pool *mon_desc_pool);

/**
 * dp_mon_desc_pool_alloc() - Monitor descriptor pool alloc
 * @mon_desc_pool: mon desc pool
 * @pool_size: Pool size
 *
 * Return: non-zero for failure, zero for success
 */
QDF_STATUS dp_mon_desc_pool_alloc(uint32_t pool_size,
				  struct dp_mon_desc_pool *mon_desc_pool);

/*
 * dp_mon_pool_frag_unmap_and_free() - free the mon desc frag called during
 *			    de-initialization of wifi module.
 *
 * @soc: DP soc handle
 * @mon_desc_pool: monitor descriptor pool pointer
 *
 * Return: None
 */
void dp_mon_pool_frag_unmap_and_free(struct dp_soc *dp_soc,
				     struct dp_mon_desc_pool *mon_desc_pool);

/*
 * dp_mon_buffers_replenish() - replenish monitor ring with nbufs
 *
 * @soc: core txrx main context
 * @dp_mon_srng: dp monitor circular ring
 * @mon_desc_pool: Pointer to free mon descriptor pool
 * @num_req_buffers: number of buffer to be replenished
 * @desc_list: list of descs if called from dp_rx_process
 *	       or NULL during dp rx initialization or out of buffer
 *	       interrupt.
 * @tail: tail of descs list
 *
 * Return: return success or failure
 */
QDF_STATUS dp_mon_buffers_replenish(struct dp_soc *dp_soc,
				struct dp_srng *dp_mon_srng,
				struct dp_mon_desc_pool *mon_desc_pool,
				uint32_t num_req_buffers,
				union dp_mon_desc_list_elem_t **desc_list,
				union dp_mon_desc_list_elem_t **tail);

/**
 * dp_mon_filter_show_filter_be() - Show the set filters
 * @mode: The filter modes
 * @tlv_filter: tlv filter
 */
void dp_mon_filter_show_filter_be(enum dp_mon_filter_mode mode,
				  struct dp_mon_filter_be *filter);

/**
 * dp_rx_add_to_free_desc_list() - Adds to a local free descriptor list
 *
 * @head: pointer to the head of local free list
 * @tail: pointer to the tail of local free list
 * @new: new descriptor that is added to the free list
 * @func_name: caller func name
 *
 * Return: void
 */
static inline
void __dp_mon_add_to_free_desc_list(union dp_mon_desc_list_elem_t **head,
				    union dp_mon_desc_list_elem_t **tail,
				    struct dp_mon_desc *new,
				    const char *func_name)
{
	qdf_assert(head && new);

	new->buf_addr = NULL;
	new->in_use = 0;

	((union dp_mon_desc_list_elem_t *)new)->next = *head;
	*head = (union dp_mon_desc_list_elem_t *)new;
	 /* reset tail if head->next is NULL */
	if (!*tail || !(*head)->next)
		*tail = *head;
}

#define dp_mon_add_to_free_desc_list(head, tail, new) \
	__dp_mon_add_to_free_desc_list(head, tail, new, __func__)

/*
 * dp_mon_add_desc_list_to_free_list() - append unused desc_list back to
 * freelist.
 *
 * @soc: core txrx main context
 * @local_desc_list: local desc list provided by the caller
 * @tail: attach the point to last desc of local desc list
 * @mon_desc_pool: monitor descriptor pool pointer
 */

void
dp_mon_add_desc_list_to_free_list(struct dp_soc *soc,
				  union dp_mon_desc_list_elem_t **local_desc_list,
				  union dp_mon_desc_list_elem_t **tail,
				  struct dp_mon_desc_pool *mon_desc_pool);
#endif /* _DP_MON_2_0_H_ */
