//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVectorSwizzle.h
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

#include <ZETypes.h>
#include "ZEMeta/ZEMTAttribute.h"

#define ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType, a, b) ZEMT_ATTRIBUTE(ZEMC.Export, false) ZEVectorType a##b() const;
#define ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType, ClassName, a, b)  ZEVectorType ClassName::a##b() const {return ZEVectorType(a, b);}

#define ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType, a, b, c) ZEMT_ATTRIBUTE(ZEMC.Export, false) ZEVectorType a##b##c() const;
#define ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType, ClassName, a, b, c)  ZEVectorType ClassName::a##b##c() const {return ZEVectorType(a, b, c);}

#define ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType, a, b, c, d) ZEMT_ATTRIBUTE(ZEMC.Export, false) ZEVectorType a##b##c##d() const;
#define ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType, ClassName, a, b, c, d) ZEVectorType ClassName::a##b##c##d() const {return ZEVectorType(a, b, c, d);}

#define ZE_VECTOR_SWIZZLE_2D_DEFINITION(ZEVectorType) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType, x, x); \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType, x, y); \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType, y, x); \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType, y, y);

#define ZE_VECTOR_SWIZZLE_2D_IMPLEMENTATION(ZEVectorType, ClassName) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType, ClassName, x, x); \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType, ClassName, y, y); \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType, ClassName, x, y); \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType, ClassName, y, x);

#define ZE_VECTOR_SWIZZLE_3D_DEFINITION(ZEVectorType3, ZEVectorType2) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, z, z)

#define ZE_VECTOR_SWIZZLE_3D_IMPLEMENTATION(ZEVectorType3, ZEVectorType2) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType3, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType3, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType3, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType3, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType3, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType3, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType3, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType3, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType3, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, x, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, x, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, x, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, x, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, x, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, x, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, x, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, x, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, x, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, y, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, y, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, y, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, y, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, y, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, y, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, y, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, y, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, y, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, z, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, z, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, z, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, z, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, z, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, z, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, z, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, z, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType3, z, z, z)


#define ZE_VECTOR_SWIZZLE_4D_DEFINITION(ZEVectorType4, ZEVectorType3, ZEVectorType2) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_2(ZEVectorType2, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, x, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, y, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, z, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_3(ZEVectorType3, w, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, x, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, y, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, z, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, x, w, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, x, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, y, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, z, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, y, w, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, x, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, y, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, z, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, z, w, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, x, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, y, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, z, w, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, x, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, x, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, x, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, x, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, y, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, y, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, y, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, y, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, z, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, z, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, z, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, z, w) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, w, x) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, w, y) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, w, z) \
	ZE_VECTOR_SWIZZLE_DEFINITION_4(ZEVectorType4, w, w, w, w)

#define ZE_VECTOR_SWIZZLE_4D_IMPLEMENTATION(ZEVectorType4, ZEVectorType3, ZEVectorType2) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_2(ZEVectorType2, ZEVectorType4, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, x, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, y, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, z, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_3(ZEVectorType3, ZEVectorType4, w, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, x, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, y, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, z, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, x, w, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, x, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, y, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, z, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, y, w, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, x, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, y, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, z, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, z, w, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, x, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, y, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, z, w, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, x, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, x, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, x, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, x, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, y, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, y, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, y, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, y, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, z, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, z, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, z, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, z, w) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, w, x) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, w, y) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, w, z) \
	ZE_VECTOR_SWIZZLE_IMPLEMENTATION_4(ZEVectorType4, ZEVectorType4, w, w, w, w)
