/*
 * This file is part of libbluray
 * Copyright (C) 2014  Petri Hintukainen <phintuka@users.sourceforge.net>
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

#include "libbluray/bluray.h"
#include "libbluray/bdj/bdjo_data.h"

#include <stdio.h>
#include <string.h>

static const char *_yes_no(int i)
{
    return i > 0 ? "yes" : i < 0 ? "unknown" : "no";
}

static const char *_binding_str(int i)
{
    switch (i) {
        case 0:  return "unbound";
        case 1:  return "disc bound";
        case 3:  return "title bound";
        default: return "???";
    }
}

static const char *_visibility_str(int i)
{
    switch (i) {
        case 0:  return "none";
        case 1:  return "applications";
        case 2:  return "user";
        default: return "???";
    }
}

static void _terminal_info_print(const BDJO_TERMINAL_INFO *p)
{
    printf("Terminal Info:\n");
    printf("    Default AWT font    : %s\n", p->default_font);
    printf("    initial HaVi config : %d\n", p->initial_havi_config_id);
    printf("    Menu call mask      : %d\n", p->menu_call_mask);
    printf("    Title search mask   : %d\n", p->menu_call_mask);
}

static void _app_cache_item_print(const BDJO_APP_CACHE_ITEM *p)
{
    printf("    %3.3s: %s%s\n",
           p->lang_code, p->ref_to_name,
           p->type == 1 ? ".jar" : p->type == 2 ? "/" : " (unknown type)");
}

static void _app_cache_info_print(const BDJO_APP_CACHE_INFO *p)
{
    unsigned ii;

    printf("Application cache info:\n");
    for (ii = 0; ii < p->num_item; ii++) {
        _app_cache_item_print(&p->item[ii]);
    }
}

static void _accessible_playlists_print(const BDJO_ACCESSIBLE_PLAYLISTS *p)
{
    unsigned ii;

    printf("Accessible playlists:\n");
    printf("    Access to all       : %s\n", _yes_no(p->access_to_all_flag));
    printf("    Autostart first     : %s\n", _yes_no(p->autostart_first_playlist_flag));

    if (p->num_pl) {
        printf("    Playlists           : %d\n", p->num_pl);
        for (ii = 0; ii < p->num_pl; ii++) {
            printf("        %s.mpls\n", p->pl[ii].name);
        }
    }
}

static void _app_profile_print(BDJO_APP_PROFILE *p)
{
    printf("        Profile %d Version %d.%d.%d\n",
           p->profile_number, p->major_version, p->minor_version, p->micro_version);
}

static void _app_print(const BDJO_APP *p)
{
    unsigned ii;

    printf("    Control code:       : %d (%s)\n", p->control_code,
           p->control_code == 1 ? "autostart" : p->control_code == 2 ? "present" : "???");
    printf("    Type                : %d (%s)\n", p->type,
           p->type == 1 ? "BD-J App" : "???");
    printf("    Organization ID     : %08X\n", p->org_id);
    printf("    Application ID      : %04X\n", p->app_id);
    printf("    Priority            : %d\n", p->priority);
    printf("    Binding             : %d (%s)\n", p->binding, _binding_str(p->binding));
    printf("    Visibility          : %d (%s)\n", p->visibility, _visibility_str(p->visibility));

    if (p->num_profile) {
        printf("    Profiles:\n");
        for (ii = 0; ii < p->num_profile; ii++) {
            _app_profile_print(&p->profile[ii]);
        }
    }

    if (p->num_name) {
        printf("    Names:\n");
        for (ii = 0; ii < p->num_name; ii++) {
            printf("        %s: %s\n", p->name[ii].lang, p->name[ii].name);
        }
    }

    printf("    Base directory      : %s\n", p->base_dir);
    printf("    Icon locator        : %s\n", p->icon_locator);
    printf("    Icon flags          : 0x%04x\n", p->icon_flags);
    printf("    Classpath extension : %s\n", p->classpath_extension);
    printf("    Initial class       : %s\n", p->initial_class);
    printf("    Parameters          : ");
    for (ii = 0; ii < p->num_param; ii++) {
        printf("%s ", p->param[ii].param);
    }
    printf("\n");
}

static void _app_management_table_print(const BDJO_APP_MANAGEMENT_TABLE *p)
{
    unsigned ii;

    for (ii = 0; ii < p->num_app; ii++) {
        printf("Application %u:\n", ii);
        _app_print(&p->app[ii]);
    }
}

static void _key_interest_table_print(const BDJO_KEY_INTEREST_TABLE *p)
{
    unsigned int v;
    memcpy(&v, p, sizeof(unsigned int));
    if (v) {
        printf("Key interest table:\n");
        printf("    %s%s%s%s%s%s%s%s%s%s%s\n",
               p->vk_play              ? "VK_PLAY " : "",
               p->vk_stop              ? "VK_STOP " : "",
               p->vk_ffw               ? "VK_FFW " : "",
               p->vk_rew               ? "VK_REW " : "",
               p->vk_track_next        ? "VK_TRACK_NEXT " : "",
               p->vk_track_prev        ? "VK_TRACK_PREV " : "",
               p->vk_pause             ? "VK_PAUSE " : "",
               p->vk_still_off         ? "VK_STILL_OFF " : "",
               p->vk_sec_audio_ena_dis ? "VK_SEC_AUDIO " : "",
               p->vk_sec_video_ena_dis ? "VK_SEC_VIDEO " : "",
               p->pg_textst_ena_dis    ? "VK_PG_TEXTST " : "");
    }
}

static void _file_access_info_print(const BDJO_FILE_ACCESS_INFO *p)
{
    printf("File access info:\n    %s\n",  p->path);
}

static void _bdjo_print(const BDJO *p)
{
    _terminal_info_print(&p->terminal_info);
    _app_cache_info_print(&p->app_cache_info);
    _accessible_playlists_print(&p->accessible_playlists);
    _app_management_table_print(&p->app_table);
    _key_interest_table_print(&p->key_interest_table);
    _file_access_info_print(&p->file_access_info);
}

int main(int argc, const char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "usage: %s <bdjo_file>\n", argv[0]);
        return 1;
    }

    int cnt;
    for (cnt = 1; cnt < argc; cnt++) {

        printf("%s\n", argv[cnt]);

        BDJO *bdjo = bd_read_bdjo(argv[cnt]);
        if (bdjo) {
            _bdjo_print(bdjo);
            bd_free_bdjo(bdjo);
        }
        printf("\n");
    }

    return 0;
}
