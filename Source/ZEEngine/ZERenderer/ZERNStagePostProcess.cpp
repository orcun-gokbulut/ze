//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNStagePostProcess.cpp
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

#include "ZERNStagePostProcess.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZERNStageID.h"
#include "ZERNStageGBuffer.h"
#include "ZERNRenderer.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGROutput.h"
#include "ZEModules/ZEDirect3D11/ZED11DepthStencilBuffer.h"
#include "ZEModules/ZEDirect3D11/ZED11Context.h"
#include "ZERNStageLighting.h"
#include "ZERNStageHDR.h"

bool ZERNStagePostProcess::InitializeSelf()
{
	LightScattering.Initialize();

	return true;
}

void ZERNStagePostProcess::DeinitializeSelf()
{
	LightScattering.Deinitialize();
}

ZEInt ZERNStagePostProcess::GetId()
{
	return ZERN_STAGE_POST_EFFECT;
}

const ZEString& ZERNStagePostProcess::GetName()
{
	static const ZEString Name = "Post Process Stage";
	return Name;
}

void ZERNStagePostProcess::SetPostProcessFlags(ZERNPostProcessFlags PostProcessFlags)
{
	this->PostProcessFlags.RaiseFlags(PostProcessFlags);
}

ZEUInt ZERNStagePostProcess::GetPostProcessFlags() const
{
	return PostProcessFlags;
}

void ZERNStagePostProcess::SetInputTexture(ZEGRTexture2D* InputTexture)
{
	this->InputTexture = InputTexture;
}

const ZEGRTexture2D* ZERNStagePostProcess::GetInputTexture() const
{
	return InputTexture;
}

void ZERNStagePostProcess::SetOutputRenderTarget(ZEGRRenderTarget* OutputRenderTarget)
{
	this->OutputRenderTarget = OutputRenderTarget;
}

const ZEGRRenderTarget* ZERNStagePostProcess::GetOutputRenderTarget() const
{
	return OutputRenderTarget;
}

void ZERNStagePostProcess::SetMultipleScattering(bool MultipleScattering)
{
	this->MultipleScattering = MultipleScattering;
}

bool ZERNStagePostProcess::Setup(ZERNRenderer* Renderer, ZEGRContext* Context, ZEList2<ZERNCommand>& Commands)
{
	ZERNStageGBuffer* StageGbuffer = (ZERNStageGBuffer*)Renderer->GetStage(ZERN_STAGE_GBUFFER);
	if(StageGbuffer == NULL)
		return false;

	if(InputTexture == NULL)
		InputTexture = StageGbuffer->GetAccumulationMap();

	if(OutputRenderTarget == NULL)
	{
		ZERNStageHDR* StageHDR = (ZERNStageHDR*)Renderer->GetStage(ZERN_STAGE_HDR);
		OutputRenderTarget = (StageHDR != NULL && StageHDR->GetEnable()) ? StageGbuffer->GetAccumulationMap()->GetRenderTarget() : Renderer->GetOutput()->GetRenderTarget(); 
	}

	if(PostProcessFlags.GetFlags(ZERN_PPF_LIGHT_SCATTERING))
	{
		LightScattering.SetInputTexture(InputTexture);

		ZED11DepthStencilBuffer* D11DepthStencilBuffer = (ZED11DepthStencilBuffer*)StageGbuffer->GetDepthStencilBuffer();
		ID3D11ShaderResourceView* NativeTexture =  D11DepthStencilBuffer->GetResourceView();
		ID3D11DeviceContext1* D11Context = static_cast<ZED11Context*>(Context)->GetContext();
		D11Context->PSSetShaderResources(1, 1, &NativeTexture);

		LightScattering.SetOutputRenderTarget(OutputRenderTarget);

		//LightScattering.SetLightDirection(ZEVector3(0.0f, -1.0f, 0.0f));
		LightScattering.SetLightIntensity(20.0f);
		LightScattering.SetLightColor(ZEVector3(1.0f, 1.0f, 1.0f));
		LightScattering.SetMieScatteringStrengh(0.76f);

		LightScattering.Process(Context, MultipleScattering);
	}

	return true;
}

void ZERNStagePostProcess::CleanUp(ZERNRenderer* Renderer, ZEGRContext* Context)
{

}

ZERNStagePostProcess::ZERNStagePostProcess()
{
	InputTexture = NULL;
	OutputRenderTarget = NULL;

	MultipleScattering = false;
}

void ZERNStagePostProcess::SetLightDirection(const ZEVector3& Direction)
{
	LightScattering.SetLightDirection(Direction);
}

