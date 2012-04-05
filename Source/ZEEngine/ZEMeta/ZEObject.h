//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEObject.h
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
#ifndef __ZE_OBJECT_H__
#define __ZE_OBJECT_H__

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEVariant.h"
#include "ZEProperty.h"
#include "ZEContainer.h"
#include "ZEMethod.h"

#define ZE_META_OBJECT_DESCRIPTION(ClassName)\
	class ClassName##Description : public ZEObjectDescription\
	{\
		public:\
			virtual const char*						GetName() const;\
			virtual ZEObjectDescription*			GetParent() const;\
			virtual const char*						GetType() const;\
			virtual const char*						GetDescription() const;\
			virtual const char*						GetIcon() const;\
			virtual const ZEPropertyDescription*	GetProperties() const;\
			virtual ZESize							GetPropertyCount() const;\
			virtual ZESize							GetPropertyOffset() const;\
			virtual const ZEContainerDescription*	GetContainers() const;\
			virtual ZESize							GetContainerCount() const;\
			virtual ZESize							GetContainerOffset() const;\
			virtual const ZEMethodDescription*		GetMethods() const;\
			virtual ZESize							GetMethodCount() const;\
			virtual ZESize							GetMethodOffset() const;\
			virtual ZEObjectProvider*				GetProvider() const;\
			virtual ZEObject*						CreateInstance() const;\
	};	

#define ZE_META_EXTENDED_OBJECT_DESCRIPTION(ClassName, ExtensionClass, Extension)\
	class ClassName##Description : public ExtensionClass\
	{\
		public:\
			virtual const char*						GetName() const;\
			virtual ZEObjectDescription*			GetParent() const;\
			virtual const char*						GetType() const;\
			virtual const char*						GetDescription() const;\
			virtual const char*						GetIcon() const;\
			virtual const ZEPropertyDescription*	GetProperties() const;\
			virtual ZESize							GetPropertyCount() const;\
			virtual ZESize							GetPropertyOffset() const;\
			virtual const ZEContainerDescription*	GetContainers() const;\
			virtual ZESize							GetContainerCount() const;\
			virtual ZESize							GetContainerOffset() const;\
			virtual const ZEMethodDescription*		GetMethods() const;\
			virtual ZESize							GetMethodCount() const;\
			virtual ZESize							GetMethodOffset() const;\
			virtual ZEObjectProvider*				GetProvider() const;\
			virtual ZEObject*						CreateInstance() const;\
			Extension\
	};	

#define ZE_META_OBJECT(ClassName)\
	friend class ClassName##Description;\
	public:\
		virtual ZEObjectDescription* GetDescription() const;\
		static ZEObjectDescription* Description();\
		virtual ZESSize GetPropertyId(const char* PropertyName) const;\
		using ZEObject::SetProperty;\
		virtual bool SetProperty(ZEInt PropertyId, const ZEVariant& Value);\
		using ZEObject::GetProperty;\
		virtual bool GetProperty(ZEInt PropertyId, ZEVariant& Value) const;\
		virtual ZESSize GetContainerId(const char* ContainerName) const;\
		using ZEObject::AddToContainer;\
		virtual bool AddToContainer(ZEInt ContainerId, ZEObject* Item);\
		using ZEObject::RemoveFromContainer;\
		virtual bool RemoveFromContainer(ZEInt ContainerId, ZEObject* Item);\
		using ZEObject::GetContainerItems;\
		virtual const ZEObject** GetContainerItems(ZEInt ContainerId) const;\
		using ZEObject::GetContainerItemCount;\
		virtual ZESize GetContainerItemCount(ZEInt ContainerId) const;\
		virtual ZESSize GetMethodId(const char* MethodName) const;\
		using ZEObject::CallMethod;\
		virtual bool CallMethod(ZEInt MethodId, const ZEVariant* Parameters, ZESize ParameterCount, ZEVariant& ReturnValue);\
	private:

#define ZE_META_EXTENDED_OBJECT(ExtensionClass, Extension, Class)\
		ZE_META_OBJECT(Class)\
	public:\
		Extension\
	private:

struct ZEPropertyDescription;
struct ZEMethodDescription;
class ZEObjectProvider;
class ZEObject;
class ZEAnimationController;
class ZESerializer;
class ZEUnserializer;

class ZEObjectDescription
{
	public:
		virtual const char*						GetName() const = 0;
		virtual ZEObjectDescription*			GetParent() const = 0;
		virtual const char*						GetType() const = 0;
		virtual const char*						GetDescription() const = 0;
		virtual const char*						GetIcon() const = 0;

		virtual const ZEPropertyDescription*	GetProperties() const = 0;
		virtual ZESize							GetPropertyCount() const = 0;
		virtual ZESize							GetPropertyOffset() const = 0;

		virtual const ZEContainerDescription*	GetContainers() const = 0;
		virtual ZESize							GetContainerCount() const = 0;
		virtual ZESize							GetContainerOffset() const = 0;

		virtual const ZEMethodDescription*		GetMethods() const = 0;
		virtual ZESize							GetMethodCount() const = 0;
		virtual ZESize							GetMethodOffset() const = 0;
		
		virtual ZEObjectProvider*				GetProvider() const = 0;
		
		virtual ZEObject*						CreateInstance() const = 0;

		static bool								CheckParent(ZEObjectDescription* Parent, ZEObjectDescription* Children);
};

class ZEObject
{
	private:
		ZEAnimationController*					AnimationController;
		ZEArray<ZERunTimeProperty>				CustomProperties;

	public:
		virtual ZEObjectDescription*			GetDescription() const = 0;

		// Property Functions
		virtual ZESSize							GetPropertyId(const char* PropertyName) const = 0;
		virtual bool							SetProperty(ZEInt PropertyId, const ZEVariant& Value) = 0;
		bool									SetProperty(const char* PropertyName, const ZEVariant& Value);
		virtual bool							GetProperty(ZEInt PropertyId, ZEVariant& Value) const = 0;
		bool									GetProperty(const char* PropertyName, ZEVariant& Value) const;
		bool									AddCustomProperty(ZERunTimeProperty Property);
		bool									RemoveCustomProperty(const char* PropertyName);
		const ZEArray<ZERunTimeProperty>*		GetCustomProperties() const;

		// Containers
		virtual ZESSize							GetContainerId(const char* ContainerName) const = 0;
		virtual bool							AddToContainer(ZEInt ContainerId, ZEObject* Item) = 0;
		bool									AddToContainer(const char* ContainerName, ZEObject* Item);
		virtual bool							RemoveFromContainer(ZEInt ContainerId, ZEObject* Item) = 0;
		bool									RemoveFromContainer(const char* ContainerName, ZEObject* Item);
		virtual const ZEObject**				GetContainerItems(ZEInt ContainerId) const = 0;
		const ZEObject**						GetContainerItems(const char* ContainerName) const;		
		virtual ZESize							GetContainerItemCount(ZEInt ContainerId) const = 0;
		ZESize									GetContainerItemCount(const char* ContainerName) const;

		// Methods
		virtual ZESSize							GetMethodId(const char* MethodName) const = 0;
		virtual bool							CallMethod(ZEInt MethodId, const ZEVariant* Parameters, ZESize ParameterCount, ZEVariant& ReturnValue) = 0;
		bool									CallMethod(const char* MethodName, const ZEVariant* Parameters, ZESize ParameterCount, ZEVariant& ReturnValue);
		bool									CallMethod(ZEInt MethodId, const ZEArray<ZEVariant>& Parameters, ZEVariant& ReturnValue);
		bool									CallMethod(const char* MethodName, const ZEArray<ZEVariant>& Parameters, ZEVariant& ReturnValue);

		// Scripting

		// Controllers

		// Animation
		virtual void							SetAnimationController(ZEAnimationController* AnimationController);
		virtual ZEAnimationController*			GetAnimationController();

		// Navigation
		bool									GetChildClass(const char* Path, ZEObject* ChildClass);
		
		// Serialization
		virtual bool							Serialize(ZESerializer* Serializer);
		virtual bool							Unserialize(ZEUnserializer* Unserialzier);

									
												ZEObject();
		virtual									~ZEObject();
};

#endif




