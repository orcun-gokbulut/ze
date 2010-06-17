//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEPPTextureOutputNode.cpp
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

#include "ZEPPTextureOutputNode.h"
#include "ZEGraphics\ZETexture2D.h"
#include "ZECore\ZEError.h"
#include "ZEDefinitions.h"

ZEPPTextureOutputNode::ZEPPTextureOutputNode()
{
	Texture = NULL;
}

ZEPostProcessorNodeType ZEPPTextureOutputNode::GetNodeType()
{
	return ZE_PPNT_OUTPUT_NODE;
}

void ZEPPTextureOutputNode::SetTexture(ZETexture2D* Texture)
{
	// Check that texture is a render target or not (If it is not a render target than you can not render directly to the texture)
	if (Texture != NULL && !Texture->IsRenderTarget())
	{
		zeError("Post Processor - Texture Output Node", "Output texture is not a render target. Texture output node only accpets textures that are render targets.");
		this->Texture = NULL;	
		return;
	}

	this->Texture = Texture;
}

ZETexture2D* ZEPPTextureOutputNode::GetTexture()
{
	return Texture;
}

ZETexture2D* ZEPPTextureOutputNode::GetOutput()
{
	return Texture;
}

bool ZEPPTextureOutputNode::Process()
{
	// Check output texture available
	if (Texture == NULL)
	{
		zeError("Post Processor - Texture Output Node", "Output texture is not available.");
		return false;
	}

	return true;
}

ZEPPTextureOutputNode* ZEPPTextureOutputNode::CreateInstance()
{
	return new ZEPPTextureOutputNode();
}




