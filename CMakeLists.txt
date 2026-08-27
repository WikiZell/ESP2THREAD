cmake_minimum_required(VERSION 3.16)

set(ESP_THREAD_BR_ROOT "${CMAKE_CURRENT_LIST_DIR}/third_party/esp-thread-br")
set(EXTRA_COMPONENT_DIRS
    "${ESP_THREAD_BR_ROOT}/components/esp_rcp_update"
)

set(PROJECT_VER "0.1.0")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
idf_build_set_property(MINIMAL_BUILD ON)
project(esp2thread)
