/*
 * This file is part of ccid-utils
 * Copyright (c) 2008 Gianni Tedesco <gianni@scaramanga.co.uk>
 * Released under the terms of the GNU GPL version 3
*/
#ifndef _CCID_H
#define _CCID_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "compiler.h"

/** \mainpage Chip Card Interface Device (CCID) Utilities.
  * \section intro Introduction.
  * blah blah...
  * \section code Code Overview
  *  -# First find a device.
  *  -# Then open it.
  *  -# Activate a chip card.
  *  -# Create a transaction buffer.
  *  -# Submit transactions to the chip card.
  *  -# Close connection to chip card device.
  */

/**
 * \defgroup g_libccid CCID Library
 * Generic top level functions of libccid.
 *
 * \defgroup g_ccid Chip Card Interface Device (CCID).
 * Represents a connection to a chip card device. Contains one or more
 * chipcard interfaces (slots or RF fields).
 *
 * \defgroup g_xfr CCID Transaction Buffer
 * Provides:
 *
 *  -# An appropriately sized send/receive buffer for a CCID.
 *  -# An interface for constructing requests.
 *  -# An interface for retreiving status words and retrieved data.
 *
 * \defgroup g_cci Chip Card Interface
 * Represents a slot or RF field in a chip card device and chip card (if one is
 * present).
 */

/** \ingroup g_ccid
 * Chip Card Interface Device
*/
typedef struct _ccid *ccid_t;

/** \ingroup g_cci
 * Chip Card
*/
typedef struct _cci *cci_t;

/** \ingroup g_xfr
 * Transaction Buffer
*/
typedef struct _xfr *xfr_t;

/** \ingroup g_ccid
 * Physical Chip Card Interface Device.
 *
 * A name referencing an unopened hardware device
*/
typedef struct libusb_device *ccidev_t;

_public ccidev_t *libccid_get_device_list(size_t *nmemb);
_public void libccid_free_device_list(ccidev_t *list);
_public ccidev_t libccid_device_by_address(uint8_t bus, uint8_t addr);
_public uint8_t libccid_device_bus(ccidev_t dev);
_public uint8_t libccid_device_addr(ccidev_t dev);

#define CCID_ERROR_IN_VALUE		1
#define CCID_ERROR_NO_MEM		2
#define CCID_ERROR_DEVICE_REMOVED	3
#define CCID_ERROR_BUS			4 /* error on device bus */
#define CCID_ERROR_NO_CARD		5
#define CCID_ERROR_CARD_IO		6 /* error on card bus */
#define CCID_ERROR_CARD_PROTO		7 /* unsupported protocol */
#define CCID_ERROR_CARD_TIMEOUT		8
#define CCID_ERROR_AUTH			9
#define CCID_ERROR_PIN_TIMEOUT		10 /* not implemented */

_public ccid_t ccid_probe(ccidev_t dev, const char *tracefile);
_public unsigned int ccid_num_slots(ccid_t ccid);
_public cci_t ccid_get_slot(ccid_t ccid, unsigned int i);
_public unsigned int ccid_num_fields(ccid_t ccid);
_public cci_t ccid_get_field(ccid_t ccid, unsigned int i);
_public void ccid_close(ccid_t ccid);
_public void ccid_log(ccid_t ccid, const char *fmt, ...) _printf(2, 3);
_public uint8_t ccid_bus(ccid_t ccid);
_public uint8_t ccid_addr(ccid_t ccid);
_public const char *ccid_name(ccid_t ccid);

_public unsigned int ccid_error(ccid_t ccid);

/* Transact xfr buffers */
_public xfr_t xfr_alloc(size_t txbuf, size_t rxbuf);
_public void xfr_reset(xfr_t xfr);
_public int xfr_tx_byte(xfr_t xfr, uint8_t byte);
_public int xfr_tx_buf(xfr_t xfr, const uint8_t *ptr, size_t len);

_public uint8_t xfr_rx_sw1(xfr_t xfr);
_public uint8_t xfr_rx_sw2(xfr_t xfr);
_public const uint8_t *xfr_rx_data(xfr_t xfr, size_t *len);

_public void xfr_free(xfr_t xfr);

/* Chip card interface */
_public ccid_t cci_ccid(cci_t cci);

_public int cci_power_off(cci_t cci);
_public int cci_transact(cci_t cci, xfr_t xfr);
_public unsigned int cci_error(cci_t cci);

/* contact interfaces only */
_public int cci_wait_for_card(cci_t cci);

/** \ingroup g_cci
 * Chip card is present in the slot, powered and clocked.
*/
#define CHIPCARD_ACTIVE		0x0
/** \ingroup g_cci
 * Chip card is present in the slot but not powered or clocked.
*/
#define CHIPCARD_PRESENT	0x1
/** \ingroup g_cci
 * Chip card is not present in the slot.
*/
#define CHIPCARD_NOT_PRESENT	0x2
/* contact interfaces only */
_public unsigned int cci_slot_status(cci_t cci);

/** \ingroup g_cci
 * There was an error while retrieving clock status.
*/
#define CHIPCARD_CLOCK_ERR	0x0
/** \ingroup g_cci
 * Clock is started.
*/
#define CHIPCARD_CLOCK_START	0x1
/** \ingroup g_cci
 * Clock signal is stopped low.
*/
#define CHIPCARD_CLOCK_STOP_L	0x2
/** \ingroup g_cci
 * Clock is stopped high.
*/
#define CHIPCARD_CLOCK_STOP_H	0x3
/** \ingroup g_cci
 * Clock is stopped in unknown state.
*/
#define CHIPCARD_CLOCK_STOP	0x4
_public unsigned int cci_clock_status(cci_t cci);

/** \ingroup g_cci
 * Automatically select chip card voltage.
*/
#define CHIPCARD_AUTO_VOLTAGE	0x0
/** \ingroup g_cci
 * 5 Volts.
*/
#define CHIPCARD_5V		0x1
/** \ingroup g_cci
 * 3 Volts.
*/
#define CHIPCARD_3V		0x2
/** \ingroup g_cci
 * 1.8 Volts.
*/
#define CHIPCARD_1_8V		0x3
_public const uint8_t *cci_power_on(cci_t cci, unsigned int voltage,
				size_t *atr_len);

/* -- Utility functions */
_public void hex_dump(const uint8_t *ptr, size_t len, size_t llen);
_public void hex_dumpf(FILE *f, const uint8_t *ptr, size_t len, size_t llen);

_public int ber_dump(const uint8_t *ptr, size_t len);
_public int ber_dumpf(FILE *f, const uint8_t *ptr, size_t len);

#endif /* _CCID_H */
