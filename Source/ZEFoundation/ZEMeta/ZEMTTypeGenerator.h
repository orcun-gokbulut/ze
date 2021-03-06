//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMTTypeGenerator.h
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
#include "ZEMTType.h"
#include <type_traits>

class ZEVector2;
class ZEVector2d;
class ZEVector3;
class ZEVector3d;
class ZEVector4;
class ZEVector4d;
class ZEMatrix3x3;
class ZEMatrix3x3d;
class ZEMatrix4x4;
class ZEMatrix4x4d;
class ZEQuaternion;
class ZEString;
class ZEClass;
class ZEObject;
template<typename Type> class ZEHolder;
template<typename ZEItemType, typename ZEAllocatorType, typename ZELockType>
class ZEArray;

template <typename T, class Enable = void>
class ZEMTTypeGenerator 
{
	public:
		static ZEMTType GetType()
		{
			return ZEMTType();
		}
};

template <typename T>
class ZEMTTypeGenerator<const T>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type = ZEMTTypeGenerator<T>::GetType();
			if (Type.BaseType == ZEMT_BT_UNDEFINED)
				return ZEMTType();

			if (Type.IsCollection())
			{
				if (Type.CollectionQualifier == ZEMT_TQ_VALUE)
					Type.CollectionQualifier = ZEMT_TQ_CONST_VALUE;
				else if (Type.CollectionQualifier == ZEMT_TQ_REFERENCE)
					Type.CollectionQualifier = ZEMT_TQ_CONST_REFERENCE;
				else
					return ZEMTType();
			}
			else
			{
				// const ZEHolder<>
				if (Type.BaseType == ZEMT_BT_OBJECT_HOLDER)
						return ZEMTType();

				if (Type.BaseQualifier == ZEMT_TQ_VALUE)
					Type.BaseQualifier = ZEMT_TQ_CONST_VALUE;
				else if (Type.BaseQualifier == ZEMT_TQ_REFERENCE)
					Type.BaseQualifier = ZEMT_TQ_CONST_REFERENCE;
				else
					return ZEMTType();
			}

			return Type;
		}
};

template <typename T>
class ZEMTTypeGenerator<T&>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type = ZEMTTypeGenerator<T>::GetType();
			if (Type.BaseType == ZEMT_BT_UNDEFINED)
				return ZEMTType();

			if (Type.IsCollection())
			{
				if (Type.CollectionQualifier == ZEMT_TQ_VALUE)
					Type.CollectionQualifier = ZEMT_TQ_REFERENCE;
				else if (Type.CollectionQualifier == ZEMT_TQ_CONST_VALUE)
					Type.CollectionQualifier = ZEMT_TQ_CONST_REFERENCE;
				else
					return ZEMTType();
			}
			else
			{
				if (Type.BaseQualifier == ZEMT_TQ_VALUE)
					Type.BaseQualifier = ZEMT_TQ_REFERENCE;
				else if (Type.BaseQualifier == ZEMT_TQ_CONST_VALUE)
					Type.BaseQualifier = ZEMT_TQ_CONST_REFERENCE;
				else
					return ZEMTType();
			}

			return Type;
		}
};

template <typename T>
class ZEMTTypeGenerator<T, typename std::enable_if<!std::is_const<T>::value && std::is_base_of<ZEObject, T>::value>::type>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type;
			Type.BaseType = ZEMT_BT_OBJECT;
			Type.BaseQualifier = ZEMT_TQ_VALUE;
			Type.Declaration = (ZEUInt64)T::Class();
			return Type;
		}
};

template <typename T>
class ZEMTTypeGenerator<T*, typename std::enable_if<std::is_base_of<ZEObject, T>::value>::type>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type;
			Type.BaseType = ZEMT_BT_OBJECT_PTR;
			Type.BaseQualifier = ZEMT_TQ_VALUE;
			Type.Declaration = (ZEUInt64)T::Class();
			return Type;
		}
};


template <typename T>
class ZEMTTypeGenerator<const T*, typename std::enable_if<std::is_base_of<ZEObject, T>::value>::type>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type;
			Type.BaseType = ZEMT_BT_OBJECT_PTR;
			Type.BaseQualifier = ZEMT_TQ_CONST_VALUE;
			Type.Declaration = (ZEUInt64)T::Class();
			return Type;
		}
};

template <typename T>
class ZEMTTypeGenerator<ZEHolder<T>, typename std::enable_if<std::is_base_of<ZEObject, T>::value>::type>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type;
			Type.BaseType = ZEMT_BT_OBJECT_HOLDER;
			Type.BaseQualifier = ZEMT_TQ_VALUE;
			Type.Declaration = (ZEUInt64)T::Class();
			return Type;
		}
};

template <typename T>
class ZEMTTypeGenerator<ZEHolder<const T>, typename std::enable_if<std::is_base_of<ZEObject, T>::value>::type>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type;
			Type.BaseType = ZEMT_BT_OBJECT_HOLDER;
			Type.BaseQualifier = ZEMT_TQ_CONST_VALUE;
			Type.Declaration = (ZEUInt64)T::Class();
			return Type;
		}
};

template <typename T, typename A, typename L>
class ZEMTTypeGenerator<ZEArray<T, A, L>>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type = ZEMTTypeGenerator<T>::GetType();
			
			if (!Type.IsValid() || Type.IsReference() || Type.IsCollection())
				return ZEMTType();

			if (Type.IsConst() && (Type.GetBaseType() != ZEMT_BT_OBJECT_PTR && Type.GetBaseType() != ZEMT_BT_OBJECT_HOLDER))
				return ZEMTType();

			Type.CollectionType = ZEMT_CT_ARRAY;
			Type.CollectionQualifier = ZEMT_TQ_VALUE;
			return Type;
		}
};

template <>
class ZEMTTypeGenerator<void>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEUInt8>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEUInt16>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEUInt32>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEUInt64>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEInt8>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEInt16>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEInt32>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEInt64>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<float>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<double>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<bool>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEVector2>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEVector2d>
{
public:
	static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEVector3>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEVector3d>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEVector4>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEVector4d>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEQuaternion>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEMatrix3x3>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEMatrix3x3d>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEMatrix4x4>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEMatrix4x4d>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEString>
{
	public:
		static ZEMTType GetType();
};

template <>
class ZEMTTypeGenerator<ZEClass*>
{
	public:
		static ZEMTType GetType();
};
