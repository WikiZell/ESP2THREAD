idf_component_register(
    SRCS "esp2thread_main.c" "factory_reset.c" "status_led.c"
    INCLUDE_DIRS "."
    REQUIRES
        esp_ot_br_server
        esp_rcp_update
        device_identity
        esp_driver_gpio
        esp_wifi
        mdns
        nvs_flash
        openthread
        spiffs
        thread_border_router
)
