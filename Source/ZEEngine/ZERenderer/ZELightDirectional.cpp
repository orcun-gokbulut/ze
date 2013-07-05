//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZELightDirectional.cpp
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

#include "ZECanvas.h"
#include "ZECamera.h"
#include "ZEMaterial.h"
#include "ZERenderer.h"
#include "ZECore/ZECore.h"
#include "ZEMath/ZEMath.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEOBBox.h"
#include "ZEMath/ZEVector.h"
#include "ZELightDirectional.h"
#include "ZEMath/ZEViewVolume.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGame/ZEDrawParameters.h"
#include "ZEGraphics/ZERenderTarget.h"
#include "ZEMaterialLightDirectional.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEConstantBuffer.h"

void ZELightDirectional::SetColor(const ZEVector3& NewColor)
{
	Changed = true;
	Color = NewColor;
}

const ZEVector3& ZELightDirectional::GetColor() const
{
	return Color;
}

void ZELightDirectional::SetIntensity(float NewValue)
{
	Changed = true;
	Intensity = NewValue;
}

float ZELightDirectional::GetIntensity() const
{
	return Intensity;
}

void ZELightDirectional::SetPenumbraSize(float Value)
{
	Changed = true;
	PenumbraSize = Value;
}

float ZELightDirectional::GetPenumbraSize() const
{
	return PenumbraSize;
}

void ZELightDirectional::SetSlopeScaledBias(float Value)
{
	Changed = true;
	BiasSlopeScaled = Value;
}

float ZELightDirectional::GetSlopeScaledBias() const
{
	return BiasSlopeScaled;
}

void ZELightDirectional::SetDepthScaledBias(float Value)
{
	Changed = true;
	BiasDepthScaled = Value;
}

float ZELightDirectional::GetDepthScaledBias() const
{
	return BiasDepthScaled;
}

void ZELightDirectional::UpdateBuffers(const ZEDrawParameters* DrawParameters)
{
	if (!Changed)
		return;

	if (DrawParameters->View->Type != ZE_VT_CAMERA)
		return;

	ZECamera* Camera = DrawParameters->View->Camera;

	ZEMaterialLightDirectional::Transformations* Transformations = NULL;
	Material->LightTransformations->Lock((void**)&Transformations);

		Transformations->InvProjectionMatrix = Camera->GetProjectionTransform().Inverse();

	Material->LightTransformations->Unlock();

	ZEMaterialLightDirectional::Properties* Properties = NULL;
	Material->LightProperties->Lock((void**)&Properties);
	
		Properties->Color = Color;
		Properties->Intensity = Intensity;
		Properties->Direction =  Camera->GetViewTransform() * -GetWorldFront();
		Properties->PenumbraSize = PenumbraSize;
		Properties->BiasDepthScaled = BiasDepthScaled;
		Properties->BiasSlopeScaled = BiasSlopeScaled;
			
	Material->LightProperties->Unlock();
}

const ZEViewVolume* ZELightDirectional::GetLightVolume()
{
	// No culling and no Light volume for directional light.
	return NULL;
}

bool ZELightDirectional::InitializeSelf()
{
	if (!ZELight::InitializeSelf())
		return false;

	// Vertex buffer
	ZECanvas Canvas;
	Canvas.AddQuad(	ZEVector3( 1.0f,  1.0f, 1.0f),
					ZEVector3(-1.0f,  1.0f, 1.0f),
					ZEVector3(-1.0f, -1.0f, 1.0f),
					ZEVector3( 1.0f, -1.0f, 1.0f));
	VertexBuffer = Canvas.CreateStaticVertexBuffer();

	// Vertex layout
	static const ZEVertexElement Element[] = 
	{
		{"POSITION", 0, ZE_VET_FLOAT4, 0, 0, ZE_VU_PER_VERTEX, 0}
	};
	VertexLayout.SetLayout(Element, 1);

	// Material
	Material = ZEMaterialLightDirectional::CreateInstance();

	// Render command
	RenderCommand.Order = 3.0f;
	RenderCommand.Priority = 3;
	RenderCommand.FirstVertex = 0;
	RenderCommand.Material = Material;
	RenderCommand.PrimitiveCount = 2;
	RenderCommand.PrimitiveType = ZE_PT_TRIANGLE_LIST;
	RenderCommand.VertexLayout = &VertexLayout;
	RenderCommand.VertexBuffers[0] = VertexBuffer;

	return CascadedShadow.Initialize(this);
}

bool ZELightDirectional::DeinitializeSelf()
{
	CascadedShadow.Deinitialize();

	return ZELight::DeinitializeSelf();
}

void ZELightDirectional::Tick(float Time)
{

}

void ZELightDirectional::Draw(ZEDrawParameters* DrawParameters)
{
	if (ShadowCaster)
	{
		CascadedShadow.Update(DrawParameters->View->Camera);
		CascadedShadow.Draw(DrawParameters);
	}
	
	UpdateBuffers(DrawParameters);
	DrawParameters->Renderer->AddRenderCommand(&RenderCommand);

	ZELight::Draw(DrawParameters);
}

ZELightDirectional::ZELightDirectional() : ZELight(ZE_LT_DIRECTIONAL)
{
	Color = ZEVector3::One;
	Intensity = 1.0f;
	PenumbraSize = 1.0f;
	BiasSlopeScaled = 0.4f;
	BiasDepthScaled = 0.6f;
	
	Material = NULL;
	VertexBuffer = NULL;
}

ZELightDirectional::~ZELightDirectional()
{
	Deinitialize();
}

ZELightDirectional* ZELightDirectional::CreateInstance()
{
	return new ZELightDirectional();
}
