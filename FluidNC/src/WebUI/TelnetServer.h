// Copyright (c) 2014 Luc Lebosse. All rights reserved.
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#pragma once

#include "Module.h"  // Module
#include <queue>

#include "Settings.h"

#include <WiFi.h>

class TelnetClient;

namespace WebUI {
    class TelnetServer : public Module {
        static const int32_t DEFAULT_TELNET_STATE      = 1;
        static const int32_t DEFAULT_TELNETSERVER_PORT = 23;

        static const int32_t MAX_TELNET_PORT = 65001;
        static const int32_t MIN_TELNET_PORT = 1;

        static const int32_t MAX_TLNT_CLIENTS = 2;

        static const int32_t FLUSHTIMEOUT = 500;

    public:
        TelnetServer(const char* name) : Module(name) {}

        static uint16_t port() { return _port; }

        static std::queue<TelnetClient*> _disconnected;

        void init() override;
        void deinit() override;
        void poll() override;
        void status_report(Channel& out) override;

        ~TelnetServer();

    private:
        bool            _setupdone  = false;
        WiFiServer*     _wifiServer = nullptr;
        static uint16_t _port;
    };
}
