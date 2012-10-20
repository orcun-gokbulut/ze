//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxMapExporterDescription.cpp
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

#include "ZE3dsMaxMapExporterDescription.h"
#include "ZE3dsMaxMapExporter.h"

#define ZE3dsMapExporter_CLASS_ID	Class_ID(0x3a8ad1e4, 0x973e26ff)

ZEInt ZE3dsMaxMapExporterDescription::IsPublic()
{ 
	return TRUE; 
}

void* ZE3dsMaxMapExporterDescription::Create(BOOL Loading) 
{ 
	return new ZE3dsMaxMapExporter(); 
}

const TCHAR* ZE3dsMaxMapExporterDescription::ClassName()
{ 
	return "ZEMapExporter"; 
}

SClass_ID ZE3dsMaxMapExporterDescription::SuperClassID()
{ 
	return SCENE_EXPORT_CLASS_ID; 
}

Class_ID ZE3dsMaxMapExporterDescription::ClassID()
{ 
	return ZE3dsMapExporter_CLASS_ID; 
}

const TCHAR* ZE3dsMaxMapExporterDescription::Category()
{
	return "Exporter"; 
}

const TCHAR* ZE3dsMaxMapExporterDescription::InternalName()
{ 
	return "ZEMap Exporter"; 
}

HINSTANCE ZE3dsMaxMapExporterDescription::HInstance()
{ 
	return hInstance; 
}

ClassDesc2* ZE3dsMaxMapExporterDescription::GetInstance()
{
	static ZE3dsMaxMapExporterDescription Desc;
	return &Desc;
}
