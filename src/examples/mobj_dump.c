/*
 * This file is part of libbluray
 * Copyright (C) 2010  hpi1
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders of libbluray
 * gives permission to link the code of its release of libbluray with the
 * OpenSSL project's "OpenSSL" library (or with modified versions of it
 * that use the same license as the "OpenSSL" library), and distribute
 * the linked executables.  You must obey the GNU General Public License
 * in all respects for all of the code used other than "OpenSSL".  If you
 * modify this file, you may extend this exception to your version of the
 * file, but you are not obligated to do so.  If you do not wish to do
 * so, delete this exception statement from your version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "../util/macro.h"

#include "libbluray/bdnav/mobj_parse.h"
#include "libbluray/bdnav/hdmv_insn.h"

const char * const psr_info[128] = {
    "/*     PSR0:  Interactive graphics stream number */",
    "/*     PSR1:  Primary audio stream number */",
    "/*     PSR2:  PG TextST stream number and PiP PG stream number */",
    "/*     PSR3:  Angle number */",
    "/*     PSR4:  Title number */",
    "/*     PSR5:  Chapter number */",
    "/*     PSR6:  PlayList ID */",
    "/*     PSR7:  PlayItem ID */",
    "/*     PSR8:  Presentation time */",
    "/*     PSR9:  Navigation timer */",
    "/*     PSR10: Selected button ID */",
    "/*     PSR11: Page ID */",
    "/*     PSR12: User style number */",
    "/* RO: PSR13: User age */",
    "/*     PSR14: Secondary audio stream number and secondary video stream number */",
    "/* RO: PSR15: player capability for audio */",
    "/* RO: PSR16: Language code for audio */",
    "/* RO: PSR17: Language code for PG and Text subtitles */",
    "/* RO: PSR18: Menu description language code */",
    "/* RO: PSR19: Country code */",
    "/* RO: PSR20: Region code */ /* 1 - A, 2 - B, 4 - C */",
    "/*     PSR21 */",
    "/*     PSR22 */",
    "/*     PSR23 */",
    "/*     PSR24 */",
    "/*     PSR25 */",
    "/*     PSR26 */",
    "/*     PSR27 */",
    "/*     PSR28 */",
    "/* RO: PSR29: player capability for video */",
    "/* RO: PSR30: player capability for text subtitle */",
    "/* RO: PSR31: Player profile and version */",
    "/*     PSR32 */",
    "/*     PSR33 */",
    "/*     PSR34 */",
    "/*     PSR35 */",
    "/*     PSR36: backup PSR4 */",
    "/*     PSR37: backup PSR5 */",
    "/*     PSR38: backup PSR6 */",
    "/*     PSR39: backup PSR7 */",
    "/*     PSR40: backup PSR8 */",
    "/*     PSR41: */",
    "/*     PSR42: backup PSR10 */",
    "/*     PSR43: backup PSR11 */",
    "/*     PSR44: backup PSR12 */",
    "/*     PSR45: */",
    "/*     PSR46: */",
    "/*     PSR47: */",
    "/* RO: PSR48: Characteristic text caps */",
    "/* RO: PSR49: Characteristic text caps */",
    "/* RO: PSR50: Characteristic text caps */",
    "/* RO: PSR51: Characteristic text caps */",
    "/* RO: PSR52: Characteristic text caps */",
    "/* RO: PSR53: Characteristic text caps */",
    "/* RO: PSR54: Characteristic text caps */",
    "/* RO: PSR55: Characteristic text caps */",
    "/* RO: PSR56: Characteristic text caps */",
    "/* RO: PSR57: Characteristic text caps */",
    "/* RO: PSR58: Characteristic text caps */",
    "/* RO: PSR59: Characteristic text caps */",
    "/* RO: PSR60: Characteristic text caps */",
    "/* RO: PSR61: Characteristic text caps */",
};

const char * const insn_groups[4] = {
    "BRANCH",
    "COMPARE",
    "SET",
};

const char * const insn_group_branch[8] = {
    "GOTO",
    "JUMP",
    "PLAY",
};

const char * const insn_group_set[8] = {
    "SET",
    "SETSYSTEM",
};

const char * const insn_opt_set[32] = {
    NULL,
    "move",
    "swap",
    "add",
    "sub",
    "mul",
    "div",
    "mod",
    "rnd",
    "and",
    "or",
    "xor",
    "bset",
    "bclr",
    "shl",
    "shr",
};

const char * const insn_opt_setsys[32] = {
    NULL,
    "SET_STREAM",
    "SET_NV_TIMER",
    "SET_BUTTON_PAGE",
    "ENABLE_BUTTON",
    "DISABLE_BUTTON",
    "SET_SEC_STREAM",
    "POPUP_OFF",
    "STILL_ON",
    "STILL_OFF",
};

const char * const insn_opt_cmp[16] = {
    NULL,
    "bc",
    "eq",
    "ne",
    "ge",
    "gt",
    "le",
    "lt",
};

const char * const insn_opt_goto[16] = {
    "nop",
    "goto",
    "break",
};

const char * const insn_opt_jump[16] = {
    "JUMP_OBJECT",
    "JUMP_TITLE",
    "CALL_OBJECT",
    "CALL_TITLE",
    "RESUME"
};

const char * const insn_opt_play[16] = {
    "PLAY_PL",
    "PLAY_PL_PI",
    "PLAY_PL_MK",
    "TERMINATE_PL",
    "LINK_PI",
    "LINK_MK"
};


static void _print_operand(int imm, uint32_t op, int *psr)
{
    if (!imm) {
        if (op & 0x80000000) {
            printf("PSR%u", op & 0x7f);
            *psr = op & 0x7f;
        } else {
            printf("r%u", op & 0xfff);
        }
    } else {
        if (op < 99999)
            printf("%-6u", op);
        else
            printf("0x%-4x", op);
    }
}

static void _print_operands(MOBJ_CMD *cmd)
{
    HDMV_INSN *insn = &cmd->insn;
    int psr1 = -1, psr2 = -1;

    if (insn->op_cnt > 0) {
        _print_operand(insn->imm_op1, cmd->dst, &psr1);
    }
    if (insn->op_cnt > 1) {
        printf(",\t");
        _print_operand(insn->imm_op2, cmd->src, &psr2);
    }

    if (psr1 >= 0)
        printf(" %s", psr_info[psr1]);
    if (psr2 >= 0 && psr2 != psr1)
        printf(" %s", psr_info[psr2]);
}

static void _print_operands_hex(MOBJ_CMD *cmd)
{
    HDMV_INSN *insn = &cmd->insn;

    if (insn->op_cnt > 0) {
            printf("0x%-4x", cmd->dst);
    }
    if (insn->op_cnt > 1) {
        printf(",\t");
        printf("0x%-4x", cmd->src);
    }
}

static void _print_cmd(MOBJ_CMD *cmd)
{
    HDMV_INSN *insn = &cmd->insn;

    printf("%08x %08x,%08x  ", MKINT_BE32((uint8_t*)&cmd->insn), cmd->dst, cmd->src);

    switch(insn->grp) {
        case INSN_GROUP_BRANCH:
            switch(insn->sub_grp) {
                case BRANCH_GOTO:
                    if (insn_opt_goto[insn->branch_opt]) {
                        printf("%-10s ", insn_opt_goto[insn->branch_opt]);
                        _print_operands(cmd);
                    } else {
                        printf("[unknown BRANCH/GOTO option in opcode 0x%08x] ", *(uint32_t*)insn);
                    }
                    break;
            case BRANCH_JUMP:
                if (insn_opt_jump[insn->branch_opt]) {
                    printf("%-10s ", insn_opt_jump[insn->branch_opt]);
                    _print_operands(cmd);
                } else {
                    printf("[unknown BRANCH/JUMP option in opcode 0x%08x] ", *(uint32_t*)insn);
                }
                break;
            case BRANCH_PLAY:
                if (insn_opt_play[insn->branch_opt]) {
                    printf("%-10s ", insn_opt_play[insn->branch_opt]);
                    _print_operands(cmd);
                } else {
                    printf("[unknown BRANCH/PLAY option in opcode 0x%08x] ", *(uint32_t*)insn);
                }
                break;
            default:
                printf("[unknown BRANCH subgroup in opcode 0x%08x] ", *(uint32_t*)insn);
                break;
            }
            break;

        case INSN_GROUP_CMP:
            if (insn_opt_cmp[insn->cmp_opt]) {
                printf("%-10s ", insn_opt_cmp[insn->cmp_opt]);
                _print_operands(cmd);
            } else {
                printf("[unknown COMPARE option in opcode 0x%08x] ", *(uint32_t*)insn);
            }
            break;

        case INSN_GROUP_SET:
            switch (insn->sub_grp) {
                case SET_SET:
                    if (insn_opt_set[insn->set_opt]) {
                        printf("%-10s ", insn_opt_set[insn->set_opt]);
                        _print_operands(cmd);
                    } else {
                        printf("[unknown SET option in opcode 0x%08x] ", *(uint32_t*)insn);
                    }
                    break;
            case SET_SETSYSTEM:
                if (insn_opt_setsys[insn->set_opt]) {
                    printf("%-10s ", insn_opt_setsys[insn->set_opt]);
                    _print_operands_hex(cmd);
                } else {
                    printf("[unknown SETSYSTEM option in opcode 0x%08x] ", *(uint32_t*)insn);
                }
                break;
            default:
                printf("[unknown SET subgroup in opcode 0x%08x] ", *(uint32_t*)insn);
                break;
            }
            break;

        default:
            printf("[unknown group in opcode 0x%08x] ", *(uint32_t*)insn);
            break;
    }
}

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
                    printf("    %04d: ", c);
                    _print_cmd(&objects->objects[o].cmds[c]);
                    printf("\n");
                }
            }
    }
}

int main(int argc, const char *argv[])
{
    char          file[1024];
    int           disasm = 0;
    MOBJ_OBJECTS *mobj;

    if (argc < 2) {
        fprintf(stderr,
                "usage: %s [-d] <disc_root>\n"
                "Options:\n"
                "    d         disassemble object code\n",
                argv[0]);
        return 1;
    }
    if (argc > 2) {
        disasm = !strcmp(argv[1], "-d");
    }

    sprintf(file, "%s/BDMV/MovieObject.bdmv", argv[argc-1]);

    mobj = mobj_parse(file);

    if (mobj) {
        _mobj_print(mobj, disasm);

        mobj_free(mobj);
    }

    return 0;
}
