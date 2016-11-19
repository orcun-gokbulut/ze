//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEDEditor.h
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

#pragma once

#include "ZEMeta/ZEObject.h"
#include "ZEInitializable.h"
#include "ZEDestroyable.h"

#include "ZEDS/ZEArray.h"
#include "ZEDUserInterface/ZEDCommand.h"

class ZEDComponent;
class ZEDEvent;
class ZEDEditorCore;
class ZEDEditorEvent;
class ZEDObjectManager;
class ZEDOperationManager;
class ZEDSelectionManager;
class ZEDTransformationManager;
class ZEDViewportManager;
class ZEDMainWindow;
class ZEUIManager;
class ZEDMenu;

enum ZEDFileState
{
	ZED_ES_NONE,
	ZED_ES_UNMODIFIED,
	ZED_ES_MODIFIED
};

class ZEDEditor : public ZEObject, public ZEInitializable, public ZEDestroyable
{
	ZE_OBJECT
	friend class ZEDEditorCore;
	private:
		ZEDEditorCore*						Core;
		ZEDFileState						FileState;
		ZEString							FileName;
		ZEArray<ZEString>					RecentFiles;

		ZEArray<ZEDComponent*>				Components;
		ZEDObjectManager*					ObjectManager;
		ZEDOperationManager*				OperationManager;
		ZEDSelectionManager*				SelectionManager;
		ZEDTransformationManager*			TransformationManager;
		ZEDViewportManager*					ViewportManager;
		ZEDMainWindow*						MainWindow;
		ZEUIManager*						UIManager;

		void								PopulateRecentFiles();
		void								RegisterRecentFile(const ZEString& FileName);

	protected:
		virtual bool						InitializeInternal();
		virtual bool						DeinitializeInternal();

											ZEDEditor();
											~ZEDEditor();

	private: /* COMMANDS */
		ZEDCommand							NewCommand;
		ZEDCommand							OpenCommand;
		ZEDCommand							SaveCommand;
		ZEDCommand							SaveAsCommand;
		ZEDCommand							CloseCommand;
		ZEDCommand							RecentFilesCommand;
		ZEDCommand							ExitCommand;

		void								RegisterCommands();
		void								UpdateCommands();

		void								NewCommand_OnAction(const ZEDCommand* Command);
		void								OpenCommand_OnAction(const ZEDCommand* Command);
		void								RecentFilesCommand_OnAction(const ZEDCommand* Command);
		void								SaveCommand_OnAction(const ZEDCommand* Command);
		void								SaveAsCommand_OnAction(const ZEDCommand* Command);
		void								CloseCommand_OnAction(const ZEDCommand* Command);
		void								ExitCommand_OnAction(const ZEDCommand* Command);

	public:
		ZEDEditorCore*						GetCore();

		ZEDObjectManager*					GetObjectManager();
		ZEDOperationManager*				GetOperationManager();
		ZEDSelectionManager*				GetSelectionManager();
		ZEDTransformationManager*			GetTransformationManager();
		ZEDViewportManager*					GetViewportManager();
		ZEDMainWindow*						GetMainWindow();
		ZEUIManager*						GetUIManager();

		ZEDComponent*						GetComponent(ZEClass* Class);
		const ZEArray<ZEDComponent*>&		GetComponents();
		void								AddComponent(ZEDComponent* Component);
		void								RemoveComponent(ZEDComponent* Component);

		ZEDFileState						GetFileState();
		const ZEString&						GetFileName();

		virtual ZEString					GetExtensions();

		virtual void						Process(float ElapsedTime);
		virtual void						PostProcess(float ElapsedTime);

		virtual void						New();
		virtual bool						Save(const ZEString& FileName);
		virtual bool						Load(const ZEString& FileName);
		virtual void						Close();

		virtual void						Exit();

		void								MarkDocumentModified();
		void								UnmarkDocumentModified();

		void								DistributeEvent(const ZEDEvent* Event);
};
