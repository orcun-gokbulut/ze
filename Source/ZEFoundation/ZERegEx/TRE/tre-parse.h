//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - tre-parse.h
 ------------------------------------------------------------------------------
 Copyright (C) 2008-2021 Yiğit Orçun GÖKBULUT. All rights reserved.

 This file is part of the Zinek Engine  Software. Zinek Engine Software and the
 accompanying  materials are  made available  under the  terms of Zinek  Engine
 Commercial License or the GNU General Public License Version 3.

                      ZINEK ENGINE COMMERCIAL LICENSE
 Licensees  holding  valid  Zinek Engine Commercial  License(s) may  use  Zinek
 Engine  Software in  accordance  with   the  commercial  license  agreement(s)
 (which can only be  issued  by  copyright  owner "Yiğit  Orçun  GÖKBULUT") and
 provided with the Software  or, alternatively,  in  accordance with the  terms
 contained  in  a  written  agreement  between  you  and  copyright  owner. For
 licensing  terms  and conditions  please  contact  with  copyright owner.

                    GNU GENERAL PUBLIC LICENSE VERSION 3
 This program is free software: you can  redistribute it and/or modify it under
 the terms of the GNU General Public  License as published by the Free Software
 Foundation, either  version 3 of  the License, or  (at your option)  any later
 version. This program is  distributed in the hope that  it will be useful, but
 WITHOUT ANY WARRANTY; without even the  implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE. See  the GNU General Public License for more
 details. You  should have received  a copy of the  GNU General  Public License
 along with this program. If not, see <https://www.gnu.org/licenses/>.

 Copyright Owner Information:
  Name: Yiğit Orçun GÖKBULUT
  Contact: orcun.gokbulut@gmail.com
  Github: https://www.github.com/orcun-gokbulut/ZE
*******************************************************************************/
//ZE_SOURCE_PROCESSOR_END()

/*
  tre-parse.c - Regexp parser definitions

  This software is released under a BSD-style license.
  See the file LICENSE for details and copyright.

*/

#ifndef TRE_PARSE_H
#define TRE_PARSE_H 1

/* Parse context. */
typedef struct {
  /* Memory allocator.	The AST is allocated using this. */
  tre_mem_t mem;
  /* Stack used for keeping track of regexp syntax. */
  tre_stack_t *stack;
  /* The parse result. */
  tre_ast_node_t *result;
  /* The regexp to parse and its length. */
  const tre_char_t *re;
  /* The first character of the entire regexp. */
  const tre_char_t *re_start;
  /* The first character after the end of the regexp. */
  const tre_char_t *re_end;
  int len;
  /* Current submatch ID. */
  int submatch_id;
  /* Current position (number of literal). */
  int position;
  /* The highest back reference or -1 if none seen so far. */
  int max_backref;
  /* This flag is set if the regexp uses approximate matching. */
  int have_approx;
  /* Compilation flags. */
  int cflags;
  /* If this flag is set the top-level submatch is not captured. */
  int nofirstsub;
  /* The currently set approximate matching parameters. */
  int params[TRE_PARAM_LAST];
} tre_parse_ctx_t;

/* Parses a wide character regexp pattern into a syntax tree.  This parser
   handles both syntaxes (BRE and ERE), including the TRE extensions. */
reg_errcode_t
tre_parse(tre_parse_ctx_t *ctx);

#endif /* TRE_PARSE_H */

/* EOF */
