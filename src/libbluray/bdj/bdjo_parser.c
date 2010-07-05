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

#include "bdjo_parser.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/bits.h"

/* Documentation: HD Cookbook
 * https://hdcookbook.dev.java.net/
 */


int get_version(const uint8_t* str)
{
    if (strcmp((const char*) str, "0100") != 0)
        return 100;
    else if (strcmp((const char*) str, "0200") != 0)
        return 200;
    else
        return BDJ_ERROR;
}

// use when string is already allocated, out should be length + 1
void get_string(BITBUFFER* buf, char* out, uint32_t length)
{
    bb_read_bytes(buf, (uint8_t*)out, length);
    out[length] = 0; // add null termination
}

void make_string(BITBUFFER* buf, char** out, uint32_t length)
{
    *out = malloc(length + 1);
    bb_read_bytes(buf, (uint8_t*)*out, length);
    (*out)[length] = 0; // add null termination
}

int parse_terminal_info(BITBUFFER* buf, BDJO_TERMINAL_INFO* info)
{
    // skip length specifier
    bb_seek(buf, 32, SEEK_CUR);

    get_string(buf, info->default_font, 5);
    DEBUG(DBG_BDJ, "[bdj] BDJO > Terminal Info > Default Font: %s\n", info->default_font);

    info->havi_config = bb_read(buf, 4);
    DEBUG(DBG_BDJ, "[bdj] BDJO > Terminal Info > HAVi Config: %X\n", info->havi_config);

    info->menu_call_mask = bb_read(buf, 1);
    DEBUG(DBG_BDJ, "[bdj] BDJO > Terminal Info > Menu Call Mask: %X\n", info->menu_call_mask);

    info->title_search_mask = bb_read(buf, 1);
    DEBUG(DBG_BDJ, "[bdj] BDJO > Terminal Info > Title Search Mask: %X\n", info->title_search_mask);

    bb_seek(buf, 34, SEEK_CUR);

    return 0;
}

int parse_app_cache_info(BITBUFFER* buf, BDJO* bdjo)
{
    // skip length specifier
    bb_seek(buf, 32, SEEK_CUR);

    bdjo->app_cache_count = bb_read(buf, 8);
    DEBUG(DBG_BDJ, "[bdj] BDJO > App Cache Info > %d entries\n", bdjo->app_cache_count);

    // skip padding
    bb_seek(buf, 8, SEEK_CUR);

    bdjo->app_cache_entries = malloc(sizeof(BDJO_APP_CACHE_ENTRY)*bdjo->app_cache_count);

    for (int i = 0; i < bdjo->app_cache_count; i++) {
        BDJO_APP_CACHE_ENTRY* entry = &bdjo->app_cache_entries[i];

        entry->type = bb_read(buf, 8);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Cache Info > %d: Type: %d\n", i, entry->type);

        get_string(buf, entry->ref_to_name, 5);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Cache Info > %d: Ref to Name: %s\n", i, entry->ref_to_name);

        get_string(buf, entry->language_code, 3);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Cache Info > %d: Language Code: %s\n", i, entry->language_code);

        // skip padding
        bb_seek(buf, 24, SEEK_CUR);
    }

    return 0;
}

int parse_accessible_playlists(BITBUFFER* buf, BDJO_ACCESSIBLE_PLAYLISTS* acc_pl)
{
    // skip length specifier
    bb_seek(buf, 32, SEEK_CUR);

    acc_pl->playlist_count = bb_read(buf, 11);
    DEBUG(DBG_BDJ, "[bdj] BDJO > Accessible Playlists > %d entries\n", acc_pl->playlist_count);

    acc_pl->access_to_all_flag = bb_read(buf, 1);
    DEBUG(DBG_BDJ, "[bdj] BDJO > Accessible Playlists > Access to All Flag: %d\n", acc_pl->access_to_all_flag);

    acc_pl->autostart_first_playlist = bb_read(buf, 1);
    DEBUG(DBG_BDJ, "[bdj] BDJO > Accessible Playlists > Autostart First Playlist: %d\n", acc_pl->autostart_first_playlist);

    // skip padding
    bb_seek(buf, 19, SEEK_CUR);

    char** pls = malloc(sizeof(char*)*acc_pl->playlist_count);
    for (unsigned int i = 0; i < acc_pl->playlist_count; i++) {
        make_string(buf, &pls[i], 5);
        DEBUG(DBG_BDJ, "[bdj] BDJO > Accessible Playlists > %d: Playlist File Name: %s\n", i, pls[i]);

        // skip padding
        bb_seek(buf, 8, SEEK_CUR);
    }
    acc_pl->playlist_file_names = pls;

    return 0;
}

int parse_app_management_table(BITBUFFER* buf, BDJO* bdjo)
{
    // skip length specifier
    bb_seek(buf, 32, SEEK_CUR);

    bdjo->app_info_count = bb_read(buf, 8);
    DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d entries\n", bdjo->app_info_count);

    // skip padding
    bb_seek(buf, 8, SEEK_CUR);

    bdjo->app_info_entries = malloc(sizeof(BDJO_APP_INFO)*bdjo->app_info_count);
    int i;
    for (i = 0; i < bdjo->app_info_count; i++) {
        BDJO_APP_INFO* entry = &bdjo->app_info_entries[i];

        entry->control_code = bb_read(buf, 8);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Control Code: %d\n", i, entry->control_code);

        entry->type = bb_read(buf, 4);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Type: %d\n", i, entry->type);

        // skip padding
        bb_seek(buf, 4, SEEK_CUR);

        entry->organization_id = bb_read(buf, 32);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Organization ID: %d\n", i, entry->organization_id);

        entry->application_id = bb_read(buf, 16);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Application ID: %d\n", i, entry->application_id);

        // skip padding
        bb_seek(buf, 80, SEEK_CUR);

        entry->app_profiles_count = bb_read(buf, 4);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Profile Count: %d\n", i, entry->app_profiles_count);

        // skip padding
        bb_seek(buf, 12, SEEK_CUR);

        entry->app_profiles = malloc(sizeof(BDJO_APP_PROFILE)*entry->app_profiles_count);

        for (int j = 0; j < entry->app_profiles_count; j++) {
            BDJO_APP_PROFILE* profile = &entry->app_profiles[j];

            profile->profile = bb_read(buf, 16);
            DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: %d: Profile: %d\n", i, j, profile->profile);

            profile->major_version = bb_read(buf, 8);
            DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: %d: Major Version: %d\n", i, j, profile->major_version);

            profile->minor_version = bb_read(buf, 8);
            DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: %d: Minor Version: %d\n", i, j, profile->minor_version);

            profile->micro_version = bb_read(buf, 8);
            DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: %d: Micro Version: %d\n", i, j, profile->micro_version);

            // skip padding
            bb_seek(buf, 8, SEEK_CUR);
        }

        entry->priority = bb_read(buf, 8);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Priority: %d\n", i, entry->priority);

        entry->binding = bb_read(buf, 2);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Binding: %d\n", i, entry->binding);

        entry->visibility = bb_read(buf, 2);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Visibility: %d\n", i, entry->visibility);

        // skip padding
        bb_seek(buf, 4, SEEK_CUR);

        uint16_t name_data_length = bb_read(buf, 16);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Name Length: %d\n", i, name_data_length);

        if (name_data_length > 0) {
            // first scan for the number of app names
            uint16_t name_bytes_read = 0;
            while (name_bytes_read < name_data_length) {
                bb_seek(buf, 24, SEEK_CUR);

                uint8_t name_length = bb_read(buf, 8);
                bb_seek(buf, 8*name_length, SEEK_CUR);

                entry->app_name_count++;
                name_bytes_read += 4 + name_length;
            }

            // seek back to beginning of names
            bb_seek(buf, -name_data_length*8, SEEK_CUR);

            entry->app_names = malloc(sizeof(BDJO_APP_PROFILE)*entry->app_name_count);

            for (int j = 0; j < entry->app_name_count; j++) {
                BDJO_APP_NAME* name = &entry->app_names[j];

                get_string(buf, name->language, 3);
                DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: %d: Language: %s\n", i, j, name->language);

                uint8_t name_length = bb_read(buf, 8);
                make_string(buf, &name->name, name_length);
                DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: %d: Name: %s\n", i, j, name->name);
            }
        }

        // skip padding to word boundary
        if ((name_data_length & 0x1) != 0) {
            bb_seek(buf, 8, SEEK_CUR);
        }

        uint8_t icon_locator_length = bb_read(buf, 8);
        make_string(buf, &entry->icon_locator, icon_locator_length);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Icon Locator: %s\n", i, entry->icon_locator);

        // skip padding to word boundary
        if ((icon_locator_length & 0x1) == 0) {
            bb_seek(buf, 8, SEEK_CUR);
        }

        entry->icon_flags = bb_read(buf, 16);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Icon Flags: %X\n", i, entry->icon_flags);

        uint8_t base_dir_length = bb_read(buf, 8);
        make_string(buf, &entry->base_directory, base_dir_length);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Base Directory: %s\n", i, entry->base_directory);

        // skip padding to word boundary
        if ((base_dir_length & 0x1) == 0) {
            bb_seek(buf, 8, SEEK_CUR);
        }

        uint8_t classpath_length = bb_read(buf, 8);
        make_string(buf, &entry->classpath_extension, classpath_length);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Classpath Extension: %s\n", i, entry->classpath_extension);

        // skip padding to word boundary
        if ((classpath_length & 0x1) == 0) {
            bb_seek(buf, 8, SEEK_CUR);
        }

        uint8_t initial_class_length = bb_read(buf, 8);
        make_string(buf, &entry->initial_class, initial_class_length);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Initial Class: %s\n", i, entry->initial_class);

        // skip padding to word boundary
        if ((initial_class_length & 0x1) == 0) {
            bb_seek(buf, 8, SEEK_CUR);
        }

        uint8_t param_data_length = bb_read(buf, 8);
        DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: Param Length: %d\n", i, param_data_length);

        if (param_data_length > 0) {
            // first scan for the number of params
            uint16_t param_bytes_read = 0;
            while (param_bytes_read < param_data_length) {
                uint8_t param_length = bb_read(buf, 8);
                bb_seek(buf, 8*param_length, SEEK_CUR);

                entry->param_count++;
                param_bytes_read += 1 + param_length;
            }

            // seek back to beginning of params
            bb_seek(buf, -param_data_length*8, SEEK_CUR);

            entry->params = malloc(sizeof(uint8_t*)*entry->param_count);

            for (int j = 0; j < entry->param_count; j++) {
                uint8_t param_length = bb_read(buf, 8);
                make_string(buf, &entry->params[j], param_length);
                DEBUG(DBG_BDJ, "[bdj] BDJO > App Management Table > %d: %d: Param: %s\n", i, j, entry->params[j]);
            }
        }

        // skip padding to word boundary
        if ((param_data_length & 0x1) == 0) {
            bb_seek(buf, 8, SEEK_CUR);
        }
    }

    return 0;
}

BDJO* parse_bdjo(BITBUFFER* buf)
{
    BDJO* bdjo = malloc(sizeof(BDJO));

    // first check magic number
    uint8_t magic[4];
    bb_read_bytes(buf, magic, 4);

    if (strcmp((const char*) magic, "BDJO") != 0)
        return NULL;

    // get version string
    uint8_t version[4];
    bb_read_bytes(buf, version, 4);
    if ((bdjo->version = get_version(version)) == BDJ_ERROR)
        return NULL;
    DEBUG(DBG_BDJ, "[bdj] BDJO > Version: %.4s\n", version);

    // skip some unnecessary data
    bb_seek(buf, 8*0x28, SEEK_CUR);

    parse_terminal_info(buf, &bdjo->terminal_info);

    parse_app_cache_info(buf, bdjo);

    parse_accessible_playlists(buf, &bdjo->accessible_playlists);

    parse_app_management_table(buf, bdjo);

    bdjo->key_interest_table = bb_read(buf, 32);
    DEBUG(DBG_BDJ, "[bdj] BDJO > Key Interest Table: %d\n", bdjo->key_interest_table);

    uint16_t file_access_length = bb_read(buf, 16);
    make_string(buf, &bdjo->file_access_info, file_access_length);
    DEBUG(DBG_BDJ, "[bdj] BDJO > File Access Info: %s\n", bdjo->file_access_info);

    return bdjo;
}

BDJO* bdjo_read(const char* file)
{
    FILE* handle = fopen(file, "rb");
    if (handle == NULL)
        return NULL;

    fseek(handle, 0, SEEK_END);
    long int length = ftell(handle);

    if (length > 0) {
        fseek(handle, 0, SEEK_SET);

        char* data = malloc(length);
        long int size_read = fread(data, 1, length, handle);
        if (size_read < length) {
            free(data);
            return NULL;
        }

        BITBUFFER* buf = malloc(sizeof(BITBUFFER));
        bb_init(buf, data, length);

        BDJO* result = parse_bdjo(buf);

        free(buf);
        fclose(handle);

        return result;
    } else {
        return NULL;
    }
}

void destroy_bdjo(BDJO* bdjo)
{
    free(bdjo->app_cache_entries);

    for (unsigned int i = 0; i < bdjo->accessible_playlists.playlist_count; i++)
        free(bdjo->accessible_playlists.playlist_file_names[i]);
    free(bdjo->accessible_playlists.playlist_file_names);

    for (int i = 0; i < bdjo->app_info_count; i++) {
        BDJO_APP_INFO info = bdjo->app_info_entries[i];
        free(info.icon_locator);
        free(info.base_directory);
        free(info.classpath_extension);
        free(info.initial_class);
        for(int j = 0; j < info.param_count; j++)
            free(info.params[j]);
        free(info.params);
    }
    free(bdjo->app_info_entries);

    free(bdjo->file_access_info);

    free(bdjo);  
}
