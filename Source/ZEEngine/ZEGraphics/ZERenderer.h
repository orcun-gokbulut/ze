//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERenderer.h
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

#include "ZETypes.h"
#include "ZEDS/ZEArray.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEGame/ZEDrawStatistics.h"

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
class ZERenderCommand;
struct ZEDrawParameters;
class ZETexture2D;
class ZEViewPort;
class ZELight;

enum ZERendererType
{
	ZE_RT_FRAME,
	ZE_RT_SHADOW
};

class ZERenderer
{
	protected:

		ZERendererStatistics					Statistics;

												ZERenderer();
		virtual									~ZERenderer();

	public:

		virtual void							SetDrawParameters(ZEDrawParameters* DrawParameters) = 0;
		virtual ZEDrawParameters*				GetDrawParameters() = 0;

		virtual ZERendererType					GetRendererType() = 0;
		virtual	const ZERendererStatistics&		GetStatistics() const;

		virtual ZEArray<ZEPostProcessor*>&		GetPostProcessors() = 0;
		virtual void							AddPostProcessor(ZEPostProcessor* PostProcessor) = 0;
		virtual void							RemovePostProcessor(ZEPostProcessor* PostProcessor) = 0;

		virtual void							SetViewPort(ZEViewPort* ViewPort) = 0;
		virtual ZEViewPort*						GetViewPort() = 0;

		virtual bool							Initialize() = 0;
		virtual void							Deinitialize() = 0;

		virtual void							Destroy();

		virtual void							AddToLightList(ZELight* Light);
		virtual void							ClearLightList();

		virtual const ZESmartArray<ZERenderCommand>& GetRenderList() const = 0;
		virtual void							AddToRenderList(ZERenderCommand* RenderCommand) = 0;
		virtual void							ClearRenderList() = 0;

		void									ClearLists();
		
		virtual void							Render(float ElaspedTime = 0) = 0;
};

#endif
