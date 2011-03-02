//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMapExporter.cpp
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

#include "ZE3dsMapExporter.h"
//#include "data_mappings.h"
#include <tchar.h>
#include "ZETOutput.h"

ZE3dsMapExporter::ZE3dsMapExporter()
{

}

ZE3dsMapExporter::~ZE3dsMapExporter() 
{

}

int ZE3dsMapExporter::ExtCount()
{
	return 1;
}

const TCHAR *ZE3dsMapExporter::Ext(int n)
{		
	return "zeMap";
}

const TCHAR *ZE3dsMapExporter::LongDesc()
{
	return "Zinek Engine Map File";
}
	
const TCHAR *ZE3dsMapExporter::ShortDesc() 
{			
	return "Zinek Engine Map";
}

const TCHAR *ZE3dsMapExporter::AuthorName()
{			
	return "Zinek Code House and Game Studio";
}

const TCHAR *ZE3dsMapExporter::CopyrightMessage() 
{	
	return "Copyright (c) 2008-2011, Zinek Code House and Game Studio";
}

const TCHAR *ZE3dsMapExporter::OtherMessage1() 
{		
	return "";
}

const TCHAR *ZE3dsMapExporter::OtherMessage2() 
{		
	return "";
}

unsigned int ZE3dsMapExporter::Version()
{				
	return 100;
}

void ZE3dsMapExporter::ShowAbout(HWND hWnd)
{			

}

BOOL ZE3dsMapExporter::SupportsOptions(int ext, DWORD options)
{
	return TRUE;
}

int ZE3dsMapExporter::GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode /*=NULL*/)
{
	int i;
	if (i_currentNode == NULL)
	{
		i_currentNode = GetCOREInterface()->GetRootNode();
	}
	else // IGame will crash 3ds Max if it is initialised with the root node.
	{
	    i_nodeTab.AppendNode(i_currentNode);
	}
	for (i = 0; i < i_currentNode->NumberOfChildren(); i++)
	{
		GetSceneNodes(i_nodeTab, i_currentNode->GetChildNode(i));
	}
	return i_nodeTab.Count();
}

ZE3dsProgressDialog* PD;
void ZEToolSDKOutputCallback(const char* Output)
{
	PD->Output((char*)Output);
}

int	ZE3dsMapExporter::DoExport(const TCHAR* name, ExpInterface* ei,Interface* i, BOOL suppressPrompts, DWORD options)
{
	INodeTab lNodes;
	GetSceneNodes(lNodes);

	IGameConversionManager * cm = GetConversionManager();
	cm->SetCoordSystem(IGameConversionManager::IGAME_D3D);

	PD = &ProgDlg;

	ZESDKOutput::SetOutputLevel(ZET_OL_LOG);
	ZESDKOutput::SetOutputCallback(ZEToolSDKOutputCallback);

	ZEMapExporterOptionsDialog Dialog;
	char OptFile[MAX_PATH];
	sprintf(OptFile, "%s\\%s", i->GetDir(APP_PLUGCFG_DIR), "zemap_export_options.cfg");
	Options.Load(OptFile);
	if (!Dialog.ShowDialog(hInstance, Options))
		return false;

	Options.Save(OptFile);

	if (Options.Logging_OutputToFile == true)
		ProgDlg.StartFileLogging(Options.Logging_OutputFile);

	ProgDlg.Create(hInstance);
	ProgDlg.Show();
	Scene = GetIGameInterface();	
	Scene->InitialiseIGame(lNodes);

	zepdLog("Processing 3ds Max Scene...\r\n");
	if (!ProcessScene())
	{
		zepdError("Can not process scene.");
		ProgDlg.SetExitMode(true);
		ProgDlg.StopFileLogging();
		return false;
	}
	
	if (!ProcessPortals())
	{
		zepdError("Can not process portals.");
		ProgDlg.SetExitMode(true);
		ProgDlg.StopFileLogging();
		return false;
	}

	if (!ProcessDoors())
	{
		zepdError("Can not process doors.");
		ProgDlg.SetExitMode(true);
		ProgDlg.StopFileLogging();
		return false;
	}

	if (!ProcessMaterials())
	{
		zepdError("Can not process materials.");
		ProgDlg.SetExitMode(true);
		ProgDlg.StopFileLogging();
		return false;
	}
		
	zepdLog("Dumping map to file...");

	if (!Map.WriteToFile(name))
	{
		zepdError("Export failed !");
		ProgDlg.SetExitMode(true);
		ProgDlg.StopFileLogging();
		return true;
	}

	ZEMapFile MapFile2;

	zepdLog("Verifying written file...");
	MapFile2.ReadFromFile(name);

	zepdLog("Export succeed");
	ProgDlg.SetExitMode(true);
	ProgDlg.StopFileLogging();
	return true;
}
