# tests/CMakeLists.txt

find_package(GTest REQUIRED)

enable_testing()

# Unit tests
add_executable(unit_tests
    unit/WavefunctionTests.cpp
)
target_link_libraries(unit_tests
    PRIVATE core config solver visualization ui GTest::gtest_main
)
add_test(NAME UnitTests COMMAND unit_tests)

# Integration tests
add_executable(integration_tests
    integration/TunnelingTest.cpp
)
target_link_libraries(integration_tests
    PRIVATE core config solver visualization ui GTest::gtest_main
)
add_test(NAME IntegrationTests COMMAND integration_tests)