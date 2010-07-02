/*
 * This file is part of libbluray
 * Copyright (C) 2010  William Hahne
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef BDJO_PARSER_H_
#define BDJO_PARSER_H_

#include <stdint.h>

typedef enum {
    HD_1920_1080 = 1,
    HD_1280_720,
    SD,
    SD_50HZ_720_576,
    SD_60HZ_720_480,
    QHD_960_540
} BDJO_HAVI_DEVICE_CONFIG;

typedef enum {
    TITLE_UNBOUND_DISC_UNBOUND,
    TITLE_UNBOUND_DISC_BOUND,
    TITLE_BOUND_DISC_BOUND = 3
} BDJO_BINDING;

typedef enum {
    NOT_VISIBLE,
    VISIBLE_TO_APPS,
    VISIBLE_TO_APPS_AND_USERS = 3
} BDJO_VISIBILITY;

typedef struct {
    char default_font[6];
    BDJO_HAVI_DEVICE_CONFIG havi_config;
    uint32_t menu_call_mask;
    uint32_t title_search_mask;
} BDJO_TERMINAL_INFO;

typedef struct {
    uint8_t type;
    char ref_to_name[6];
    char language_code[4];
} BDJO_APP_CACHE_ENTRY;

typedef struct {
    uint32_t access_to_all_flag;
    uint32_t autostart_first_playlist;

    uint32_t playlist_count;
    char** playlist_file_names;
} BDJO_ACCESSIBLE_PLAYLISTS;

typedef struct {
    uint16_t profile;
    uint8_t major_version;
    uint8_t minor_version;
    uint8_t micro_version;
} BDJO_APP_PROFILE;

typedef struct {
    char language[3];
    char* name;
} BDJO_APP_NAME;

typedef struct {
    uint8_t control_code;
    uint32_t type;
    uint32_t organization_id;
    uint16_t application_id;

    // application descriptor
    uint8_t app_profiles_count;
    BDJO_APP_PROFILE* app_profiles;
    uint8_t priority;
    BDJO_BINDING binding;
    BDJO_VISIBILITY visibility;
    uint16_t app_name_count;
    BDJO_APP_NAME* app_names;
    char* icon_locator;
    uint16_t icon_flags;
    char* base_directory;
    char* classpath_extension;
    char* initial_class;
    uint8_t param_count;
    char** params;
} BDJO_APP_INFO;

typedef struct {
    int version;

    BDJO_TERMINAL_INFO terminal_info;

    uint8_t app_cache_count;
    BDJO_APP_CACHE_ENTRY* app_cache_entries;

    BDJO_ACCESSIBLE_PLAYLISTS accessible_playlists;

    uint8_t app_info_count;
    BDJO_APP_INFO* app_info_entries;

    uint32_t key_interest_table;
    char* file_access_info;
} BDJO;

BDJO* bdjo_read(const char* file);
void bdjo_destroy(BDJO* bdjo);

#endif /* BDJO_PARSER_H_ */
