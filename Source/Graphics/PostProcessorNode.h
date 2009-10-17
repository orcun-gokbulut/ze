//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - PostProcessorNode.h
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
#ifndef __ZE_POST_PROCESSOR_NODE_H__
#define __ZE_POST_PROCESSOR_NODE_H__

#include "ZEDS/Array.h"

class ZERenderer;
class ZETexture2D;

class ZEPostProcessorNode
{
	protected:
										ZEPostProcessorNode();
		virtual							~ZEPostProcessorNode();	

	public:
		virtual size_t					GetDependencyCount();
		virtual ZEPostProcessorNode**	GetDependencies();

		virtual bool					Initialize();
		virtual void					Deinitialize();

		virtual void					Destroy();

		virtual bool					Process();

		virtual ZETexture2D*			GetOutput() = 0;
};

class ZEPPColorInputNode : public ZEPostProcessorNode
{
	protected:
		ZERenderer*						Renderer;

										ZEPPColorInputNode();
		virtual							~ZEPPColorInputNode();	

	public:
		void							SetRenderer(ZERenderer* Renderer);
		ZERenderer*						GetRenderer();

		virtual ZETexture2D*			GetOutput();

		static ZEPPColorInputNode*		CreateInstance();
};

class ZEPPDepthInputNode : public ZEPostProcessorNode
{
	protected:
		ZERenderer*						Renderer;

										ZEPPDepthInputNode();
		virtual							~ZEPPDepthInputNode();	

	public:
		void							SetRenderer(ZERenderer* Renderer);
		ZERenderer*						GetRenderer();

		virtual ZETexture2D*			GetOutput();

		static ZEPPDepthInputNode*		CreateInstance();
};

class ZEPPTextureInputNode : public ZEPostProcessorNode
{
	protected:
		ZETexture2D*					Texture;

										ZEPPTextureInputNode();
		virtual							~ZEPPTextureInputNode();	

	public:
		void							SetTexture(ZETexture2D* Texture);
		ZETexture2D*					GetTexture();

		virtual ZETexture2D*			GetOutput();

		static ZEPPTextureInputNode*	CreateInstance();
};

class ZEPPOutputNode
{
	protected:
		ZERenderer*						Renderer;

										ZEPPOutputNode();
		virtual							~ZEPPOutputNode();	

	public:
		void							SetRenderer(ZERenderer* Renderer);
		ZERenderer*						GetRenderer();

		virtual ZETexture2D*			GetOutput();

		static ZEPPOutputNode*			CreateInstance();
};

#endif
