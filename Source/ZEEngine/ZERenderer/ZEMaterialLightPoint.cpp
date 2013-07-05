//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialLightPoint.cpp
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

#include "ZECamera.h"
#include "ZECanvas.h"
#include "ZERenderStage.h"
#include "ZEGame/ZEScene.h"
#include "ZEGraphics/ZEShader.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZEMaterialLightPoint.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEShaderCompileOptions.h"

void ZEMaterialLightPoint::UpdateShaders()
{
	ZEShaderCompileOptions Compileoptions;

	if (VertexShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_VERTEX;
		Compileoptions.FileName = "PointLight.hlsl";
		Compileoptions.EntryPoint = "ZEPointLight_VertexShader";
		VertexShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
	if (PixelShader == NULL)
	{
		Compileoptions.Model = ZE_SM_4_0;
		Compileoptions.Type = ZE_ST_PIXEL;
		Compileoptions.FileName = "PointLight.hlsl";
		Compileoptions.EntryPoint = "ZEPointLight_PixelShader";
		PixelShader = ZEShaderCompiler::CompileShaderFromFile(&Compileoptions);
	}
}

void ZEMaterialLightPoint::UpdateBuffers()
{
	if (LightTransformations == NULL)
	{
		LightTransformations = ZEConstantBuffer::CreateInstance();
		LightTransformations->Create(VertexShader->GetMetaTable()->GetBufferInfo("TransformationsVS"));
	}
	if (LightProperties == NULL)
	{
		LightProperties = ZEConstantBuffer::CreateInstance();
		LightProperties->Create(PixelShader->GetMetaTable()->GetBufferInfo("LightParametersPS"));	
	}

}

void ZEMaterialLightPoint::DestroyShaders()
{
	ZE_DESTROY(VertexShader);
	ZE_DESTROY(PixelShader);
}

void ZEMaterialLightPoint::DestroyBuffers()
{
	ZE_DESTROY(LightTransformations);
	ZE_DESTROY(LightProperties);
}

bool ZEMaterialLightPoint::SetupLightingPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	UpdateMaterial();

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();
	
	Device->SetVertexShader(VertexShader);
	Device->SetPixelShader(PixelShader);
	
	Device->SetPixelShaderBuffer(0, LightProperties);
	Device->SetVertexShaderBuffer(0, LightTransformations);
	
	return true;
}

ZESize ZEMaterialLightPoint::GetHash() const
{
	return ZEHashGenerator::Hash(ZEString("ZEMaterialLightPoint"));
}

bool ZEMaterialLightPoint::UpdateMaterial()
{
	UpdateShaders();
	UpdateBuffers();

	return true;
}

bool ZEMaterialLightPoint::SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	zeDebugCheck(Stage == NULL, "Null pointer.");
	zeDebugCheck(RenderCommand == NULL, "Null pointer.");

	if (!Stage->GetStageType().GetFlags(ZE_RST_LIGHTING))
		return true;

	return SetupLightingPass(Stage, RenderCommand);
}

ZEMaterialLightPoint* ZEMaterialLightPoint::CreateInstance()
{
	return new ZEMaterialLightPoint();
}

ZEMaterialLightPoint::ZEMaterialLightPoint()
{
	VertexShader = NULL;
	PixelShader = NULL;
	LightTransformations = NULL;
	LightProperties = NULL;
}

ZEMaterialLightPoint::~ZEMaterialLightPoint()
{
	DestroyBuffers();
	DestroyShaders();
}
