cmake_minimum_required(VERSION 3.16)

# Add linker flag globally (needed for duplicate symbols in ROM)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-zmuldefs")

set(JANOS_VERSION "1.6.9")
set(PROJECT_VER "${JANOS_VERSION}")

# Ensure IDF FatFs has exFAT enabled for all developers building this repo.
option(PROJECTZERO_PATCH_IDF_EXFAT "Patch IDF ffconf.h to enable exFAT support" ON)
if(PROJECTZERO_PATCH_IDF_EXFAT)
  include(${CMAKE_SOURCE_DIR}/tools/patch_idf_exfat.cmake)
endif()

option(PROJECTZERO_PATCH_IDF_LINENOISE_UTF8 "Patch IDF linenoise.c to allow UTF-8 bytes in console input" ON)
if(PROJECTZERO_PATCH_IDF_LINENOISE_UTF8)
  include(${CMAKE_SOURCE_DIR}/tools/patch_idf_linenoise_utf8.cmake)
endif()

# Bootstrap ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(projectZero)

# ===== MITM: allow lwIP to forward packets back out the same WiFi interface =====
idf_component_get_property(lwip_lib lwip COMPONENT_LIB)
target_compile_definitions(${lwip_lib} PRIVATE IP_FORWARD_ALLOW_TX_ON_RX_NETIF=1)

# ===== POST-BUILD: copy binaries to binaries-esp32c5 =====
set(EXPORT_DIR "${CMAKE_SOURCE_DIR}/binaries-esp32c5")
file(MAKE_DIRECTORY "${EXPORT_DIR}")

set(COPY_STAMP "${CMAKE_BINARY_DIR}/copy_bins.stamp")
set(GEN_APP_BIN_TARGET "gen_${CMAKE_PROJECT_NAME}_binary")
set(_copy_base_dep "")
if(TARGET ${GEN_APP_BIN_TARGET})
  set(_copy_base_dep ${GEN_APP_BIN_TARGET})
elseif(TARGET app)
  set(_copy_base_dep app)
else()
  message(WARNING "copy_firmware: no binary generation target found; disabling auto copy")
endif()

if(_copy_base_dep)
  set(COPY_DEPS ${_copy_base_dep})

  add_custom_command(
    OUTPUT "${COPY_STAMP}"
    COMMAND ${CMAKE_COMMAND}
            -DDEST="${EXPORT_DIR}"
            -DBLD="${CMAKE_BINARY_DIR}"
            -DAPP="${PROJECT_NAME}.bin"
            -P "${CMAKE_SOURCE_DIR}/post_build.cmake"
    COMMAND ${CMAKE_COMMAND} -E touch "${COPY_STAMP}"
    DEPENDS ${COPY_DEPS}
    COMMENT "Copying firmware binaries to ${EXPORT_DIR}"
  )

  add_custom_target(copy_firmware ALL DEPENDS "${COPY_STAMP}")
  add_dependencies(copy_firmware ${COPY_DEPS})
endif()cmake_minimum_required(VERSION 3.16)

# Add linker flag globally (needed for duplicate symbols in ROM)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-zmuldefs")

set(JANOS_VERSION "1.6.9")
set(PROJECT_VER "${JANOS_VERSION}")

# Ensure IDF FatFs has exFAT enabled for all developers building this repo.
option(PROJECTZERO_PATCH_IDF_EXFAT "Patch IDF ffconf.h to enable exFAT support" ON)
if(PROJECTZERO_PATCH_IDF_EXFAT)
  include(${CMAKE_SOURCE_DIR}/tools/patch_idf_exfat.cmake)
endif()

option(PROJECTZERO_PATCH_IDF_LINENOISE_UTF8 "Patch IDF linenoise.c to allow UTF-8 bytes in console input" ON)
if(PROJECTZERO_PATCH_IDF_LINENOISE_UTF8)
  include(${CMAKE_SOURCE_DIR}/tools/patch_idf_linenoise_utf8.cmake)
endif()

# Bootstrap ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(projectZero)

# ======================== ONLY EDIT THIS BLOCK BELOW ========================
idf_component_register(SRCS "oled_display.c" "main.c" "attack_handshake.c" "projectzero_gui.c"
                       INCLUDE_DIRS .
                       REQUIRES esp_timer esp_wifi esp_event esp_psram nvs_flash 
                                console driver fatfs mbedtls esp-tls esp_http_server 
                                esp_http_client esp_https_ota app_update cjson
                                lwip sdmmc espressif__led_strip bt
                                frame_analyzer hccapx_serializer pcap_serializer sniffer
                                nrf24_jammer zig_recon capture_gateway
                                esp_driver_uart esp_driver_gpio esp_driver_spi esp_driver_sdspi
                                esp_driver_i2c esp_lcd)
# ============================================================================

# Suppress strict null-pointer warnings from GCC 15 in IDF v6.0
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=nonnull)

# ===== MITM: allow lwIP to forward packets back out the same WiFi interface =====
idf_component_get_property(lwip_lib lwip COMPONENT_LIB)
target_compile_definitions(${lwip_lib} PRIVATE IP_FORWARD_ALLOW_TX_ON_RX_NETIF=1)

# ===== POST-BUILD: copy binaries to binaries-esp32c5 =====
set(EXPORT_DIR "${CMAKE_SOURCE_DIR}/binaries-esp32c5")
file(MAKE_DIRECTORY "${EXPORT_DIR}")

set(COPY_STAMP "${CMAKE_BINARY_DIR}/copy_bins.stamp")
set(GEN_APP_BIN_TARGET "gen_${CMAKE_PROJECT_NAME}_binary")
set(_copy_base_dep "")
if(TARGET ${GEN_APP_BIN_TARGET})
  set(_copy_base_dep ${GEN_APP_BIN_TARGET})
elseif(TARGET app)
  set(_copy_base_dep app)
else()
  message(WARNING "copy_firmware: no binary generation target found; disabling auto copy")
endif()

if(_copy_base_dep)
  set(COPY_DEPS ${_copy_base_dep})

  add_custom_command(
    OUTPUT "${COPY_STAMP}"
    COMMAND ${CMAKE_COMMAND}
            -DDEST="${EXPORT_DIR}"
            -DBLD="${CMAKE_BINARY_DIR}"
            -DAPP="${PROJECT_NAME}.bin"
            -P "${CMAKE_SOURCE_DIR}/post_build.cmake"
    COMMAND ${CMAKE_COMMAND} -E touch "${COPY_STAMP}"
    DEPENDS ${COPY_DEPS}
    COMMENT "Copying firmware binaries to ${EXPORT_DIR}"
  )

  add_custom_target(copy_firmware ALL DEPENDS "${COPY_STAMP}")
  add_dependencies(copy_firmware ${COPY_DEPS})
endif()
