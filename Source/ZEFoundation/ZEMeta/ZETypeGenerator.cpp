//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETypeGenerator.cpp
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

#include "ZETypeGenerator.h"

#ifndef NULL
#define NULL 0
#endif

ZEType ZETypeGenerator<void>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_VOID;
	return Type;
}

ZEType ZETypeGenerator<ZEUInt8>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_UNSIGNED_INTEGER_8;
	return Type;
}

ZEType ZETypeGenerator<ZEUInt16>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_UNSIGNED_INTEGER_16;
	return Type;
}

ZEType ZETypeGenerator<ZEUInt32>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_UNSIGNED_INTEGER_32;
	return Type;
}

ZEType ZETypeGenerator<ZEUInt64>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_UNSIGNED_INTEGER_64;
	return Type;
}

ZEType ZETypeGenerator<ZEInt8>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_8;
	return Type;
}

ZEType ZETypeGenerator<ZEInt16>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_16;
	return Type;
}

ZEType ZETypeGenerator<ZEInt32>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_32;
	return Type;
}

ZEType ZETypeGenerator<ZEInt64>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_INTEGER_64;
	return Type;
}

ZEType ZETypeGenerator<float>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_FLOAT;
	return Type;
}

ZEType ZETypeGenerator<double>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_DOUBLE;
	return Type;
}

ZEType ZETypeGenerator<bool>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_BOOLEAN;
	return Type;
}

ZEType ZETypeGenerator<ZEVector2>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR2;
	return Type;
}

ZEType ZETypeGenerator<ZEVector3>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR3;
	return Type;
}

ZEType ZETypeGenerator<ZEVector4>:: GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_VECTOR4;
	return Type;
}

ZEType ZETypeGenerator<ZEQuaternion>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_QUATERNION;
	return Type;
}

ZEType ZETypeGenerator<ZEMatrix3x3>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX3X3;
	return Type;
}

ZEType ZETypeGenerator<ZEMatrix4x4>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_MATRIX4X4;
	return Type;
}

ZEType ZETypeGenerator<ZEString>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_STRING;
	return Type;
}

ZEType ZETypeGenerator<ZEClass>::GetType()
{
	ZEType Type;
	Type.Type = ZE_TT_CLASS;
	return Type;
}
