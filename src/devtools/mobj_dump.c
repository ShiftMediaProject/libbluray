/*
 * This file is part of libbluray
 * Copyright (C) 2010  hpi1
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

#include "libbluray/hdmv/mobj_data.h"
#include "libbluray/hdmv/mobj_print.h"

#include <stdio.h>
#include <string.h>

static void _mobj_print(MOBJ_OBJECTS *objects, int disasm)
{
    int o, c;

    printf("Number of objects: %d\n", objects->num_objects);

    for (o = 0; o < objects->num_objects; o++) {

        printf("Object %d:\n"
               "  number of commands:    %d\n"
               "  resume intention flag: %d\n"
               "  menu call mask:        %d\n"
               "  title search mask:     %d\n",
               o, objects->objects[o].num_cmds,
               objects->objects[o].resume_intention_flag,
               objects->objects[o].menu_call_mask,
               objects->objects[o].title_search_mask);

            if (disasm) {
                printf("  program:\n");
                for (c = 0; c < objects->objects[o].num_cmds; c++) {
                    char buf[256];
                    mobj_sprint_cmd(buf, &objects->objects[o].cmds[c]);
                    printf("    %04d: %s\n", c, buf);
                }
            }
    }
}

int main(int argc, const char *argv[])
{
    int           disasm = 0;
    MOBJ_OBJECTS *mobj = NULL;

    if (argc < 2) {
        fprintf(stderr,
                "usage: %s [-d] <file>\n"
                "Options:\n"
                "    d         disassemble object code\n",
                argv[0]);
        return 1;
    }
    if (argc > 2) {
        disasm = !strcmp(argv[1], "-d");
    }

    mobj = bd_read_mobj(argv[argc-1]);

    if (mobj) {
        _mobj_print(mobj, disasm);

        bd_free_mobj(mobj);
    }

    return 0;
}
