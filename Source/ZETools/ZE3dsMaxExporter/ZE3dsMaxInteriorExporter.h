//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxInteriorExporter.h
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

#include "ZE3dsMaxUtils.h"

extern HINSTANCE hInstance;

class ZEMLNode;
class ZEProgressDialog;
class ZEResourceConfiguratorWidget;
class ZE3dsMaxInteriorExporterOptionsDialog;
class QApplication;
class QWindow;

class ZE3dsMaxInteriorExporter : public SceneExport 
{
	private:
		ZE3dsMaxInteriorExporterOptionsDialog*		OptionsDialog;
		ZEResourceConfiguratorWidget*				ResourceConfigurationDialog;

		ZEMLRoot									InteriorRoot;
		ZEMLNode*									ExportOptions;
		ZEMLNode									InteriorNode;
		QWindow*									WinWidget;
		ZEProgressDialog*							ProgressDialog;
		ZEString									ExportPath;

		IGameScene*									Scene;

		Tab<IGameNode*>								SceneGeometryObjects;
		Tab<IGameNode*>								Rooms;
		Tab<IGameNode*>								Doors;
		Tab<IGameNode*>								Helpers;
		Tab<IGameMaterial*>							Materials;

		void										CollectResources();

		void										LoadOptions(const char* FilePath);
		void										SaveOptions(const char* FilePath);
		bool										ShowOptionsDialog(HWND ParentWindow);
		bool										ShowResourceConfigurationDialog(HWND ParentWindow, const char* MaxFilePath, ZEString ExportPath);

		ZEInt32										FindRoomIndex(IGameNode* Node);
		ZEInt32										ProcessFaceMaterial(IGameMaterial* Material);

		bool										ProcessScene();
		bool										ProcessMaterials(const char* FileName);
		bool										ProcessRooms();
		void										ProcessPhysicalMesh(IGameNode* ParentNode, IGameNode* Node, ZEMLNode* PhysicalMeshNode);
		bool										ProcessDoors();
		bool										ProcessHelpers();

	protected:
		ZEInt32										GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode = NULL);

	public:	
		virtual ZEInt								ExtCount();
		virtual const TCHAR *						Ext(ZEInt n);
		virtual const TCHAR *						LongDesc();
		virtual const TCHAR *						ShortDesc();
		virtual const TCHAR *						AuthorName();
		virtual const TCHAR *						CopyrightMessage();
		virtual const TCHAR *						OtherMessage1();
		virtual const TCHAR *						OtherMessage2();
		virtual ZEUInt								Version();
		virtual void								ShowAbout(HWND hWnd);

		virtual BOOL								SupportsOptions(ZEInt ext, DWORD options);
		virtual ZEInt								DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts = FALSE, DWORD options = 0);

													ZE3dsMaxInteriorExporter();
		virtual										~ZE3dsMaxInteriorExporter();


};
