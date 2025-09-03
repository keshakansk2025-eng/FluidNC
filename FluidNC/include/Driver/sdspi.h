#include <system_error>

bool sd_init_slot(uint32_t freq_hz, int32_t cs_pin, int32_t cd_pin = -1, int32_t wp_pin = -1);
void sd_unmount();
void sd_deinit_slot();

std::error_code sd_mount(int32_t max_files = 1);
