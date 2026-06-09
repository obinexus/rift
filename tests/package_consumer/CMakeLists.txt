cmake_minimum_required(VERSION 3.16)
project(RIFTPackageConsumer C)

find_package(RIFT CONFIG REQUIRED)

add_executable(rift_package_consumer main.c)
target_link_libraries(rift_package_consumer PRIVATE
    RIFT::rift
    RIFT::riftlang
    RIFT::rifttest
)
