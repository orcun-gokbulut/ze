//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxMapExporter.h
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
#ifndef __ZE3DS_MAP_EXPORTER_H__
#define __ZE3DS_MAP_EXPORTER_H__

#include "ZE3dsMaxUtils/ZE3dsMaxUtils.h"

extern HINSTANCE hInstance;

class ZE3dsMaxMapExporterOptionsDialog;
class QApplication;
class QWinWidget;
class ZEProgressDialog;
class ZEMLNode;
class ZEResourceConfigurationWidget;


class ZE3dsMaxMapExporter : public SceneExport 
{
	private:

		QApplication*					QtApplication;

		ZE3dsMaxMapExporterOptionsDialog*		OptionsDialog;
		ZEResourceConfigurationWidget*	ResourceConfigurationDialog;

		ZEMLNode						MapNode;
		ZEMLNode*						ExportOptions;
		QWinWidget*						WinWidget;
		ZEProgressDialog*				ProgressDialog;
		ZEString						ExportPath;

		IGameScene*						Scene;

		Tab<IGameNode*>					SceneGeometryObjects;
		Tab<IGameNode*>					Portals;
		Tab<IGameNode*>					Doors;
		Tab<IGameMaterial*>				Materials;

		void						CollectResources();

		void						LoadOptions(const char* FilePath);
		void						SaveOptions(const char* FilePath);
		bool						ShowOptionsDialog(HWND ParentWindow);
		bool						ShowResourceConfigurationDialog(HWND ParentWindow, const char* MaxFilePath);

		ZEInt						FindPortalIndex(IGameNode* Node);
		ZEInt						ProcessFaceMaterial(IGameMaterial* Material);

		bool						ProcessScene();
		bool						ProcessMaterials(const char* FileName);
		bool						ProcessPortals();
		void						ProcessPhysicalMesh(IGameObject* Object, ZEMLNode* PhysicalMeshNode);
		bool						ProcessDoors();

	protected:
		ZEInt						GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode = NULL);

	public:	
		virtual ZEInt				ExtCount();
		virtual const TCHAR *		Ext(ZEInt n);
		virtual const TCHAR *		LongDesc();
		virtual const TCHAR *		ShortDesc();
		virtual const TCHAR *		AuthorName();
		virtual const TCHAR *		CopyrightMessage();
		virtual const TCHAR *		OtherMessage1();
		virtual const TCHAR *		OtherMessage2();
		virtual ZEUInt				Version();
		virtual void				ShowAbout(HWND hWnd);

		virtual BOOL				SupportsOptions(ZEInt ext, DWORD options);
		virtual ZEInt				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts = FALSE, DWORD options = 0);

									ZE3dsMaxMapExporter();
		virtual						~ZE3dsMaxMapExporter();


};

#endif
