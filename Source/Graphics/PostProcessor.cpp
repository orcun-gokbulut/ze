//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PostProcessor.cpp
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

#include "PostProcessor.h"
#include "PostProcessorNode.h"
#include "Graphics/GraphicsModule.h"

ZEPostProcessor::ZEPostProcessor()
{
}

ZEPostProcessor::~ZEPostProcessor()
{
}

void ZEPostProcessor::SetInput(ZERenderer* Renderer)
{
}

void ZEPostProcessor::SetInput(ZETexture2D* Input)
{
}

void ZEPostProcessor::SetOutput(ZETexture2D* Output)
{
}

void ZEPostProcessor::SetOuputToFrameBuffer()
{
}


ZEArray<ZEPostProcessorNode*>& ZEPostProcessor::GetNodes()
{
	return Nodes;
}

ZEPostProcessorNode* ZEPostProcessor::CreateNode(const char* Name)
{
	return NULL;
}

ZEPostProcessorNode* ZEPostProcessor::CreateNode(size_t Index)
{
	return NULL;
}
		
void ZEPostProcessor::AddNode(ZEPostProcessorNode* Node)
{
	Nodes.Add(Node);
}

void ZEPostProcessor::RemoveNode(ZEPostProcessorNode* Node)
{
	Nodes.DeleteValue(Node);
}

bool ZEPostProcessor::Initialize()
{
	// Set node state count
	NodeStates.SetCount(Nodes.GetCount());

	// Fill node states with false which mean that node is not processed
	NodeStates.FillWith(false);

	// Initialize nodes
	for (size_t I = 0; I < Nodes.GetCount(); I++)
		Nodes[I]->Initialize();
	
	return true;
}

void ZEPostProcessor::Deinitialize()
{
	for (size_t I = 0; I < Nodes.GetCount(); I++)
		Nodes[I]->Deinitialize();
}

void ZEPostProcessor::Process()
{
	for (size_t I = 0; I < Nodes.GetCount(); I++)
		if (NodeStates[I] == false)
			Nodes[I]->Process();
}

void ZEPostProcessor::Destroy()
{
	delete this;
}

ZEPostProcessor* ZEPostProcessor::CreateInstance()
{
	return zeGraphics->CreatePostProcessor();
}
