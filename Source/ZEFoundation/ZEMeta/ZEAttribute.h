//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEAttribute.h
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

#include "ZETypes.h"
#include "ZEMacro/ZEMacro.h"

#ifdef ZE_META_COMPILER
	#define ZE_META_ATTRIBUTE(Args...) ZE_META_ATTRIBUTE_2(Args)
	#define ZE_META_ATTRIBUTE_2(Args...)__attribute__((annotate(#Args)))
#else
	#define ZE_META_ATTRIBUTE(...)
#endif

#define ZE_META_ATTRIBUTE_DELETE(AttributeName)										ZE_META_ATTRIBUTE("!"#AttributeName)
#define ZE_META_ATTRIBUTE_VALUES_APPEND(AttributeName, ...)							ZE_META_ATTRIBUTE("+"#AttributeName, __VA_ARGS__)
#define ZE_META_ATTRIBUTE_VALUES_REMOVE(AttributeName, ...)							ZE_META_ATTRIBUTE("-"#AttributeName, __VA_ARGS__)
#define ZE_META_ATTRIBUTE_VALUES_CLEAR(AttributeName)								ZE_META_ATTRIBUTE("%"#AttributeName)

// Member Naming:
// @ Property (@EntityId)
// ~ Method (~Tick(float ElapsedTime)
// * Everything
// @* All Attributes
// #* All Methods

#define ZE_META_ATTRIBUTE_MEMBER(MemberName, AttributeName, ...)					ZE_META_ATTRIBUTE(#MemberName, #AttributeName, __VA_ARGS__)
#define ZE_META_ATTRIBUTE_MEMBER_DELETE(MemberName, AttributeName)					ZE_META_ATTRIBUTE(#MemberName, "!"#AttributeName)
#define ZE_META_ATTRIBUTE_MEMBER_ADD_VALUE(MemberName, AttributeName, ...)			ZE_META_ATTRIBUTE(#MemberName, "+"#AttributeName, __VA_ARGS__)
#define ZE_META_ATTRIBUTE_MEMBER_REMOVE_VALUE(MemberName, AttributeName, ...)		ZE_META_ATTRIBUTE(#MemberName, "-"#AttributeName, __VA_ARGS__)
#define ZE_META_ATTRIBUTE_MEMBER_CLEAR_VALUES(MemberName, AttributeName)			ZE_META_ATTRIBUTE(#MemberName, "%"#AttributeName)

#define ZE_META_ATTRIBUTE_PROPERTY(PropertyName, AttributeName, ...)				ZE_META_ATTRIBUTE("@"#PropertyName, #AttributeName, __VA_ARGS__)
#define ZE_META_ATTRIBUTE_PROPERTY_DELETE(PropertyName, AttributeName)				ZE_META_ATTRIBUTE("@"#PropertyName, "!"#AttributeName)
#define ZE_META_ATTRIBUTE_PROPERTY_ADD_VALUE(PropertyName, AttributeName, ...)		ZE_META_ATTRIBUTE("@"#PropertyName, "+"#AttributeName, __VA_ARGS__)
#define ZE_META_ATTRIBUTE_PROPERTY_REMOVE_VALUE(PropertyName, AttributeName, ...)	ZE_META_ATTRIBUTE("@"#PropertyName, "-"#AttributeName, __VA_ARGS__)
#define ZE_META_ATTRIBUTE_PROPERTY_CLEAR_VALUES(PropertyName, AttributeName)		ZE_META_ATTRIBUTE("@"#PropertyName, "%"#AttributeName)

#define ZE_META_ATTRIBUTE_METHOD(MethodName, AttributeName, ...)					ZE_META_ATTRIBUTE("~"#MethodName, "#"AttributeName, __VA_ARGS__)
#define ZE_META_ATTRIBUTE_METHOD_DELETE(MethodName, AttributeName)					ZE_META_ATTRIBUTE("~"#MethodName, "!"#AttributeName)
#define ZE_META_ATTRIBUTE_METHOD_ADD_VALUE(MethodName, AttributeName, ...)			ZE_META_ATTRIBUTE("~"#MethodName, "+"#AttributeName, __VA_ARGS__)
#define ZE_META_ATTRIBUTE_METHOD_REMOVE_VALUE(MethodName, AttributeName, ...)		ZE_META_ATTRIBUTE("~"#MethodName, "-"#AttributeName, __VA_ARGS__)
#define ZE_META_ATTRIBUTE_METHOD_CLEAR_VALUES(MethodName, AttributeName)			ZE_META_ATTRIBUTE("~"#MethodName, "%"#AttributeName)

struct ZEAttribute
{
	const char*		Name;
	const char**	Values;
	ZESize			ValueCount;
};
