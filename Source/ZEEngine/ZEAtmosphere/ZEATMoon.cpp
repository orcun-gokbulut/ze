//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATMoon.cpp
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

#include "ZEATMoon.h"

#include "ZEMath/ZEAngle.h"
#include "ZEMath/ZEMath.h"
#include "ZEATPeriodicTerms.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZEGame/ZEMoon.h"
#include "ZERenderer/ZERNMoonMaterial.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZEGraphics/ZEGRContext.h"

bool ZEATMoon::CalculateMoonPositionScreen(const ZERNView& View, ZEVector2& OutVector)
{
	ZEVector3 MoonDirectionView;
	Direction.NormalizeSelf();
	ZEMatrix4x4::Transform3x3(MoonDirectionView, View.ViewTransform, -Direction);
	float MoonPositionScreenX = MoonDirectionView.x * View.ProjectionTransform.M11;
	float MoonPositionScreenY = MoonDirectionView.y * View.ProjectionTransform.M22;

	if (MoonPositionScreenX >= -1.0f && MoonPositionScreenX <= 1.0f &&
		MoonPositionScreenY >= -1.0f && MoonPositionScreenY <= 1.0f &&
		ZEVector3::DotProduct(View.N, -Direction) > 0.0f)
	{
		OutVector.x = MoonPositionScreenX;
		OutVector.y = MoonPositionScreenY;

		return true;
	}

	return false;
}

bool ZEATMoon::InitializeSelf()
{
	if (!ZEEntity::InitializeSelf())
		return false;

	Material = ZERNMoonMaterial::CreateInstance();
	Material->Initialize();

	return true;
}

bool ZEATMoon::DeinitializeSelf()
{
	Material.Release();

	return ZEEntity::DeinitializeSelf();
}

ZEDrawFlags ZEATMoon::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEATMoon::SetColor(const ZEVector3& Color)
{
	this->Color = Color;
}

const ZEVector3& ZEATMoon::GetColor() const
{
	return Color;
}

void ZEATMoon::SetDirection(const ZEVector3& Direction)
{
	this->Direction = Direction;
}

const ZEVector3& ZEATMoon::GetDirection() const
{
	return Direction;
}

void ZEATMoon::SetIntensity(float Intensity)
{
	this->Intensity = Intensity;
}

float ZEATMoon::GetIntensity() const
{
	return Intensity;
}

void ZEATMoon::SetTextureFile(const ZEString& FileName, ZEUInt HorizTileCount, ZEUInt VertTileCount)
{
	Material->SetPhaseTextureFile(FileName, HorizTileCount, VertTileCount);
}

const ZEString& ZEATMoon::GetTextureFile() const
{
	return Material->GetPhaseTextureFile();
}

bool ZEATMoon::PreRender(const ZERNCullParameters* CullParameters)
{
	const ZERNView& View = *CullParameters->View;

	ZEVector2 MoonPositionScreen;
	if (!CalculateMoonPositionScreen(View, MoonPositionScreen))
		return false;

	ZEVector2 MoonSizeScreen = ZEAngle::Tan(ZEAngle::ToRadian(0.55f * 0.5f)) * ZEVector2(View.ProjectionTransform.M11, View.ProjectionTransform.M22);
	MoonSizeScreen *= 5.0f;

	Material->SetMoonPositionScreen(MoonPositionScreen);
	Material->SetMoonSizeScreen(MoonSizeScreen);
	Material->SetMoonPhase(31.5f / 53.0f);

	Command.Entity = this;
	Command.StageMask = Material->GetStageMask();
	Command.Priority = 3;

	CullParameters->Renderer->AddCommand(&Command);

	return true;
}

void ZEATMoon::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	ZEGRContext* Context = Parameters->Context;
	ZERNStage* Stage = Parameters->Stage;

	if (!Material->SetupMaterial(Context, Stage))
		return;

	Context->SetVertexBuffers(0, 0, NULL);
	Context->Draw(4, 0);

	Material->CleanupMaterial(Context, Stage);
}

ZEATMoon::ZEATMoon()
{
	Color = ZEVector3::One;
	Direction = ZEVector3(0.0f, -1.0f, 0.0f);
	Intensity = 1.0f;
}
