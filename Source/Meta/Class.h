//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Class.h
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

#include "ZEDS/Variant.h"
#include "Property.h"

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
			virtual const ZEMethodDescription*		GetMethods() const;\
			virtual size_t							GetMethodCount() const;\
			virtual size_t							GetMethodOffset() const;\
			virtual ZEClassProvider*				GetProvider() const;\
			virtual ZEClass*						CreateInstance() const;\
			Extension\
	};	

#define ZE_META_CLASS()\
	public:\
		virtual ZEClassDescription* GetClassDescription();\
		static ZEClassDescription* ClassDescription();\
		virtual int GetPropertyId(const char* PropertyName) const;\
		virtual bool SetProperty(int PropertyId, const ZEVariant& Value);\
		virtual bool SetProperty(const char* PropertyName, const ZEVariant& Value);\
		virtual bool GetProperty(int PropertyId, ZEVariant& Value) const;\
		virtual bool GetProperty(const char* PropertyName, ZEVariant& Value) const;\
	private:

#define ZE_META_EXTENDED_CLASS(ExtensionClass, Extension)\
	public:\
		virtual ZEClassDescription* GetClassDescription();\
		static ZEClassDescription* ClassDescription();\
		virtual int GetPropertyId(const char* PropertyName) const;\
		virtual bool SetProperty(int PropertyId, const ZEVariant& Value);\
		virtual bool SetProperty(const char* PropertyName, const ZEVariant& Value);\
		virtual bool GetProperty(int PropertyId, ZEVariant& Value) const;\
		virtual bool GetProperty(const char* PropertyName, ZEVariant& Value) const;\
		Extension\
	private:

class ZEPropertyDescription;
class ZEMethodDescription;
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

		virtual const ZEMethodDescription*		GetMethods() const = 0;
		virtual size_t							GetMethodCount() const = 0;
		virtual size_t							GetMethodOffset() const = 0;
		virtual ZEClassProvider*				GetProvider() const = 0;
		virtual ZEClass*						CreateInstance() const = 0;
};

class ZEClass
{
	private:
		ZEAnimationController*					AnimationController;

	public:
		virtual ZEClassDescription*				GetClassDescription() = 0;

		// Property Functions
		virtual int								GetPropertyId(const char* PropertyName) const = 0;

/*		virtual bool							SetPropertyController(int PropertyId, ZEClass* Class, int PropertyId);
		virtual bool							SetPropertyController(int PropertyId, ZEClass* Class, const char* PropertyName);
		virtual bool							SetPropertyController(const char* PropertyName, ZEClass* Class, int PropertyId);
		virtual bool							SetPropertyController(const char* PropertyName, ZEClass* Class, const char* PropertyName);*/

		virtual bool							SetProperty(int PropertyId, const ZEVariant& Value) = 0;
		virtual bool							SetProperty(const char* PropertyName, const ZEVariant& Value) = 0;

		virtual bool							GetProperty(int PropertyId, ZEVariant& Value) const = 0;
		virtual bool							GetProperty(const char* PropertyName, ZEVariant& Value) const = 0;

		virtual bool							Serialize(ZESerializer* Serializer);
		virtual bool							Unserialize(ZEUnserializer* Unserialzier);

		// Methods

		// Child Classes

		// Animation Controller
		virtual void							SetAnimationController(ZEAnimationController* AnimationController);
		virtual ZEAnimationController*			GetAnimationController();
									
												ZEClass();
};

#endif
