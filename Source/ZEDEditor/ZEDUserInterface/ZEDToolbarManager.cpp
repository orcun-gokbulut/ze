//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDToolbarManager.cpp
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

#include "ZEDToolbarManager.h"

#include "ZEDToolbar.h"

#include "ZEML/ZEMLReader.h"
#include "ZEML/ZEMLWriter.h"

ZEDToolbarManager::ZEDToolbarManager()
{

}

ZEDToolbarManager::~ZEDToolbarManager()
{
	for (ZESize I = 0; I < Toolbars.GetCount(); I++)
		Toolbars[I]->Destroy();
	Toolbars.Clear();
}

const ZEArray<ZEDToolbar*>& ZEDToolbarManager::GetToolbars()
{
	return Toolbars;
}

ZEDToolbar* ZEDToolbarManager::GetToolbar(const ZEString& Name)
{
	for (ZESize I = 0; I < Toolbars.GetCount(); I++)
	{
		if (Toolbars[I]->GetName() == Name)
			return Toolbars[I];
	}

	return NULL;
}

bool ZEDToolbarManager::AddToolbar(ZEDToolbar* Toolbar)
{
	zeCheckError(Toolbar == NULL, false, "Cannot add toolbar. Toolbar is NULL.");
	zeCheckError(Toolbars.Exists(Toolbar), false, "Cannot add toolbar. Toolbar has been already added. Toolbar Name: \"%s\".", Toolbar->GetName().ToCString());

	for (ZESize I = 0; I < Toolbars.GetCount(); I++)
	{
		if (Toolbars[I]->GetName() == Toolbar->GetName())
		{
			zeError("Cannot add toolbar. Another toolbar with the same name has been already added. Toolbar Name: \"%s\".", Toolbar->GetName().ToCString());
			return false;
		}
	}

	Toolbar->Manager = this;
	Toolbars.Add(Toolbar);
	
	return true;
}

bool ZEDToolbarManager::RemoveToolbar(ZEDToolbar* Toolbar)
{
	zeCheckError(Toolbar == NULL, false, "Cannot remove toolbar. Toolbar is NULL.");
	zeCheckError(!Toolbars.Exists(Toolbar), false, "Cannot remove toolbar. Toolbar is not added. Toolbar Name: \"%s\".", Toolbar->GetName().ToCString());

	Toolbar->Manager = NULL;
	Toolbars.RemoveValue(Toolbar);

	return true;
}

void ZEDToolbarManager::ClearToolbars()
{
	while(Toolbars.GetCount() != 0)
		Toolbars.GetFirstItem()->Destroy();
}

bool ZEDToolbarManager::Load(const ZEString& ConfigurationFile)
{
	ClearToolbars();

	ZEMLReader Reader;
	if (!Reader.Open(ConfigurationFile))
	{
		zeError("Cannor load configuration. File Name: \"%s\".", ConfigurationFile.ToCString());
		return false;
	}

	ZEMLReaderNode RootNode = Reader.GetRootNode();
	if (!RootNode.IsValid() || RootNode.GetName() != "ZEDToolbarManager")
	{
		zeError("Cannor load configuration. Unknown structure. File Name: \"%s\".", ConfigurationFile.ToCString());
		return false;
	}

	ZEMLReaderNode ToolbarsNode = RootNode.GetNode("Toolbars");
	if (ToolbarsNode.IsValid())
	{
		ZESize ToolbarNodeCount = ToolbarsNode.GetNodeCount("Toolbar");
		for (ZESize I = 0; I < ToolbarNodeCount; I++)
		{
			ZEMLReaderNode& ToolbarNode = ToolbarsNode.GetNode("Toolbar", I);
			ZEDToolbar* NewToolbar = ZEDToolbar::CreateInstance();
			if (!NewToolbar->Load(&ToolbarNode))
			{
				NewToolbar->Destroy();
				continue;
			}

			AddToolbar(NewToolbar);
		}
	}

	Update();

	return true;
}

bool ZEDToolbarManager::Save(const ZEString& ConfigurationFile)
{
	ZEMLWriter Writer;
	Writer.SetFormat(ZEMLFormat::GetDefaultTextFormat()->CreateInstance());
	if (!Writer.Open(ConfigurationFile))
	{
		zeError("Cannor load configuration. File Name: \"%s\".", ConfigurationFile.ToCString());
		return false;
	}

	ZEMLWriterNode RootNode;
	if (!Writer.OpenRootNode("ZEDToolbarManager", RootNode))
		return false;

	ZEMLWriterNode ToolbarsNode;
	RootNode.OpenNode("Toolbars", ToolbarsNode);

	for (ZESize I = 0; I < Toolbars.GetCount(); I++)
		Toolbars[I]->Save(&ToolbarsNode);


	ToolbarsNode.CloseNode();
	RootNode.CloseNode();
	Writer.Close();

	return false;
}

void ZEDToolbarManager::Update()
{
	for (ZESize I = 0; I < Toolbars.GetCount(); I++)
		Toolbars[I]->Update();
}

void ZEDToolbarManager::Destroy()
{
	delete this;
}

ZEDToolbarManager* ZEDToolbarManager::CreateInstance()
{
	return new ZEDToolbarManager;
}
