//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATSun.cpp
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

#include "ZEATSun.h"

#include "ZEATPeriodicTerms.h"
#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"
#include "ZEGraphics/ZEGRContext.h"
#include "ZEGraphics/ZEGRViewport.h"
#include "ZEGraphics/ZEGRRenderTarget.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZERenderer/ZERNSunMaterial.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNStage.h"

bool ZEATSun::CalculateSunPositionScreen(const ZERNView& View, ZEVector2& OutVector)
{
	ZEVector3 SunDirectionView;
	Direction.NormalizeSelf();
	ZEMatrix4x4::Transform3x3(SunDirectionView, View.ViewTransform, -Direction);
	float SunPositionScreenX = SunDirectionView.x * View.ProjectionTransform.M11 / SunDirectionView.z;
	float SunPositionScreenY = SunDirectionView.y * View.ProjectionTransform.M22 / SunDirectionView.z;

	if (SunPositionScreenX >= -1.0f && SunPositionScreenX <= 1.0f &&
		SunPositionScreenY >= -1.0f && SunPositionScreenY <= 1.0f &&
		ZEVector3::DotProduct(View.N, -Direction) > 0.0f)
	{
		OutVector.x = SunPositionScreenX;
		OutVector.y = SunPositionScreenY;
		
		return true;
	}

	return false;
}

bool ZEATSun::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	Material = ZERNSunMaterial::CreateInstance();
	Material->Initialize();

	return true;
}

bool ZEATSun::DeinitializeSelf()
{
	Material.Release();

	return ZEEntity::DeinitializeSelf();
}

ZEDrawFlags ZEATSun::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEATSun::SetDirection(const ZEVector3& Direction)
{
	this->Direction = Direction;
}

const ZEVector3& ZEATSun::GetDirection() const
{
	return Direction;
}

void ZEATSun::SetDiskRadius(float DiskRadius)
{
	this->DiskRadius = DiskRadius;
}

float ZEATSun::GetDiskRadius() const
{
	return DiskRadius;
}

bool ZEATSun::PreRender(const ZERNCullParameters* CullParameters)
{
	const ZERNView& View = *CullParameters->View;

	ZEVector2 SunPositionScreen;
	if (!CalculateSunPositionScreen(View, SunPositionScreen))
		return false;

	ZEVector2 SunSizeScreen = DiskRadius * ZEVector2(View.ProjectionTransform.M11, View.ProjectionTransform.M22);

	Material->SetSunPositionScreen(SunPositionScreen);
	Material->SetSunSizeScreen(SunSizeScreen);

	Command.Entity = this;
	Command.StageMask = Material->GetStageMask();
	Command.Priority = 2;

	CullParameters->Renderer->AddCommand(&Command);

	return true;
}

void ZEATSun::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	ZEGRContext* Context = Parameters->Context;
	ZERNStage* Stage = Parameters->Stage;

	if (!Material->SetupMaterial(Context, Stage))
		return;

	const ZEGRTexture2D* ColorTexture = Stage->GetOutput(ZERN_SO_ACCUMULATION);
	const ZEGRRenderTarget* RenderTarget = ColorTexture->GetRenderTarget();

	Context->SetRenderTargets(1, &RenderTarget, NULL);
	Context->SetVertexBuffers(0, 0, NULL);
	Context->SetViewports(1, &ZEGRViewport(0.0f, 0.0f, RenderTarget->GetWidth(), RenderTarget->GetHeight()));
	Context->Draw(4, 0);

	Context->SetRenderTargets(0, NULL, NULL);

	Material->CleanupMaterial(Context, Stage);
}

ZEATSun::ZEATSun()
{
	Direction = ZEVector3(0.0f, -1.0f, 0.0f);
}
