//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZE3dsMaxModelExporter.h
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

class ZEProgressDialog;
class ZE3dsMaxModelExporterOptionsDialog;
class ZEResourceConfiguratorWidget;
class QApplication;
class QWindow;

class ZE3dsMaxModelExporter : public SceneExport 
{
	private:
		ZE3dsMaxModelExporterOptionsDialog*	OptionsDialog;
		ZEResourceConfiguratorWidget*		ResourceConfiguratorDialog;

		ZEMLNode*							ExportOptions;
		QWindow*							WinWidget;
		ZEProgressDialog*					ProgressDialog;
		ZEString							ExportPath;

		IGameScene*							Scene;
		ZEMLRoot							ModelRoot;
		ZEMLNode							ModelNode;

		Tab<IGameNode*>						ProcessedBones;
		Tab<IGameNode*>						ProcessedMeshes;
		Tab<IGameNode*>						ProcessedMasterMeshes;
		Tab<IGameNode*>						ProcessedHelpers;
		Tab<IGameMaterial*>					ProcessedMaterials;

		ZEInt32								TotalFrameCount;
		ZEInt32								TicksPerFrame;

		void								CollectResources();
		void								LoadOptions(const char* FilePath);
		void								SaveOptions(const char* FilePath);
		bool								ShowOptionsDialog(HWND ParentWindow);
		bool								ShowResourceConfigurationDialog(HWND ParentWindow, const char* MaxFilePath);

		bool								ProcessBone(IGameNode* Node, ZEMLNode* BonesNode);
		bool								ProcessBones();
		ZEInt32								ProcessMeshMaterial(IGameMaterial* Material);
		bool								ProcessMaterials(const char* FileName);
		bool								ProcessMeshLODVertices(IGameNode* Node, ZEMLNode* MeshLODNode, bool IsMasterMesh = false);
		void								ProcessPhysicalBodyConvexShape(IGameNode* Node, IGameNode* OwnerNode, ZEMLNode* ShapeNode);
		bool								ProcessPhysicalShape(IGameNode* Node, IGameNode* OwnerNode, ZEMLNode* PhysicalShapeNode);
		void								ProcessPhysicalBody(IGameNode* Node, ZEMLNode* ParentNode);
		bool								ProcessPhysicalJoint(IGameNode* Node, ZEMLNode* PhysicalJointNode);
		bool								ProcessMasterMesh(IGameNode* Node, ZEMLNode* MeshesNode);
		bool								ProcessMeshLODs(IGameNode* Node, ZEMLNode* MeshesNode);
		bool								ProcessMeshes();
		void								ProcessAnimationFrames(ZESize AnimationStartFrame, ZESize AnimationFrameCount, ZEMLNode* AnimationNode);
		bool								ProcessAnimations(ZEMLNode* AnimationsNode);
		bool								ProcessHelpers();
		bool								ProcessHelper(IGameNode* Node, ZEMLNode* HelpersNode);

		ZEInt32								GetBoneId(IGameNode* Node);
		ZEInt32								GetMeshId(IGameNode* Node);

	protected:
		ZEInt32								GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode =NULL);

	public:
		virtual ZEInt						ExtCount();
		virtual const TCHAR*				Ext(ZEInt n);
		virtual const TCHAR*				LongDesc();
		virtual const TCHAR*				ShortDesc();
		virtual const TCHAR*				AuthorName();
		virtual const TCHAR*				CopyrightMessage();
		virtual const TCHAR*				OtherMessage1();
		virtual const TCHAR*				OtherMessage2();
		virtual ZEUInt						Version();
		virtual void						ShowAbout(HWND hWnd);
		
		virtual	BOOL						SupportsOptions(ZEInt ext, DWORD options);	
		virtual ZEInt						DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);

		virtual	bool						WriteToFile(const char* FilePath);
		

											ZE3dsMaxModelExporter();
		virtual								~ZE3dsMaxModelExporter();
};
