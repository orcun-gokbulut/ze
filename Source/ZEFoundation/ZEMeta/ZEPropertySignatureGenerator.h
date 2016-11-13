//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPropertySignatureGenerator.h
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
#include "ZEPropertySignature.h"

#include <type_traits>
#include "ZEProvider.h"

template <typename T>
class ZEPropertySignatureGenerator;

template <typename T>
class ZEPropertySignatureGenerator<const T>
{
	public:
		static ZEPropertySignature GetType()
		{
			ZEPropertySignature Type = ZEPropertySignatureGenerator<T>::GetType();
			Type.CanonicalType = ZE_CT_CONST_VALUE;
			return Type;
		}
};

template <typename T>
class ZEPropertySignatureGenerator<T *>
{
	public:
		static ZEPropertySignature GetType()
		{
			ZEPropertySignature Type = ZEPropertySignatureGenerator<T>::GetType();
			Type.CanonicalType = ZE_CT_POINTER;
			return Type;
		}
};

template <typename T>
class ZEPropertySignatureGenerator<const T *>
{
	public:
		static ZEPropertySignature GetType()
		{
			ZEPropertySignature Type = ZEPropertySignatureGenerator<T>::GetType();
			Type.CanonicalType = ZE_CT_CONST_POINTER;
			return Type;
		}
};

template <typename T>
class ZEPropertySignatureGenerator<T &>
{
	public:
		static ZEPropertySignature GetType()
		{
			ZEPropertySignature Type = ZEPropertySignatureGenerator<T>::GetType();
			Type.CanonicalType = ZE_CT_REFERENCE;
			return Type;
		}
};

template <typename T>
class ZEPropertySignatureGenerator<const T &>
{
	public:
		static ZEPropertySignature GetType()
		{
			ZEPropertySignature Type = ZEPropertySignatureGenerator<T>::GetType();
			Type.CanonicalType = ZE_CT_CONST_REFERENCE;
			return Type;
		}
};

template <>
class ZEPropertySignatureGenerator<void>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEUInt8>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEUInt16>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEUInt32>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEUInt64>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEInt8>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEInt16>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEInt32>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEInt64>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<float>
{
	public:
		static ZEPropertySignature GetType();
};


template <>
class ZEPropertySignatureGenerator<double>
{
	public:
		static ZEPropertySignature GetType();
};


template <>
class ZEPropertySignatureGenerator<bool>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEVector2>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEVector3>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEVector4>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEQuaternion>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEMatrix3x3>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEMatrix4x4>
{
	public:
		static ZEPropertySignature GetType();
};

template <>
class ZEPropertySignatureGenerator<ZEString>
{
	public:
		static ZEPropertySignature GetType();
};


template <typename T>
class ZEPropertySignatureGenerator
{
	public:
		static ZEPropertySignature GetType()
		{
			ZEPropertySignature Type;
			
			Type.BaseClass = ZEProvider::GetInstance()->GetClass(typeid(T).name());
			if (Type.BaseClass == NULL)
				Type.Type = ZE_VRT_UNDEFINED;

			Type.CanonicalType = ZE_CT_VALUE;
			return Type;
		}
};
