// Copyright 2022 Mitch Bradley
//
// Interface to the ESP32 alarm timer for step timing
// Uses the timer_ll API from ESP-IDF v4.4.1

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/timer_types_legacy.h"
#include "hal/timer_ll.h"
#include "esp_intr_alloc.h"
#include "esp_clk_tree.h"
// #include "soc/timer_periph.h"

static const uint32_t fTimers = 80000000;  // the frequency of ESP32 timers

static bool (*timer_isr_callback)(void);

static void IRAM_ATTR timer_isr(void* arg) {
#if 0
    uint32_t intr_status = timer_ll_get_intr_status(&TIMERG0);

    if (intr_status & TIMER_LL_EVENT_ALARM(TIMER_0) {
        // Note: when alarm event happens, the alarm will be disabled automatically by hardware
        timer_ll_clear_intr_status(timer->hal.dev, TIMER_LL_EVENT_ALARM(timer->timer_id));
#endif
    // esp_intr_alloc_intrstatus() takes care of filtering based on the interrupt status register
    timer_ll_clear_intr_status(&TIMERG0, TIMER_0);

    if (timer_isr_callback()) {
        // We could just pass the result of timer_isr_callback() as
        // the argument to timer_ll_set_alarm_enable(), but the
        // enable is automatically cleared when the alarm occurs,
        // so setting it to false is redundant.  Writing the
        // device register is much slower than a branch, so
        // this way of doing it is the most efficient.
        timer_ll_enable_alarm(&TIMERG0, TIMER_0, true);
    }
}

// Possibly-unnecessary optimization to avoid rewriting the alarm value
static uint32_t old_ticks = 0xffffffff;

static void IRAM_ATTR set_counter_value(uint64_t ticks) {
    // From    gptimer_set_raw_count(timer->timer_handle, ticks);
    // from    timer_hal_set_counter_value(&timer->hal, value);
    // - `timer_ll_set_reload_value()` will only indicate the `reload_value`
    // - `timer_ll_set_reload_value()` + ``timer_ll_trigger_soft_reload()` can update the HW counter value by software
    // Therefore, after updating the HW counter value, we need to restore the previous `reload_value`.
    // Attention: The following process should be protected by a lock in the driver layer.
    // save current reload value
    uint64_t old_reload = timer_ll_get_reload_value(&TIMERG0, TIMER_0);
    timer_ll_set_reload_value(&TIMERG0, TIMER_0, ticks);
    timer_ll_trigger_soft_reload(&TIMERG0, TIMER_0);
    // restore the previous reload value
    timer_ll_set_reload_value(&TIMERG0, TIMER_0, old_reload);
}

void IRAM_ATTR stepTimerSetTicks(uint32_t ticks) {
    if (ticks != old_ticks) {
        set_counter_value(ticks);
        old_ticks = ticks;
    }
}

void IRAM_ATTR stepTimerStart() {
    stepTimerSetTicks(10ULL);  // Interrupt soon to start the stepping
    // gptimer_start(timer->timer_handle);
    timer_ll_enable_alarm(&TIMERG0, TIMER_0, true);
    // Note here, if the alarm target is set very close to the current counter value
    // an alarm interrupt may be triggered very quickly after we start the timer
    timer_ll_enable_counter(&TIMERG0, TIMER_0, true);
}

void IRAM_ATTR stepTimerStop() {
    // From   gptimer_stop(timer->timer_handle);
    timer_ll_enable_counter(&TIMERG0, TIMER_0, false);
    timer_ll_enable_alarm(&TIMERG0, TIMER_0, false);
}

void stepTimerInit(uint32_t frequency, bool (*callback)(void)) {
    // esp_clk_tree_enable_src() is a no-op for most ESP32 variants except for P4
    // esp_clk_tree_enable_src((soc_module_clk_t)SOC_GPTIMER_CLKS, true);
    //    gptimer_select_periph_clock(timer, SOC_GPTIMER_CLKS, config->resolution_hz)

    // gptimer_stop()
    timer_ll_enable_counter(&TIMERG0, TIMER_0, false);
    timer_ll_enable_alarm(&TIMERG0, TIMER_0, false);

    set_counter_value(0);
    timer_ll_set_count_direction(&TIMERG0, TIMER_0, GPTIMER_COUNT_UP);

    // interrupt register is shared by all timers in the same group
    timer_ll_enable_intr(&TIMERG0, TIMER_LL_EVENT_ALARM(TIMER_0), false);  // disable interrupt
    timer_ll_clear_intr_status(&TIMERG0, TIMER_LL_EVENT_ALARM(TIMER_0));   // clear pending interrupt event

    intr_handle_t intr;
    //    esp_intr_alloc_intrstatus(soc_timg_gptimer_signals[TIMER_GROUP_0][TIMER_0].irq_id,
    esp_intr_alloc_intrstatus(ETS_TG0_T0_LEVEL_INTR_SOURCE,
                              ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_LEVEL3,
                              (uint32_t)timer_ll_get_intr_status_reg(&TIMERG0),
                              TIMER_LL_EVENT_ALARM(TIMER_0),
                              timer_isr,
                              nullptr,  // Argument to ISR callback
                              &intr     // Return handle for later deallocation
    );

    // gptimer_enable(..);
    esp_intr_enable(intr);

    // gptimer_start(timer->timer_handle);
    timer_ll_enable_alarm(&TIMERG0, TIMER_0, true);
}

#ifdef __cplusplus
}
#endif
