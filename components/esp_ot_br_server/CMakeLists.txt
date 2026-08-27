set(upstream_dir "${CMAKE_CURRENT_LIST_DIR}/../../third_party/esp-thread-br/components/esp_ot_br_server")
set(requires mdns fatfs spiffs esp_eth nvs_flash freertos openthread esp_http_server protocol_examples_common esp_wifi esp_app_format esp_timer)

if("${IDF_VERSION_MAJOR}" VERSION_LESS "6")
    list(APPEND requires json)
endif()

idf_component_register(
    SRC_DIRS "${upstream_dir}/src"
    INCLUDE_DIRS "${upstream_dir}/include"
    PRIV_INCLUDE_DIRS "${upstream_dir}/private_include"
    PRIV_REQUIRES ${requires}
    EMBED_FILES "${upstream_dir}/favicon.ico"
    EMBED_TXTFILES "frontend/wifi_configuration.html"
)

spiffs_create_partition_image(web_storage ${CMAKE_CURRENT_SOURCE_DIR}/frontend FLASH_IN_PROJECT)
