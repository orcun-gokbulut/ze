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

#include <tchar.h>
#include "QWinWidget/qwinwidget.h"
#include "QtGui/QApplication"
#include "ZE3dsModelExporterOptionsDialog.h"
#include "ZEFile/ZEFileUtils.h"
#include "ZEFile/ZEFile.h"
#include "ZEToolComponents/ZEProgressDialog/ZEProgressDialog.h"

ZEModelExporter::ZEModelExporter()
{
	Scene = NULL;
	QtApplication = NULL;
	ModelNode.SetName("ZEModel");

	FrameCount = 0;
	TicksPerFrame = 0;
}

ZEModelExporter::~ZEModelExporter() 
{
	QtApplication->quit();
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
	return _T("");
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

ZEInt ZEModelExporter::DoExport(const TCHAR* name, ExpInterface* ei,Interface* i, BOOL suppressPrompts, DWORD options)
{
	INodeTab lNodes;
	GetSceneNodes(lNodes);

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

	int Argc = 0;
	ZEMLNode* ExportOptions = new ZEMLNode("Options");
	ZEString OptionsFilePath((ZEString(i->GetCurFilePath().data())) + ".zecfg");
	ZEFile* OptionsFile = new ZEFile();

	if(OptionsFile->Open(OptionsFilePath, ZE_FOM_READ_WRITE, ZE_FCM_NONE))
		ExportOptions->Read(OptionsFile);

	if(OptionsFile->IsOpen())
		OptionsFile->Close();

	if(QApplication::instance() == NULL)
		QtApplication = new QApplication(Argc, NULL);

	QWinWidget* ExporterWindow = new QWinWidget(i->GetMAXHWnd());
	ZE3dsModelExporterOptionsDialogNew* ExporterDialog = new ZE3dsModelExporterOptionsDialogNew(ExporterWindow, ExportOptions);
	ExporterWindow->showCentered();
	ZEInt DialogResult = ExporterDialog->exec();

	if(DialogResult == QDialog::Rejected)
		return false;

	if(strlen(i->GetCurFilePath().data()) != 0)
	{
		OptionsFile->Open(OptionsFilePath, ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE);
		ExporterDialog->GetOptions()->Write(OptionsFile);
		OptionsFile->Close();
		delete OptionsFile;
	}

	ProgDlg.Create(hInstance);
	//ProgDlg.SetOutputLevel(Options.OutputLevel);
	ProgDlg.Show();
	Scene = GetIGameInterface();	
	Scene->InitialiseIGame(lNodes);
	Scene->SetStaticFrame(0);
	TicksPerFrame = Scene->GetSceneTicks();
	FrameCount = Scene->GetSceneEndTime() / TicksPerFrame;

	ZEProgressDialog* ProgressDialog = ZEProgressDialog::CreateInstance();
	ProgressDialog->SetTitle("Model Export Progress");
	ProgressDialog->SetProgressBarVisibility(false);
	ProgressDialog->Start();
	ProgressDialog->OpenTask("Model Exporter", true);

	zeLog("Exporting model to file \"%s\".", name);

	ProgressDialog->OpenTask("Bone Process", true);
	if (!ProcessBones(ModelNode.AddSubNode("Bones")))
	{
		zeError("Processing bone failed.");
		return false;
	}
	ProgressDialog->CloseTask();

	ProgressDialog->OpenTask("Mesh Process", true);
	if (!ProcessMeshes(ModelNode.AddSubNode("Meshes")))
	{
		zeError("Processing mesh failed.");
		return false;
	}
	ProgressDialog->CloseTask();

	ProgressDialog->OpenTask("Animation Process", true);
	if (true/*Options.ExportAnimation*/)
	{
		if(!ProcessAnimation(ModelNode.AddSubNode("Animation")))
		{
			zeError("Processing animation failed.");
			return false;
		}
	}
	ProgressDialog->CloseTask();

	ProgressDialog->OpenTask("Material Process", true);
	if (!ProcessMaterials(name, ModelNode.AddSubNode("Materials")))
	{
		zeError("Processing materials failed.");
		return false;
	}
	ProgressDialog->CloseTask();

	ProgressDialog->OpenTask("Writing File");
	zeLog("Writing zeModel to file.");
	if (!WriteToFile(name))
	{
		zeError("Writing model to file failed.");
		return false;
	}
	ProgressDialog->CloseTask();

	zeLog("Export process completed succesfully.");
	ProgressDialog->End();

	return TRUE;
}

bool ZEModelExporter::WriteToFile(const char* FilePath)
{
	ZEFile File;

	if (!File.Open(FilePath, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		zeError("Cant open file to write ZEModel file.");
		return false;
	}
	
	ModelNode.Write(&File);

	File.Close();

	return true;
}
