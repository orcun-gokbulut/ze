//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZERNMap.cpp
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

#include "ZERNMap.h"

#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"
#include "ZEMath/ZEMath.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETexture3DResource.h"
#include "ZETexture/ZETextureCubeResource.h"
#include "ZEGraphics/ZEGRSampler.h"

void ZERNMap::Clean()
{
	if (Resource != NULL)
	{
		Resource->Release();
		Resource = NULL;
	}

	Texture = NULL;
	Sampler = NULL;
}

void ZERNMap::SetSampler(const ZEGRSampler* Sampler)
{
	this->Sampler = Sampler;
}

const ZEGRSampler* ZERNMap::GetSampler() const
{
	return Sampler;
}

void ZERNMap::SetTexture(const ZEGRTexture* Texture)
{
	if (Resource != NULL)
	{
		Resource->Release();
		Resource = NULL;
	}

	this->Texture = Texture;
}

const ZEGRTexture* ZERNMap::GetTexture() const
{
	if (Resource != NULL)
		return Resource->GetTexture();
	else
		return Texture;
}

void ZERNMap::SetTextureResource(ZETextureResource* Resource)
{
	if (this->Resource == Resource)
		return;

	if (this->Resource != NULL)
		this->Resource->Release();

	this->Resource = Resource;

	this->Resource->AddReferance();

	Texture = Resource->GetTexture();
}

const ZETextureResource* ZERNMap::GetTextureResource() const
{
	return Resource;
}

void ZERNMap::Load2D(const ZEString& FileName)
{
	SetTextureResource(ZETexture2DResource::LoadSharedResource(FileName));
}

void ZERNMap::Load3D(const ZEString& FileName, ZEUInt HorizontalTileCount, ZEUInt VerticalTileCount)
{
	SetTextureResource(ZETexture3DResource::LoadSharedResource(FileName, HorizontalTileCount, VerticalTileCount));
}

void ZERNMap::LoadCube(const ZEString& FileName)
{
	SetTextureResource(ZETextureCubeResource::LoadSharedResource(FileName));
}

const ZEString& ZERNMap::GetTextureFile() const
{
	if (Resource == NULL)
		return ZEString::Empty;

	return Resource->GetFileName();
}

bool ZERNMap::IsAvailable() const
{
	return (Texture != NULL);
}

void ZERNMap::Write(ZEMLWriterNode& ParentNode, const ZEString& Name)
{
	ZEMLWriterNode Node;
	if (ParentNode.OpenNode(Name, Node))
		return;

	const ZEGRSamplerDescription& SamplerDescription = Sampler->GetDescription();

	Node.WriteString("Resource", GetTextureFile());
	Node.WriteUInt8("AddressU", SamplerDescription.AddressU);
	Node.WriteUInt8("AddressV", SamplerDescription.AddressV);
	Node.WriteUInt8("AddressW", SamplerDescription.AddressW);
	Node.WriteUInt8("MinFilter", SamplerDescription.MinFilter);
	Node.WriteUInt8("MagFilter", SamplerDescription.MagFilter);
	Node.WriteUInt8("MipFilter", SamplerDescription.MipFilter);
	Node.WriteUInt8("MaxAnisotropy", SamplerDescription.MaxAnisotropy);
	Node.WriteFloat("MinLOD", SamplerDescription.MinLOD);
	Node.WriteFloat("MaxLOD", SamplerDescription.MaxLOD);
	Node.WriteFloat("MipLODBias", SamplerDescription.MipMapLODBias);
	Node.WriteVector4("BorderColor", SamplerDescription.BorderColor);

	ParentNode.CloseNode();
}

void ZERNMap::Read(ZEMLReaderNode& ParentNode, const ZEString& Name)
{
	Clean();

	ZEString Filename = ParentNode.ReadString(Name, "");

	ZEGRSamplerDescription SamplerDescription;
	SamplerDescription.AddressU = ZEGR_TAM_WRAP;
	SamplerDescription.AddressV = ZEGR_TAM_WRAP;
	SamplerDescription.AddressW = ZEGR_TAM_WRAP;
	SamplerDescription.MinFilter = ZEGR_TFM_LINEAR;
	SamplerDescription.MagFilter = ZEGR_TFM_LINEAR;
	SamplerDescription.MipFilter = ZEGR_TFM_LINEAR;

	ZEMLReaderNode Node = ParentNode.GetNode(Name);
	if (Node.IsValid())
	{
		SamplerDescription.AddressU = (ZEGRTextureAddressing)Node.ReadUInt8("AddressU", ZEGR_TAM_WRAP);
		SamplerDescription.AddressV = (ZEGRTextureAddressing)Node.ReadUInt8("AddressV", ZEGR_TAM_WRAP);
		SamplerDescription.AddressW = (ZEGRTextureAddressing)Node.ReadUInt8("AddressW", ZEGR_TAM_WRAP);
		SamplerDescription.MinFilter = (ZEGRTextureFilter)Node.ReadUInt8("MinFilter", ZEGR_TFM_LINEAR);
		SamplerDescription.MagFilter = (ZEGRTextureFilter)Node.ReadUInt8("MagFilter", ZEGR_TFM_LINEAR);
		SamplerDescription.MipFilter = (ZEGRTextureFilter)Node.ReadUInt8("MipFilter", ZEGR_TFM_LINEAR);
		SamplerDescription.BorderColor = Node.ReadVector4("BorderColor", ZEVector4::Zero);
		SamplerDescription.MaxAnisotropy = Node.ReadUInt8("MaxAnisotropy", 8);
		SamplerDescription.MinLOD = Node.ReadFloat("MinLOD", ZE_FLOAT_MIN);
		SamplerDescription.MaxLOD = Node.ReadFloat("MaxLOD", ZE_FLOAT_MAX);
		SamplerDescription.MipMapLODBias = Node.ReadFloat("MipLODBias", 0.0f);
		Filename = Node.ReadString("FileName", "");
	}

	if (!Filename.IsEmpty())
	{
		Load2D(ZEPathInfo::CombineRelativePath(ParentNode.GetFile()->GetPath(), Filename));
		SetSampler(ZEGRSampler::GetSampler(SamplerDescription));
	}
}

ZERNMap::ZERNMap()
{
	Resource = NULL;
	Texture = NULL;
	Sampler = NULL;
}

ZERNMap::ZERNMap(const ZEString& FileName, ZEGRTextureType Type, ZEGRSampler* Sampler)
{
	Resource = NULL;
	switch (Type)
	{
		default:
		case ZEGR_TT_NONE:
			break;

		case ZEGR_TT_2D:
			Load2D(FileName);
			break;

		case ZEGR_TT_3D:
			Load3D(FileName, 1, 1);
			break;

		case ZEGR_TT_CUBE:
			LoadCube(FileName);
			break;
	}

	if (Sampler != NULL)
		SetSampler(Sampler);
	else
		SetSampler(ZEGRSampler::GetDefaultSampler());
}

ZERNMap::ZERNMap(ZEGRTexture* Texture, ZEGRSampler* Sampler)
{
	Resource = NULL;
	SetTexture(Texture);
	if (Sampler != NULL)
		SetSampler(Sampler);
	else
		SetSampler(ZEGRSampler::GetDefaultSampler());
}

ZERNMap::ZERNMap(ZETextureResource* Resource, ZEGRSampler* Sampler)
{
	this->Resource = NULL;
	SetTextureResource(Resource);
	if (Sampler != NULL)
		SetSampler(Sampler);
	else
		SetSampler(ZEGRSampler::GetDefaultSampler());
}

ZERNMap::~ZERNMap()
{
	Clean();
}
