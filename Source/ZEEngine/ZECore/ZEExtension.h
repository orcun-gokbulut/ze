//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEExtension.h
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
#ifndef	__ZE_EXTENSION_H__
#define __ZE_EXTENSION_H__

#include "ZEBase.h"
#include "ZETypes.h"
#include "ZEVersion.h"
#include "ZEDS\ZEString.h"

class ZEExtensionDescription;
class ZEModuleDescription;
class ZEOptionSection;
class ZEExtension;

#define ZE_EXTENSION(Extension)\
	public:\
		friend class Extension##Description;\
		static ZEExtensionDescription* Description();\
		virtual ZEExtensionDescription* GetDescription();\
	private:


#define ZE_EXTENSION_DESCRIPTION(Extension, ParentExtension, Options)\
class Extension##Description : public ZEExtensionDescription\
{\
	public:\
		virtual ZEExtensionDescription*		GetParent(){ return ParentExtension::Description();}\
		virtual ZEString					GetName() { return #Extension; }\
		virtual ZEOptionSection*			GetOptions() { return Options; }\
		virtual	bool						CheckCompatible() { return true; }\
		virtual	ZEExtension*				CreateInstance() { return new Extension(); }\
};\
ZEExtensionDescription* Extension::Description()\
{\
	static Extension##Description Desc;\
	return &Desc; \
}\
ZEExtensionDescription* Extension::GetDescription()\
{\
	return Extension::Description();\
}

#define ZE_EXTENSION_DESCRIPTION_ABSTRACT(Extension, ParentExtension, Options)\
class Extension##Description : public ZEExtensionDescription\
{\
public:\
	virtual ZEExtensionDescription*		GetParent(){ return ParentExtension::Description();}\
	virtual ZEString					GetName() { return #Extension; }\
	virtual ZEOptionSection*			GetOptions() { return Options; }\
	virtual	bool						CheckCompatible() { return true; }\
	virtual	ZEExtension*				CreateInstance() { return NULL; }\
};\
	ZEExtensionDescription* Extension::Description()\
{\
	static Extension##Description Desc;\
	return &Desc; \
}\
	ZEExtensionDescription* Extension::GetDescription()\
{\
	return Extension::Description();\
}


class ZEExtensionDescription
{
	public:
		virtual ZEExtensionDescription*		GetParent() = 0;
		virtual ZEString					GetName() = 0;

		virtual ZEOptionSection*			GetOptions() = 0;
		virtual	bool						CheckCompatible() = 0;

		virtual	ZEExtension*				CreateInstance() = 0;
};

class ZEExtension : public ZEBase
{
	public:
		static ZEExtensionDescription*		Description();
		virtual ZEExtensionDescription*		GetDescription() = 0;
};

#endif
