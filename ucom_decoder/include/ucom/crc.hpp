/*
   Copyright © 2025 Oxford Technical Solutions (OxTS)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

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
