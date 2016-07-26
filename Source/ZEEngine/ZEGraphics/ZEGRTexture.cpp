//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRTexture.cpp
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

#include "ZEGRTexture.h"

#include "ZEMath/ZEMath.h"
#include "ZEGRGraphicsModule.h"
#include "ZEGRTexture.h"
#include "ZEGRContext.h"
#include "ZEGRDepthStencilBuffer.h"
#include "ZEGRRenderTarget.h"


ZEGRTextureOptions::ZEGRTextureOptions()
{
	CompressionFormat = ZEGR_TF_NONE;
	MaximumMipmapLevel = 0;
	GenerateMipMaps = false;
	sRGB = false;
}

void ZEGRTexture::SetFormat(ZEGRFormat Format)
{
	this->Format = Format;
}

void ZEGRTexture::SetLevelCount(ZEUInt LevelCount)
{
	this->LevelCount = LevelCount;
}

void ZEGRTexture::SetBoundStage(ZEGRShaderType Shader, ZEInt Slot, bool BoundAsShaderResource, bool BoundAsUnorderedAccess)
{
	zeDebugCheck(BoundAsShaderResource && BoundAsUnorderedAccess, "A texture cannot be bound as both shader resource and unordered access");

	BoundStages[Shader].BoundAsShaderResource = BoundAsShaderResource;
	BoundStages[Shader].BoundAsUnorderedAccess = BoundAsUnorderedAccess;
	BoundStages[Shader].Slot = Slot;
}

const ZEArray<ZEGRTexture::BoundStage>& ZEGRTexture::GetBoundStages() const
{
	return BoundStages;
}

const ZEArray<ZEHolder<ZEGRRenderTarget>>& ZEGRTexture::GetRenderTargets() const
{
	return RenderTargets;
}

const ZEArray<ZEHolder<ZEGRDepthStencilBuffer>>& ZEGRTexture::GetDepthStencilBuffers() const
{
	return DepthStencilBuffers;
}

ZESize ZEGRTexture::CalculateSize(ZEUInt Width, ZEUInt Height, ZEUInt LevelCount, ZEGRFormat Format)
{
	const ZEGRFormatDefinition* FormatDefinition = ZEGRFormatDefinition::GetDefinition(Format);
	if (FormatDefinition->Format == ZEGR_TF_NONE)
		return 0;

	ZESize Output = 0;

	Width /= (ZEUInt)FormatDefinition->BlockDimension;
	Height /= (ZEUInt)FormatDefinition->BlockDimension;

	for (ZESize I = 0; I < LevelCount; I++)
		Output += (Width >> I) * (Height >> I);

	return Output * FormatDefinition->BlockSize;
}

ZESize ZEGRTexture::CalculateLevelCount(ZEUInt Width, ZEUInt Height, ZEGRFormat Format)
{
	const ZEGRFormatDefinition* FormatDefinition = ZEGRFormatDefinition::GetDefinition(Format);
	if (FormatDefinition->Format == ZEGR_TF_NONE)
		return 0;

	Width /= (ZEUInt)FormatDefinition->BlockDimension;
	Height /= (ZEUInt)FormatDefinition->BlockDimension;

	if (!ZEMath::IsPowerOfTwo(Width) || !ZEMath::IsPowerOfTwo(Height))
		return 1;

	ZEUInt Level = 1;
	while(Width != 1 && Height != 1)
	{
		Width >>= 1;
		Height >>= 1;
		Level++;
	}

	return Level;
}

ZEGRTexture::ZEGRTexture()
{
	Format = ZEGR_TF_NONE;
	LevelCount = 0;

	BoundStages.SetCount(ZEGR_SHADER_TYPE_COUNT);
	BoundStage Stage;
	Stage.BoundAsShaderResource = false;
	Stage.BoundAsUnorderedAccess = false;
	Stage.Slot = -1;
	BoundStages.Fill(Stage);

	DepthStencilBuffers.SetCount(8);
}

ZEGRTexture::~ZEGRTexture()
{
	ZEGRContext* Context = ZEGRGraphicsModule::GetInstance()->GetMainContext();
	if (Context != NULL)
	{
		ze_for_each(Stage, BoundStages)
		{
			if (Stage->BoundAsShaderResource)
				Context->ClearShaderResources((ZEGRShaderType)Stage.GetIndex(), Stage->Slot, 1);

			else if (Stage->BoundAsUnorderedAccess)
				Context->ClearUnorderedAccesses(Stage->Slot, 1);
		}
	}

	RenderTargets.Clear();
	DepthStencilBuffers.Clear();
}

ZEGRFormat ZEGRTexture::GetFormat() const
{
	return Format;
}

ZEUInt ZEGRTexture::GetLevelCount() const
{
	return LevelCount;
}

