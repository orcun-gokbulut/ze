//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationNodeMap.h
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

#ifndef __ZE_MODEL_ANIMATION_NODE_MAP_H__
#define __ZE_MODEL_ANIMATION_NODE_MAP_H__

#include "ZEModelAnimationNode.h"

class ZEModelAnimationNodeMap : public ZEModelAnimationNode
{
	private:

		ZEModelAnimationMap					TargetMap;
		ZEArray<ZEInt32>					BoneLinks;
		ZEArray<ZEInt32>					MeshLinks;
		ZEModelAnimationMap					CurrentMap;

	protected:

		virtual const ZEModelAnimationMap*	GenerateMap();

		virtual bool						GenerateOutput(ZEModelAnimationFrame& output);
		virtual void						ProcessSelf(float elapsedTime);

											ZEModelAnimationNodeMap();
		virtual								~ZEModelAnimationNodeMap();

	public:
		
		bool								SetInputNode(ZEModelAnimationNode* node);
		const ZEModelAnimationNode*			GetInputNode() const;

		void								SetTargetMap(const ZEModelAnimationMap& map);
		void								SetCurrentMap(const ZEModelAnimationMap& map);
		void								SetBoneLinks(const ZEArray<ZEInt32>& links);
		void								SetMeshLinks(const ZEArray<ZEInt32>& links);

		//void								MapBone(ZEString targetName, ZEString currentName);
		//void								MapMesh(ZEString targetName, ZEString currentName);

		static ZEModelAnimationNodeMap*		CreateInstance();
};


#endif
