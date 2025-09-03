// Copyright (c) 2022 Mitch Bradley
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void tmc_spi_bus_setup();
void tmc_spi_transfer_data(uint8_t* out, int32_t out_bitlen, uint8_t* in, int32_t in_bitlen);
void tmc_spi_rw_reg(uint8_t cmd, uint32_t data, int32_t index);

#ifdef __cplusplus
}
#endif
