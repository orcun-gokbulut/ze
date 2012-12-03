//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - tre-ast.h
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
  tre-ast.h - Abstract syntax tree (AST) definitions

  This software is released under a BSD-style license.
  See the file LICENSE for details and copyright.

*/


#ifndef TRE_AST_H
#define TRE_AST_H 1

#include "tre-mem.h"
#include "tre-internal.h"
#include "tre-compile.h"

/* The different AST node types. */
typedef enum {
  LITERAL,
  CATENATION,
  ITERATION,
  UNION
} tre_ast_type_t;

/* Special subtypes of TRE_LITERAL. */
#define EMPTY	  -1   /* Empty leaf (denotes empty string). */
#define ASSERTION -2   /* Assertion leaf. */
#define TAG	  -3   /* Tag leaf. */
#define BACKREF	  -4   /* Back reference leaf. */
#define PARAMETER -5   /* Parameter. */

#define IS_SPECIAL(x)	((x)->code_min < 0)
#define IS_EMPTY(x)	((x)->code_min == EMPTY)
#define IS_ASSERTION(x) ((x)->code_min == ASSERTION)
#define IS_TAG(x)	((x)->code_min == TAG)
#define IS_BACKREF(x)	((x)->code_min == BACKREF)
#define IS_PARAMETER(x) ((x)->code_min == PARAMETER)


/* A generic AST node.  All AST nodes consist of this node on the top
   level with `obj' pointing to the actual content. */
typedef struct {
  tre_ast_type_t type;   /* Type of the node. */
  void *obj;             /* Pointer to actual node. */
  int nullable;
  int submatch_id;
  int num_submatches;
  int num_tags;
  tre_pos_and_tags_t *firstpos;
  tre_pos_and_tags_t *lastpos;
} tre_ast_node_t;


/* A "literal" node.  These are created for assertions, back references,
   tags, matching parameter settings, and all expressions that match one
   character. */
typedef struct {
  long code_min;
  long code_max;
  int position;
  union {
    tre_ctype_t class;
    int *params;
  } u;
  tre_ctype_t *neg_classes;
} tre_literal_t;

/* A "catenation" node.	 These are created when two regexps are concatenated.
   If there are more than one subexpressions in sequence, the `left' part
   holds all but the last, and `right' part holds the last subexpression
   (catenation is left associative). */
typedef struct {
  tre_ast_node_t *left;
  tre_ast_node_t *right;
} tre_catenation_t;

/* An "iteration" node.	 These are created for the "*", "+", "?", and "{m,n}"
   operators. */
typedef struct {
  /* Subexpression to match. */
  tre_ast_node_t *arg;
  /* Minimum number of consecutive matches. */
  int min;
  /* Maximum number of consecutive matches. */
  int max;
  /* If 0, match as many characters as possible, if 1 match as few as
     possible.	Note that this does not always mean the same thing as
     matching as many/few repetitions as possible. */
  unsigned int minimal:1;
  /* Approximate matching parameters (or NULL). */
  int *params;
} tre_iteration_t;

/* An "union" node.  These are created for the "|" operator. */
typedef struct {
  tre_ast_node_t *left;
  tre_ast_node_t *right;
} tre_union_t;

tre_ast_node_t *
tre_ast_new_node(tre_mem_t mem, tre_ast_type_t type, size_t size);

tre_ast_node_t *
tre_ast_new_literal(tre_mem_t mem, int code_min, int code_max, int position);

tre_ast_node_t *
tre_ast_new_iter(tre_mem_t mem, tre_ast_node_t *arg, int min, int max,
		 int minimal);

tre_ast_node_t *
tre_ast_new_union(tre_mem_t mem, tre_ast_node_t *left, tre_ast_node_t *right);

tre_ast_node_t *
tre_ast_new_catenation(tre_mem_t mem, tre_ast_node_t *left,
		       tre_ast_node_t *right);

#ifdef TRE_DEBUG
void
tre_ast_print(tre_ast_node_t *tree);

/* XXX - rethink AST printing API */
void
tre_print_params(int *params);
#endif /* TRE_DEBUG */

#endif /* TRE_AST_H */

/* EOF */
