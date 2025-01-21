//============================================================================================================
//!
//! The software is protected by copyright of Oxford Technical Solutions.
//! Copyright (C) 2020 Oxford Technical Solutions - http://www.oxts.com
//!
//! Redistribution and use in source and binary forms, with or without modification, are only permitted with
//! the specific prior written permission of Oxford Technical Solutions.
//!
//! $LastChangedDate: 2019-10-23 15:17:01 +0100 (Wed, 23 Oct 2019) $
//! $LastChangedRevision: 28687 $
//! $LastChangedBy: mromanski $
//!
//! \file crc.hpp
//!
//! \brief CRC functions
//!
//============================================================================================================

#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>
#include <stdlib.h>

/** \brief CRC32 checksum based on a 0x04C11DB7 polynomial.
 *
 * \param data The data for which to calculate the checksum.
 * \param length The length of the data, in bytes.
 * \param seed The initial CRC value.
 *
 * \return The 32-bit checksum.
 */
uint32_t crc32(const uint8_t* data, size_t length, uint32_t seed = 0);

/** \brief CRC16 checksum based on a 0x1021 polynomial.
 *
 * \param data The data for which to calculate the checksum.
 * \param length The length of the data, in bytes.
 * \param seed The initial CRC value.
 *
 * \return The 16-bit checksum.
 */
uint16_t crc16_ccitt(const uint8_t* data, size_t length, uint16_t seed = 0);

#endif // _CRC_H_
