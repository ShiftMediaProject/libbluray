/*
 * This file is part of libbluray
 * Copyright (C) 2012  Petri Hintukainen <phintuka@users.sourceforge.net>
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

#include "libbluray/bdj/native/register_native.h"

#include "libbluray/bdj/bdj_util.h"

int bdj_register_native_methods(JNIEnv *env)
{
    extern const JNINativeMethod Java_org_videolan_Logger_methods[];
    extern const JNINativeMethod Java_org_videolan_Libbluray_methods[];
    extern const JNINativeMethod Java_java_awt_BDGraphics_methods[];
    extern const JNINativeMethod Java_java_awt_BDFontMetrics_methods[];
    extern const int Java_org_videolan_Logger_methods_count;
    extern const int Java_org_videolan_Libbluray_methods_count;
    extern const int Java_java_awt_BDGraphics_methods_count;
    extern const int Java_java_awt_BDFontMetrics_methods_count;

    return
      bdj_register_methods(env, "org/videolan/Logger",
                           Java_org_videolan_Logger_methods,
                           Java_org_videolan_Logger_methods_count)
      *
      bdj_register_methods(env, "org/videolan/Libbluray",
                           Java_org_videolan_Libbluray_methods,
                           Java_org_videolan_Libbluray_methods_count)
      *
      /* BDFontMetrics must be registered before BDGraphics */
      bdj_register_methods(env, "java/awt/BDFontMetrics",
                           Java_java_awt_BDFontMetrics_methods,
                           Java_java_awt_BDFontMetrics_methods_count)
      *
      bdj_register_methods(env, "java/awt/BDGraphics",
                           Java_java_awt_BDGraphics_methods,
                           Java_java_awt_BDGraphics_methods_count)
      ;
}
