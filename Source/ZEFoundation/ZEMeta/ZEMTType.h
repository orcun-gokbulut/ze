//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMTType.h
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

#include "ZEDS/ZEString.h"

enum ZEMTBaseType : unsigned
{
	ZEMT_BT_UNDEFINED				= 0,
	ZEMT_BT_VOID					= 1,
	ZEMT_BT_INTEGER_8				= 2,
	ZEMT_BT_INTEGER_16				= 3,
	ZEMT_BT_INTEGER_32				= 4,
	ZEMT_BT_INTEGER_64				= 5,
	ZEMT_BT_UNSIGNED_INTEGER_8		= 6,
	ZEMT_BT_UNSIGNED_INTEGER_16		= 7,
	ZEMT_BT_UNSIGNED_INTEGER_32		= 8,
	ZEMT_BT_UNSIGNED_INTEGER_64		= 9,
	ZEMT_BT_FLOAT					= 10,
	ZEMT_BT_DOUBLE					= 11,
	ZEMT_BT_BOOLEAN					= 12,
	ZEMT_BT_STRING					= 13,
	ZEMT_BT_VECTOR2					= 14,
	ZEMT_BT_VECTOR2D				= 15,
	ZEMT_BT_VECTOR3					= 16,
	ZEMT_BT_VECTOR3D				= 17,
	ZEMT_BT_VECTOR4					= 18,
	ZEMT_BT_VECTOR4D				= 19,
	ZEMT_BT_QUATERNION				= 20,
	ZEMT_BT_MATRIX3X3				= 21,
	ZEMT_BT_MATRIX3X3D				= 22,
	ZEMT_BT_MATRIX4X4				= 23,
	ZEMT_BT_MATRIX4X4D				= 24,
	ZEMT_BT_OBJECT					= 25,
	ZEMT_BT_OBJECT_PTR				= 26,
	ZEMT_BT_OBJECT_HOLDER			= 27,
	ZEMT_BT_CLASS					= 28,
	ZEMT_BT_ENUMERATOR				= 29,
	ZEMT_BT_FLAGS					= 30
};

enum ZEMTCollectionType : unsigned
{
	ZEMT_CT_NONE,
	ZEMT_CT_ARRAY,
	ZEMT_CT_LIST,
	ZEMT_CT_PROPERTY
};

enum ZEMTTypeQualifier : unsigned
{
	ZEMT_TQ_VALUE,
	ZEMT_TQ_CONST_VALUE,
	ZEMT_TQ_REFERENCE,
	ZEMT_TQ_CONST_REFERENCE
};

class ZEMTDeclaration;
class ZEClass;
class ZEMTEnumerator;
template<typename T, class E> class ZEMTTypeGenerator;

class ZEMTType
{
	template<typename T, class E> friend class ZEMTTypeGenerator;
	template<typename T, class E> friend class ZEMTTypeGeneratorDynamic;
	private:
		ZEMTBaseType			BaseType : 5;
		ZEMTTypeQualifier		BaseQualifier : 2;
		ZEMTCollectionType		CollectionType : 2;
		ZEMTTypeQualifier		CollectionQualifier : 2;
		bool					CannonicalBit : 1;
		bool					Binded : 1;
		ZEUInt64				Declaration : 51;
	
	public:
		void					SetType(ZEMTBaseType BaseType, ZEMTTypeQualifier BaseQualifier, ZEMTCollectionType CollectionType, ZEMTTypeQualifier CollectionQualifier, ZEMTDeclaration* Declaration);
		void					SetTypeDynamic(ZEMTBaseType BaseType, ZEMTTypeQualifier BaseQualifier, ZEMTCollectionType CollectionType, ZEMTTypeQualifier CollectionQualifier, const char* DeclarationName);

		void					SetBaseType(ZEMTBaseType Type, ZEMTDeclaration* Declaration);
		ZEMTBaseType			GetBaseType() const;
		
		void					SetBaseQualifier(ZEMTTypeQualifier Qualifier);
		ZEMTTypeQualifier		GetBaseQualifier() const;
		
		void					SetCollectionType(ZEMTCollectionType Type);
		ZEMTCollectionType		GetCollectionType() const;

		void					SetCollectionQualifier(ZEMTTypeQualifier Qualifier);
		ZEMTTypeQualifier		GetCollectionQualifier() const;

		void					SetDeclaration(ZEMTDeclaration* Declaration);
		ZEMTDeclaration*		GetDeclaration() const;

		ZEClass*				GetClass() const;
		ZEMTEnumerator*			GetEnumerator() const;
		
		void					SetBinded(bool Binded);

		bool					IsBinded() const;
		bool					IsValid() const;
		bool					IsReference() const;
		bool					IsConst() const;
		bool					IsCollection() const;
		bool					IsCollectionReference() const;
		bool					IsCollectionConst() const;
		ZEMTType				GetCollectionItemType() const;

		ZESize					GetInstanceSize() const;
		void*					CreateInstance() const;
		void*					CloneInstance(const void* Instance) const;
		bool					DestroyInstance(void* Instance) const;
		bool					ConstructInstance(void* Instance) const;
		bool					CopyConstructInstance(void* DestinationInstance, const void* SourceInstance) const;
		bool					DeconstructInstance(void* Instance) const;
		bool					AssignInstance(void* DestinationInstance, const void* SourceInstance) const;

		bool					operator==(const ZEMTType& Other) const;
		bool					operator!=(const ZEMTType& Other) const;

		ZEString				ToString() const;

								ZEMTType();
								ZEMTType(ZEMTBaseType Type, ZEMTTypeQualifier TypeQualifier,
									ZEMTCollectionType CollectionType, ZEMTTypeQualifier CollectionQualifier,
									ZEMTDeclaration* Declaration);

		static bool				Equal(const ZEMTType& A, const ZEMTType& B);

};
