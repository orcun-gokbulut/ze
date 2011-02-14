//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESoundBrush3D.cpp
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

#include "ZESoundBrush3D.h"
#include "ZEGraphics/ZERenderer.h"
#include "ZEGame/ZEDrawParameters.h"

void ZESoundBrush3D::Draw(ZEDrawParameters* DrawParameters)
{
	if (SoundSource != NULL)
	{
		RenderOrder.WorldMatrix = SoundSource->GetWorldTransform();
		DrawParameters->Renderer->AddToRenderList(&RenderOrder);
	}
}

ZEDWORD ZESoundBrush3D::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

ZESoundSource3D* ZESoundBrush3D::GetSoundSource()
{
	return SoundSource;
}

void ZESoundBrush3D::SetSoundResource(ZESoundResource* SoundResource)
{
	SoundSource->SetSoundResource(SoundResource);
}

ZESoundResource* ZESoundBrush3D::GetSoundResource()
{
	return SoundSource->GetSoundResource();
}

bool ZESoundBrush3D::Initialize()
{
	return true;
}
void ZESoundBrush3D::Deinitialize()
{
	if (SoundSource != NULL)
	{
		UnregisterComponent(SoundSource);
		SoundSource->Destroy();
		SoundSource = NULL;
	}
}

ZESoundBrush3D::ZESoundBrush3D()
{
	Material = ZEFixedMaterial::CreateInstance();
	Material->SetZero();
	Material->SetAmbientEnabled(true);
	Material->SetAmbientColor(ZEVector3(1.0f, 0.68f, 0.79f));
	Material->SetAmbientFactor(0.65f);
	this->SoundSource = ZESoundSource3D::CreateInstance();
	Canvas.AddSphere(0.2f, 50, 50);
	
	
	SoundSource->SetPosition(ZEVector3(0.0f, 0.0f, 0.0f));

	RenderOrder.SetZero();
	RenderOrder.Material = Material;
	RenderOrder.WorldMatrix = GetWorldTransform();
	RenderOrder.PrimitiveType = ZE_ROPT_TRIANGLE;
	RenderOrder.VertexDeclaration = ZECanvasVertex::GetVertexDeclaration();
	RenderOrder.Flags = ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_Z_CULLING;

	RenderOrder.VertexBuffer = Canvas.CreateStaticVertexBuffer();
	RenderOrder.PrimitiveCount = Canvas.Vertices.GetCount() / 3;


	SetBoundingVolumeMechanism(ZE_BVM_USE_BOTH);

	RegisterComponent(SoundSource);
}
