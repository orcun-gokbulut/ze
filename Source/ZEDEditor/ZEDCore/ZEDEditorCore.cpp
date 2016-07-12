//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEditorCore.cpp
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

#include "ZEDEditorCore.h"

#include "ZEMeta/ZEProvider.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEOptionManager.h"
#include "ZECore/ZESystemMessageManager.h"
#include "ZEDEditor.h"

#include <QApplication>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#undef RegisterClass

extern HINSTANCE ApplicationInstance;

void ZEDEditorCore::LoadClasses()
{
	#define ZE_META_REGISTER_ENUM(Name) ZEEnumerator* Name ## _Declaration();
	#define ZE_META_REGISTER_CLASS(Name) ZEClass* Name ## _Class();
	#include "../ZEMetaRegister.h"
	#undef ZE_META_REGISTER_ENUM
	#undef ZE_META_REGISTER_CLASS

	#define ZE_META_REGISTER_ENUM(Name) ZEProvider::GetInstance()->RegisterEnumerator(Name ## _Declaration());
	#define ZE_META_REGISTER_CLASS(Name) ZEProvider::GetInstance()->RegisterClass(Name ## _Class());
	#include "../ZEMetaRegister.h"
	#undef ZE_META_REGISTER_ENUM
	#undef ZE_META_REGISTER_CLASS
}

bool ZEDEditorCore::InitializeSelf()
{
	if (!ZEInitializable::InitializeSelf())
		return false;

	ZEInt argc = 0;
	char** argv = NULL;
	Application = new QApplication(argc, argv);

	EngineCore = ZECore::GetInstance();

	EngineCore->GetOptions()->Load("options.ini");
	EngineCore->GetOptions()->ResetChanges();
	EngineCore->GetSystemMessageManager()->SetEnabled(false);
	ApplicationInstance = *((HINSTANCE*)GetModuleHandle(NULL));

	LoadClasses();

	if (!EngineCore->StartUp())
	{
		zeError("Cannot start up core.");
		return false;
	}

	EngineCore->SetCoreState(ZE_CS_RUNNING);

	for (ZESize I = 0; I < Editors.GetCount(); I++)
		Editors[I]->Initialize();

	return true;
}

bool ZEDEditorCore::DeinitializeSelf()
{
	for (ZESize I = 0; I < Editors.GetCount(); I++)
		Editors[I]->Deinitialize();

	zeCore->ShutDown();
	Application->exit();

	return ZEInitializable::DeinitializeSelf();
}

ZEDEditorCore::ZEDEditorCore()
{
	Application = NULL;
	EngineCore = NULL;
	ExitFlag = false;
}

ZEDEditorCore::~ZEDEditorCore()
{
	delete Application;
}

const ZEArray<ZEDEditor*>& ZEDEditorCore::GetEditors()
{
	return Editors;
}

void ZEDEditorCore::ExecuteEditor(ZEDEditor* Editor)
{
	zeCheckError(Editor == NULL, ZE_VOID, "Editor is NULL.");
	zeCheckError(Editors.Exists(Editor), ZE_VOID, "Editor is already executing.");

	Editors.Add(Editor);
	Editor->Core = this;

	if (IsInitializedOrInitializing())
		Editor->Initialize();
}

void ZEDEditorCore::Execute()
{
	while(!ExitFlag)
	{
		ZECoreState State = EngineCore->GetCoreState();

		if (State == ZE_CS_TERMINATE || State ==  ZE_CS_SHUTDOWN)
			DeinitializeSelf();

		EngineCore->MainLoop();

		for (ZESize I = 0; I < Editors.GetCount(); I++)
			Editors[I]->Process(EngineCore->GetElapsedTime());

		for (ZESize I = 0; I < Editors.GetCount(); I++)
			Editors[I]->PostProcess(EngineCore->GetElapsedTime());

		Application->processEvents();
	}
}

void ZEDEditorCore::Exit()
{
	Deinitialize();
	std::exit(EXIT_SUCCESS);
}

void ZEDEditorCore::Destroy()
{
	delete this;
}

ZEDEditorCore* ZEDEditorCore::CreateInstance()
{
	return new ZEDEditorCore();
}
