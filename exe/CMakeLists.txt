add_executable(clitest CliTest.cpp)
target_link_libraries(clitest PRIVATE mylib)

# boost as a required dependency for mylib
find_package(Boost COMPONENTS program_options REQUIRED)
target_include_directories(clitest PRIVATE "${Boost_INCLUDE_DIR}")
target_link_libraries(clitest PRIVATE "${Boost_LIBRARIES}")