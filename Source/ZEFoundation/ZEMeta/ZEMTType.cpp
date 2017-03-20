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

#include "ZEMTProvider.h"
#include "ZEClass.h"
#include "ZEDS/ZEString.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEFormat.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEVectord.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEMath/ZEMatrixd.h"
#include "ZEMath/ZEQuaternion.h"
#include "ZEObject.h"


template<typename ZEItemType>
void CastAndAssign(void* DestinationInstance, const void* SourceInstance)
{
	*static_cast<ZEItemType*>(DestinationInstance) = *static_cast<const ZEItemType*>(SourceInstance);
}

template<typename ZEItemType>
void CastAndCopyConstruct(void* DestinationInstance, const void* SourceInstance)
{
	new(static_cast<ZEItemType*>(DestinationInstance)) ZEItemType(*static_cast<const ZEItemType*>(SourceInstance));
}

template<typename ZEItemType>
ZEItemType* CastAndClone(const void* Instance)
{
	return new ZEItemType(*static_cast<const ZEItemType*>(Instance));
}

void ZEMTType::SetType(ZEMTBaseType BaseType, ZEMTTypeQualifier BaseQualifier, ZEMTCollectionType CollectionType, ZEMTTypeQualifier CollectionQualifier, ZEMTDeclaration* Declaration)
{
	this->BaseType = BaseType;
	this->BaseQualifier = BaseQualifier;
	this->CollectionType = CollectionType;
	this->CollectionQualifier = CollectionQualifier;
	this->Binded = true;
	if (BaseType == ZEMT_BT_OBJECT || BaseType == ZEMT_BT_OBJECT_PTR || BaseType == ZEMT_BT_OBJECT_HOLDER || BaseType == ZEMT_BT_ENUMERATOR || BaseType == ZEMT_BT_FLAGS)
		SetDeclaration(Declaration);
	else
		SetDeclaration(NULL);
}

void ZEMTType::SetTypeDynamic(ZEMTBaseType BaseType, ZEMTTypeQualifier BaseQualifier, ZEMTCollectionType CollectionType, ZEMTTypeQualifier CollectionQualifier, const char* DeclarationName)
{
	this->BaseType = BaseType;
	this->BaseQualifier = BaseQualifier;
	this->CollectionType = CollectionType;
	this->CollectionQualifier = CollectionQualifier;
	this->Binded = true;

	if (BaseType == ZEMT_BT_OBJECT || BaseType == ZEMT_BT_OBJECT_PTR || BaseType == ZEMT_BT_OBJECT_HOLDER)
		SetDeclaration((ZEMTDeclaration*)ZEMTProvider::GetInstance()->GetClass(DeclarationName));
	else if (BaseType == ZEMT_BT_ENUMERATOR || BaseType == ZEMT_BT_FLAGS)
		SetDeclaration((ZEMTDeclaration*)ZEMTProvider::GetInstance()->GetEnumerator(DeclarationName));
	else
		SetDeclaration(NULL);
}

void ZEMTType::SetBaseType(ZEMTBaseType Type, ZEMTDeclaration* Declaration)
{
	BaseType = Type;
	Binded = true;

	if (BaseType == ZEMT_BT_CLASS || 
		BaseType == ZEMT_BT_OBJECT || BaseType == ZEMT_BT_OBJECT_PTR || BaseType == ZEMT_BT_OBJECT_HOLDER ||
		BaseType == ZEMT_BT_ENUMERATOR || BaseType == ZEMT_BT_FLAGS)
	{
		SetDeclaration(Declaration);
	}
	else
	{
		SetDeclaration(NULL);
	}
}

ZEMTBaseType ZEMTType::GetBaseType() const
{
	return BaseType;
}

void ZEMTType::SetBaseQualifier(ZEMTTypeQualifier Qualifier)
{
	BaseQualifier = Qualifier;
}

ZEMTTypeQualifier ZEMTType::GetBaseQualifier() const
{
	return BaseQualifier;
}

void ZEMTType::SetCollectionType(ZEMTCollectionType Type)
{
	CollectionType = Type;
}

ZEMTCollectionType ZEMTType::GetCollectionType() const
{
	return CollectionType;
}

void ZEMTType::SetCollectionQualifier(ZEMTTypeQualifier Qualifier)
{
	CollectionQualifier = Qualifier;
}

ZEMTTypeQualifier ZEMTType::GetCollectionQualifier() const
{
	return CollectionQualifier;
}

void ZEMTType::SetDeclaration(ZEMTDeclaration* Declaration)
{
	CannonicalBit = ((ZEUInt64)Declaration & 0x8000000000000000ull) != 0;
	this->Declaration = (ZEUInt64)Declaration & 0x000FFFFFFFFFFFFFull;
}

ZEMTDeclaration* ZEMTType::GetDeclaration() const
{
	if (CannonicalBit)
		return static_cast<ZEMTDeclaration*>((void*)(Declaration | 0xFFF0000000000000ull));
	else
		return static_cast<ZEMTDeclaration*>((void*)Declaration);
}

ZEClass* ZEMTType::GetClass() const
{
	if (BaseType != ZEMT_BT_CLASS && 
		BaseType != ZEMT_BT_OBJECT && BaseType != ZEMT_BT_OBJECT_PTR && BaseType != ZEMT_BT_OBJECT_HOLDER)
	{
		return NULL;
	}

	return static_cast<ZEClass*>(GetDeclaration());
}

ZEMTEnumerator* ZEMTType::GetEnumerator() const
{
	if (BaseType != ZEMT_BT_ENUMERATOR && BaseType != ZEMT_BT_FLAGS)
		return NULL;

	return static_cast<ZEMTEnumerator*>(GetDeclaration());
}

void ZEMTType::SetBinded(bool Binded)
{
	this->Binded = Binded;
}

bool ZEMTType::IsBinded() const
{
	return Binded;
}

bool ZEMTType::IsValid() const
{
	return BaseType != ZEMT_BT_UNDEFINED;
}

bool ZEMTType::IsReference() const
{
	return (BaseQualifier == ZEMT_TQ_REFERENCE || BaseQualifier == ZEMT_TQ_CONST_REFERENCE);
}

bool ZEMTType::IsConst() const
{
	return (BaseQualifier == ZEMT_TQ_CONST_VALUE || BaseQualifier == ZEMT_TQ_CONST_REFERENCE);
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
	return ZEMTType(BaseType, BaseQualifier, ZEMT_CT_NONE, ZEMT_TQ_VALUE, GetDeclaration());
}

ZESize ZEMTType::GetInstanceSize() const
{
	if (!IsCollection())
	{
		if (BaseQualifier == ZEMT_TQ_REFERENCE || BaseQualifier == ZEMT_TQ_CONST_REFERENCE)
			return sizeof(void*);

		switch (BaseType)
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
				if (Declaration == NULL)
					return 0;
				return static_cast<ZEClass*>(GetDeclaration())->GetSizeOfObject();

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
	if (IsReference() || IsCollectionReference())
	{
		return new void*();
	}
	else if (!IsCollection())
	{	
		switch (BaseType)
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
				if (Declaration == NULL)
					return NULL;
				return static_cast<ZEClass*>(GetDeclaration())->CreateInstance();

			case ZEMT_BT_OBJECT_PTR:
				if (Declaration == NULL)
					return NULL;
				return new ZEObject*;

			case ZEMT_BT_OBJECT_HOLDER:
				if (Declaration == NULL)
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

		ZEArray<void*>* Instance = new ZEArray<void *>();
		Instance->Type.SetType(BaseType, BaseQualifier, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, GetDeclaration());
		return Instance;
	}

	return NULL;
}

void* ZEMTType::CloneInstance(const void* Instance) const
{
	if (IsReference() || IsCollectionReference())
	{
		return new void*(*(void*const*)Instance);
	}
	else if (!IsCollection())
	{
		switch (BaseType)
		{
			case ZEMT_BT_UNDEFINED:
			case ZEMT_BT_VOID:
				return NULL;

			case ZEMT_BT_INTEGER_8:
				return CastAndClone<ZEInt8>(Instance);

			case ZEMT_BT_INTEGER_16:
				return CastAndClone<ZEInt16>(Instance);

			case ZEMT_BT_INTEGER_32:
				return CastAndClone<ZEInt32>(Instance);

			case ZEMT_BT_INTEGER_64:
				return CastAndClone<ZEInt64>(Instance);

			case ZEMT_BT_UNSIGNED_INTEGER_8:
				return CastAndClone<ZEUInt8>(Instance);

			case ZEMT_BT_UNSIGNED_INTEGER_16:
				return CastAndClone<ZEUInt16>(Instance);

			case ZEMT_BT_UNSIGNED_INTEGER_32:
				return CastAndClone<ZEUInt32>(Instance);

			case ZEMT_BT_UNSIGNED_INTEGER_64:
				return CastAndClone<ZEUInt64>(Instance);

			case ZEMT_BT_FLOAT:
				return CastAndClone<float>(Instance);

			case ZEMT_BT_DOUBLE:
				return CastAndClone<double>(Instance);

			case ZEMT_BT_BOOLEAN:
				return CastAndClone<bool>(Instance);

			case ZEMT_BT_STRING:
				return CastAndClone<ZEString>(Instance);

			case ZEMT_BT_QUATERNION:
				return CastAndClone<ZEQuaternion>(Instance);

			case ZEMT_BT_VECTOR2:
				return CastAndClone<ZEVector2>(Instance);

			case ZEMT_BT_VECTOR2D:
				return CastAndClone<ZEVector2d>(Instance);

			case ZEMT_BT_VECTOR3:
				return CastAndClone<ZEVector3>(Instance);

			case ZEMT_BT_VECTOR3D:
				return CastAndClone<ZEVector3d>(Instance);

			case ZEMT_BT_VECTOR4:
				return CastAndClone<ZEVector4>(Instance);

			case ZEMT_BT_VECTOR4D:
				return CastAndClone<ZEVector4d>(Instance);

			case ZEMT_BT_MATRIX3X3:
				return CastAndClone<ZEMatrix3x3>(Instance);

			case ZEMT_BT_MATRIX3X3D:
				return CastAndClone<ZEMatrix3x3d>(Instance);

			case ZEMT_BT_MATRIX4X4:
				return CastAndClone<ZEMatrix4x4>(Instance);

			case ZEMT_BT_MATRIX4X4D:
				return CastAndClone<ZEMatrix4x4d>(Instance);

			case ZEMT_BT_OBJECT:
				if (Declaration == NULL)
					return NULL;
				return GetClass()->Clone(*(const ZEObject**)(Instance));

			case ZEMT_BT_OBJECT_PTR:
				if (Declaration == NULL)
					return NULL;
				return CastAndClone<ZEObject*>(Instance);

			case ZEMT_BT_OBJECT_HOLDER:
				if (Declaration == NULL)
					return NULL;
				return new ZEHolder<ZEReferenceCounted>(*(ZEHolder<ZEReferenceCounted>*)(Instance));

			case ZEMT_BT_CLASS:
				return NULL;

			case ZEMT_BT_ENUMERATOR:
				return new int(*static_cast<const ZEInt8*>(Instance));

			case ZEMT_BT_FLAGS:
				return new int(*static_cast<const ZEInt8*>(Instance));
		}
	}
	else
	{
		if (CollectionType == ZEMT_CT_ARRAY)
		{
			const ZEArray<ZEBYTE>* SourceArray = static_cast<const ZEArray<ZEBYTE>*>(Instance);
			ZEArray<ZEBYTE>* DestinationArray = new ZEArray<ZEBYTE>();
			DestinationArray->Type.SetType(BaseType, BaseQualifier, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, GetDeclaration());

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
					if (!ItemType.CopyConstructInstance(DestinationCursor, SourceCursor))
					{
						SourceArray->UnlockRead();
						delete DestinationArray;
						return NULL;
					}
					DestinationCursor += ItemSize;
					SourceCursor += ItemSize;
				}
			}
			SourceArray->UnlockRead();
			DestinationArray->Count = ItemCount;
			DestinationArray->Allocator.Size = ItemCount;

			return DestinationArray;
		}
	}

	return NULL;
}

bool ZEMTType::DestroyInstance(void* Instance) const
{
	if (IsReference())
	{
		delete Instance;
		return true;
	}
	else
	{
		if (!IsCollection())
		{
			switch (BaseType)
			{
				case ZEMT_BT_UNDEFINED:
				case ZEMT_BT_VOID:
					return false;

				case ZEMT_BT_INTEGER_8:
					delete (ZEInt8*)Instance;
					return true;

				case ZEMT_BT_INTEGER_16:
					delete (ZEInt16*)Instance;
					return true;

				case ZEMT_BT_INTEGER_32:
					delete (ZEInt32*)Instance;
					return true;

				case ZEMT_BT_INTEGER_64:
					delete (ZEInt64*)Instance;
					return true;

				case ZEMT_BT_UNSIGNED_INTEGER_8:
					delete (ZEUInt8*)Instance;
					return true;

				case ZEMT_BT_UNSIGNED_INTEGER_16:
					delete (ZEUInt16*)Instance;
					return true;

				case ZEMT_BT_UNSIGNED_INTEGER_32:
					delete (ZEUInt32*)Instance;
					return true;

				case ZEMT_BT_UNSIGNED_INTEGER_64:
					delete (ZEUInt64*)Instance;
					return true;

				case ZEMT_BT_FLOAT:
					delete (float*)Instance;
					return true;

				case ZEMT_BT_DOUBLE:
					delete (double*)Instance;
					return true;

				case ZEMT_BT_BOOLEAN:
					delete (bool*)Instance;
					return true;

				case ZEMT_BT_STRING:
					delete (ZEString*)Instance;
					return true;

				case ZEMT_BT_QUATERNION:
					delete (ZEQuaternion*)Instance;
					return true;

				case ZEMT_BT_VECTOR2:
					delete (ZEVector2*)Instance;
					return true;

				case ZEMT_BT_VECTOR2D:
					delete (ZEVector2d*)Instance;
					return true;

				case ZEMT_BT_VECTOR3:
					delete (ZEVector3*)Instance;
					return true;

				case ZEMT_BT_VECTOR3D:
					delete (ZEVector3d*)Instance;
					return true;

				case ZEMT_BT_VECTOR4:
					delete (ZEVector4*)Instance;
					return true;

				case ZEMT_BT_VECTOR4D:
					delete (ZEVector4d*)Instance;
					return true;

				case ZEMT_BT_MATRIX3X3:
					delete (ZEMatrix3x3*)Instance;
					return true;

				case ZEMT_BT_MATRIX3X3D:
					delete (ZEMatrix3x3d*)Instance;
					return true;

				case ZEMT_BT_MATRIX4X4:
					delete (ZEMatrix4x4*)Instance;
					return true;

				case ZEMT_BT_MATRIX4X4D:
					delete (ZEMatrix4x4d*)Instance;
					return true;

				case ZEMT_BT_OBJECT:
					if (Declaration == NULL)
						return false;
					return static_cast<ZEClass*>(GetDeclaration())->Destroy((ZEObject*)Instance);

				case ZEMT_BT_OBJECT_PTR:
					if (Declaration == NULL)
						return NULL;
					delete (ZEInt8*)Instance;
					return true;

				case ZEMT_BT_OBJECT_HOLDER:
					if (Declaration == NULL)
						return NULL;
					((ZEHolder<ZEReferenceCounted>*)Instance)->SetObjectPtrConst(NULL);
					delete (ZEHolder<ZEReferenceCounted>*)Instance;
					return true;

				case ZEMT_BT_CLASS:
					return true;

				case ZEMT_BT_ENUMERATOR:
					delete (ZEInt32*)Instance;
					return true;

				case ZEMT_BT_FLAGS:
					delete (ZEInt32*)Instance;
					return true;
			}
		}
		else if (CollectionType == ZEMT_CT_ARRAY)
		{
			ZEArray<ZEBYTE>* Array = static_cast<ZEArray<ZEBYTE>*>(Instance);
			if (BaseType == ZEMT_BT_STRING || BaseType == ZEMT_BT_OBJECT || BaseType == ZEMT_BT_OBJECT_HOLDER)
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

bool ZEMTType::AssignInstance(void* DestinationInstance, const void* SourceInstance) const
{
	if (IsReference() || IsCollectionReference())
	{
		CastAndAssign<void*>(DestinationInstance, SourceInstance);
		return true;
	}
	else 
	{
		if (!IsCollection())
		{

			switch (BaseType)
			{
				default:
				case ZEMT_BT_UNDEFINED:
				case ZEMT_BT_VOID:
					return false;

				case ZEMT_BT_INTEGER_8:
					CastAndAssign<ZEInt8>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_INTEGER_16:
					CastAndAssign<ZEInt16>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_INTEGER_32:
					CastAndAssign<ZEInt32>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_INTEGER_64:
					CastAndAssign<ZEInt64>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_UNSIGNED_INTEGER_8:
					CastAndAssign<ZEUInt8>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_UNSIGNED_INTEGER_16:
					CastAndAssign<ZEUInt16>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_UNSIGNED_INTEGER_32:
					CastAndAssign<ZEUInt32>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_UNSIGNED_INTEGER_64:
					CastAndAssign<ZEUInt64>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_FLOAT:
					CastAndAssign<float>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_DOUBLE:
					CastAndAssign<double>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_BOOLEAN:
					CastAndAssign<bool>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_STRING:
					CastAndAssign<ZEString>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_QUATERNION:
					CastAndAssign<ZEQuaternion>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_VECTOR2:
					CastAndAssign<ZEVector2>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_VECTOR2D:
					CastAndAssign<ZEVector2d>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_VECTOR3:
					CastAndAssign<ZEVector3>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_VECTOR3D:
					CastAndAssign<ZEVector3d>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_VECTOR4:
					CastAndAssign<ZEVector4>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_VECTOR4D:
					CastAndAssign<ZEVector4d>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_MATRIX3X3:
					CastAndAssign<ZEMatrix3x3>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_MATRIX3X3D:
					CastAndAssign<ZEMatrix3x3d>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_MATRIX4X4:
					CastAndAssign<ZEMatrix4x4>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_MATRIX4X4D:
					CastAndAssign<ZEMatrix4x4d>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_OBJECT:
					if (Declaration == NULL)
						return false;			
					return static_cast<ZEClass*>(GetDeclaration())->Assign(static_cast<ZEObject*>(DestinationInstance), static_cast<const ZEObject*>(SourceInstance));

				case ZEMT_BT_OBJECT_PTR:
					CastAndAssign<ZEObject*>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_OBJECT_HOLDER:
					CastAndAssign<ZEHolder<ZEReferenceCounted>>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_CLASS:
					CastAndAssign<ZEClass>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_ENUMERATOR:
					CastAndAssign<int>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_FLAGS:
					CastAndAssign<int>(DestinationInstance, SourceInstance);
					return true;
			}
		}
		else
		{
			if (CollectionType == ZEMT_CT_ARRAY)
			{
				const ZEArray<ZEBYTE>* SourceArray = static_cast<const ZEArray<ZEBYTE>*>(SourceInstance);
				ZEArray<ZEBYTE>* DestinationArray = static_cast<ZEArray<ZEBYTE>*>(DestinationInstance);

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
						if (!ItemType.CopyConstructInstance(DestinationCursor, SourceCursor))
						{
							SourceArray->UnlockRead();
							return false;
						}
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
	}

	return false;
}

bool ZEMTType::ConstructInstance(void* Instance) const
{
	if (IsReference() || IsCollectionReference())
	{
		return true;
	}
	else
	{
		if (!IsCollection())
		{
			if (ZEMT_BT_OBJECT)
			{
				if (Declaration == NULL)
					return false;

				return static_cast<ZEClass*>(GetDeclaration())->Construct(static_cast<ZEObject*>(Instance));
			}
			else if (ZEMT_BT_OBJECT_HOLDER)
			{
				new (static_cast<ZEHolder<ZEReferenceCounted>*>(Instance)) ZEHolder<ZEReferenceCounted>();
				return true;
			}
		}
		else 
		{
			if (CollectionType == ZEMT_CT_ARRAY)
			{
				ZEArray<void*>* Array = static_cast<ZEArray<void*>*>(Instance);
				new(Array) ZEArray<void*>();
				Array->Type.SetType(BaseType, BaseQualifier, ZEMT_CT_ARRAY, ZEMT_TQ_VALUE, GetDeclaration());
				return true;
			}
		}
	}

	return false;
}

bool ZEMTType::CopyConstructInstance(void* DestinationInstance, const void* SourceInstance) const
{
	if (IsReference() || IsCollectionReference())
	{
		CastAndCopyConstruct<void*>(DestinationInstance, SourceInstance);
		return true;
	}
	else 
	{
		if (!IsCollection())
		{

			switch (BaseType)
			{
				default:
				case ZEMT_BT_UNDEFINED:
				case ZEMT_BT_VOID:
					return false;

				case ZEMT_BT_INTEGER_8:
					CastAndCopyConstruct<ZEInt8>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_INTEGER_16:
					CastAndCopyConstruct<ZEInt16>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_INTEGER_32:
					CastAndCopyConstruct<ZEInt32>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_INTEGER_64:
					CastAndCopyConstruct<ZEInt64>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_UNSIGNED_INTEGER_8:
					CastAndCopyConstruct<ZEUInt8>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_UNSIGNED_INTEGER_16:
					CastAndCopyConstruct<ZEUInt16>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_UNSIGNED_INTEGER_32:
					CastAndCopyConstruct<ZEUInt32>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_UNSIGNED_INTEGER_64:
					CastAndCopyConstruct<ZEUInt64>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_FLOAT:
					CastAndCopyConstruct<float>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_DOUBLE:
					CastAndCopyConstruct<double>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_BOOLEAN:
					CastAndCopyConstruct<bool>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_STRING:
					CastAndCopyConstruct<ZEString>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_QUATERNION:
					CastAndCopyConstruct<ZEQuaternion>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_VECTOR2:
					CastAndCopyConstruct<ZEVector2>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_VECTOR2D:
					CastAndCopyConstruct<ZEVector2d>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_VECTOR3:
					CastAndCopyConstruct<ZEVector3>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_VECTOR3D:
					CastAndCopyConstruct<ZEVector3d>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_VECTOR4:
					CastAndCopyConstruct<ZEVector4>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_VECTOR4D:
					CastAndCopyConstruct<ZEVector4d>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_MATRIX3X3:
					CastAndCopyConstruct<ZEMatrix3x3>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_MATRIX3X3D:
					CastAndCopyConstruct<ZEMatrix3x3d>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_MATRIX4X4:
					CastAndCopyConstruct<ZEMatrix4x4>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_MATRIX4X4D:
					CastAndCopyConstruct<ZEMatrix4x4d>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_OBJECT:
					if (Declaration == NULL)
						return false;			
					return static_cast<ZEClass*>(GetDeclaration())->CopyConstruct(static_cast<ZEObject*>(DestinationInstance), static_cast<const ZEObject*>(SourceInstance));

				case ZEMT_BT_OBJECT_PTR:
					CastAndCopyConstruct<ZEObject*>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_OBJECT_HOLDER:
					CastAndCopyConstruct<ZEHolder<ZEReferenceCounted>>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_CLASS:
					CastAndCopyConstruct<ZEClass>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_ENUMERATOR:
					CastAndCopyConstruct<int>(DestinationInstance, SourceInstance);
					return true;

				case ZEMT_BT_FLAGS:
					CastAndCopyConstruct<int>(DestinationInstance, SourceInstance);
					return true;
			}
		}
		else
		{
			if (CollectionType == ZEMT_CT_ARRAY)
			{
				const ZEArray<ZEBYTE>* SourceArray = static_cast<const ZEArray<ZEBYTE>*>(SourceInstance);
				ZEArray<ZEBYTE>* DestinationArray = static_cast<ZEArray<ZEBYTE>*>(DestinationInstance);

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
						if (!ItemType.CopyConstructInstance(DestinationCursor, SourceCursor))
						{
							SourceArray->UnlockRead();
							return false;
						}
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
	}

	return false;	
}

bool ZEMTType::DeconstructInstance(void* Instance) const
{
	if (IsReference() || IsCollectionReference())
	{
		return true;
	}
	else 
	{
		if (!IsCollection())
		{
			if (BaseType == ZEMT_BT_STRING)
			{
				static_cast<ZEString*>(Instance)->~ZEString();
				return true;
			}
			else if (BaseType == ZEMT_BT_OBJECT)
			{
				if (Declaration == NULL)
					return false;

				return static_cast<ZEClass*>(GetDeclaration())->Deconstruct(static_cast<ZEObject*>(Instance));
			}
			else if (BaseType == ZEMT_BT_OBJECT_HOLDER)
			{
				static_cast<ZEHolder<ZEReferenceCounted>*>(Instance)->~ZEHolder();
				return false;
			}
			else
			{
				return true;
			}
		}
		else
		{
			if (CollectionType == ZEMT_CT_ARRAY)
			{
				ZEArray<ZEBYTE>* Array = static_cast<ZEArray<ZEBYTE>*>(Instance);
				if (BaseType == ZEMT_BT_STRING || BaseType == ZEMT_BT_OBJECT || BaseType == ZEMT_BT_OBJECT_HOLDER)
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

					Array->~ZEArray();
					return true;
				}
				else
				{
					Array->~ZEArray();
					return true;
				}
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

static ZEString ConvertBaseTypeToName(const ZEMTType& Type)
{
	switch (Type.GetBaseType())
	{
		default:
		case ZEMT_BT_UNDEFINED:
			return "";

		case ZEMT_BT_VOID:
			return "void";

		case ZEMT_BT_INTEGER_8:
			return "signed char";

		case ZEMT_BT_INTEGER_16:
			return "signed short";

		case ZEMT_BT_INTEGER_32:
			return "signed int";

		case ZEMT_BT_UNSIGNED_INTEGER_8:
			return "unsigned char";

		case ZEMT_BT_UNSIGNED_INTEGER_16:
			return "unsigned short";

		case ZEMT_BT_UNSIGNED_INTEGER_32:
			return "unsigned int";

		case ZEMT_BT_INTEGER_64:
			return "signed long long";

		case ZEMT_BT_UNSIGNED_INTEGER_64:
			return "unsigned long long";

		case ZEMT_BT_FLOAT:
			return "float";

		case ZEMT_BT_DOUBLE:
			return "double";

		case ZEMT_BT_BOOLEAN:
			return "bool";

		case ZEMT_BT_STRING:
			return "ZEString";

		case ZEMT_BT_QUATERNION:
			return "ZEQuaternion";

		case ZEMT_BT_VECTOR2:
			return "ZEVector2";

		case ZEMT_BT_VECTOR2D:
			return "ZEVector2d";

		case ZEMT_BT_VECTOR3:
			return "ZEVector3";

		case ZEMT_BT_VECTOR3D:
			return "ZEVector3d";

		case ZEMT_BT_VECTOR4:
			return "ZEVector4";

		case ZEMT_BT_VECTOR4D:
			return "ZEVector4d";

		case ZEMT_BT_MATRIX3X3:
			return "ZEMatrix3x3";

		case ZEMT_BT_MATRIX3X3D:
			return "ZEMatrix3x3d";

		case ZEMT_BT_MATRIX4X4:
			return "ZEMatrix4x4";

		case ZEMT_BT_MATRIX4X4D:
			return "ZEMatrix4x4d";

		case ZEMT_BT_CLASS:
			return "ZEClass*";

		case ZEMT_BT_OBJECT:
		{
			ZEClass* Class = Type.GetClass();
			if (Class != NULL)
				return Class->GetName();
			else
				return "%Null%";
		}

		case ZEMT_BT_OBJECT_PTR:
		{
			ZEString ClassName;
			ZEClass* Class = Type.GetClass();
			if (Class != NULL)
				ClassName = Class->GetName();
			else
				ClassName = "%Null%";

			return ZEFormat::Format("{0}*", ClassName);
		}

		case ZEMT_BT_OBJECT_HOLDER:
		{
			ZEString ClassName;
			ZEClass* Class = Type.GetClass();
			if (Class != NULL)
				ClassName = Class->GetName();
			else
				ClassName = "%Null%";

			return ZEFormat::Format("ZEHolder<{0}{1}>", (Type.IsConst() ? "const " : ""), ClassName);
		}

		case ZEMT_BT_FLAGS:
		case ZEMT_BT_ENUMERATOR:
		{
			ZEMTEnumerator* Enumerator = Type.GetEnumerator();
			if (Enumerator != NULL)
				return Enumerator->GetName();
			else
				return "%Null%";
		}
	}
}

ZEString ZEMTType::ToString() const
{
	ZEString Output;

	if (IsCollection())
	{
		if (IsCollectionConst())
			Output.Append("const ");

		if (CollectionType == ZEMT_CT_ARRAY)
			Output.Append("ZEArray<");
		else
			Output.Append("%!UnknownCollection!%<");
	}

	if (IsConst() && BaseType != ZEMT_BT_OBJECT_HOLDER)
		Output.Append("const ");

	Output.Append(ConvertBaseTypeToName(*this));

	if (IsReference())
		Output.Append("&");

	if (IsCollection())
	{
		if (CollectionType == ZEMT_CT_ARRAY)
			Output.Append(">");
		else
			Output.Append(">");

		if (IsCollectionReference())
			Output.Append("&");
	}

	return Output;
}

ZEMTType::ZEMTType()
{
	BaseType = ZEMT_BT_UNDEFINED;
	BaseQualifier = ZEMT_TQ_VALUE;
	Binded = false;
	CollectionType = ZEMT_CT_NONE;
	this->CollectionQualifier = ZEMT_TQ_VALUE;
	Declaration = NULL;
}

ZEMTType::ZEMTType(ZEMTBaseType Type, ZEMTTypeQualifier TypeQualifier, ZEMTCollectionType CollectionType, ZEMTTypeQualifier CollectionQualifier, ZEMTDeclaration* Declaration)
{
	this->BaseType = Type;
	this->BaseQualifier = TypeQualifier;
	this->CollectionType = CollectionType;
	this->CollectionQualifier = CollectionQualifier;
	SetDeclaration(Declaration);
}

bool ZEMTType::Equal(const ZEMTType& A, const ZEMTType& B)
{
	if (A.BaseType == ZEMT_BT_UNDEFINED || B.BaseType == ZEMT_BT_UNDEFINED)
		return false;

	if (A.BaseType != B.BaseType || A.BaseQualifier != B.BaseQualifier || 
		A.CollectionType != B.CollectionType || A.CollectionQualifier != B.CollectionQualifier)
	{
		return false;
	}

	if (A.BaseType == ZEMT_BT_OBJECT || A.BaseType == ZEMT_BT_OBJECT_PTR || A.BaseType == ZEMT_BT_OBJECT_HOLDER ||
		A.BaseType == ZEMT_BT_ENUMERATOR || A.BaseType == ZEMT_BT_FLAGS) 
	{
		if (A.Declaration != B.Declaration)
			return false;

		if (A.Declaration == NULL)
			return false;
	}

	return true;
}
