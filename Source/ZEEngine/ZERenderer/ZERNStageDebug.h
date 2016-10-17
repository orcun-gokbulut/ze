//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStageDebug.h
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

#include "ZERNStage.h"

#include "ZETypes.h"
#include "ZEDS/ZEFlags.h"
#include "ZEMath/ZEVector.h"
#include "ZEPointer/ZEHolder.h"
#include "ZEGraphics/ZEGRCanvas.h"

class ZEGRShader;
class ZEGRBuffer;
class ZEGRTexture;
class ZEGRRenderStateData;
class ZEEntity;

class ZERNStageDebug : public ZERNStage
{
	ZE_OBJECT
	private:
		ZEFlags							DirtyFlags;
		ZEGRCanvas						LineCanvas;
		ZEGRCanvas						TriangleCanvas;

		ZEHolder<ZEGRBuffer>			LineVertexBuffer;
		ZEHolder<ZEGRBuffer>			TriangleVertexBuffer;

		ZEHolder<ZEGRShader>			VertexShader;
		ZEHolder<ZEGRShader>			GeometryShader;
		ZEHolder<ZEGRShader>			PixelShader;
		ZEHolder<ZEGRRenderStateData>	RenderStateData;
		ZEHolder<ZEGRBuffer>			ConstantBuffer;

		ZEHolder<ZEGRShader>			SkinnedVertexShader;
		ZEHolder<ZEGRRenderStateData>	SkinnedRenderStateData;

		ZEHolder<ZEGRShader>			CanvasVertexShader;
		ZEHolder<ZEGRRenderStateData>	LineRenderStateData;
		ZEHolder<ZEGRRenderStateData>	TriangleRenderStateData;

		ZEHolder<ZEGRTexture>			DepthMap;

		ZEHolder<const ZEGRTexture>		OutputTexture;

		struct  
		{
			ZEBool32					ShowNormalVectors;
			ZEBool32					ShowBoundingBox;
			ZEBool32					ShowWireframe;
			ZEBool32					CullBackface;
		} Constants;

		bool							UpdateShaders();
		bool							UpdateRenderStates();
		bool							UpdateConstantBuffers();
		bool							Update();

		void							ProcessCommands(ZEGRContext* Context);
		void							GatherVertices(ZEEntity* Entity);

		virtual bool					InitializeInternal();
		virtual bool					DeinitializeInternal();

	public:
		virtual ZEInt					GetId() const;
		virtual const ZEString&			GetName() const;

		void							SetShowNormalVectors(bool ShowNormalVectors);
		bool							GetShowNormalVectors() const;

		void							SetShowBoundingBox(bool ShowBoundingBox);
		bool							GetShowBoundingBox() const;

		void							SetShowWireframe(bool ShowWireframe);
		bool							GetShowWireframe() const;

		void							SetCullBackface(bool CullBackface);
		bool							GetCullBackface() const;

		virtual void					Resized(ZEUInt Width, ZEUInt Height);

		virtual bool					Setup(ZEGRContext* Context);
		virtual void					CleanUp(ZEGRContext* Context);

										ZERNStageDebug();
};
