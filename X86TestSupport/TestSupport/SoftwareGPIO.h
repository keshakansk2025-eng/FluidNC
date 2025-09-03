#pragma once

#include "Arduino.h"
#include "../FluidNC/src/Assertion.h"
#include <random>
#include "esp32-hal-gpio.h"

struct SoftwarePin {
    SoftwarePin() : callback(), argument(nullptr), mode(0), driverValue(false), padValue(false), pinMode(0) {}

    void (*callback)(void*, bool v);
    void*   argument;
    int32_t mode;

    bool    driverValue;
    bool    padValue;
    int32_t pinMode;

    void handleISR(bool nv) { callback(argument, nv); }

    void reset() {
        callback    = nullptr;
        argument    = nullptr;
        mode        = 0;
        driverValue = false;
        padValue    = false;
        pinMode     = 0;
    }

    void handlePadChangeWithHystesis(bool newval) {
        auto oldval = padValue;
        if (oldval != newval) {
            std::default_random_engine       generator;
            std::normal_distribution<double> distribution(5, 2);
            int32_t                          count = int(distribution(generator));

            // Bound it a bit
            if (count < 0) {
                count = 0;
            } else if (count > 8) {
                count = 8;
            }
            count = count * 2 + 1;  // make it odd.

            auto currentVal = oldval;
            for (int32_t i = 0; i < count; ++i) {
                currentVal = !currentVal;
                handlePadChange(currentVal);
            }
        }
    }

    void handlePadChange(bool newval) {
        auto oldval = padValue;
        if (oldval != newval) {
            switch (mode) {
                case RISING:
                    if (!oldval && newval) {
                        handleISR(true);
                    }
                    break;
                case FALLING:
                    if (oldval && !newval) {
                        handleISR(false);
                    }
                    break;
                case CHANGE:
                    if (oldval != newval) {
                        handleISR(!oldval && newval);
                    }
                    break;
            }
            padValue = newval;
        }
    }
};

typedef void (*HandleCircuit)(SoftwarePin* pins, int32_t pin, bool value);

class SoftwareGPIO {
    SoftwareGPIO() {}
    SoftwareGPIO(const SoftwareGPIO&) = delete;

    SoftwarePin   pins[256];
    HandleCircuit virtualCircuit         = nullptr;
    bool          circuitHandlesHystesis = false;

public:
    static SoftwareGPIO& instance() {
        static SoftwareGPIO instance_;
        return instance_;
    }

    static void reset(HandleCircuit circuit, bool circuitHandlesHystesis) {
        auto& inst = instance();
        for (int32_t i = 0; i < 256; ++i) {
            inst.pins[i].reset();
        }

        inst.virtualCircuit         = circuit;
        inst.circuitHandlesHystesis = circuitHandlesHystesis;
    }

    void setMode(int32_t index, int32_t mode) {
        auto& pin              = pins[index];
        auto  oldModeHasOutput = (pin.pinMode & OUTPUT) == OUTPUT;
        pin.pinMode            = mode;
        auto modeHasOutput     = (pin.pinMode & OUTPUT) == OUTPUT;

        if (modeHasOutput && !oldModeHasOutput) {
            if (virtualCircuit != nullptr) {
                virtualCircuit(pins, index, pin.driverValue);
            } else if (circuitHandlesHystesis) {
                pins[index].handlePadChange(pin.driverValue);
            }
        }
    }

    void writeOutput(int32_t index, bool value) {
        auto oldValue = pins[index].padValue;

        if ((pins[index].pinMode & OUTPUT) == OUTPUT) {
            if (virtualCircuit != nullptr) {
                virtualCircuit(pins, index, value);
            } else if (circuitHandlesHystesis) {
                pins[index].handlePadChange(value);
            } else {
                pins[index].handlePadChangeWithHystesis(value);
            }
        } else {
            pins[index].handlePadChange(value);
        }
    }

    bool read(int32_t index) const { return pins[index].padValue; }

    void attachISR(int32_t index, void (*callback)(void* arg, bool v), void* arg, int32_t mode) {
        auto& pin = pins[index];
        Assert(pin.mode == 0, "ISR mode should be 0 when attaching interrupt. Another interrupt is already attached.");

        pin.callback = callback;
        pin.argument = arg;
        pin.mode     = mode;
    }

    void detachISR(int32_t index) {
        auto& pin    = pins[index];
        pin.mode     = 0;
        pin.argument = nullptr;
    }
};
