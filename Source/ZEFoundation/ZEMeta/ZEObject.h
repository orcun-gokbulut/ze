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

#include "ZEClass.h"
#include "ZEExport.ZEFoundation.h"
#include "ZEMTExport.h"

#define ZEMT_INCLUDE(IncludeFile) ZEMT_ATTRIBUTE_GLOBAL(ZEMC.Include, IncludeFile)
#define ZEMT_FORWARD_DECLARE(ClassName) class ZEMT_ATTRIBUTE(ZEMC.ForwardDeclare, #ClassName) ClassName

#define ZE_OBJECT \
	public: \
		virtual	ZEClass*			GetClass() const; \
		static ZEClass*				Class(); \
	private:
		
#define ZEMT_OBJECT_IMPLEMENTATION(ClassName) \
	ZEClass* ClassName::GetClass() const \
	{ \
		ZEClass* ClassName##_Class(); \
		return ClassName##_Class(); \
	} \
	\
	ZEClass* ClassName::Class() \
	{ \
		ZEClass* ClassName##_Class(); \
		return ClassName##_Class(); \
	} \
	\
	ZEMT_EXPORT ZEClass* ClassName##_Class() \
	{ \
		static ClassName##Class Class;\
		return &Class; \
	}

class ZEMTEventBase;

class ZE_EXPORT_ZEFOUNDATION ZEObject
{
	template<typename TSignature> friend class ZEEvent;
	private:
		void*						EventConnections;
	
		void						CloneEventConnections(ZEObject* Other);
		void						AddEventConnection(ZEMTEventBase* Event);
		void						RemoveEventConnection(ZEMTEventBase* Event);

	public:
		virtual	ZEClass*			GetClass() const;
		static ZEClass*				Class();

		ZEObject&					operator=(const ZEObject& Object);

									ZEObject();
									ZEObject(const ZEObject& Object);
									~ZEObject();
}
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper.Icon, "#R:/ZEDEditor/Icons/ZEDObjectWrapper/ZEObject.png")
ZEMT_ATTRIBUTE(ZEDEditor.ObjectWrapper.Selectable, true)
ZEMT_ATTRIBUTE_MEMBER(*, ZEMC.Export, true)
ZEMT_ATTRIBUTE_MEMBER(~*, ZEMC.Accessor, true)
ZEMT_ATTRIBUTE_MEMBER(@*, ZEMeta.Serialization, true)
ZEMT_ATTRIBUTE_MEMBER(@*, ZEDEditor.PropertyEditor.Display, true);
