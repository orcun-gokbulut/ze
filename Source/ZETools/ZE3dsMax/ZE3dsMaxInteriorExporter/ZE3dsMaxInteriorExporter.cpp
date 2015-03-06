//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxInteriorExporter.cpp
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

#include "ZE3dsMaxInteriorExporter.h"
//#include "data_mappings.h"

#include <tchar.h>
#include "QWinWidget/qwinwidget.h"
#include "QtGui/QApplication"
#include "ZE3dsMaxInteriorExporterOptionsDialog.h"
#include "ZEToolComponents/ZEProgressDialog/ZEProgressDialog.h"
#include "ZEToolComponents/ZEResourceConfigurationWidget/ZEResourceConfigurationWidget.h"
#include "ZEFile/ZEFile.h"
#include "ZEFile/ZEFileInfo.h"

ZE3dsMaxInteriorExporter::ZE3dsMaxInteriorExporter()
{
	Scene = NULL;
	QtApplication = NULL;
	ExportOptions = NULL;
	WinWidget = NULL;
	OptionsDialog = NULL;
	ProgressDialog = NULL;
	ResourceConfigurationDialog = NULL;
}

ZE3dsMaxInteriorExporter::~ZE3dsMaxInteriorExporter() 
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

ZEInt ZE3dsMaxInteriorExporter::ExtCount()
{
	return 1;
}

const TCHAR *ZE3dsMaxInteriorExporter::Ext(ZEInt n)
{		
	return "zeInterior";
}

const TCHAR *ZE3dsMaxInteriorExporter::LongDesc()
{
	return "Zinek Engine Interior File";
}
	
const TCHAR *ZE3dsMaxInteriorExporter::ShortDesc() 
{			
	return "Zinek Engine Interior";
}

const TCHAR *ZE3dsMaxInteriorExporter::AuthorName()
{			
	return "Zinek Code House and Game Studio";
}

const TCHAR *ZE3dsMaxInteriorExporter::CopyrightMessage() 
{	
	return "Copyright (c) 2008-2011, Zinek Code House and Game Studio";
}

const TCHAR *ZE3dsMaxInteriorExporter::OtherMessage1() 
{		
	return "";
}

const TCHAR *ZE3dsMaxInteriorExporter::OtherMessage2() 
{		
	return "";
}

ZEUInt ZE3dsMaxInteriorExporter::Version()
{				
	return 100;
}

void ZE3dsMaxInteriorExporter::ShowAbout(HWND hWnd)
{			

}

BOOL ZE3dsMaxInteriorExporter::SupportsOptions(ZEInt ext, DWORD options)
{
	return TRUE;
}

ZEInt32 ZE3dsMaxInteriorExporter::GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode /*=NULL*/)
{
	ZEInt32 i;
	if (i_currentNode == NULL)
	{
		i_currentNode = GetCOREInterface()->GetRootNode();
	}
	else // IGame will crash 3ds Max if it is initialized with the root node.
	{
	    i_nodeTab.AppendNode(i_currentNode);
	}
	for (i = 0; i < i_currentNode->NumberOfChildren(); i++)
	{
		GetSceneNodes(i_nodeTab, i_currentNode->GetChildNode(i));
	}
	return i_nodeTab.Count();
}

void ZE3dsMaxInteriorExporter::LoadOptions(const char* FilePath)
{
	ZEString OptionsFilePath((ZEString(FilePath)) + ".zecfg");

	if(strlen(FilePath) != 0)
	{
		ZEFile OptionsFile;

		if(ExportOptions == NULL)
			ExportOptions = new ZEMLNode("Options");

		if(OptionsFile.Open(OptionsFilePath, ZE_FOM_READ, ZE_FCM_NONE))
		{
			ExportOptions->Read(&OptionsFile);
			OptionsFile.Close();
		}
		else
			zeWarning("Can not open options file.");
	}
}

void ZE3dsMaxInteriorExporter::SaveOptions(const char* FilePath)
{
	ZEString OptionsFilePath((ZEString(FilePath)) + ".zecfg");

	if(ExportOptions != NULL)
	{
		ZEFile OptionsFile;
		if(OptionsFile.Open(OptionsFilePath, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		{
			OptionsDialog->GetOptions()->Write(&OptionsFile);
			OptionsFile.Close();
		}
		else
			zeWarning("Can not save options file.");
	}
	else
	{
		ExportOptions = OptionsDialog->GetOptions();
	}
}

bool ZE3dsMaxInteriorExporter::ShowOptionsDialog(HWND ParentWindow)
{
	int Argc = 0;
	if(QApplication::instance() == NULL)
		QtApplication = new QApplication(Argc, NULL);
	else
		QtApplication = (QApplication*)QApplication::instance();

	if(WinWidget == NULL)
		WinWidget = new QWinWidget(ParentWindow);

	if(OptionsDialog == NULL)
		OptionsDialog = new ZE3dsMaxInteriorExporterOptionsDialog(WinWidget);

	if(ExportOptions != NULL)
		OptionsDialog->SetOptions(ExportOptions);

	WinWidget->showCentered();
	ZEInt32 DialogResult = OptionsDialog->exec();

	if(DialogResult == QDialog::Rejected)
		return false;

	return true;
}

bool ZE3dsMaxInteriorExporter::ShowResourceConfigurationDialog(HWND ParentWindow, const char* MaxFilePath, ZEString ExportPath)
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

	ResourceConfigurationDialog->SetDefaultExportPath(ExportPath);
	ResourceConfigurationDialog->LoadPreset(ZEString(MaxFilePath) + ".ZEPRESET");
	CollectResources();

	WinWidget->showCentered();
	ZEInt32 DialogResult = ResourceConfigurationDialog->Show();

	if(DialogResult == QDialog::Rejected)
		return false;

	ResourceConfigurationDialog->SavePreset(ZEString(MaxFilePath) + ".ZEPRESET");
	return true;
}

ZEInt ZE3dsMaxInteriorExporter::DoExport(const TCHAR* name, ExpInterface* ei,Interface* i, BOOL suppressPrompts, DWORD options)
{
	ExportPath = ZEFileInfo(name).GetParentDirectory();
	LoadOptions(i->GetCurFilePath());

	INodeTab lNodes;
	GetSceneNodes(lNodes);
	IGameConversionManager * cm = GetConversionManager();
	cm->SetCoordSystem(IGameConversionManager::IGAME_D3D);
	Scene = GetIGameInterface();	
	Scene->InitialiseIGame(lNodes);

	if(!ShowOptionsDialog(i->GetMAXHWnd()))
		return true;

	if(!ShowResourceConfigurationDialog(i->GetMAXHWnd(), i->GetCurFilePath(), ExportPath))
		return true;

	SaveOptions(i->GetCurFilePath());

	InteriorNode.SetName("ZEInterior");

	if(ProgressDialog == NULL)
		ProgressDialog = ZEProgressDialog::CreateInstance();

	ProgressDialog->SetTitle("Interior Export Progress");
	ProgressDialog->SetProgressBarVisibility(false);
	ProgressDialog->SetFileLoggingEnabled(OptionsDialog->GetFileLoggingEnabled());
	ProgressDialog->SetLogFilePath(OptionsDialog->GetLogFilePath());
	ProgressDialog->Start();
	ProgressDialog->OpenTask("Interior Exporter", true);

	ProgressDialog->OpenTask("Scene Process");
	zeLog("Processing 3ds Max Scene...");
	if (!ProcessScene())
	{
		zeError("Can not process scene.");
		return false;
	}
	ProgressDialog->CloseTask();
	
	ProgressDialog->OpenTask("Rooms", true);
	if (!ProcessRooms())
	{
		zeError("Can not process rooms.");
		return false;
	}
	ProgressDialog->CloseTask();

	ProgressDialog->OpenTask("Doors", true);
	if (!ProcessDoors())
	{
		zeError("Can not process doors.");
		return false;
	}
	ProgressDialog->CloseTask();

	ProgressDialog->OpenTask("Helpers", true);
	if (!ProcessHelpers())
	{
		zeError("Can not process helpers.");
		return false;
	}
	ProgressDialog->CloseTask();

	ProgressDialog->OpenTask("Materials", true);
	if (!ProcessMaterials(name))
	{
		zeError("Can not process materials.");
		return false;
	}
	ProgressDialog->CloseTask();
	
	InteriorNode.AddProperty("RoomCount", (ZEUInt32)Rooms.Count());
	InteriorNode.AddProperty("DoorCount", (ZEUInt32)Doors.Count());
	InteriorNode.AddProperty("HelperCount", (ZEUInt32)Helpers.Count());
	InteriorNode.AddProperty("MaterialCount", (ZEUInt32)Materials.Count());

	ProgressDialog->OpenTask("Writing File");
	zeLog("Writing ZEInterior to file...");
	ZEFile ExportFile;
	if(ExportFile.Open(name, ZE_FOM_READ_WRITE, ZE_FCM_OVERWRITE))
	{
		InteriorNode.Write(&ExportFile);
		ExportFile.Close();
	}
	ProgressDialog->CloseTask();

	zeLog("Export process completed succesfully.");
	ProgressDialog->End();

	return true;
}
