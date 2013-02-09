//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMacroDataStructures.h
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

#pragma once
#ifndef __ZE_MACRO_DATA_STRUCTURES_H__
#define __ZE_MACRO_DATA_STRUCTURES_H__

#define ZE_MACRO_TUPLE(Size, Index, Tuple) ZE_MACRO_TUPLE_INTERNAL_##Size##_##Index(Tuple)

#define ZE_MACRO_TUPLE_INTERNAL_1_0(E0) E0

#define ZE_MACRO_TUPLE_INTERNAL_2_0(E0, E1) E0
#define ZE_MACRO_TUPLE_INTERNAL_2_1(E0, E1) E1

#define ZE_MACRO_TUPLE_INTERNAL_3_0(E0, E1, E2) E0
#define ZE_MACRO_TUPLE_INTERNAL_3_1(E0, E1, E2) E1
#define ZE_MACRO_TUPLE_INTERNAL_3_2(E0, E1, E2) E2

#define ZE_MACRO_TUPLE_INTERNAL_4_0(E0, E1, E2, E3) E0
#define ZE_MACRO_TUPLE_INTERNAL_4_1(E0, E1, E2, E3) E1
#define ZE_MACRO_TUPLE_INTERNAL_4_2(E0, E1, E2, E3) E2
#define ZE_MACRO_TUPLE_INTERNAL_4_3(E0, E1, E2, E3) E3

#define ZE_MACRO_TUPLE_INTERNAL_5_0(E0, E1, E2, E3, E4) E0
#define ZE_MACRO_TUPLE_INTERNAL_5_1(E0, E1, E2, E3, E4) E1
#define ZE_MACRO_TUPLE_INTERNAL_5_2(E0, E1, E2, E3, E4) E2
#define ZE_MACRO_TUPLE_INTERNAL_5_3(E0, E1, E2, E3, E4) E3
#define ZE_MACRO_TUPLE_INTERNAL_5_4(E0, E1, E2, E3, E4) E4

#endif
