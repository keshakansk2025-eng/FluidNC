#include "gtest/gtest.h"

int32_t main(int32_t argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    // if you plan to use GMock, replace the line above with
    // ::testing::InitGoogleMock(&argc, argv);

    if (RUN_ALL_TESTS()) {}

    // Always return zero-code and allow PlatformIO to parse results
    return 0;
}
