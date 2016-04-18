//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDCore.cpp
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

#include "ZEDCore.h"
#include "ZEDOperationManager.h"
#include "ZEDSelectionManager.h"
#include "ZEDTransformationManager.h"
#include "ZEDModule.h"
#include "ZEDObjectWrapper.h"
#include "ZEDViewport.h"
#include "ZEDS/ZEString.h"
#include "ZECore/ZECore.h"
#include "ZECore/ZEWindow.h"
#include "ZECore/ZEOptionManager.h"
#include "ZEMeta/ZEProvider.h"
#include <windows.h>
#include <QtCore/QObject>

extern HINSTANCE ApplicationInstance;


bool ZEDCore::InitializeSelf()
{
	zeCore->SetApplicationModule(EditorModule);

	zeCore->GetOptions()->Load("options.ini");
	zeCore->GetOptions()->ResetChanges();
	ApplicationInstance = *((HINSTANCE*)GetModuleHandle(NULL));

	if (!zeCore->StartUp())
		return false;

	zeCore->SetCoreState(ZE_CS_RUNNING);

	if (!EditorModule->Initialize())
		return false;

	WrapperTypes = ZEProvider::GetInstance()->GetClass(ZEDObjectWrapper::Class());
}

void ZEDCore::DeinitializeSelf()
{
	zeCore->ShutDown();
}

ZEDCore::ZEDCore()
{
	OperationManager = new ZEDOperationManager();
	SelectionManager = new ZEDSelectionManager();
	TransformationManager = new ZEDTransformationManager();
	EditorModule = new ZEDModule();
}

ZEDCore::~ZEDCore()
{
	delete OperationManager;
	delete SelectionManager;
	delete TransformationManager;
	delete EditorModule;
}

ZEDOperationManager* ZEDCore::GetOperationManager()
{
	return OperationManager;
}

ZEDSelectionManager* ZEDCore::GetSelectionManager()
{
	return SelectionManager;
}

ZEDTransformationManager* ZEDCore::GetTransformationManager()
{
	return TransformationManager;
}

ZEDModule* ZEDCore::GetEditorModule()
{
	return EditorModule;
}

const ZEArray<ZEClass*>& ZEDCore::GetWrapperTypes()
{
	return WrapperTypes;
}

void ZEDCore::ProcessEngine()
{
	ZECoreState State = zeCore->GetCoreState();

	if (State == ZE_CS_TERMINATE || State ==  ZE_CS_SHUTDOWN)
		DeinitializeSelf();

	zeCore->MainLoop();
}


void ZEDCore::Destroy()
{
	delete this;
}

ZEDCore* ZEDCore::GetInstance()
{
	static ZEDCore Core;
	return &Core;
}
