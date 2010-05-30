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

#if !defined(_MOBJ_PARSE_H_)
#define _MOBJ_PARSE_H_

#include <stdint.h>

typedef struct {
  uint8_t op_cnt     : 3;  /* operand count */
  uint8_t grp        : 2;  /* command group */
  uint8_t sub_grp    : 3;  /* command sub-group */

  uint8_t imm_op1    : 1;  /* I-flag for operand 1 */
  uint8_t imm_op2    : 1;  /* I-flag for operand 2 */
  uint8_t reserved1  : 2;
  uint8_t branch_opt : 4;  /* branch option */

  uint8_t reserved2  : 4;
  uint8_t cmp_opt    : 4;  /* compare option */

  uint8_t reserved3  : 3;
  uint8_t set_opt    : 5;  /* set option */
} HDMV_INSN;

typedef struct {
    HDMV_INSN insn;
    uint32_t  dst;
    uint32_t  src;
} MOBJ_CMD;

typedef struct {
    uint8_t     resume_intention_flag : 1;
    uint8_t     menu_call_mask        : 1;
    uint8_t     title_search_mask     : 1;

    uint16_t    num_cmds;
    MOBJ_CMD   *cmds;
} MOBJ_OBJECT;

typedef struct {
    uint16_t    num_objects;
    MOBJ_OBJECT objects[];
} MOBJ_OBJECTS;


MOBJ_OBJECTS* mobj_parse(const char *path); /* parse MovieObject.bdmv */
void mobj_free(MOBJ_OBJECTS *index);

#endif // _MOBJ_PARSE_H_

