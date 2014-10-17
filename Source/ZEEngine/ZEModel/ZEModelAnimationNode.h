//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEModelAnimationNode.h
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

#ifndef __ZE_MODEL_ANIMATION_NODE_H__
#define __ZE_MODEL_ANIMATION_NODE_H__

#include "ZEModelResource.h"
#include "ZEModelAnimation.h"

//struct ZEModelAnimationMapItem
//{
//	ZEString		Name;
//	ZESize			Id;
//};

struct ZEModelAnimationMap
{
	ZEArray<ZEString> BoneItems;
	ZEArray<ZEString> MeshItems;
};

class ZEModelAnimationGraph;
//typedef ZEModelResourceAnimationFrame ZEModelAnimationFrame;

class ZEModelAnimationNode
{
	friend class ZEModelAnimationGraph;

	private:
		ZEModelAnimationGraph*						Owner;
		ZEString									Name;
		bool										Initialized;
		bool										Enabled;

		void										SetOwner(ZEModelAnimationGraph* graph);
		void										Initialize();
		void										Deinitialize();

	protected:

		ZEModelAnimationNode*						OutputNode;
		ZEArray<ZEModelAnimationNode*>				InputNodes;

		void										SetInputNodeCount(ZESize count);

		bool										AddInputNode(ZEModelAnimationNode* node);
		bool										SetInputNode(ZESize index, ZEModelAnimationNode* node);
		bool										RemoveInputNode(ZEModelAnimationNode* node);

		virtual const ZEModelAnimationMap*			GenerateMap();

		virtual bool								GenerateOutput(ZEModelAnimationFrame& output) = 0;
		virtual void								ProcessSelf(float elapsedTime) = 0;

		virtual void								InitializeSelf();
		virtual void								DeinitializeSelf();

													ZEModelAnimationNode();
		virtual										~ZEModelAnimationNode();

	public:
		bool										SetName(const ZEString& name);
		const ZEString&								GetName() const;

		void										SetEnabled(bool enabled);
		bool										GetEnabled() const;
		const ZEModelAnimationGraph*				GetOwner() const;

		bool										IsInitialized() const;

		void										Process(float elapsedTime);
		bool										GetOutput(ZEModelAnimationFrame& output);
		const ZEModelAnimationMap*					GetMap();

		bool										SetOutputNode(ZEModelAnimationNode* node);
		const ZEModelAnimationNode*					GetOutputNode() const;
};

#endif
