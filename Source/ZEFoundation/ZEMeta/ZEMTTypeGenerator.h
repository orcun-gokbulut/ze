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

template<class T, class B> struct Derived_from 
{
	static void constraints(T* p) { B* pb = p; }
	Derived_from() { void(*p)(T*) = constraints; }
};

template <typename T>
class ZEMTTypeGenerator;

template <typename T>
class ZEMTTypeGenerator<const T>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type = ZEMT_BT_UNDEFINED;
			Type.Class = 0;
			Type.TypeQualifier = ZE_TT_VALUE;
			return Type;
		}
};

template <typename T>
class ZEMTTypeGenerator<T *>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type = ZEMTTypeGenerator<T>::GetType();
			
			if (Type.Type == ZEMT_BT_OBJECT)
			{
				Type.Type = ZEMT_BT_OBJECT_PTR;
			}
			else if (Type.Type == ZEMT_BT_CLASS)
			{
				return Type;
			}

			return ZEMTType();
		}
};

template <typename T>
class ZEMTTypeGenerator<const T *>
{
	public:
		static ZEMTType GetType()
		{
			return ZEMTType();
		}
};

template <typename T>
class ZEMTTypeGenerator<T &>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type = ZEMTTypeGenerator<T>::GetType();
			Type.Class = 0;
			Type.TypeQualifier = ZEMT_TQ_REFERENCE;
			return Type;
		}
};

template <typename T>
class ZEMTTypeGenerator<const T &>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type = ZEMTTypeGenerator<T>::GetType();
			Type.Class = NULL;
			Type.TypeQualifier = ZEMT_TQ_CONST_REFERENCE;
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
class ZEMTTypeGenerator<ZEObject>
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

template <typename T>
class ZEMTTypeGenerator : Derived_from<T, ZEObject>
{
	public:
		static ZEMTType GetType()
		{
			ZEMTType Type;
			Type.Type = ZEMT_BT_OBJECT;
			Type.TypeQualifier = ZEMT_TQ_VALUE;
			Type.Class = T::Class();
			return Type;
		}
};
