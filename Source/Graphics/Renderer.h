//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Renderer.h
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
#ifndef __ZE_RENDERER_H__
#define __ZE_RENDERER_H__

#include "ZEDS/Array.h"
#include "ZEMath/Vector.h"
#include "ZEMath/Matrix.h"

struct ZEViewPoint
{
	float									FOV;
	float									NearZ;
	float									FarZ;
	ZEVector3								ViewPosition;
	ZEMatrix4x4								ViewMatrix;
	ZEMatrix4x4								ProjMatrix;
	ZEMatrix4x4								ViewProjMatrix;
};

class ZEPostProcessor;
class ZECamera;
class ZERenderOrder;
class ZETexture2D;
class ZERenderer
{
	protected:
											ZERenderer();
		virtual								~ZERenderer();

	public:
		virtual void						SetRenderColorTexture(bool Enable);
		virtual bool						GetRenderColorTexture();
		virtual void						SetRenderDepthTexture(bool Enable);
		virtual bool						GetRenderDepthTexture();

		virtual void						SetRenderVelocityTexture(bool Enable);
		virtual bool						GetRenderVelocityTexture();

		virtual void						SetRenderObjectTexture(bool Enable);
		virtual bool						GetRenderObjectTexture();

		virtual ZETexture2D*				GetColorTexture();
		virtual ZETexture2D*				GetDepthTexture();
		virtual ZETexture2D*				GetVelocityTexture();
		virtual ZETexture2D*				GetObjectTexture();

		virtual bool						Initialize() = 0;
		virtual void						Deinitialize() = 0;

		virtual void						Destroy();

		virtual ZEArray<ZEPostProcessor*>&	GetPostProcessors() = 0;
		virtual void						AddPostProcessor(ZEPostProcessor* PostProcessor) = 0;
		virtual void						RemovePostProcessor(ZEPostProcessor* PostProcessor) = 0;

		virtual void						SetCamera(ZECamera* Camera) = 0;
		virtual ZECamera*					GetCamera() = 0;

		virtual void						AddToRenderList(ZERenderOrder* RenderOrder) = 0;
		virtual void						ClearList() = 0;

		virtual void						Render(float ElaspedTime = 0) = 0;
};

#endif
