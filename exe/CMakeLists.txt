add_executable(qc-tool CliTools.cpp)
target_link_libraries(qc-tool PRIVATE mylib)

# boost as a required dependency for mylib
find_package(Boost COMPONENTS program_options REQUIRED)
target_include_directories(qc-tool PRIVATE "${Boost_INCLUDE_DIR}")
target_link_libraries(qc-tool PRIVATE "${Boost_LIBRARIES}")