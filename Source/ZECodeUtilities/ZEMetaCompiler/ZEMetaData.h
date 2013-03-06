//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMetaData.h
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
#ifndef __ZE_META_DATA_H__
#define __ZE_META_DATA_H__

#include "ZETypes.h"
#include "ZEDS/ZEList.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"

class ZEClassData;

enum ZEMetaTypeType
{
	ZE_MTT_UNDEFINED				= 0,
	ZE_MTT_NULL						= 1,

	ZE_MTT_INTEGER_8				= 2,
	ZE_MTT_INTEGER_16				= 3,
	ZE_MTT_INTEGER_32				= 4,
	ZE_MTT_UNSIGNED_INTEGER_8		= 5,
	ZE_MTT_UNSIGNED_INTEGER_16		= 6,
	ZE_MTT_UNSIGNED_INTEGER_32		= 7,

	ZE_MTT_INTEGER_64				= 16,
	ZE_MTT_UNSIGNED_INTEGER_64		= 17,

	ZE_MTT_FLOAT					= 32,
	ZE_MTT_DOUBLE					= 33,

	ZE_MTT_BOOLEAN					= 64,
	ZE_MTT_ENUMERATOR				= 65,

	ZE_MTT_STRING					= 128,
	ZE_MTT_QUATERNION				= 129,
	ZE_MTT_VECTOR2					= 130,
	ZE_MTT_VECTOR3					= 131,
	ZE_MTT_VECTOR4					= 132,
	ZE_MTT_MATRIX3X3				= 133,
	ZE_MTT_MATRIX4X4				= 134,

	ZE_MTT_CLASS					= 135,
	ZE_MTT_OBJECT					= 136,
	ZE_MTT_OBJECT_PTR				= 137,
	ZE_MTT_ARRAY					= 138,
	ZE_MTT_LIST						= 139
};

enum ZEMetaTypeQualifier
{
	ZE_MTQ_VALUE,
	ZE_MTQ_REFERENCE,
	ZE_MTQ_CONST_REFERENCE
};

class ZEMetaType
{
	public:
		ZEMetaTypeType			Type;
		ZEMetaTypeQualifier		TypeQualifier;

		ZEMetaTypeType			SubType;
		ZEMetaTypeQualifier		SubTypeQualifier;
		ZEString				SubTypeClassName;

		ZEClassData*			ClassData;
		ZEString				EnumName;

		bool					operator==(const ZEMetaType& Other) const;
		bool					operator!=(const ZEMetaType& Other) const;

		static bool				Equal(const ZEMetaType& A, const ZEMetaType& B);

		ZEMetaType();
		ZEMetaType(ZEMetaTypeType Type, ZEMetaTypeQualifier TypeQualifier = ZE_MTQ_VALUE, ZEMetaTypeType SubType = ZE_MTT_UNDEFINED, ZEMetaTypeQualifier SubTypeQualifier = ZE_MTQ_VALUE, ZEString ClassName = "", ZEClassData* ClassData = NULL);
		ZEMetaType(ZEMetaTypeType Type, ZEMetaTypeQualifier TypeQualifier, ZEString ClassName = "", ZEClassData* ClassData = NULL);
};

class ZEEnumParameterData
{
	public:
		ZEString						Name;
		ZEUInt32						Value;
};

class ZEForwardDeclared
{
	public:
		ZEString						HeaderFileDeclaredIn;
		ZEString						ClassName;
		ZEString						HeaderName;
};

class ZEEnumData
{
	public:
		ZEString						Name;
		ZEUInt32						Hash;
		ZEArray<ZEEnumParameterData*>	Parameters;

		ZEClassData*					BaseClass;

		virtual							~ZEEnumData();
};

class ZEAttributeData
{
	public:
		ZEString						Name;
		ZEArray<ZEString>				Parameters;

		ZEString						MemberOf;
};

class ZETypeData
{
	public:
		ZEString						Name;
		ZEUInt32						Hash;
		ZEArray<ZEAttributeData*>		Attributes;

		virtual							~ZETypeData();
};

class ZEEventParameterData
{
	public:
		ZEString						Name;
		ZEMetaType						Type;
		ZEClassData*					BaseClass;
		ZEEnumData*						EnumData;
};

class ZEEventData : public ZETypeData
{
	public:
		bool							IsStatic;
		ZEString						ClassName;

		ZEEventParameterData			ReturnParameter;
		ZEArray<ZEEventParameterData*>	Parameters;

		virtual							~ZEEventData();
};

struct ZEPropertyData : public ZETypeData
{
	public:
		bool							IsGeneratedByMetaCompiler;
		bool							IsStatic;
		bool							IsContainer;

		ZEMetaType						Type;
		ZEClassData*					BaseClass;
		ZEEnumData*						EnumData;

		ZESize							ID;
		ZEString						MemberOf;

		ZEString						Getter;
		ZEString						Setter;
};

class ZEMethodParameterData
{
	public:
		ZEString						Name;
		ZEMetaType						Type;
		ZEClassData*					BaseClass;
		ZEEnumData*						EnumData;
};

struct ZEMethodData : public ZETypeData
{
	public:
		bool							IsStatic;
		bool							IsEvent;
	
		ZEMethodParameterData			ReturnParameter;
		ZEArray<ZEMethodParameterData*>	Parameters;

		ZESize							ID;
		ZEString						MemberOf;

		virtual							~ZEMethodData();
};

class ZEClassData : public ZETypeData
{
	public:
		ZEClassData*					BaseClass;
		ZEArray<ZEPropertyData*>		Properties;
		ZEArray<ZEMethodData*>			Methods;
		ZEArray<ZEEventData*>			Events;

		bool							HasPublicConstructor;
		bool							IsAbstract;
		bool							IsBuiltInClass;

		virtual							~ZEClassData();
};

class ZEMetaData
{
	public:
		ZEArray<ZETypeData*>			TargetTypes;
		ZEArray<ZETypeData*>			Types;
		ZEArray<ZEEnumData*>			EnumTypes;
		ZEArray<ZEForwardDeclared*>		ForwardDeclaredClasses;

										~ZEMetaData();
};

#endif
