#ZE_SOURCE_PROCESSOR_START(License, 1.0)
#[[*****************************************************************************
 Zinek Engine - CMakeLists.txt
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
*****************************************************************************]]
#ZE_SOURCE_PROCESSOR_END()

cmake_minimum_required (VERSION 2.8)

ze_add_source(regcomp.c 				Sources)
ze_add_source(regerror.c 				Sources)
ze_add_source(regex.h 					Sources)
ze_add_source(regexec.c 				Sources)
ze_add_source(tre-ast.c 				Sources)
ze_add_source(tre-ast.h 				Sources)
ze_add_source(tre-compile.c 			Sources)
ze_add_source(tre-compile.h 			Sources)
ze_add_source(tre-config.h.in 			Sources)
ze_add_source(tre-internal.h 			Sources)
ze_add_source(tre-match-approx.c 		Sources)
ze_add_source(tre-match-backtrack.c 	Sources)
ze_add_source(tre-match-parallel.c 		Sources)
ze_add_source(tre-match-utils.h 		Sources)
ze_add_source(tre-mem.c 				Sources)
ze_add_source(tre-mem.h 				Sources)
ze_add_source(tre-parse.c 				Sources)
ze_add_source(tre-parse.h 				Sources)
ze_add_source(tre-stack.c 				Sources)
ze_add_source(tre-stack.h 				Sources)
ze_add_source(tre.h 					Sources)
ze_add_source(xmalloc.c 				Sources)
ze_add_source(xmalloc.h 				Sources)

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

ze_add_library(TARGET TRE SOURCES ${Sources})
