//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMetaAttribute.h
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
#ifndef __ZE_META_ATTRIBUTE__
#define __ZE_META_ATTRIBUTE__

#include "ZETypes.h"

#ifdef ZE_META_COMPILER
	#define ZE_META_ATTRIBUTE_INTERNAL(Value) __attribute__((annotate(Value)))
#else
	#define ZE_META_ATTRIBUTE_INTERNAL(Value)
#endif

#define ZE_META_ATTRIBUTE_0(Name) ZE_META_ATTRIBUTE_INTERNAL(#Name)
#define ZE_META_ATTRIBUTE_1(Name, Parameter0) ZE_META_ATTRIBUTE_INTERNAL(#Name "," #Parameter0)
#define ZE_META_ATTRIBUTE_2(Name, Parameter0, Parameter1) ZE_META_ATTRIBUTE_INTERNAL(#Name "," #Parameter0 "," #Parameter1)
#define ZE_META_ATTRIBUTE_3(Name, Parameter0, Parameter1, Parameter2) ZE_META_ATTRIBUTE_INTERNAL(#Name "," #Parameter0 "," #Parameter1 "," #Parameter2)
#define ZE_META_ATTRIBUTE_4(Name, Parameter0, Parameter1, Parameter2, Parameter3) ZE_META_ATTRIBUTE_INTERNAL(#Name "," #Parameter0 "," #Parameter1 "," #Parameter2 "," #Parameter3)
#define ZE_META_ATTRIBUTE_5(Name, Parameter0, Parameter1, Parameter2, Parameter3, Parameter4) ZE_META_ATTRIBUTE_INTERNAL(#Name "," #Parameter0 "," #Parameter1 "," #Parameter2 "," #Parameter3  "," #Parameter4)
#define ZE_META_ATTRIBUTE_6(Name, Parameter0, Parameter1, Parameter2, Parameter3, Parameter4, Parameter5) ZE_META_ATTRIBUTE_INTERNAL(#Name "," #Parameter0 "," #Parameter1 "," #Parameter2 "," #Parameter4  "," #Parameter5)

struct ZEMetaAttribute
{
	const char*		Name;
	const char**	Values;
	ZESize			ValueCount;
};

#endif
