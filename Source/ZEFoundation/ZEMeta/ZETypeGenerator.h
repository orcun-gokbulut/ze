//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZETypeGenerator.h
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
#ifndef __ZE_TYPE_GENERATOR_H__
#define __ZE_TYPE_GENERATOR_H__

#include "ZETypes.h"
#include "ZEType.h"

class ZEVector2;
class ZEVector3;
class ZEVector4;
class ZEMatrix3x3;
class ZEMatrix4x4;
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
class ZETypeGenerator;

template <typename T>
class ZETypeGenerator<const T>
{
	public:
		static ZEType GetType()
		{
			ZEType Type = ZE_TT_UNDEFINED;
			Type.Class = 0;
			Type.TypeQualifier = ZE_TT_VALUE;
			return Type;
		}
};

template <typename T>
class ZETypeGenerator<T *>
{
	public:
		static ZEType GetType()
		{
			ZEType Type = ZETypeGenerator<T>::GetType();
			
			if (Type.Type == ZE_TT_OBJECT)
			{
				Type.Type = ZE_TT_OBJECT_PTR;
			}
			else if (Type.Type == ZE_TT_CLASS)
			{
				return Type;
			}

			return ZEType();
		}
};

template <typename T>
class ZETypeGenerator<const T *>
{
	public:
		static ZEType GetType()
		{
			return ZEType();
		}
};

template <typename T>
class ZETypeGenerator<T &>
{
	public:
		static ZEType GetType()
		{
			ZEType Type = ZETypeGenerator<T>::GetType();
			Type.Class = 0;
			Type.TypeQualifier = ZE_TQ_REFERENCE;
			return Type;
		}
};

template <typename T>
class ZETypeGenerator<const T &>
{
	public:
		static ZEType GetType()
		{
			ZEType Type = ZETypeGenerator<T>::GetType();
			Type.Class = NULL;
			Type.TypeQualifier = ZE_TQ_CONST_REFERENCE;
			return Type;
		}
};

template <>
class ZETypeGenerator<void>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEUInt8>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEUInt16>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEUInt32>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEUInt64>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEInt8>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEInt16>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEInt32>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEInt64>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<float>
{
	public:
		static ZEType GetType();
};


template <>
class ZETypeGenerator<double>
{
	public:
		static ZEType GetType();
};


template <>
class ZETypeGenerator<bool>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEVector2>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEVector3>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEVector4>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEQuaternion>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEMatrix3x3>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEMatrix4x4>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEString>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEObject>
{
	public:
		static ZEType GetType();
};

template <>
class ZETypeGenerator<ZEClass*>
{
	public:
		static ZEType GetType();
};

template <typename T>
class ZETypeGenerator : Derived_from<T, ZEObject>
{
	public:
		static ZEType GetType()
		{
			ZEType Type;
			Type.Type = ZE_TT_OBJECT;
			Type.TypeQualifier = ZE_TQ_VALUE;
			Type.Class = T::Class();
			return Type;
		}
};

#endif
