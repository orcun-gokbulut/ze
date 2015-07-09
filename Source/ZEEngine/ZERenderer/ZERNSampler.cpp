//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNSampler.cpp
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

#include "ZERNSampler.h"

#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETexture3DResource.h"
#include "ZETexture/ZETextureCubeResource.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"
#include "ZEMath/ZEMath.h"

void ZERNSampler::SetTexture(ZEGRTexture* Texture)
{
	this->Texture = Texture;
	if (Resource != NULL)
	{
		Resource->Release();
		Resource = NULL;
	}
}

ZEGRTexture* ZERNSampler::GetTexture() const
{
	if (Resource != NULL)
		return Resource->GetTexture();
	else
		return Texture;
}

void ZERNSampler::SetTextureResource(ZETextureResource* Resource)
{
	if (this->Resource == Resource)
		return;

	if (this->Resource != NULL)
		this->Resource->Release();

	this->Resource = Resource;

	this->Resource->AddReferance();
}

ZETextureResource* ZERNSampler::GetTextureResource() const
{
	return Resource;
}

void ZERNSampler::Load2D(const ZEString& FileName)
{
	SetTextureResource(ZETexture2DResource::LoadSharedResource(FileName));
}

void ZERNSampler::Load3D(const ZEString& FileName)
{
	SetTextureResource(ZETexture3DResource::LoadSharedResource(FileName, 1, 1));
}

void ZERNSampler::LoadCube(const ZEString& FileName)
{
	SetTextureResource(ZETextureCubeResource::LoadSharedResource(FileName));
}

const ZEString& ZERNSampler::GetTextureFile() const
{
	if (Resource == NULL)
		return ZEString::Empty;

	return Resource->GetFileName();
}

void ZERNSampler::Write(ZEMLWriterNode& Node)
{
	Node.WriteString("Resource", GetTextureFile());

	Node.WriteUInt8("AddressU", Sampler.GetAddressU());
	Node.WriteUInt8("AddressV", Sampler.GetAddressV());
	Node.WriteUInt8("AddressW", Sampler.GetAddressW());
	Node.WriteUInt8("MinFilter", Sampler.GetMinFilter());
	Node.WriteUInt8("MagFilter", Sampler.GetMagFilter());
	Node.WriteUInt8("MipFilter", Sampler.GetMipFilter());
	Node.WriteUInt8("MaxAnisotropy", Sampler.GetMaxAnisotrophy());
	Node.WriteFloat("MinLOD", Sampler.GetMinLOD());
	Node.WriteFloat("MaxLOD", Sampler.GetMaxLOD());
	Node.WriteFloat("MipLODBias", Sampler.GetMipLODBias());
	Node.WriteVector4("BorderColor", Sampler.GetBorderColor());

}

void ZERNSampler::Read(ZEMLReaderNode& Node)
{
	if (!Node.IsValid())
	{
		Texture.Release();
		if (Resource != NULL)
			Resource->Release();
		Sampler.SetToDefault();
		return;
	}

	Load2D(Node.ReadString("Resource", ""));
	Sampler.SetAddressU((ZEGRTextureAddressing)Node.ReadUInt8("AddressU", ZEGR_TAM_CLAMP));
	Sampler.SetAddressV((ZEGRTextureAddressing)Node.ReadUInt8("AddressV", ZEGR_TAM_CLAMP));
	Sampler.SetAddressW((ZEGRTextureAddressing)Node.ReadUInt8("AddressW", ZEGR_TAM_CLAMP));
	Sampler.SetMinFilter((ZEGRTextureFilter)Node.ReadUInt8("MinFilter", ZEGR_TFM_POINT));
	Sampler.SetMagFilter((ZEGRTextureFilter)Node.ReadUInt8("MagFilter", ZEGR_TFM_POINT));
	Sampler.SetMipFilter((ZEGRTextureFilter)Node.ReadUInt8("MipFilter", ZEGR_TFM_POINT));
	Sampler.SetMaxAnisotrophy(Node.ReadUInt8("MaxAnisotropy", 8));
	Sampler.SetMinLOD(Node.ReadFloat("MinLOD", ZE_FLOAT_MIN));
	Sampler.SetMaxLOD(Node.ReadFloat("MaxLOD", ZE_FLOAT_MIN));
	Sampler.SetMipLODBias(Node.ReadFloat("MipLODBias", 0.0f));
	Sampler.SetBorderColor(Node.ReadVector4("BorderColor", ZEVector4::Zero));
}

ZERNSampler::ZERNSampler()
{
	Resource = NULL;
	Texture = NULL;
}

ZERNSampler::~ZERNSampler()
{
	Texture.Release();
	if (Resource != NULL)
	{
		Resource->Release();
		Resource = NULL;
	}

}
