// Copyright (c) 2021 -	Stefan de Bruijn
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#pragma once

namespace Configuration {
    class ParseException {
        int32_t           _linenum = 0;
        const std::string _description;

    public:
        ParseException()                      = default;
        ParseException(const ParseException&) = default;

        ParseException(int32_t linenum, const char* description) : _linenum(linenum), _description(description) {}

        inline int32_t            LineNumber() const { return _linenum; }
        inline const std::string& What() const { return _description; }
    };
}
