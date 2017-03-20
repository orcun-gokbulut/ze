//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMTPropertySignatureGenerator.h
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
#include "ZEClass.h"
#include "ZEMTPropertySignature.h"

#include <type_traits>
#include "ZEMTProvider.h"

template <typename T>
class ZEMTPropertySignatureGenerator;

template <typename T>
class ZEMTPropertySignatureGenerator<const T>
{
	public:
		static ZEMTPropertySignature GetType()
		{
			ZEMTPropertySignature Type = ZEMTPropertySignatureGenerator<T>::GetType();
			Type.CanonicalType = ZE_CT_CONST_VALUE;
			return Type;
		}
};

template <typename T>
class ZEMTPropertySignatureGenerator<T *>
{
	public:
		static ZEMTPropertySignature GetType()
		{
			ZEMTPropertySignature Type = ZEMTPropertySignatureGenerator<T>::GetType();
			Type.CanonicalType = ZE_CT_POINTER;
			return Type;
		}
};

template <typename T>
class ZEMTPropertySignatureGenerator<const T *>
{
	public:
		static ZEMTPropertySignature GetType()
		{
			ZEMTPropertySignature Type = ZEMTPropertySignatureGenerator<T>::GetType();
			Type.CanonicalType = ZE_CT_CONST_POINTER;
			return Type;
		}
};

template <typename T>
class ZEMTPropertySignatureGenerator<T &>
{
	public:
		static ZEMTPropertySignature GetType()
		{
			ZEMTPropertySignature Type = ZEMTPropertySignatureGenerator<T>::GetType();
			Type.CanonicalType = ZE_CT_REFERENCE;
			return Type;
		}
};

template <typename T>
class ZEMTPropertySignatureGenerator<const T &>
{
	public:
		static ZEMTPropertySignature GetType()
		{
			ZEMTPropertySignature Type = ZEMTPropertySignatureGenerator<T>::GetType();
			Type.CanonicalType = ZE_CT_CONST_REFERENCE;
			return Type;
		}
};

template <>
class ZEMTPropertySignatureGenerator<void>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEUInt8>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEUInt16>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEUInt32>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEUInt64>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEInt8>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEInt16>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEInt32>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEInt64>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<float>
{
	public:
		static ZEMTPropertySignature GetType();
};


template <>
class ZEMTPropertySignatureGenerator<double>
{
	public:
		static ZEMTPropertySignature GetType();
};


template <>
class ZEMTPropertySignatureGenerator<bool>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEVector2>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEVector3>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEVector4>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEQuaternion>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEMatrix3x3>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEMatrix4x4>
{
	public:
		static ZEMTPropertySignature GetType();
};

template <>
class ZEMTPropertySignatureGenerator<ZEString>
{
	public:
		static ZEMTPropertySignature GetType();
};


template <typename T>
class ZEMTPropertySignatureGenerator
{
	public:
		static ZEMTPropertySignature GetType()
		{
			ZEMTPropertySignature Type;
			
			Type.BaseClass = ZEMTProvider::GetInstance()->GetClass(typeid(T).name());
			if (Type.BaseClass == NULL)
				Type.Type = ZE_VRT_UNDEFINED;

			Type.CanonicalType = ZE_CT_VALUE;
			return Type;
		}
};
