//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNRenderer.h
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

#include "ZEMeta/ZEObject.h"
#include "ZEInitializable.h"

#include "ZEDS/ZEArray.h"
#include "ZEDS/ZEList2.h"
#include "ZEPointer/ZEHolder.h"
#include "ZERNView.h"
#include "ZERNStageID.h"
#include "ZEModel/ZEMDVertex.h"

ZEMT_FORWARD_DECLARE(ZEScene);
ZEMT_FORWARD_DECLARE(ZERNStage);

class ZERNCommand;
class ZEGRContext;
class ZEGRBuffer;
class ZEGRTexture;
class ZEGRSampler;
class ZEGRRenderTarget;

struct ZERNInstanceData
{
	ZEMatrix4x4	WorldTransform;
	ZEMatrix4x4	WorldTransformInverseTranspose;

	ZEVector4	DrawColor;
	ZEUInt32_4	DrawLODTransition;
};

class ZERNRenderer : public ZEObject, public ZEInitializable
{
	ZE_OBJECT
	friend class ZERNStage;
	friend class ZEModelDraw;
	private:
		ZERNView									View;
		ZEList2<ZERNStage>							Stages;
		ZEHolder<ZEGRBuffer>						ViewConstantBuffer;
		ZEHolder<ZEGRBuffer>						RendererConstantBuffer;
		ZEInt										CurrentSceneIndex;
		ZEArray<ZEScene*>							Scenes;
		ZEList2<ZERNCommand>						CommandList;
		ZEList2<ZERNCommand>						CommandListInstanced;

		ZEHolder<ZEGRBuffer>						InstanceVertexBuffer;

		ZEGRContext*								Context;
		ZEHolder<const ZEGRTexture>					OutputTexture;

		ZEMatrix4x4									PrevViewProjectionTransform;

		bool										DirtyPipeline;
		bool										Resized;

		struct RendererConstants
		{
			float									Elapsedtime;
			float									Time;
			ZEUInt32								FrameId;
			ZEUInt32								Reserved0;
			ZEVector2								OutputSize;
			ZEVector2								InvOutputSize;
			ZEMatrix3x3Shader						ScreenTransform;
		} RendererConstants;

		ZEHolder<ZEGRSampler>						SamplerLinearClamp;
		ZEHolder<ZEGRSampler>						SamplerLinearWrap;
		ZEHolder<ZEGRSampler>						SamplerLinearBorderZero;
		ZEHolder<ZEGRSampler>						SamplerPointClamp;
		ZEHolder<ZEGRSampler>						SamplerPointWrap;
		ZEHolder<ZEGRSampler>						SamplerComparisonLinearPointClamp;

		virtual bool								InitializeInternal();
		virtual bool								DeinitializeInternal();
		
		void										CreatePredefinedSamplers();

		void										UpdateConstantBuffers();

		void										PopulateStageCommands();
		void										RenderStages();
		void										BindStages();

	public:
		void										SetContext(ZEGRContext* Context);
		ZEGRContext*								GetContext();

		void										SetView(const ZERNView& View);
		const ZERNView&								GetView();

		void										SetOutputTexture(const ZEGRTexture* OutputTexture);
		const ZEGRTexture*							GetOutputTexture() const;

		const ZEList2<ZERNStage>&					GetStages();
		ZERNStage*									GetStage(ZERNStageID Id);
		ZERNStage*									GetStage(ZEClass* Class);
		void										AddStage(ZERNStage* Stage);
		void										RemoveStage(ZERNStage* Stage);
		void										CleanStages();
		
		void										MarkDirtyPipeline();

		void										BeginScene(ZEScene* Scene);
		void										EndScene();

		void										AddCommand(ZERNCommand* Command);
		void										CleanCommands();

		void										BeginNestedRenderer();
		void										EndNestedRenderer();

		void										Render();

													ZERNRenderer();
		virtual										~ZERNRenderer();
};
