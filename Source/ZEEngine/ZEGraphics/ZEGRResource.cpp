//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGRResource.cpp
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

#include "ZEGRResource.h"

#include "ZEGRGraphicsModule.h"
#include "ZEGRContext.h"

void ZEGRResource::SetBoundStage(ZEGRShaderType Shader, ZEInt Slot, bool BoundAsShaderResource, bool BoundAsUnorderedAccess)
{
	zeDebugCheck(BoundAsShaderResource && BoundAsUnorderedAccess, "A resource cannot be bound as both shader resource and unordered access");

	BoundStages[Shader].BoundAsShaderResource = BoundAsShaderResource;
	BoundStages[Shader].BoundAsUnorderedAccess = BoundAsUnorderedAccess;
	BoundStages[Shader].Slot = Slot;
}

const ZEArray<ZEGRResource::BoundStage>& ZEGRResource::GetBoundStages() const
{
	return BoundStages;
}

void ZEGRResource::SetSize(ZESize Size)
{
	this->Size = Size;
	this->SetMemoryUsage(ZERS_MP_GPU, Size);
}

void ZEGRResource::SetResourceUsage(ZEGRResourceUsage Usage)
{
	this->Usage = Usage;
}

void ZEGRResource::SetResourceBindFlags(ZEFlags BindFlags)
{
	this->BindFlags = BindFlags;
}

void ZEGRResource::SetFormat(ZEGRFormat Format)
{
	this->Format = Format;
}

ZEGRResource::ZEGRResource()
{
	Size = 0;
	Usage = ZEGR_RU_STATIC;
	BindFlags = ZEGR_RBF_SHADER_RESOURCE;
	Format = ZEGR_TF_NONE;

	BoundStages.SetCount(ZEGR_SHADER_TYPE_COUNT);
	BoundStage Stage;
	Stage.BoundAsShaderResource = false;
	Stage.BoundAsUnorderedAccess = false;
	Stage.Slot = -1;
	BoundStages.Fill(Stage);

}

ZEGRResource::~ZEGRResource()
{
	SetSize(0);
	Usage = ZEGR_RU_STATIC;
	BindFlags = ZEGR_RBF_SHADER_RESOURCE;
	Format = ZEGR_TF_NONE;

	ZEGRGraphicsModule* GraphicsModule = ZEGRGraphicsModule::GetInstance();
	if (GraphicsModule != NULL)
	{
		ZEGRContext* Context = GraphicsModule->GetMainContext();
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
	}
}

void ZEGRResource::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEGRResource::GetName() const
{
	return Name;
}

ZESize ZEGRResource::GetSize() const
{
	return Size;
}

ZEGRResourceUsage ZEGRResource::GetResourceUsage() const
{
	return Usage;
}

ZEFlags ZEGRResource::GetResourceBindFlags() const
{
	return BindFlags;
}

ZEGRFormat ZEGRResource::GetFormat() const
{
	return Format;
}
