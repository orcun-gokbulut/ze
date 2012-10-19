//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsModelExporter.h
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
#ifndef __ZE3DS_MODEL_EXPORTER_H__
#define __ZE3DS_MODEL_EXPORTER_H__

#include "ZE3dsMaxUtils/ZE3dsMaxUtils.h"

extern HINSTANCE hInstance;
class QApplication;
class QWinWidget;
class ZEProgressDialog;
class ZE3dsModelExporterOptionsDialog;
class ZEResourceConfigurationWidget;

class ZE3dsModelExporter : public SceneExport 
{
	private:

		QApplication*						QtApplication;

		ZE3dsModelExporterOptionsDialog*	OptionsDialog;
		ZEResourceConfigurationWidget*		ResourceConfigurationDialog;

		ZEMLNode*							ExportOptions;
		QWinWidget*							WinWidget;
		ZEProgressDialog*					ProgressDialog;
		ZEString							ExportPath;

		IGameScene*					Scene;
		ZEMLNode					ModelNode;

		
		Tab<IGameNode*>				ProcessedBones;
		Tab<IGameNode*>				ProcessedMeshes;
		Tab<IGameNode*>				ProcessedMasterMeshes;
		Tab<IGameMaterial*>			ProcessedMaterials;

		ZEInt						TotalFrameCount;
		ZEInt						TicksPerFrame;

		void						CollectResources();
		void						LoadOptions(const char* FilePath);
		void						SaveOptions(const char* FilePath);
		bool						ShowOptionsDialog(HWND ParentWindow);
		bool						ShowResourceConfigurationDialog(HWND ParentWindow, const char* MaxFilePath);

		bool						ProcessBone(IGameNode* Node, ZEMLNode* BonesNode);
		bool						ProcessBones();
		ZEInt						ProcessMeshMaterial(IGameMaterial* Material);
		bool						ProcessMaterials(const char* FileName);
		bool						ProcessMeshLODVertices(IGameNode* Node, ZEMLNode* MeshLODNode);
		void						ProcessPhysicalBodyConvexShape(IGameNode* Node, IGameNode* OwnerNode, ZEMLNode* ShapeNode);
		bool						ProcessPhysicalShape(IGameNode* Node, IGameNode* OwnerNode, ZEMLNode* PhysicalShapeNode);
		void						ProcessPhysicalBody(IGameNode* Node, ZEMLNode* ParentNode);
		bool						ProcessPhysicalJoint(IGameNode* Node, ZEMLNode* PhysicalJointNode);
		bool						ProcessMasterMesh(IGameNode* Node, ZEMLNode* MeshesNode);
		bool						ProcessMeshLODs(IGameNode* Node, ZEMLNode* MeshesNode);
		bool						ProcessMeshes();
		void						ProcessAnimationFrames(ZESize AnimationStartFrame, ZESize AnimationFrameCount, ZEMLNode* AnimationNode);
		bool						ProcessAnimations(ZEMLNode* AnimationsNode);

		bool						DumpPropertyContainer(IExportEntity* Node);
		bool						GetRelativePath(const char* RealPath, char* RelativePath);
		ZEInt						GetBoneId(IGameNode* Node);
		ZEInt						GetMeshId(IGameNode* Node);

	protected:
		ZEInt						GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode =NULL);

	public:
		
		virtual ZEInt				ExtCount();										// Number of extensions supported
		virtual const TCHAR*		Ext(ZEInt n);									// Extension #n (i.e. "3DS")
		virtual const TCHAR*		LongDesc();										// Long ASCII description (i.e. "Autodesk 3D Studio File")
		virtual const TCHAR*		ShortDesc();									// Short ASCII description (i.e. "3D Studio")
		virtual const TCHAR*		AuthorName();									// ASCII Author name
		virtual const TCHAR*		CopyrightMessage();								// ASCII Copyright message
		virtual const TCHAR*		OtherMessage1();								// Other message #1
		virtual const TCHAR*		OtherMessage2();								// Other message #2
		virtual ZEUInt				Version();										// Version number * 100 (i.e. v3.01 = 301)
		virtual void				ShowAbout(HWND hWnd);							// Show DLL's "About..." box
		
		virtual	BOOL				SupportsOptions(ZEInt ext, DWORD options);	
		virtual ZEInt				DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		virtual	bool				WriteToFile(const char* FilePath);
		

									ZE3dsModelExporter();
		virtual						~ZE3dsModelExporter();
};

#endif
