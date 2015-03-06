//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEResourceOptionTreeItem.cpp
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

#include "ZEResourceOptionTreeItem.h"
#include "ZEDS\ZEString.h"
#include "ZEFile\ZEFileInfo.h"

ZEResourceOptionTreeItem::ZEResourceOptionTreeItem()
{
	ResourceOption = NULL;
	StatusString = "";
}

void ZEResourceOptionTreeItem::Update()
{
	setText(0, ZEString(ResourceOption->Identifier).ToCString());
	setText(1, ZEString(ResourceOption->Type).ToCString());
	setText(2, ResourceOption->PhysicalPath.ToCString());
	setText(3, ResourceOption->ExportPath.ToCString());

	if(ResourceOption->Action == ZE_ROAA_DO_NOTHING)
		setText(5, "Do Nothing");
	else if(ResourceOption->Action == ZE_ROAA_COPY_OVERWRITE)
		setText(5, "Overwrite");
	else if(ResourceOption->Action == ZE_ROAA_COPY_IF_MISSING)
		setText(5, "Copy If Missing");
	else
		setText(5, "Copy If Newer");

	StatusString = "";

	if(ResourceOption->PhysicalPath.GetLength() != 0)
	{
		if(ZEFileInfo(ResourceOption->PhysicalPath).IsFile())
			StatusString += "(P)OK ";
		else
			StatusString += "(P)NOK ";
	}
	else
		StatusString += "(P)NOK ";

	if(ResourceOption->ExportPath.GetLength() != 0)
	{
		if(ZEFileInfo(ResourceOption->ExportPath + "\\" + ResourceOption->Identifier).IsFile())
			StatusString += "/ (E)OK";
		else
			StatusString += "/ (E)NOK";
	}
	else
		StatusString += "/ (E)NOK";

	setText(4, StatusString.ToCString());
}

void ZEResourceOptionTreeItem::SetResourceOption(ZEResourceOption* Option)
{
	this->ResourceOption = Option;
	Update();
}

ZEResourceOption* ZEResourceOptionTreeItem::GetResourceOption() const
{
	return ResourceOption;
}
