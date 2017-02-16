//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMTPropertySignatureGenerator.cpp
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

#include "ZEMTPropertySignatureGenerator.h"

ZEMTPropertySignature ZEMTPropertySignatureGenerator<void>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_NULL;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEUInt8>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_UNSIGNED_INTEGER_8;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEUInt16>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_UNSIGNED_INTEGER_16;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEUInt32>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_UNSIGNED_INTEGER_32;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEUInt64>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_UNSIGNED_INTEGER_64;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEInt8>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_INTEGER_8;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEInt16>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_INTEGER_16;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEInt32>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_INTEGER_32;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEInt64>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_INTEGER_64;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<float>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_FLOAT;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<double>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_DOUBLE;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<bool>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_BOOLEAN;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEVector2>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_VECTOR2;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEVector3>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_VECTOR3;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEVector4>:: GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_VECTOR4;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEQuaternion>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_QUATERNION;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEMatrix3x3>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_MATRIX3X3;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEMatrix4x4>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_MATRIX4X4;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}

ZEMTPropertySignature ZEMTPropertySignatureGenerator<ZEString>::GetType()
{
	ZEMTPropertySignature Type;
	Type.Type = ZE_VRT_STRING;
	Type.BaseClass = NULL;
	Type.CanonicalType = ZE_CT_VALUE;
	return Type;
}
