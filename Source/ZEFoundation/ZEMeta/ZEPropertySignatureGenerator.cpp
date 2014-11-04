//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPropertySignatureGenerator.cpp
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

#include "ZEPropertySignatureGenerator.h"

ZEPropertySignature ZEPropertySignatureGenerator<void>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_NULL;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEUInt8>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_UNSIGNED_INTEGER_8;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEUInt16>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_UNSIGNED_INTEGER_16;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEUInt32>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_UNSIGNED_INTEGER_32;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEUInt64>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_UNSIGNED_INTEGER_64;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEInt8>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_INTEGER_8;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEInt16>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_INTEGER_16;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEInt32>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_INTEGER_32;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEInt64>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_INTEGER_64;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<float>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_FLOAT;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<double>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_DOUBLE;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<bool>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_BOOLEAN;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEVector2>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_VECTOR2;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEVector3>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_VECTOR3;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEVector4>:: GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_VECTOR4;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEQuaternion>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_QUATERNION;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEMatrix3x3>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_MATRIX3X3;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEMatrix4x4>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_MATRIX4X4;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEPropertySignature ZEPropertySignatureGenerator<ZEString>::GetType()
{
	ZEPropertySignature Type;
	Type.Type = ZE_VRT_STRING;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}
