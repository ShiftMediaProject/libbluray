/*
 * This file is part of libbluray
 * Copyright (C) 2011  VideoLAN
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <windows.h>

using namespace Platform;
using namespace Windows::ApplicationModel;

extern "C"
{
#include "dirs.h"

    char *win32_get_font_dir(const char *font_file)
    {
        return NULL;
    }

    char *file_get_config_home(void)
    {
        return file_get_data_home();
    }

    char *file_get_data_home(void)
    {
        auto localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
        int len = WideCharToMultiByte(CP_UTF8, 0, localFolder->Path->Data(), -1, NULL, 0, NULL, NULL);
        char *appdir = (char *)malloc(len);
        if (appdir) {
            WideCharToMultiByte(CP_UTF8, 0, localFolder->Path->Data(), -1, appdir, len, NULL, NULL);
        }
        return appdir;
    }

    char *file_get_cache_home(void)
    {
        return file_get_data_home();
    }

    const char *file_get_config_system(const char *dir)
    {
        if (!dir) // first call
            return file_get_data_home();

        return NULL; // next call
    }

    char *file_get_install_dir(void)
    {
        try
        {
            String^ path = Package::Current->InstalledLocation->Path;
            int len = WideCharToMultiByte(CP_UTF8, 0, path->Data(), -1, NULL, 0, NULL, NULL);
            char *instdir = (char *)malloc(len);
            if (instdir) {
                WideCharToMultiByte(CP_UTF8, 0, path->Data(), -1, instdir, len, NULL, NULL);
            }
            return instdir;
        }
        catch (Exception^)
        {
            return NULL;
        }
    }
}