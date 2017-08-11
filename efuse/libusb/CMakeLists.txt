aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_DIR)
set(SRC_LIST  ${SRC_DIR})

include_directories(include)
include_directories(/usr/include/libusb-1.0)
#add_definitions(-std=gnu99 -Wall -Werror -UNDEBUG)
add_definitions(-std=gnu99 -Wall)
add_executable(jolin_rtl ${SRC_LIST})
target_link_libraries(jolin_rtl pthread rt usb-1.0)
