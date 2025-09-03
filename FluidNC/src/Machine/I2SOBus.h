// Copyright (c) 2021 -  Stefan de Bruijn
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#pragma once

#include "Configuration/Configurable.h"

#include <cstdint>

namespace Machine {
    class I2SOBus : public Configuration::Configurable {
        static const int32_t NUMBER_PINS = 32;

        uint32_t portData_;

    public:
        I2SOBus() = default;

        Pin _bck;
        Pin _data;
        Pin _ws;
        Pin _oe;

        int32_t _min_pulse_us = 2;

        void validate() override;
        void group(Configuration::HandlerBase& handler) override;

        void init();

        void write(int32_t index, int32_t high);
        void push();

        inline int32_t read(int32_t index) { return !!(portData_ & bitnum_to_mask(index)); }

        ~I2SOBus() = default;
    };
}
