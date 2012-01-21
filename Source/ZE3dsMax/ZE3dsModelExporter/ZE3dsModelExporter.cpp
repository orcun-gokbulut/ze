//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsModelExporter.cpp
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

#include "ZE3dsModelExporter.h"
#include "ZETOutput.h"

ZEModelExporter::ZEModelExporter()
{
	Scene = NULL;
	FrameCount = 0;
	TicksPerFrame = 0;
}

ZEModelExporter::~ZEModelExporter() 
{

}

ZEInt ZEModelExporter::ExtCount()
{
	return 1;
}

const TCHAR *ZEModelExporter::Ext(ZEInt n)
{		
	return _T("zeModel");
}

const TCHAR *ZEModelExporter::LongDesc()
{
	return _T("Zinek Engine Model File");
}
	
const TCHAR *ZEModelExporter::ShortDesc() 
{			
	return _T("Zinek Engine Model");
}

const TCHAR *ZEModelExporter::AuthorName()
{			
	return _T("Zinek Engine Staff");
}

const TCHAR *ZEModelExporter::CopyrightMessage() 
{	
	return _T("Copyright (c) 2008, Zinek Engine Staff");
}

const TCHAR *ZEModelExporter::OtherMessage1() 
{		
	return _T("Orcun rulz !!!");
}

const TCHAR *ZEModelExporter::OtherMessage2() 
{		
	return _T("");
}

ZEUInt ZEModelExporter::Version()
{				
	return 300;
}

void ZEModelExporter::ShowAbout(HWND hWnd)
{			

}

BOOL ZEModelExporter::SupportsOptions(ZEInt ext, DWORD options)
{
	return TRUE;
}


ZEInt ZEModelExporter::GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode /*=NULL*/)
{
	ZEInt i;
	if (i_currentNode == NULL)
	{
		i_currentNode = GetCOREInterface()->GetRootNode();
	}
	else
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
void Output(const char* Output)
{
	
	PD->Output((char*)Output);
}

ZEInt	ZEModelExporter::DoExport(const TCHAR* name, ExpInterface* ei,Interface* i, BOOL suppressPrompts, DWORD options)
{
	PD = &ProgressDialog;
	ProgressDialog.Create(hInstance);
	ZEModelExporterOptionsDialog OptionsDialog;

	IGameConversionManager * cm = GetConversionManager();
	cm->SetCoordSystem(IGameConversionManager::IGAME_D3D);
	/*UserCoord UserCoord = {
		0,	//Left Handed
		1,	//X axis goes right
		2,	//Y Axis goes up
		5,	//Z Axis goes out.
		1,	//U Tex axis is right
		1,  //V Tex axis is down
	};*/
	//cm->SetUserCoordSystem(UserCoord);


	INodeTab lNodes;
	GetSceneNodes(lNodes);

	Scene = GetIGameInterface();	
	Scene->InitialiseIGame(lNodes);
	Scene->SetStaticFrame(0);

	TicksPerFrame = Scene->GetSceneTicks();
	FrameCount = Scene->GetSceneEndTime() / TicksPerFrame;

	char OptFile[MAX_PATH];
	sprintf(OptFile, "%s\\%s", i->GetDir(APP_PLUGCFG_DIR), "zemodel_export.cfg");
	ExporterOptions.Load(OptFile);
	if (!OptionsDialog.ShowDialog(hInstance, ExporterOptions))
		return false;

	ExporterOptions.Save(OptFile);
	ProgressDialog.SetOutputLevel((ZE3dsProgressDialogOutputLevel)ExporterOptions.OutputLevel);
	ZESDKOutput::SetOutputLevel(ZET_OL_DEBUG);
	ZESDKOutput::SetOutputCallback(Output);

	/*TicksPerFrame = Scene->GetSceneTicks();
	FrameCount = Scene->GetSceneEndTime() / TicksPerFrame;
	*/
	zepdLog("Exporting model to file \"%s\".", name);

	if (!ProcessBones())
	{
		zepdError("Processing bone failed.");
		return false;
	}

	if (!ProcessMeshes())
	{
		zepdError("Processing mesh failed.");
		return false;
	}

	if (ExporterOptions.ExportAnimation)
		if(!ProcessAnimation())
		{
			zepdError("Processing animation failed.");
			return false;
		}

	if (!ProcessMaterials())
	{
		zepdError("Processing materials failed.");
		return false;
	}

	zepdLog("Writing zeModel to file.");
	if (!ModelFile.WriteToFile(name))
	{
		zepdError("Writing model to file failed.");
		return false;
	}

	zepdLog("Export process completed succesfully.");
	ProgressDialog.SetExitMode(true);
	return TRUE;
}
