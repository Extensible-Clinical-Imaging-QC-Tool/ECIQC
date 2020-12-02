add_executable(qctool CliTools.cpp)
target_link_libraries(qctool PRIVATE qctool_lib)

# boost as a required dependency for mylib
find_package(Boost COMPONENTS program_options REQUIRED)
target_include_directories(qctool PRIVATE "${Boost_INCLUDE_DIR}")
target_link_libraries(qctool PRIVATE "${Boost_LIBRARIES}")
