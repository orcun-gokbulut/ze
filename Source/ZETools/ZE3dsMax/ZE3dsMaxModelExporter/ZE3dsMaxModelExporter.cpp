//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxModelExporter.cpp
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

#include "ZE3dsMaxModelExporter.h"

#include <tchar.h>
#include "ZEToolComponents/QWinWidget/qwinwidget.h"
#include "QtGui/QApplication"
#include "ZE3dsMaxModelExporterOptionsDialog.h"
#include "ZEToolComponents/ZEProgressDialog/ZEProgressDialog.h"
#include "ZEToolComponents/ZEResourceConfigurationWidget/ZEResourceConfigurationWidget.h"
#include "ZEFile/ZEFile.h"
#include "ZEFile/ZEFileInfo.h"

ZE3dsMaxModelExporter::ZE3dsMaxModelExporter()
{
	Scene = NULL;
	QtApplication = NULL;
	ExportOptions = NULL;
	WinWidget = NULL;
	OptionsDialog = NULL;
	ProgressDialog = NULL;
	ResourceConfigurationDialog = NULL;

	TotalFrameCount = 0;
	TicksPerFrame = 0;
}

ZE3dsMaxModelExporter::~ZE3dsMaxModelExporter() 
{
	if(ExportOptions != NULL)
	{
		delete ExportOptions;
		ExportOptions = NULL;
	}

	if(ProgressDialog != NULL)
	{
		delete ProgressDialog;
		ProgressDialog = NULL;
	}

	if(OptionsDialog != NULL)
	{
		OptionsDialog->hide();
		delete OptionsDialog;
		OptionsDialog = NULL;
	}

	if(ResourceConfigurationDialog != NULL)
	{
		ResourceConfigurationDialog->Hide();
		delete ResourceConfigurationDialog;
		ResourceConfigurationDialog = NULL;
	}

	if(WinWidget != NULL)
	{
		WinWidget->hide();
		delete WinWidget;
		WinWidget = NULL;
	}

	if(QtApplication != NULL)
	{
		QtApplication->quit();
		delete QtApplication;
		QtApplication = NULL;
	}
}

ZEInt ZE3dsMaxModelExporter::ExtCount()
{
	return 1;
}

const TCHAR *ZE3dsMaxModelExporter::Ext(ZEInt n)
{		
	return "zeModel";
}

const TCHAR *ZE3dsMaxModelExporter::LongDesc()
{
	return "Zinek Engine Model File";
}
	
const TCHAR *ZE3dsMaxModelExporter::ShortDesc() 
{			
	return "Zinek Engine Model";
}

const TCHAR *ZE3dsMaxModelExporter::AuthorName()
{			
	return "Zinek Engine Staff";
}

const TCHAR *ZE3dsMaxModelExporter::CopyrightMessage() 
{	
	return "Copyright (c) 2008, Zinek Engine Staff";
}

const TCHAR *ZE3dsMaxModelExporter::OtherMessage1() 
{		
	return "";
}

const TCHAR *ZE3dsMaxModelExporter::OtherMessage2() 
{		
	return "";
}

ZEUInt ZE3dsMaxModelExporter::Version()
{				
	return 300;
}

void ZE3dsMaxModelExporter::ShowAbout(HWND hWnd)
{			

}

BOOL ZE3dsMaxModelExporter::SupportsOptions(ZEInt ext, DWORD options)
{
	return TRUE;
}


ZEInt32 ZE3dsMaxModelExporter::GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode /*=NULL*/)
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

void ZE3dsMaxModelExporter::LoadOptions(const char* FilePath)
{
	if(strlen(FilePath) != 0)
	{
		ZEString OptionsFilePath(FilePath);
		OptionsFilePath += ".zecfg";

		ZEFile OptionsFile;

		if(ExportOptions == NULL)
			ExportOptions = new ZEMLNode("Options");

		if(OptionsFile.Open(OptionsFilePath, ZE_FOM_READ_WRITE, ZE_FCM_NONE))
		{
			ModelRoot.SetRootNode(ExportOptions);
			ModelRoot.Read(&OptionsFile);
			OptionsFile.Close();
		}
	}
}

void ZE3dsMaxModelExporter::SaveOptions(const char* FilePath)
{
	ZEString OptionsFilePath(FilePath);
	OptionsFilePath += ".zecfg";

	if(ExportOptions != NULL)
	{
		ZEFile OptionsFile;
		if(OptionsFile.Open(OptionsFilePath, ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE))
		{
			ModelRoot.SetRootNode(OptionsDialog->GetOptions());
			ModelRoot.Write(&OptionsFile);
			OptionsFile.Close();
		}
	}
	else
	{
		ExportOptions = OptionsDialog->GetOptions();
	}
}

bool ZE3dsMaxModelExporter::ShowOptionsDialog(HWND ParentWindow)
{
	int Argc = 0;
	if(QApplication::instance() == NULL)
		QtApplication = new QApplication(Argc, NULL);
	else
		QtApplication = (QApplication*)QApplication::instance();

	if(WinWidget == NULL)
		WinWidget = new QWinWidget(ParentWindow);

	if(OptionsDialog == NULL)
		OptionsDialog = new ZE3dsMaxModelExporterOptionsDialog(WinWidget);

	if(ExportOptions != NULL)
		OptionsDialog->SetOptions(ExportOptions);

	WinWidget->showCentered();
	ZEInt32 DialogResult = OptionsDialog->exec();

	if(DialogResult == QDialog::Rejected)
		return false;

	return true;
}

bool ZE3dsMaxModelExporter::ShowResourceConfigurationDialog(HWND ParentWindow, const char* MaxFilePath)
{
	int Argc = 0;
	if(QApplication::instance() == NULL)
		QtApplication = new QApplication(Argc, NULL);
	else
		QtApplication = (QApplication*)QApplication::instance();

	if(WinWidget == NULL)
		WinWidget = new QWinWidget(ParentWindow);

	if(ResourceConfigurationDialog == NULL)
		ResourceConfigurationDialog = new ZEResourceConfigurationWidget(WinWidget);

	ResourceConfigurationDialog->LoadPreset(ZEString(MaxFilePath) + ".ZEPRESET");
	CollectResources();

	WinWidget->showCentered();
	ZEInt32 DialogResult = ResourceConfigurationDialog->Show();

	if(DialogResult == QDialog::Rejected)
		return false;

	ResourceConfigurationDialog->SavePreset(ZEString(MaxFilePath) + ".ZEPRESET");
	return true;
}

ZEInt ZE3dsMaxModelExporter::DoExport(const TCHAR* name, ExpInterface* ei,Interface* i, BOOL suppressPrompts, DWORD options)
{
	ExportPath = ZEFileInfo(name).GetParentDirectory();
	LoadOptions(i->GetCurFilePath());

	INodeTab lNodes;
	GetSceneNodes(lNodes);
	IGameConversionManager * cm = GetConversionManager();
	cm->SetCoordSystem(IGameConversionManager::IGAME_D3D);
	Scene = GetIGameInterface();	
	Scene->InitialiseIGame(lNodes);
	Scene->SetStaticFrame(0);
	TicksPerFrame = Scene->GetSceneTicks();
	TotalFrameCount = Scene->GetSceneEndTime() / TicksPerFrame;

	if(!ShowOptionsDialog(i->GetMAXHWnd()))
		return true;

	if(!ShowResourceConfigurationDialog(i->GetMAXHWnd(), i->GetCurFilePath()))
		return true;

	SaveOptions(i->GetCurFilePath());

	ModelRoot.SetRootNode(&ModelNode);
	ModelNode.SetName("ZEModel");
	ZEUInt8 MajorVersion = 1;
	ZEUInt8 MinorVersion = 0;
	ModelNode.AddProperty("MajorVersion")->SetUInt8(MajorVersion);
	ModelNode.AddProperty("MinorVersion")->SetUInt8(MinorVersion);

	if(ProgressDialog == NULL)
		ProgressDialog = ZEProgressDialog::CreateInstance();

	ProgressDialog->SetTitle("Model Export Progress");
	ProgressDialog->SetProgressBarVisibility(false);
	ProgressDialog->Start();
	ProgressDialog->OpenTask("Model Exporter", true);

	zeLog("Exporting model to file \"%s\".", name);

	Tab<IGameNode*> Meshes = Scene->GetIGameNodeByType(IGameObject::IGAME_MESH);
	const char* Type;

	for (ZESize I = 0; I < (ZESize)Meshes.Count(); I++)
	{
		Type = NULL;
		if (ZE3dsMaxUtils::GetProperty(Meshes[I]->GetIGameObject(), ZE_STRING_PROP, "ZEType", Type) && strcmp(Type, "BoundingBox") == 0)
		{
			IGameMesh* BoundingBoxMesh = (IGameMesh*)Meshes[I]->GetIGameObject();

			Box3 TempBoundingBox;
			BoundingBoxMesh->GetBoundingBox(TempBoundingBox);
			ZEMLNode* UserDefinedBoundingBox = ModelNode.AddNode("UserDefinedBoundingBox");
			UserDefinedBoundingBox->AddProperty("Min")->SetVector3(ZE3dsMaxUtils::MaxtoZE(TempBoundingBox.pmin));
			UserDefinedBoundingBox->AddProperty("Max")->SetVector3(ZE3dsMaxUtils::MaxtoZE(TempBoundingBox.pmax));

			break;
		}
	}

	ProgressDialog->OpenTask("Bone Process", true);
	if (!ProcessBones())
	{
		zeError("Processing bones failed.");
		return false;
	}
	ProgressDialog->CloseTask();

	ProgressDialog->OpenTask("Mesh Process", true);
	if (!ProcessMeshes())
	{
		zeError("Processing meshes failed.");
		return false;
	}
	ProgressDialog->CloseTask();

	ProgressDialog->OpenTask("Helper Process", true);
	if(!ProcessHelpers())
	{
		zeError("Processing helpers failed.");
		return false;
	}
	ProgressDialog->CloseTask();

	ProgressDialog->OpenTask("Animation Process", true);

	bool IsAnimationExportEnabled = ((ZEMLProperty*)(ExportOptions->GetProperty("IsAnimationExportEnabled")))->GetValue().GetBoolean();

	if (IsAnimationExportEnabled)
	{
		if(!ProcessAnimations(ModelNode.AddNode("Animations")))
		{
			zeError("Processing animations failed.");
			return false;
		}
	}
	ProgressDialog->CloseTask();

	ProgressDialog->OpenTask("Material Process", true);
	if (!ProcessMaterials(name))
	{
		zeError("Processing materials failed.");
		return false;
	}
	ProgressDialog->CloseTask();

	ProgressDialog->OpenTask("Writing File");
	zeLog("Writing ZEModel to file...");
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

bool ZE3dsMaxModelExporter::WriteToFile(const char* FilePath)
{
	ZEFile File;

	if (!File.Open(FilePath, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
	{
		zeError("Cant open file to write ZEModel file.");
		return false;
	}
	
	bool CurrentOption = false;

	CurrentOption = ((ZEMLProperty*)(ExportOptions->GetProperty("IsBoneExportEnabled")))->GetValue().GetBoolean();

	if (!CurrentOption)
	{
		if (ModelNode.GetNodes("Bones").GetCount() > 0)
		{
			ZEMLNode* BonesNode = ModelNode.GetNodes("Bones").GetFirstItem();
			ModelNode.RemoveElement(BonesNode);
		}
	}

	CurrentOption = ((ZEMLProperty*)(ExportOptions->GetProperty("IsMeshExportEnabled")))->GetValue().GetBoolean();

	if (!CurrentOption)
	{
		if (ModelNode.GetNodes("Meshes").GetCount() > 0)
		{
			ZEMLNode* MeshesNode = ModelNode.GetNodes("Meshes").GetFirstItem();
			ModelNode.RemoveElement(MeshesNode);
		}
	}

	ModelRoot.Write(&File);

	File.Close();

	return true;
}
