/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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

/*
 * DOC: wlan_cm_roam_ucfg_api.h
 *
 * Implementation for roaming public ucfg API interfaces.
 */

#ifndef _WLAN_CM_ROAM_UCFG_API_H_
#define _WLAN_CM_ROAM_UCFG_API_H_

#include "wlan_cm_roam_api.h"

#ifdef ROAM_OFFLOAD_V1

/**
 * ucfg_user_space_enable_disable_rso  - Enable/Disable Roam Scan offload
 * to firmware.
 * @pdev: Pointer to pdev
 * @vdev_id: vdev id
 * @is_fast_roam_enabled: Value provided by userspace.
 * is_fast_roam_enabled - true: enable RSO if FastRoamEnabled ini is enabled
 *                        false: disable RSO
 *
 * Return: QDF_STATUS
 */
QDF_STATUS
ucfg_user_space_enable_disable_rso(struct wlan_objmgr_pdev *pdev,
				   uint8_t vdev_id,
				   const bool is_fast_roam_enabled);

/*
 * ucfg_cm_abort_roam_scan() -abort current roam scan cycle by roam scan
 * offload module.
 * @pdev: Pointer to pdev
 * vdev_id - vdev Identifier
 *
 * Return QDF_STATUS
 */
QDF_STATUS ucfg_cm_abort_roam_scan(struct wlan_objmgr_pdev *pdev,
				   uint8_t vdev_id);

/**
 * ucfg_cm_rso_init_deinit  - Init or Deinit roaming module at firmware
 * @pdev: Pointer to pdev
 * @vdev_id: vdev id
 * @enable: true: Send RSO init and RSO enable
 *          false: Send RSO stop
 *
 * Return: QDF_STATUS
 */
static inline QDF_STATUS
ucfg_cm_rso_init_deinit(struct wlan_objmgr_pdev *pdev, uint8_t vdev_id,
			bool enable)
{
	return wlan_cm_rso_init_deinit(pdev, vdev_id, enable);
}

/**
 * ucfg_cm_disable_rso  - Disable roam scan offload to firmware
 * @pdev: Pointer to pdev
 * @vdev_id: vdev id
 * @requestor: RSO disable requestor
 * @reason: Reason for RSO disable
 *
 * Return:  QDF_STATUS
 */
static inline
QDF_STATUS ucfg_cm_disable_rso(struct wlan_objmgr_pdev *pdev, uint32_t vdev_id,
			       enum wlan_cm_rso_control_requestor requestor,
			       uint8_t reason)
{
	return wlan_cm_disable_rso(pdev, vdev_id, requestor, reason);
}

/**
 * ucfg_cm_enable_rso  - Enable roam scan offload to firmware
 * @pdev: Pointer to pdev
 * @vdev_id: vdev id
 * @requestor: RSO disable requestor
 * @reason: Reason for RSO disable
 *
 * Return:  QDF_STATUS
 */
static inline
QDF_STATUS ucfg_cm_enable_rso(struct wlan_objmgr_pdev *pdev, uint32_t vdev_id,
			      enum wlan_cm_rso_control_requestor requestor,
			      uint8_t reason)
{
	return wlan_cm_enable_rso(pdev, vdev_id, requestor, reason);
}
#endif
#endif
