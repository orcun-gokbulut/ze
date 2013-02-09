//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - regex.h
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
  regex.h - TRE legacy API

  This software is released under a BSD-style license.
  See the file LICENSE for details and copyright.

  This header is for source level compatibility with old code using
  the <tre/regex.h> header which defined the TRE API functions without
  a prefix.  New code should include <tre/tre.h> instead.

*/

#ifndef TRE_REXEX_H
#define TRE_REGEX_H 1

#include "tre.h"

#ifndef TRE_USE_SYSTEM_REGEX_H
#define regcomp	   tre_regcomp
#define regerror   tre_regerror
#define regexec	   tre_regexec
#define regfree	   tre_regfree
#endif /* TRE_USE_SYSTEM_REGEX_H */

#define regacomp   tre_regacomp
#define regaexec   tre_regaexec
#define regancomp  tre_regancomp
#define reganexec  tre_reganexec
#define regawncomp tre_regawncomp
#define regawnexec tre_regawnexec
#define regncomp   tre_regncomp
#define regnexec   tre_regnexec
#define regwcomp   tre_regwcomp
#define regwexec   tre_regwexec
#define regwncomp  tre_regwncomp
#define regwnexec  tre_regwnexec

#endif /* TRE_REGEX_H */
