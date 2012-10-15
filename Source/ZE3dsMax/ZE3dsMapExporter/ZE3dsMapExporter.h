//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMapExporter.h
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

#include "ZETypes.h"

#include <Max.h>
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <IGame/IGame.h>
#include "ZEML/ZEMLNode.h"

#define ZE_MPFL_MAX_NAME_SIZE				128
#define ZE_MPFL_MAX_FILENAME_SIZE			256


extern HINSTANCE hInstance;

class ZEMapExporterOptionsDialog;
class QApplication;
class QWinWidget;
class ZEProgressDialog;
class ZEMLNode;
class ZEResourceConfigurationWidget;

struct ZEMapFilePhysicalMeshPolygon
{
	ZEUInt32								Indices[3];
};

struct ZEMapFilePhysicalMesh
{
	ZEArray<ZEVector3>						Vertices;
	ZEArray<ZEMapFilePhysicalMeshPolygon>	Polygons;
};

struct ZEMapFileVertex
{
	ZEVector3								Position;
	ZEVector3								Normal;
	ZEVector3								Tangent;
	ZEVector3								Binormal;
	ZEVector2								Texcoord;
};

struct ZEMapFilePolygon
{	
	ZEUInt32								Material;
	ZEMapFileVertex							Vertices[3];
};

class ZE3dsMapExporter : public SceneExport 
{
	private:

		QApplication*					QtApplication;

		ZEMapExporterOptionsDialog*		OptionsDialog;
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
		bool						ProcessDoors();

	protected:
		ZEInt						GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode = NULL);

	public:	
		virtual ZEInt				ExtCount();					// Number of extensions supported
		virtual const TCHAR *		Ext(ZEInt n);				// Extension #n (i.e. "3DS")
		virtual const TCHAR *		LongDesc();					// Long ASCII description (i.e. "Autodesk 3D Studio File")
		virtual const TCHAR *		ShortDesc();				// Short ASCII description (i.e. "3D Studio")
		virtual const TCHAR *		AuthorName();				// ASCII Author name
		virtual const TCHAR *		CopyrightMessage();			// ASCII Copyright message
		virtual const TCHAR *		OtherMessage1();			// Other message #1
		virtual const TCHAR *		OtherMessage2();			// Other message #2
		virtual ZEUInt				Version();					// Version number * 100 (i.e. v3.01 = 301)
		virtual void				ShowAbout(HWND hWnd);		// Show DLL's "About..." box

		virtual BOOL				SupportsOptions(ZEInt ext, DWORD options);
		virtual ZEInt				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts = FALSE, DWORD options = 0);

									ZE3dsMapExporter();
		virtual						~ZE3dsMapExporter();


};

#endif
