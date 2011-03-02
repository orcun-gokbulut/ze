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

#include "ZE3dsModelExporterOptions.h"
#include "ZE3dsProgressDialog/ZE3dsProgressDialog.h"
#include "ZE3dsModelExporterResources.h"
#include "ZETModelFile/ZETModelFile.h"

#include <Max.h>
#include <istdplug.h>
#include <iparamb2.h>
#include <iparamm2.h>
#include <IGame/IGame.h>
#include <IGame/IGameModifier.h> 

extern HINSTANCE hInstance;

class ZEModelExporter : public SceneExport 
{
	public:
		
		virtual int					ExtCount();
		virtual const TCHAR *		Ext(int n);
		virtual const TCHAR *		LongDesc();
		virtual const TCHAR *		ShortDesc();
		virtual const TCHAR *		AuthorName();
		virtual const TCHAR *		CopyrightMessage();
		virtual const TCHAR *		OtherMessage1();
		virtual const TCHAR *		OtherMessage2();
		virtual unsigned int		Version();
		virtual void				ShowAbout(HWND hWnd);
		virtual	BOOL				SupportsOptions(int ext, DWORD options);

		virtual int					GetSceneNodes(INodeTab& i_nodeTab, INode* i_currentNode =NULL);
		virtual int					DoExport(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0);
		
		Tab<IGameNode*>				ProcessedBones;
		Tab<IGameMaterial*>			ProcessedMaterials;
		Tab<IGameNode*>				ProcessedMeshes;
		Tab<IGameNode*>				ProcessedMasterMeshes;
		Tab<IGameMaterial*>			Materials;

		int							FrameCount;
		int							TicksPerFrame;

		ZEModelFile					ModelFile;
		IGameScene*					Scene;
		ZE3dsProgressDialog			ProgressDialog;
		ZEModelExporterOptions		ExporterOptions;

		bool						DumpPropertyContainer(IExportEntity* Node);
		bool						GetRelativePath(const char* RealPath, char* RelativePath);
		int							GetBoneId(IGameNode* Node);
		int							GetMeshId(IGameNode* Node);

		bool						ProcessBone(IGameNode* Node);
		bool						ProcessBones();
		int							ProcessMeshMaterial(IGameMaterial* Material);
		bool						ProcessMaterials();
		bool						ProcessMeshLODVertices(IGameNode* Node,  ZEModelFileMeshLOD* ZEMeshLod);
		void						ProcessPhysicalBodyConvexShape(IGameNode* Node, IGameNode* OwnerNode, ZEModelFilePhysicalShape* Shape);
		bool						ProcessPhysicalShape(IGameNode* Node, IGameNode* OwnerNode, ZEModelFilePhysicalShape* PhysicalShape);
		bool						ProcessPhysicalBody(IGameNode* Node, ZEModelFilePhysicalBody* Body);
		bool						ProcessPhysicalJoint(IGameNode* Node, ZEModelFilePhysicalJoint* Joint);
		bool						ProcessMesh(IGameNode* Node);
		bool						ProcessMeshes();
		bool						ProcessAnimation();

									ZEModelExporter();
		virtual						~ZEModelExporter();
};

#endif
