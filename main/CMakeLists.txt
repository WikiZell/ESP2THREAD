idf_component_register(
    SRCS "esp2thread_main.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_ot_br_server
        esp_rcp_update
        mdns
        nvs_flash
        openthread
        spiffs
        thread_border_router
)

