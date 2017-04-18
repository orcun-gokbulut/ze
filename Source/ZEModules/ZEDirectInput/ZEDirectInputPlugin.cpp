//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDirectInputPlugin.cpp
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

#include "ZEDirectInputPlugin.h"

#include "ZEVersion.h"
#include "ZEDS/ZEArray.h"
#include "ZEMeta/ZEClass.h"
#include "ZEMeta/ZEEnumerator.h"

ZEDirectInputPlugin::ZEDirectInputPlugin()
{

}

ZEDirectInputPlugin::~ZEDirectInputPlugin()
{

}

const char* ZEDirectInputPlugin::GetName() const
{
	return "ZEDirectInputPlugin";
}

ZEVersion ZEDirectInputPlugin::GetVersion() const
{
	return ZEVersion::GetZinekVersion();
}

ZEVersion ZEDirectInputPlugin::GetEngineVersion() const
{
	return ZEVersion::GetZinekVersion();
}

ZEMTDeclaration* const* ZEDirectInputPlugin::GetDeclarations() const
{
	static ZEArray<ZEMTDeclaration*> Declarations;
	static bool Populated = false;
	
	if (!Populated)
	{
		#define ZEMT_REGISTER_ENUM(Name) ZEMTEnumerator* Name ## _Enumerator(); Declarations.Add(Name ## _Enumerator());
		#define ZEMT_REGISTER_CLASS(Name) ZEClass* Name ## _Class(); Declarations.Add(Name ## _Class());
		#include "ZEDirectInput.ZEMetaRegister.h"
		#undef ZEMT_REGISTER_ENUM
		#undef ZEMT_REGISTER_CLASS

		Populated = true;
	}

	return Declarations.GetConstCArray();
}

ZESize ZEDirectInputPlugin::GetDeclarationCount() const
{
	static ZESSize DeclarationCount = -1;

	if (DeclarationCount == -1)
	{
		DeclarationCount = 0;

		#define ZEMT_COUNT_DECLARATION(Name) DeclarationCount++;
		#define ZEMT_REGISTER_ENUM ZEMT_COUNT_DECLARATION
		#define ZEMT_REGISTER_CLASS ZEMT_COUNT_DECLARATION
		#include "ZEDirectInput.ZEMetaRegister.h"
		#undef ZEMT_REGISTER_ENUM
		#undef ZEMT_REGISTER_CLASS
		#undef ZEMT_COUNT_DECLARATION
	}

	return (ZESize)DeclarationCount;
}

void ZEDirectInputPlugin::Destroy()
{
	delete this;
}

ZEPlugin* zeCreatePluginInstance()
{
	return new ZEDirectInputPlugin();
}
