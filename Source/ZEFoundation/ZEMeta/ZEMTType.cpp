//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMTType.cpp
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

#include "ZEMTType.h"

#include "ZEClass.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEVectord.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEMatrixd.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEObject.h"


template<typename ZEItemType>
void CastAndCopy(void* Destination, const void* Source)
{
	*static_cast<ZEItemType*>(Destination) = *static_cast<const ZEItemType*>(Source);
}

bool ZEMTType::IsValid() const
{
	return Type == ZEMT_BT_UNDEFINED;
}

bool ZEMTType::IsReference() const
{
	return (TypeQualifier == ZEMT_TQ_REFERENCE || TypeQualifier == ZEMT_TQ_CONST_REFERENCE);
}

bool ZEMTType::IsConst() const
{
	return (TypeQualifier == ZEMT_TQ_CONST_VALUE || TypeQualifier == ZEMT_TQ_CONST_REFERENCE);
}

bool ZEMTType::IsCollection() const
{
	return CollectionType != ZEMT_CT_NONE;
}

bool ZEMTType::IsCollectionReference() const
{
	return (CollectionQualifier == ZEMT_TQ_REFERENCE || CollectionQualifier == ZEMT_TQ_CONST_REFERENCE);
}

bool ZEMTType::IsCollectionConst() const
{
	return (CollectionQualifier == ZEMT_TQ_CONST_VALUE || CollectionQualifier == ZEMT_TQ_CONST_REFERENCE);
}

ZEMTType ZEMTType::GetCollectionItemType() const
{
	return ZEMTType(Type, TypeQualifier, ZEMT_CT_NONE, ZEMT_TQ_VALUE, Class, Enumerator);
}

ZESize ZEMTType::GetInstanceSize() const
{
	if (!IsCollection())
	{
		if (TypeQualifier == ZEMT_TQ_REFERENCE || TypeQualifier == ZEMT_TQ_CONST_REFERENCE)
			return sizeof(void*);

		switch (Type)
		{
			default:
			case ZEMT_BT_UNDEFINED:
				return 0;

			case ZEMT_BT_VOID:
				return 0;

			case ZEMT_BT_INTEGER_8:
				return sizeof(ZEInt8);

			case ZEMT_BT_INTEGER_16:
				return sizeof(ZEInt16);

			case ZEMT_BT_INTEGER_32:
				return sizeof(ZEInt32);

			case ZEMT_BT_INTEGER_64:
				return sizeof(ZEInt64);

			case ZEMT_BT_UNSIGNED_INTEGER_8:
				return sizeof(ZEUInt8);

			case ZEMT_BT_UNSIGNED_INTEGER_16:
				return sizeof(ZEUInt16);

			case ZEMT_BT_UNSIGNED_INTEGER_32:
				return sizeof(ZEUInt32);

			case ZEMT_BT_UNSIGNED_INTEGER_64:
				return sizeof(ZEUInt64);

			case ZEMT_BT_FLOAT:
				return sizeof(float);

			case ZEMT_BT_DOUBLE:
				return sizeof(double);

			case ZEMT_BT_BOOLEAN:
				return sizeof(bool);

			case ZEMT_BT_STRING:
				return sizeof(ZEString);

			case ZEMT_BT_QUATERNION:
				return sizeof(ZEQuaternion);

			case ZEMT_BT_VECTOR2:
				return sizeof(ZEVector2);

			case ZEMT_BT_VECTOR2D:
				return sizeof(ZEVector2d);

			case ZEMT_BT_VECTOR3:
				return sizeof(ZEVector3);

			case ZEMT_BT_VECTOR3D:
				return sizeof(ZEVector3d);

			case ZEMT_BT_VECTOR4:
				return sizeof(ZEVector4);

			case ZEMT_BT_VECTOR4D:
				return sizeof(ZEVector4d);

			case ZEMT_BT_MATRIX3X3:
				return sizeof(ZEMatrix3x3);

			case ZEMT_BT_MATRIX3X3D:
				return sizeof(ZEMatrix3x3d);

			case ZEMT_BT_MATRIX4X4:
				return sizeof(ZEMatrix4x4);

			case ZEMT_BT_MATRIX4X4D:
				return sizeof(ZEMatrix4x4d);

			case ZEMT_BT_OBJECT:
				if (Class == NULL)
					return 0;

				return Class->GetSizeOfObject();

			case ZEMT_BT_OBJECT_PTR:
				return sizeof(ZEObject*);

			case ZEMT_BT_OBJECT_HOLDER:
				return sizeof(ZEHolder<ZEReferenceCounted>);

			case ZEMT_BT_CLASS:
				return sizeof(ZEClass*);

			case ZEMT_BT_ENUMERATOR:
				return sizeof(int);

			case ZEMT_BT_FLAGS:
				return sizeof(int);
		}
	}
	else if (CollectionType != ZEMT_CT_ARRAY)
	{
		if (IsCollectionReference())
			return sizeof(void*);
		else
			return sizeof(ZEArray<void*>);
	}


	return 0;
}

void* ZEMTType::CreateInstance() const
{
	if (!IsCollection())
	{
		if (IsReference())
			return new void*;

		switch (Type)
		{
			case ZEMT_BT_UNDEFINED:
			case ZEMT_BT_VOID:
				return NULL;

			case ZEMT_BT_INTEGER_8:
				return new ZEInt8;

			case ZEMT_BT_INTEGER_16:
				return new ZEInt16;

			case ZEMT_BT_INTEGER_32:
				return new ZEInt32;

			case ZEMT_BT_INTEGER_64:
				return new ZEInt64;

			case ZEMT_BT_UNSIGNED_INTEGER_8:
				return new ZEUInt8;

			case ZEMT_BT_UNSIGNED_INTEGER_16:
				return new ZEUInt16;

			case ZEMT_BT_UNSIGNED_INTEGER_32:
				return new ZEUInt32;

			case ZEMT_BT_UNSIGNED_INTEGER_64:
				return new ZEUInt64;

			case ZEMT_BT_FLOAT:
				return new float;

			case ZEMT_BT_DOUBLE:
				return new double;

			case ZEMT_BT_BOOLEAN:
				return new bool;

			case ZEMT_BT_STRING:
				return new ZEString;

			case ZEMT_BT_QUATERNION:
				return new ZEQuaternion;

			case ZEMT_BT_VECTOR2:
				return new ZEVector2;

			case ZEMT_BT_VECTOR2D:
				return new ZEVector2d;

			case ZEMT_BT_VECTOR3:
				return new ZEVector3;

			case ZEMT_BT_VECTOR3D:
				return new ZEVector3d;

			case ZEMT_BT_VECTOR4:
				return new ZEVector4;

			case ZEMT_BT_VECTOR4D:
				return new ZEVector4d;

			case ZEMT_BT_MATRIX3X3:
				return new ZEMatrix3x3;

			case ZEMT_BT_MATRIX3X3D:
				return new ZEMatrix3x3d;

			case ZEMT_BT_MATRIX4X4:
				return new ZEMatrix4x4;

			case ZEMT_BT_MATRIX4X4D:
				return new ZEMatrix4x4d;

			case ZEMT_BT_OBJECT:
				if (Class == NULL)
					return NULL;

				return Class->CreateInstance();

			case ZEMT_BT_OBJECT_PTR:
				if (Class == NULL)
					return NULL;

				return new ZEObject*;

			case ZEMT_BT_OBJECT_HOLDER:
				if (Class == NULL)
					return NULL;

				return new ZEHolder<ZEReferenceCounted>();

			case ZEMT_BT_CLASS:
				return NULL;

			case ZEMT_BT_ENUMERATOR:
				return new int;

			case ZEMT_BT_FLAGS:
				return new int;
			}
		}
		else if (CollectionType == ZEMT_CT_ARRAY)
		{
			if (CollectionQualifier == ZEMT_TQ_REFERENCE || CollectionQualifier == ZEMT_TQ_CONST_REFERENCE)
				return new void*;

			return new ZEArray<void*>();
		}

	return NULL;
}

bool ZEMTType::CopyInstance(void* Destination, const void* Source) const
{
	if (!IsCollection())
	{
		if (IsReference())
		{
			CastAndCopy<void*>(Destination, Source);
			return true;
		}

		switch (Type)
		{
			default:
			case ZEMT_BT_UNDEFINED:
			case ZEMT_BT_VOID:
				return false;

			case ZEMT_BT_INTEGER_8:
				CastAndCopy<ZEInt8>(Destination, Source);
				return true;

			case ZEMT_BT_INTEGER_16:
				CastAndCopy<ZEInt16>(Destination, Source);
				return true;

			case ZEMT_BT_INTEGER_32:
				CastAndCopy<ZEInt32>(Destination, Source);
				return true;

			case ZEMT_BT_INTEGER_64:
				CastAndCopy<ZEInt64>(Destination, Source);
				return true;

			case ZEMT_BT_UNSIGNED_INTEGER_8:
				CastAndCopy<ZEUInt8>(Destination, Source);
				return true;

			case ZEMT_BT_UNSIGNED_INTEGER_16:
				CastAndCopy<ZEUInt16>(Destination, Source);
				return true;

			case ZEMT_BT_UNSIGNED_INTEGER_32:
				CastAndCopy<ZEUInt32>(Destination, Source);
				return true;

			case ZEMT_BT_UNSIGNED_INTEGER_64:
				CastAndCopy<ZEUInt64>(Destination, Source);
				return true;

			case ZEMT_BT_FLOAT:
				CastAndCopy<float>(Destination, Source);
				return true;

			case ZEMT_BT_DOUBLE:
				CastAndCopy<double>(Destination, Source);
				return true;

			case ZEMT_BT_BOOLEAN:
				CastAndCopy<bool>(Destination, Source);
				return true;

			case ZEMT_BT_STRING:
				CastAndCopy<ZEString>(Destination, Source);
				return true;

			case ZEMT_BT_QUATERNION:
				CastAndCopy<ZEQuaternion>(Destination, Source);
				return true;

			case ZEMT_BT_VECTOR2:
				CastAndCopy<ZEVector2>(Destination, Source);
				return true;

			case ZEMT_BT_VECTOR2D:
				CastAndCopy<ZEVector2d>(Destination, Source);
				return true;

			case ZEMT_BT_VECTOR3:
				CastAndCopy<ZEVector3>(Destination, Source);
				return true;

			case ZEMT_BT_VECTOR3D:
				CastAndCopy<ZEVector3d>(Destination, Source);
				return true;

			case ZEMT_BT_VECTOR4:
				CastAndCopy<ZEVector4>(Destination, Source);
				return true;

			case ZEMT_BT_VECTOR4D:
				CastAndCopy<ZEVector4d>(Destination, Source);
				return true;

			case ZEMT_BT_MATRIX3X3:
				CastAndCopy<ZEMatrix3x3>(Destination, Source);
				return true;

			case ZEMT_BT_MATRIX3X3D:
				CastAndCopy<ZEMatrix4x4>(Destination, Source);
				return true;

			case ZEMT_BT_MATRIX4X4:
				CastAndCopy<ZEMatrix4x4>(Destination, Source);
				return true;

			case ZEMT_BT_MATRIX4X4D:
				CastAndCopy<ZEMatrix4x4d>(Destination, Source);
				return true;

			case ZEMT_BT_OBJECT:
				if (Class == NULL)
					return false;
			
				return Class->Copy(static_cast<ZEObject*>(Destination), static_cast<const ZEObject*>(Source));

			case ZEMT_BT_OBJECT_PTR:
				CastAndCopy<ZEObject*>(Destination, Source);
				return true;

			case ZEMT_BT_OBJECT_HOLDER:
				CastAndCopy<ZEHolder<ZEReferenceCounted>>(Destination, Source);
				return true;

			case ZEMT_BT_CLASS:
				CastAndCopy<ZEClass>(Destination, Source);
				return true;

			case ZEMT_BT_ENUMERATOR:
				CastAndCopy<int>(Destination, Source);
				return true;

			case ZEMT_BT_FLAGS:
				CastAndCopy<int>(Destination, Source);
				return true;
		}
	}
	else if (CollectionType != ZEMT_CT_ARRAY)
	{
		if (IsCollectionReference())
		{
			CastAndCopy<ZEArray<void*>>(Destination, Source);
			return true;
		}
		else
		{
			const ZEArray<ZEBYTE>* SourceArray = static_cast<const ZEArray<ZEBYTE>*>(Source);
			ZEArray<ZEBYTE>* DestinationArray = static_cast<ZEArray<ZEBYTE>*>(Destination);

			ZEMTType ItemType = GetCollectionItemType();
			ZESize ItemSize = ItemType.GetInstanceSize();

			DestinationArray->Resize(SourceArray->GetCount() * ItemSize);
			
			ZEBYTE* DestinationCursor = DestinationArray->GetCArray();
			ZESize ItemCount;
			SourceArray->LockRead();
			{
				ItemCount = SourceArray->GetCount();
				const ZEBYTE* SourceCursor = SourceArray->GetConstCArray();
				for (ZESize I = 0; I < ItemCount; I++)
				{
					if (!ItemType.CopyInstance(DestinationCursor, SourceCursor))
						return false;
					DestinationCursor += ItemSize;
					SourceCursor += ItemSize;
				}
			}
			SourceArray->UnlockRead();
			DestinationArray->Count = ItemCount;
			DestinationArray->Allocator.Size = ItemCount;

			return true;
		}
	}

	return false;
}

bool ZEMTType::ConstructInstance(void* Destination) const
{
	if (!IsCollection())
	{
		if (IsReference())
			return true;

		if (ZEMT_BT_OBJECT)
		{
			if (Class == NULL)
				return false;

			return Class->Construct(static_cast<ZEObject*>(Destination));
		}
		else if (ZEMT_BT_OBJECT_HOLDER)
		{
			new (static_cast<ZEHolder<ZEReferenceCounted>*>(Destination)) ZEHolder<ZEReferenceCounted>();
			return true;
		}
	}
	else 
	{
		if (IsCollectionReference())
			return true;

		if (CollectionType == ZEMT_CT_ARRAY)
		{
			new(static_cast<ZEArray<void*>*>(Destination)) ZEArray<void*>();
			return true;
		}
	}

	return false;
}

bool ZEMTType::DeconstructInstance(void* Destination) const
{
	if (!IsCollection())
	{
		if (IsReference())
			return true;

		if (Type == ZEMT_BT_STRING)
		{
			static_cast<ZEString*>(Destination)->~ZEString();
			return true;
		}
		else if (Type == ZEMT_BT_OBJECT)
		{
			return Class->Deconstruct(static_cast<ZEObject*>(Destination));
		}
		else if (Type == ZEMT_BT_OBJECT_HOLDER)
		{
			static_cast<ZEHolder<ZEReferenceCounted>*>(Destination)->~ZEHolder();
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		if (IsCollectionReference())
			return true;

		if (CollectionType == ZEMT_CT_ARRAY)
		{
			ZEArray<ZEBYTE>* Array = static_cast<ZEArray<ZEBYTE>*>(Destination);
			if (Type == ZEMT_BT_STRING || Type == ZEMT_BT_OBJECT || Type == ZEMT_BT_OBJECT_HOLDER)
			{
				ZEMTType ItemType = GetCollectionItemType();
				ZESize ItemSize = ItemType.GetInstanceSize();
				ZESize ItemCount;
				Array->LockWrite();
				{
					ZEBYTE* ArrayCursor = Array->GetCArray();
					ItemCount = Array->GetCount();
					for (ZESize I = 0; I < ItemCount; I++)
					{
						if (!ItemType.DeconstructInstance(ArrayCursor))
							return false;

						ArrayCursor += ItemSize;
					}
				}
				Array->UnlockWrite();

				delete Array;
				return true;
			}
			else
			{
				delete Array;
				return true;
			}
		}
	}
	
	return false;
}

bool ZEMTType::operator==(const ZEMTType& Other) const
{
	return Equal(*this, Other);
}

bool ZEMTType::operator!=(const ZEMTType& Other) const
{
	return !Equal(*this, Other);
}

ZEMTType::ZEMTType()
{
	Type = ZEMT_BT_UNDEFINED;
	TypeQualifier = ZEMT_TQ_VALUE;
	CollectionType = ZEMT_CT_NONE;
	Class = NULL;
	Enumerator = NULL;
}

ZEMTType::ZEMTType(ZEMTBaseType Type, ZEMTTypeQualifier TypeQualifier, ZEMTCollectionType CollectionType, ZEMTTypeQualifier CollectionQualifier, ZEClass* Class, ZEMTEnumerator* Enumerator)
{
	this->Type = Type;
	this->TypeQualifier = TypeQualifier;
	this->CollectionType = CollectionType;
	this->Enumerator = Enumerator;
	this->Class = Class;
}

bool ZEMTType::Equal(const ZEMTType& A, const ZEMTType& B)
{
	if (A.Type != B.Type || A.TypeQualifier != B.TypeQualifier || 
		A.CollectionType != B.CollectionType || A.CollectionQualifier != B.CollectionQualifier)
	{
		return false;
	}

	if (A.Type == ZEMT_BT_OBJECT || A.Type == ZEMT_BT_OBJECT_PTR || A.Type == ZEMT_BT_OBJECT_HOLDER) 
	{
		if (A.Class != B.Class)
			return false;
	}

	if (A.Type == ZEMT_BT_ENUMERATOR || A.Type == ZEMT_BT_FLAGS) 
	{
		if (A.Enumerator != B.Enumerator)
			return false;
	}

	return true;
}
