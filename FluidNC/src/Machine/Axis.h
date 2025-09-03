// Copyright (c) 2021 -  Stefan de Bruijn
// Copyright (c) 2021 -  Mitch Bradley
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#pragma once

#include "Configuration/Configurable.h"
// #include "Axes.h"
#include "Motor.h"
#include "Homing.h"

namespace MotorDrivers {
    class MotorDriver;
}

namespace Machine {
    class Axis : public Configuration::Configurable {
        int32_t _axis;
        int32_t motorsWithSwitches();

    public:
        Axis(int32_t currentAxis) : _axis(currentAxis) {
            for (int32_t i = 0; i < MAX_MOTORS_PER_AXIS; ++i) {
                _motors[i] = nullptr;
            }
        }

        static const int32_t MAX_MOTORS_PER_AXIS = 2;

        Motor*  _motors[MAX_MOTORS_PER_AXIS];
        Homing* _homing = nullptr;

        float _stepsPerMm   = 80.0f;
        float _maxRate      = 1000.0f;
        float _acceleration = 25.0f;
        float _maxTravel    = 1000.0f;
        bool  _softLimits   = false;

        // Configuration system helpers:
        void group(Configuration::HandlerBase& handler) override;
        void afterParse() override;

        // Checks if a motor matches this axis:
        bool hasMotor(const MotorDrivers::MotorDriver* const driver) const;
        bool hasDualMotor();

        float commonPulloff();
        float extraPulloff();

        void init();
        void config_motors();

        ~Axis();
    };
}
