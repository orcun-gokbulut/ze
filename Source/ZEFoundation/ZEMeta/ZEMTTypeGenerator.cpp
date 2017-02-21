//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMTTypeGenerator.cpp
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

#include "ZEMTTypeGenerator.h"

#ifndef NULL
#define NULL 0
#endif

ZEMTType ZEMTTypeGenerator<void>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_VOID;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEUInt8>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_UNSIGNED_INTEGER_8;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEUInt16>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_UNSIGNED_INTEGER_16;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEUInt32>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_UNSIGNED_INTEGER_32;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEUInt64>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_UNSIGNED_INTEGER_64;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEInt8>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_INTEGER_8;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEInt16>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_INTEGER_16;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEInt32>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_INTEGER_32;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEInt64>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_INTEGER_64;
	return Type;
}

ZEMTType ZEMTTypeGenerator<float>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_FLOAT;
	return Type;
}

ZEMTType ZEMTTypeGenerator<double>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_DOUBLE;
	return Type;
}

ZEMTType ZEMTTypeGenerator<bool>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_BOOLEAN;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEVector2>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_VECTOR2;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEVector2d>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_VECTOR2D;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEVector3>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_VECTOR3;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEVector3d>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_VECTOR3D;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEVector4>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_VECTOR4;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEVector4d>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_VECTOR4D;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEQuaternion>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_QUATERNION;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEMatrix3x3>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_MATRIX3X3;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEMatrix3x3d>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_MATRIX3X3D;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEMatrix4x4>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_MATRIX4X4;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEMatrix4x4d>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_MATRIX4X4D;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEString>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_STRING;
	return Type;
}

ZEMTType ZEMTTypeGenerator<ZEClass*>::GetType()
{
	ZEMTType Type;
	Type.Type = ZEMT_BT_CLASS;
	return Type;
}
