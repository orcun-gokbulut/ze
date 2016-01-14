//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEATAtmosphere.cpp
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

#include "ZEATAtmosphere.h"
#include "ZEGame/ZEScene.h"
#include "ZEATSun.h"
#include "ZERenderer/ZERNRenderParameters.h"
#include "ZERenderer/ZERNRenderer.h"
#include "ZERenderer/ZERNCuller.h"
#include "ZERenderer/ZERNStageGBuffer.h"
#include "ZEGraphics/ZEGRTexture2D.h"

bool ZEATAtmosphere::InitializeSelf()
{
	Sun = new ZEATSun();
	Sun->SetIntensity(10.0f);
	Sun->SetColor(ZEVector3::One);
	Sun->SetObserver(Observer);

	AtmosphericScattering.SetLightIntensity(Sun->GetIntensity());
	AtmosphericScattering.SetLightColor(Sun->GetColor());
	AtmosphericScattering.SetMultipleScattering(true);
	AtmosphericScattering.SetOrderCount(5);

	AtmosphericScattering.Initialize();

	return true;
}

bool ZEATAtmosphere::DeinitializeSelf()
{
	AtmosphericScattering.Deinitialize();

	delete Sun;
	Sun = NULL;

	return true;
}

void ZEATAtmosphere::SetObserver(const ZEATObserver& Observer)
{
	this->Observer = Observer;
}

const ZEATObserver& ZEATAtmosphere::GetObserver() const
{
	return Observer;
}

void ZEATAtmosphere::SetMultipleScattering(bool MultipleScattering)
{
	AtmosphericScattering.SetMultipleScattering(MultipleScattering);
}

bool ZEATAtmosphere::GetMultipleScattering()
{
	return AtmosphericScattering.GetMultipleScattering();
}

ZEATAtmosphere::ZEATAtmosphere()
{
	Sun = NULL;
	Command.Entity = this;
	Command.StageMask = ZERN_STAGE_POST_EFFECT;
}

ZEATAtmosphere::~ZEATAtmosphere()
{
	DeinitializeSelf();
}

ZEDrawFlags ZEATAtmosphere::GetDrawFlags() const
{
	return ZE_DF_DRAW;
}

void ZEATAtmosphere::Tick(float Time)
{
	Sun->Tick(Time);
}

bool ZEATAtmosphere::PreRender(const ZERNCullParameters* CullParameters)
{
	CullParameters->Renderer->AddCommand(&Command);

	return true;
}

void ZEATAtmosphere::Render(const ZERNRenderParameters* Parameters, const ZERNCommand* Command)
{
	//AtmosphericScattering.SetLightDirection(Sun->GetDirection());
	AtmosphericScattering.SetLightDirection(ZEVector3(-1.0f, -1.0f, 0.0f));
	AtmosphericScattering.Process(Parameters->Context);
}
