//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEClass.h
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
#ifndef __ZE_CLASS_DESCRIPTION_H__
#define __ZE_CLASS_DESCRIPTION_H__

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEVariant.h"
#include "ZEProperty.h"
#include "ZEContainer.h"
#include "ZEMethod.h"

#define ZE_META_CLASS_DESCRIPTION(ClassName)\
	class ClassName##Description : public ZEClassDescription\
	{\
		public:\
			virtual const char*						GetName() const;\
			virtual ZEClassDescription*				GetParent() const;\
			virtual const char*						GetType() const;\
			virtual const char*						GetDescription() const;\
			virtual const char*						GetIcon() const;\
			virtual const ZEPropertyDescription*	GetProperties() const;\
			virtual size_t							GetPropertyCount() const;\
			virtual size_t							GetPropertyOffset() const;\
			virtual const ZEContainerDescription*	GetContainers() const;\
			virtual size_t							GetContainerCount() const;\
			virtual size_t							GetContainerOffset() const;\
			virtual const ZEMethodDescription*		GetMethods() const;\
			virtual size_t							GetMethodCount() const;\
			virtual size_t							GetMethodOffset() const;\
			virtual ZEClassProvider*				GetProvider() const;\
			virtual ZEClass*						CreateInstance() const;\
	};	

#define ZE_META_EXTENDED_CLASS_DESCRIPTION(ClassName, ExtensionClass, Extension)\
	class ClassName##Description : public ExtensionClass\
	{\
		public:\
			virtual const char*						GetName() const;\
			virtual ZEClassDescription*				GetParent() const;\
			virtual const char*						GetType() const;\
			virtual const char*						GetDescription() const;\
			virtual const char*						GetIcon() const;\
			virtual const ZEPropertyDescription*	GetProperties() const;\
			virtual size_t							GetPropertyCount() const;\
			virtual size_t							GetPropertyOffset() const;\
			virtual const ZEContainerDescription*	GetContainers() const;\
			virtual size_t							GetContainerCount() const;\
			virtual size_t							GetContainerOffset() const;\
			virtual const ZEMethodDescription*		GetMethods() const;\
			virtual size_t							GetMethodCount() const;\
			virtual size_t							GetMethodOffset() const;\
			virtual ZEClassProvider*				GetProvider() const;\
			virtual ZEClass*						CreateInstance() const;\
			Extension\
	};	

#define ZE_META_CLASS()\
	public:\
		virtual ZEClassDescription* GetClassDescription() const;\
		static ZEClassDescription* ClassDescription();\
		virtual int GetPropertyId(const char* PropertyName) const;\
		using ZEClass::SetProperty;\
		virtual bool SetProperty(int PropertyId, const ZEVariant& Value);\
		using ZEClass::GetProperty;\
		virtual bool GetProperty(int PropertyId, ZEVariant& Value) const;\
		virtual int GetContainerId(const char* ContainerName) const;\
		using ZEClass::AddToContainer;\
		virtual bool AddToContainer(int ContainerId, ZEClass* Item);\
		using ZEClass::RemoveFromContainer;\
		virtual bool RemoveFromContainer(int ContainerId, ZEClass* Item);\
		using ZEClass::GetContainerItems;\
		virtual const ZEClass** GetContainerItems(int ContainerId) const;\
		virtual size_t GetContainerItemCount(int ContainerId) const;\
		virtual int GetMethodId(const char* MethodName) const;\
		using ZEClass::CallMethod;\
		virtual bool CallMethod(int MethodId, const ZEVariant* Parameters, size_t ParameterCount, ZEVariant& ReturnValue);\
	private:

#define ZE_META_EXTENDED_CLASS(ExtensionClass, Extension)\
		ZE_META_CLASS()\
	public:\
		Extension\
	private:

struct ZEPropertyDescription;
struct ZEMethodDescription;
class ZEClassProvider;
class ZEClass;
class ZEAnimationController;
class ZEClassDescription
{
	public:
		virtual const char*						GetName() const = 0;
		virtual ZEClassDescription*				GetParent() const = 0;
		virtual const char*						GetType() const = 0;
		virtual const char*						GetDescription() const = 0;
		virtual const char*						GetIcon() const = 0;

		virtual const ZEPropertyDescription*	GetProperties() const = 0;
		virtual size_t							GetPropertyCount() const = 0;
		virtual size_t							GetPropertyOffset() const = 0;

		virtual const ZEContainerDescription*	GetContainers() const = 0;
		virtual size_t							GetContainerCount() const = 0;
		virtual size_t							GetContainerOffset() const = 0;

		virtual const ZEMethodDescription*		GetMethods() const = 0;
		virtual size_t							GetMethodCount() const = 0;
		virtual size_t							GetMethodOffset() const = 0;
		
		virtual ZEClassProvider*				GetProvider() const = 0;
		
		virtual ZEClass*						CreateInstance() const = 0;

		static bool								CheckParent(ZEClassDescription* Parent, ZEClassDescription* Children);
};

class ZEClass
{
	private:
		ZEClass*								Owner;
		ZEAnimationController*					AnimationController;

	public:
		virtual ZEClassDescription*				GetClassDescription() const = 0;

		// Owner
		virtual void							SetOwner(ZEClass* Class);
		virtual ZEClass*						GetOwner();

		// Property Functions
		virtual int								GetPropertyId(const char* PropertyName) const = 0;

		virtual bool							SetProperty(int PropertyId, const ZEVariant& Value) = 0;
		bool									SetProperty(const char* PropertyName, const ZEVariant& Value);

		virtual bool							GetProperty(int PropertyId, ZEVariant& Value) const = 0;
		bool									GetProperty(const char* PropertyName, ZEVariant& Value) const;

		// Containers
		virtual int								GetContainerId(const char* ContainerName) const = 0;

		virtual bool							AddToContainer(int ContainerId, ZEClass* Item) = 0;
		bool									AddToContainer(const char* ContainerName, ZEClass* Item);
		
		virtual bool							RemoveFromContainer(int ContainerId, ZEClass* Item) = 0;
		bool									RemoveFromContainer(const char* ContainerName, ZEClass* Item);
		
		virtual const ZEClass**					GetContainerItems(int ContainerId) const = 0;
		const ZEClass**							GetContainerItems(const char* ContainerName) const;
		
		virtual size_t							GetContainerItemCount(int ContainerId) const = 0;
		size_t									GetContainerItemCount(const char* ContainerName) const;

		// Methods
		virtual int								GetMethodId(const char* MethodName) const = 0;

		virtual bool							CallMethod(int MethodId, const ZEVariant* Parameters, size_t ParameterCount, ZEVariant& ReturnValue) = 0;
		bool									CallMethod(const char* MethodName, const ZEVariant* Parameters, size_t ParameterCount, ZEVariant& ReturnValue);
		bool									CallMethod(int MethodId, const ZEArray<ZEVariant>& Parameters, ZEVariant& ReturnValue);
		bool									CallMethod(const char* MethodName, const ZEArray<ZEVariant>& Parameters, ZEVariant& ReturnValue);

		// Scripting

		// Controllers

		// Animation
		virtual void							SetAnimationController(ZEAnimationController* AnimationController);
		virtual ZEAnimationController*			GetAnimationController();

		// Navigation
		bool									GetChildClass(const char* Path, ZEClass* ChildClass);
		
		// Serialization
		virtual bool							Serialize(ZESerializer* Serializer);
		virtual bool							Unserialize(ZEUnserializer* Unserialzier);

									
												ZEClass();
		virtual									~ZEClass();
};

#endif




