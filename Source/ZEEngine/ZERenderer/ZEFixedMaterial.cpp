//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFixedMaterial.cpp
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
#include "ZEML/ZEMLNode.h"
#include "ZEML/ZEMLItem.h"
#include "ZEML/ZEMLNode.h"
#include "ZERenderStage.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEMatrix.h"
#include "ZEFixedMaterial.h"
#include "ZEML/ZEMLProperty.h"
#include "ZEGraphics/ZEShader.h"
#include "ZEMaterialComponents.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZETextureCube.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETextureCubeResource.h"
#include "ZEGraphics/ZEShaderCompileOptions.h"

#include <Memory.h>
#include "ZECore/ZECore.h"

struct FixedMaterialVertexShaderCommonData
{
	ZEMatrix4x4	WorldMatrix;
	ZEMatrix4x4	WorldViewMatrix;
	ZEMatrix4x4	WorldViewProjMatrix;
	ZEMatrix4x4	WorldViewInvTrpsMatrix;
	ZEVector4	CameraPosition;
	ZEUInt32	EnableSkin;
	//float		Dummy[3];
	//ZEMatrix4x4	BoneMatrices[58];
};

struct FixedMaterialGBufferPassPixelShaderData
{
	float		Opacity;
	float		AlphaCullLimit;
	ZEVector2	DetailMapTiling;
	float		DetailDistance;
	float		DetailFadeDistance;
	float		SpecularShininess;
	float		SubSurfaceScatteringFactor;
};

struct FixedMaterialForwardPassPixelShaderData
{
	ZEVector2	DetailMapTiling;
	ZEVector2	PixelSize;
	float		Opacity;
	float		AlphaCullLimit;
	float		DetailDistance;
	float		DetailFadeDistance;
	ZEVector3	AmbientColor;
	float		ReflectionFactor;
	ZEVector3	SpecularColor;
	float		RefractionFactor;
	ZEVector3	DiffuseColor;
	float		RefractionIndex;
	ZEVector3	EmmisiveColor;
	float		SubSurfaceScatteringFactor;
	ZEMatrix4x4	InvViewMatrix;
};

ZEFixedMaterial::ZEFixedMaterial()
{
	Recompile = false;
	DetailDistance = 4.0f;
	DetailFadeDistance = 4.0f;
	TwoSided = false;
	Wireframe = false;
	TransparancyMode = ZE_MTM_NONE;
	AlphaCullLimit = 1;
	OpacityComponent = ZE_MOC_CONSTANT;
	Opacity = 1.0f;
	SpecularShininess = 0.1f;
	AmbientFactor = 1.0f;
	DiffuseFactor = 1.0f;
	SpecularFactor = 1.0f;
	EmissiveFactor = 1.0f;
	DistortionFactor = 1.0f;
	AmbientColor = ZEVector3::Zero;
	DiffuseColor = ZEVector3::One;
	SpecularColor = ZEVector3::Zero;
	EmissiveColor = ZEVector3::Zero;
	MaterialAmbientColor = ZEVector3::Zero;
	MaterialSpecularColor = ZEVector3::Zero;
	MaterialEmissiveColor = ZEVector3::Zero;
	MaterialDiffuseColor = ZEVector3::Zero;
	SubSurfaceScatteringFactor = 0.0f;
	SkinTransform = false;
	ShadowReciver = false;
	
	AdressingModes.BaseMapU = ZE_TAM_WRAP;
	AdressingModes.BaseMapV = ZE_TAM_WRAP;
	AdressingModes.DetailBaseMapU = ZE_TAM_WRAP;
	AdressingModes.DetailBaseMapV = ZE_TAM_WRAP;
	AdressingModes.DetailNormalMapU = ZE_TAM_WRAP;
	AdressingModes.DetailBaseMapV = ZE_TAM_WRAP;
	AdressingModes.DetailNormalMapU = ZE_TAM_WRAP;
	AdressingModes.DetailNormalMapV = ZE_TAM_WRAP;
	AdressingModes.DistortionMapU = ZE_TAM_WRAP;
	AdressingModes.DistortionMapV = ZE_TAM_WRAP;
	AdressingModes.EmissiveMapU = ZE_TAM_WRAP;
	AdressingModes.EmissiveMapV = ZE_TAM_WRAP;
	AdressingModes.EnvironmentMapU = ZE_TAM_WRAP;
	AdressingModes.EnvironmentMapV = ZE_TAM_WRAP;
	AdressingModes.EnvironmentMapW = ZE_TAM_WRAP;
	AdressingModes.LightMapU = ZE_TAM_WRAP;
	AdressingModes.LightMapV = ZE_TAM_WRAP;
	AdressingModes.NormalMapU = ZE_TAM_WRAP;
	AdressingModes.NormalMapV = ZE_TAM_WRAP;
	AdressingModes.OpacityMapU = ZE_TAM_WRAP;
	AdressingModes.OpacityMapV = ZE_TAM_WRAP;
	AdressingModes.ParallaxMapU = ZE_TAM_WRAP;
	AdressingModes.ParallaxMapV = ZE_TAM_WRAP;
	AdressingModes.SpecularMapU = ZE_TAM_WRAP;
	AdressingModes.SpecularMapV = ZE_TAM_WRAP;
	
	memset((void*)&Components, 0, sizeof (ZEFixedMaterialComponents));
	memset((void*)&Shaders, 0, sizeof (ZEFixedMaterialShaders));
	memset((void*)&Textures, 0, sizeof(ZEFixedMaterialTextures));
	memset((void*)&Resources, 0, sizeof (ZEFixedMaterialResources));
	memset((void*)&Buffers, 0, sizeof (ZEFixedMaterialBuffers));
}

ZEFixedMaterial::~ZEFixedMaterial()
{
	DestroyShaders();
}

void ZEFixedMaterial::CreateShaders()
{
	DestroyShaders();

	ZEShaderCompileOptions Options;
	Options.Model = ZE_SM_4_0;
	Options.FileName = "FixedMaterial.hlsl";

	ZEShaderCompilerParameter TempParam;
	
	// Create parameter  list
	if (Components.Ambient)
	{
		TempParam.Name = "ZE_SHADER_AMBIENT";
		Options.Parameters.Add(TempParam);
	}
	if (Components.Diffuse)
	{
		TempParam.Name = "ZE_SHADER_DIFFUSE";
		Options.Parameters.Add(TempParam);
	}
	if (Components.BaseMap)
	{
		TempParam.Name = "ZE_SHADER_BASE_MAP";
		Options.Parameters.Add(TempParam);
	}
	if (Components.NormalMap)
	{
		TempParam.Name = "ZE_SHADER_NORMAL_MAP";
		Options.Parameters.Add(TempParam);
	}
	if (Components.ParallaxMap)
	{
		TempParam.Name = "ZE_SHADER_PARALLAX_MAP";
		Options.Parameters.Add(TempParam);
	}
	if (Components.Specular)
	{
		TempParam.Name = "ZE_SHADER_SPECULAR";
		Options.Parameters.Add(TempParam);
	}
	if (Components.SpecularMap)
	{
		TempParam.Name = "ZE_SHADER_SPECULAR_MAP";
		Options.Parameters.Add(TempParam);
	}
	if (Components.Emissive)
	{
		TempParam.Name = "ZE_SHADER_EMMISIVE";
		Options.Parameters.Add(TempParam);
	}
	if (Components.EmissiveMap)
	{
		TempParam.Name = "ZE_SHADER_EMMISIVE_MAP";
		Options.Parameters.Add(TempParam);
	}
	if (Components.Opacity)
	{
		TempParam.Name = "ZE_SHADER_OPACITY";
		Options.Parameters.Add(TempParam);
	}
	if (Components.OpacityBaseAlpha)
	{
		TempParam.Name = "ZE_SHADER_OPACITY_BASE_ALPHA";
		Options.Parameters.Add(TempParam);
	}
	if (Components.OpacityConstant)
	{
		TempParam.Name = "ZE_SHADER_OPACITY_CONSTANT";
		Options.Parameters.Add(TempParam);
	}
	if (Components.OpacityMap)
	{
		TempParam.Name = "ZE_SHADER_OPACITY_MAP";
		Options.Parameters.Add(TempParam);
	}
	if (Components.DetailMap)
	{
		TempParam.Name = "ZE_SHADER_DETAIL_MAP";
		Options.Parameters.Add(TempParam);
	}
	if (Components.DetailBaseMap)
	{
		TempParam.Name = "ZE_SHADER_DETAIL_BASE_MAP";
		Options.Parameters.Add(TempParam);
	}
	if (Components.DetailNormalMap)
	{
		TempParam.Name = "ZE_SHADER_DETAIL_NORMAL_MAP";
		Options.Parameters.Add(TempParam);
	}
	if (Components.Reflection)
	{
		TempParam.Name = "ZE_SHADER_REFLECTION";
		Options.Parameters.Add(TempParam);
	}	
	if (Components.Refraction)
	{
		TempParam.Name = "ZE_SHADER_REFRACTION";
		Options.Parameters.Add(TempParam);
	}	
	if (Components.LightMap)
	{
		TempParam.Name = "ZE_SHADER_LIGHT_MAP";
		Options.Parameters.Add(TempParam);
	}
	if (Components.DistortionMap)
	{
		TempParam.Name = "ZE_SHADER_DISTORTION_MAP";
		Options.Parameters.Add(TempParam);
	}	
	if (Components.AlphaCull)
	{
		TempParam.Name = "ZE_SHADER_ALPHA_CULL";
		Options.Parameters.Add(TempParam);
	}
	if (Components.VertexColor)
	{
		TempParam.Name = "ZE_SHADER_VERTEX_COLOR";
		Options.Parameters.Add(TempParam);
	}

// 	if (TransparancyMode == ZE_MTM_NONE)
// 	{	
		Options.Type = ZE_ST_VERTEX;
		Options.EntryPoint = "ZEFixedMaterial_GBuffer_VertexShader";
		Shaders.GBufferPassVertexShader = ZEShaderCompiler::CompileShaderFromFile(&Options);
		
		Buffers.GBufferPassVertexShader = ZEConstantBuffer::CreateInstance();
		Buffers.GBufferPassVertexShader->Create(Shaders.GBufferPassVertexShader->GetMetaTable()->GetBufferInfo("CommonVSData"));

		Options.Type = ZE_ST_PIXEL;
		Options.EntryPoint = "ZEFixedMaterial_GBuffer_PixelShader";
		Shaders.GBufferPassPixelShader = ZEShaderCompiler::CompileShaderFromFile(&Options);

		Buffers.GBufferPassPixelShader = ZEConstantBuffer::CreateInstance();
		Buffers.GBufferPassPixelShader->Create(Shaders.GBufferPassPixelShader->GetMetaTable()->GetBufferInfo("GPassPixelShaderData"));
// 	}
// 	else
// 	{
// 		Shaders.GBufferPassVertexShader = NULL;
// 		Shaders.GBufferPassPixelShader = NULL;
// 
// 		Buffers.GBufferPassVertexShader = NULL;
// 		Buffers.GBufferPassPixelShader = NULL;
// 	}
	
	Options.Type = ZE_ST_VERTEX;
	Options.EntryPoint = "ZEFixedMaterial_ForwardPass_VertexShader";
	Shaders.ForwardPassVertexShader = ZEShaderCompiler::CompileShaderFromFile(&Options);

	Buffers.ForwardPassVertexShader = ZEConstantBuffer::CreateInstance();
	Buffers.ForwardPassVertexShader->Create(Shaders.ForwardPassVertexShader->GetMetaTable()->GetBufferInfo("CommonVSData"));

	Options.Type = ZE_ST_PIXEL;
	Options.EntryPoint = "ZEFixedMaterial_ForwardPass_PixelShader";
	Shaders.ForwardPassPixelShader = ZEShaderCompiler::CompileShaderFromFile(&Options);
	
	Buffers.ForwardPassPixelShader = ZEConstantBuffer::CreateInstance();
	Buffers.ForwardPassPixelShader->Create(Shaders.ForwardPassPixelShader->GetMetaTable()->GetBufferInfo("ForwardPassPixelShaderData"));

	// No shadows for now
	Shaders.ShadowPassPixelShader = NULL;
	Shaders.ShadowPassVertexShader = NULL;
	Buffers.ShadowPassPixelShader = NULL;
	Buffers.ShadowPassVertexShader = NULL;

	Recompile = false;
}

void ZEFixedMaterial::DestroyShaders()
{
	ZE_DESTROY(Shaders.GBufferPassVertexShader);
	ZE_DESTROY(Shaders.GBufferPassPixelShader);
	ZE_DESTROY(Shaders.ForwardPassVertexShader);
	ZE_DESTROY(Shaders.ForwardPassPixelShader);
	ZE_DESTROY(Shaders.ShadowPassVertexShader);
	ZE_DESTROY(Shaders.ShadowPassPixelShader);
	ZE_DESTROY(Buffers.GBufferPassVertexShader);
	ZE_DESTROY(Buffers.GBufferPassPixelShader);
	ZE_DESTROY(Buffers.ForwardPassVertexShader);
	ZE_DESTROY(Buffers.ForwardPassPixelShader);
	ZE_DESTROY(Buffers.ShadowPassVertexShader);
	ZE_DESTROY(Buffers.ShadowPassPixelShader);
}

ZEUInt32 ZEFixedMaterial::GetHash() const
{
	return *((ZEUInt32*)&Components);
}

ZEMaterialFlags ZEFixedMaterial::GetMaterialFlags() const
{
	return ZE_MTF_G_BUFFER_PASS | ZE_MTF_FORWARD_PASS | ZE_MTF_SUPPORTS_SKINNING;
}

void ZEFixedMaterial::SetTwoSided(bool Enable)
{
	TwoSided = Enable;
}

bool ZEFixedMaterial::GetTwoSided() const
{
	return TwoSided;
}

void ZEFixedMaterial::SetWireframe(bool Enable)
{
	Wireframe = Enable;
}

bool ZEFixedMaterial::GetWireframe() const
{
	return Wireframe;
}

void ZEFixedMaterial::SetTransparancyMode(ZEMaterialTransparancyMode Mode)
{
	TransparancyMode = Mode;
}

ZEMaterialTransparancyMode ZEFixedMaterial::GetTransparancyMode() const
{
	return TransparancyMode;
}

void ZEFixedMaterial::SetAlphaCullEnabled(bool Value)
{
	if (Components.AlphaCull != Value)
	{
		Components.AlphaCull = Value;
		Recompile = true;
	}
}

bool ZEFixedMaterial::GetAlphaCullEnabled() const
{
	return Components.AlphaCull;
}

void ZEFixedMaterial::SetAlphaCullLimit(float Limit)
{
	AlphaCullLimit = Limit;
}

float ZEFixedMaterial::GetAlphaCullLimit() const
{
	return AlphaCullLimit;
}

void ZEFixedMaterial::SetAmbientEnabled(bool Value)
{
	if (Components.Ambient != Value)
	{
		Components.Ambient = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetAmbientEnabled() const
{
	return Components.Ambient;
}

void ZEFixedMaterial::SetAmbientColor(const ZEVector3& Color)
{
	AmbientColor = Color;
	MaterialAmbientColor = AmbientColor * AmbientFactor;
}

const ZEVector3& ZEFixedMaterial::GetAmbientColor() const
{
	return AmbientColor;
}

void ZEFixedMaterial::SetAmbientFactor(float Factor)
{
	AmbientFactor = Factor;
	MaterialAmbientColor = AmbientColor * AmbientFactor;
}

float ZEFixedMaterial::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZEFixedMaterial::SetDiffuseEnabled(bool Value)
{
	if (Components.Diffuse != Value)
	{
		Components.Diffuse = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetDiffuseEnabled() const
{
	return Components.Diffuse;
}

void ZEFixedMaterial::SetDiffuseColor(const ZEVector3& Color)
{
	DiffuseColor = Color;
	MaterialDiffuseColor = DiffuseColor * DiffuseFactor;
}

const ZEVector3& ZEFixedMaterial::GetDiffuseColor() const
{
	return DiffuseColor;
}

void ZEFixedMaterial::SetDiffuseFactor(float Factor)
{
	DiffuseFactor = Factor;
	MaterialDiffuseColor = DiffuseColor * DiffuseFactor;
}

float ZEFixedMaterial::GetDiffuseFactor() const
{
	return DiffuseFactor;
}

void ZEFixedMaterial::SetDiffuseSubSurfaceScatteringFactor(float Factor)
{
	SubSurfaceScatteringFactor = Factor;
}

float ZEFixedMaterial::GetDiffuseSubSurfaceScatteringFactor()
{
	return SubSurfaceScatteringFactor;
}

void ZEFixedMaterial::SetBaseMapEnabled(bool Value)
{
	if (Components.BaseMap != Value)
	{
		Components.BaseMap = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetBaseMapEnabled() const
{
	return Components.BaseMap;
}

void ZEFixedMaterial::SetBaseMap(ZETexture2D* Texture)
{
	if (Resources.BaseMap != NULL)
	{
		Resources.BaseMap->Release();
		Resources.BaseMap = NULL;
	}

	Textures.BaseMap = Texture;
}

ZETexture2D* ZEFixedMaterial::GetBaseMap() const
{
	return Textures.BaseMap;
}

void ZEFixedMaterial::SetBaseMapFile(const char* Filename)
{
	if (Resources.BaseMap != NULL)
	{
		Resources.BaseMap->Release();
		Resources.BaseMap = NULL;
	}

	ZETexture2DResource* Resource = ZETexture2DResource::LoadSharedResource(Filename);
	
	if (Resource != NULL)
	{
		Resources.BaseMap = Resource;
		Textures.BaseMap = Resource->GetTexture();
	}
	else
	{
		Textures.BaseMap = NULL;
	}
}

const char* ZEFixedMaterial::GetBaseMapFile() const
{
	if (Resources.BaseMap != NULL)
		return Resources.BaseMap->GetFileName().ToCString();
	else
		return "";
}

void ZEFixedMaterial::SetBaseMapAddressModeU(ZETextureAddressMode Mode)
{
	AdressingModes.BaseMapU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetBaseMapAddressModeU() const
{
	return AdressingModes.BaseMapU;
}

void ZEFixedMaterial::SetBaseMapAddressModeV(ZETextureAddressMode Mode)
{
	AdressingModes.BaseMapV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetBaseMapAddressModeV() const
{
	return AdressingModes.BaseMapV;
}

void ZEFixedMaterial::SetSpecularEnabled(bool Value)
{
	if (Components.Specular != Value)
	{
		Components.Specular = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetSpecularEnabled() const
{
	return Components.Specular;
}

void ZEFixedMaterial::SetSpecularColor(const ZEVector3& Color)
{
	SpecularColor = Color;
	MaterialSpecularColor = SpecularColor * SpecularFactor;
}

const ZEVector3& ZEFixedMaterial::GetSpecularColor() const
{
	return SpecularColor;
}

void ZEFixedMaterial::SetSpecularFactor(float Factor)
{
	SpecularFactor = Factor;
	MaterialSpecularColor = SpecularColor * SpecularFactor;
}

float ZEFixedMaterial::GetSpecularFactor() const
{
	return SpecularFactor;
}

void ZEFixedMaterial::SetSpecularShininess(float Shininess)
{
	SpecularShininess = Shininess;
}

float ZEFixedMaterial::GetSpecularShininess() const
{
	return SpecularShininess;
}

void ZEFixedMaterial::SetSpecularMapEnabled(bool Value)
{
	if (Components.SpecularMap != Value)
	{
		Components.SpecularMap = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetSpecularMapEnabled() const
{
	return Components.SpecularMap;
}

void ZEFixedMaterial::SetSpecularMap(ZETexture2D* Texture)
{
	if (Resources.SpecularMap != NULL)
	{
		Resources.SpecularMap->Release();
		Resources.SpecularMap = NULL;
	}

	Textures.SpecularMap = Texture;
}

ZETexture2D* ZEFixedMaterial::GetSpecularMap() const
{
	return Textures.SpecularMap;
}

void ZEFixedMaterial::SetSpecularMapFile(const char* Filename)
{
	if (Resources.SpecularMap != NULL)
	{
		Resources.SpecularMap->Release();
		Resources.SpecularMap = NULL;
	}

	ZETexture2DResource* Resource = ZETexture2DResource::LoadSharedResource(Filename);
	
	if (Resource != NULL)
	{
		Resources.SpecularMap = Resource;
		Textures.SpecularMap = Resource->GetTexture();
	}
	else
	{
		Textures.SpecularMap = NULL;
	}
}

const char* ZEFixedMaterial::GetSpecularMapFile() const
{
	if (Resources.SpecularMap != NULL)
		return Resources.SpecularMap->GetFileName();
	else
		return "";
}

void ZEFixedMaterial::SetSpecularMapAddressModeU(ZETextureAddressMode Mode)
{
	AdressingModes.SpecularMapU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetSpecularMapAddressModeU() const
{
	return AdressingModes.SpecularMapU;
}

void ZEFixedMaterial::SetSpecularMapAddressModeV(ZETextureAddressMode Mode)
{
	AdressingModes.SpecularMapV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetSpecularMapAddressModeV() const
{
	return AdressingModes.SpecularMapV;
}

void ZEFixedMaterial::SetEmissiveEnabled(bool Value)
{
	if (Components.Emissive != Value)
	{
		Components.Emissive = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetEmissiveEnabled() const
{
	return Components.Emissive;
}

void ZEFixedMaterial::SetEmissiveFactor(float Factor)
{
	EmissiveFactor = Factor;
	MaterialEmissiveColor = EmissiveColor * EmissiveFactor;
}

float ZEFixedMaterial::GetEmissiveFactor() const
{
	return EmissiveFactor;
}

void ZEFixedMaterial::SetEmissiveColor(const ZEVector3& Color)
{
	EmissiveColor = Color;
	MaterialEmissiveColor = EmissiveColor * EmissiveFactor;
}

const ZEVector3& ZEFixedMaterial::GetEmissiveColor() const
{
	return EmissiveColor;
}

void ZEFixedMaterial::SetEmissiveMapEnabled(bool Value)
{
	if (Components.EmissiveMap != Value)
	{
		Components.EmissiveMap = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetEmissiveMapEnabled() const
{
	return Components.EmissiveMap;
}

void ZEFixedMaterial::SetEmissiveMap(ZETexture2D* Texture)
{
	if (Resources.EmissiveMap != NULL)
	{
		Resources.EmissiveMap->Release();
		Resources.EmissiveMap = NULL;
	}

	Textures.EmissiveMap = Texture;
}

ZETexture2D* ZEFixedMaterial::GetEmissiveMap() const
{
	return Textures.EmissiveMap;
}

void ZEFixedMaterial::SetEmissiveMapFile(const char* Filename)
{
	if (Resources.EmissiveMap != NULL)
	{
		Resources.EmissiveMap->Release();
		Resources.EmissiveMap = NULL;
	}

	ZETexture2DResource* Resource = ZETexture2DResource::LoadSharedResource(Filename);
	
	if (Resource != NULL)
	{
		Resources.EmissiveMap = Resource;
		Textures.EmissiveMap = Resource->GetTexture();
	}
	else
	{
		Textures.EmissiveMap = NULL;
	}
}

const char* ZEFixedMaterial::GetEmissiveMapFile() const
{
	if (Resources.EmissiveMap != NULL)
		return Resources.EmissiveMap->GetFileName();
	else
		return "";
}

void ZEFixedMaterial::SetEmissiveMapAddressModeU(ZETextureAddressMode Mode)
{
	AdressingModes.EmissiveMapU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetEmissiveMapAddressModeU() const
{
	return AdressingModes.EmissiveMapU;
}

void ZEFixedMaterial::SetEmissiveMapAddressModeV(ZETextureAddressMode Mode)
{
	AdressingModes.EmissiveMapV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetEmissiveMapAddressModeV() const
{
	return AdressingModes.EmissiveMapV;
}

void ZEFixedMaterial::SetNormalMapEnabled(bool Value)
{
	if (Components.NormalMap != Value)
	{
		Components.NormalMap = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetNormalMapEnabled() const
{
	return Components.NormalMap;
}

void ZEFixedMaterial::SetNormalMap(ZETexture2D* Texture)
{
	if (Resources.NormalMap != NULL)
	{
		Resources.NormalMap->Release();
		Resources.NormalMap = NULL;
	}

	Textures.NormalMap = Texture;
}

ZETexture2D* ZEFixedMaterial::GetNormalMap() const
{
	return Textures.NormalMap;
}

void ZEFixedMaterial::SetNormalMapFile(const char* Filename)
{
	if (Resources.NormalMap != NULL)
	{
		Resources.NormalMap->Release();
		Resources.NormalMap = NULL;
	}

	ZETexture2DResource* Resource = ZETexture2DResource::LoadSharedResource(Filename);
	
	if (Resource != NULL)
	{
		Resources.NormalMap = Resource;
		Textures.NormalMap = Resource->GetTexture();
	}
	else
	{
		Textures.NormalMap = NULL;
	}
}

const char* ZEFixedMaterial::GetNormalMapFile() const
{
	if (Resources.NormalMap != NULL)
		return Resources.NormalMap->GetFileName();
	else
		return "";
}

void ZEFixedMaterial::SetNormalMapAddressModeU(ZETextureAddressMode Mode)
{
	AdressingModes.NormalMapU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetNormalMapAddressModeU() const
{
	return AdressingModes.NormalMapU;
}

void ZEFixedMaterial::SetNormalMapAddressModeV(ZETextureAddressMode Mode)
{
	AdressingModes.NormalMapV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetNormalMapAddressModeV() const
{
	return AdressingModes.NormalMapV;
}

void ZEFixedMaterial::SetParallaxMapEnabled(bool Value)
{
	if (Components.ParallaxMap != Value)
	{
		Components.ParallaxMap = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetParallaxMapEnabled() const
{
	return Components.ParallaxMap;
}

void ZEFixedMaterial::SetParallaxMap(ZETexture2D* Texture)
{
	if (Resources.ParallaxMap != NULL)
	{
		Resources.ParallaxMap->Release();
		Resources.ParallaxMap = NULL;
	}

	Textures.ParallaxMap = Texture;
}

void ZEFixedMaterial::SetParallaxMapFile(const char* Filename)
{
	if (Resources.ParallaxMap != NULL)
	{
		Resources.ParallaxMap->Release();
		Resources.ParallaxMap = NULL;
	}

	ZETexture2DResource* Resource = ZETexture2DResource::LoadSharedResource(Filename);
	
	if (Resource != NULL)
	{
		Resources.ParallaxMap = Resource;
		Textures.ParallaxMap = Resource->GetTexture();
	}
	else
	{
		Textures.ParallaxMap = NULL;
	}
}

const char* ZEFixedMaterial::GetParallaxMapFile() const
{
	if (Resources.ParallaxMap != NULL)
		return Resources.ParallaxMap->GetFileName();
	else
		return "";
}

ZETexture2D* ZEFixedMaterial::GetParallaxMap() const
{
	return Textures.ParallaxMap;
}

void ZEFixedMaterial::SetParallaxMapAddressModeU(ZETextureAddressMode Mode)
{
	AdressingModes.ParallaxMapU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetParallaxMapAddressModeU() const
{
	return AdressingModes.ParallaxMapU;
}

void ZEFixedMaterial::SetParallaxMapAddressModeV(ZETextureAddressMode Mode)
{
	AdressingModes.ParallaxMapV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetParallaxMapAddressModeV() const
{
	return AdressingModes.ParallaxMapV;
}

void ZEFixedMaterial::SetOpacityEnabled(bool Value)
{
	if (Components.Opacity != Value)
	{
		Components.Opacity = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetOpacityEnabled() const
{
	return Components.Opacity;
}

void ZEFixedMaterial::SetOpacity(float Value)
{
	Opacity = Value;
}

float ZEFixedMaterial::GetOpacity() const
{
	return Opacity;
}

void ZEFixedMaterial::SetOpacityComponent(ZEMaterialOpacityComponent Component)
{
	Components.OpacityMap = false;
	Components.OpacityConstant = false;
	Components.OpacityBaseAlpha = false;

	switch(Component)
	{
		case ZE_MOC_BASE_MAP_ALPHA:
			OpacityComponent = ZE_MOC_BASE_MAP_ALPHA;
			Components.OpacityBaseAlpha = true;
			Recompile = true;
			break;
		default:
		case ZE_MOC_CONSTANT:
			OpacityComponent = ZE_MOC_CONSTANT;
			Components.OpacityConstant = true;
			Recompile = true;
			break;
		case ZE_MOC_OPACITY_MAP:
			OpacityComponent = ZE_MOC_OPACITY_MAP;
			Components.OpacityMap = true;
			Recompile = true;
			break;
	}
}

ZEMaterialOpacityComponent ZEFixedMaterial::GetOpacityComponent() const
{
	return OpacityComponent;
}

void ZEFixedMaterial::SetOpacityMapEnabled(bool Value)
{
	if (Components.OpacityMap != Value)
	{
		Components.OpacityMap = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetOpacityMapEnabled() const
{
	return Components.OpacityMap;
}

void ZEFixedMaterial::SetOpacityMap(ZETexture2D* Texture)
{
	if (Resources.OpacityMap != NULL)
	{
		Resources.OpacityMap->Release();
		Resources.OpacityMap = NULL;
	}

	Textures.OpacityMap = Texture;
}

ZETexture2D* ZEFixedMaterial::GetOpacityMap() const
{
	return Textures.OpacityMap;
}

void ZEFixedMaterial::SetOpacityMapFile(const char* Filename)
{
	if (Resources.OpacityMap != NULL)
	{
		Resources.OpacityMap->Release();
		Resources.OpacityMap = NULL;
	}

	ZETexture2DResource* Resource = ZETexture2DResource::LoadSharedResource(Filename);
	
	if (Resource != NULL)
	{
		Resources.OpacityMap = Resource;
		Textures.OpacityMap = Resource->GetTexture();
	}
	else
	{
		Textures.OpacityMap = NULL;
	}
}

const char* ZEFixedMaterial::GetOpacityMapFile() const
{
	if (Resources.OpacityMap != NULL)
		return Resources.OpacityMap->GetFileName();
	else
		return "";
}

void ZEFixedMaterial::SetOpacityMapAddressModeU(ZETextureAddressMode Mode)
{
	AdressingModes.OpacityMapU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetOpacityMapAddressModeU() const
{
	return AdressingModes.OpacityMapU;
}

void ZEFixedMaterial::SetOpacityMapAddressModeV(ZETextureAddressMode Mode)
{
	AdressingModes.OpacityMapV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetOpacityMapAddressModeV() const
{
	return AdressingModes.OpacityMapV;
}


void ZEFixedMaterial::SetEnvironmentMap(ZETextureCube* Texture)
{
	if (Resources.EnvironmentMap != NULL)
	{
		Resources.EnvironmentMap->Release();
		Resources.EnvironmentMap = NULL;
	}

	Textures.EnvironmentMap = Texture;
}

ZETextureCube* ZEFixedMaterial::GetEnvironmentMap() const
{
	return Textures.EnvironmentMap;
}

void ZEFixedMaterial::SetEnvironmentMapFile(const char* Filename)
{
	if (Resources.EnvironmentMap != NULL)
	{
		Resources.EnvironmentMap->Release();
		Resources.EnvironmentMap = NULL;
	}

	ZETextureCubeResource* Resource = ZETextureCubeResource::LoadSharedResource(Filename);
	
	if (Resource != NULL)
	{
		Resources.EnvironmentMap = Resource;
		Textures.EnvironmentMap = Resource->GetTexture();
	}
	else
	{
		Textures.EnvironmentMap = NULL;
	}
}

const char* ZEFixedMaterial::GetEnvironmentMapFile() const
{
	if (Resources.EnvironmentMap != NULL)
		return Resources.EnvironmentMap->GetFileName();
	else
		return "";
}

void ZEFixedMaterial::SetEnvironmentMapAddressModeU(ZETextureAddressMode Mode)
{
	AdressingModes.EnvironmentMapU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetEnvironmentMapAddressModeU() const
{
	return AdressingModes.EnvironmentMapU;
}

void ZEFixedMaterial::SetEnvironmentMapAddressModeV(ZETextureAddressMode Mode)
{
	AdressingModes.EnvironmentMapV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetEnvironmentMapAddressModeV() const
{
	return AdressingModes.EnvironmentMapV;
}

void ZEFixedMaterial::SetEnvironmentMapAddressModeW(ZETextureAddressMode Mode)
{
	AdressingModes.EnvironmentMapW = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetEnvironmentMapAddressModeW() const
{
	return AdressingModes.EnvironmentMapW;
}

void ZEFixedMaterial::SetReflectionEnabled(bool Value)
{
	if (Components.Reflection != Value)
	{
		Components.Reflection = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetReflectionEnabled() const
{
	return Components.Reflection;
}

void ZEFixedMaterial::SetReflectionFactor(float Factor)
{
	ReflectionFactor = Factor;
}

float ZEFixedMaterial::GetReflectionFactor() const
{
	return ReflectionFactor;
}

void ZEFixedMaterial::SetRefractionEnabled(bool Value)
{
	if (Components.Refraction != Value)
	{
		Components.Refraction = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetRefractionEnabled() const
{
	return Components.Refraction;
}

void ZEFixedMaterial::SetRefractionIndex(float Index)
{
	RefractionIndex = Index;
}

float ZEFixedMaterial::GetRefractionIndex() const
{
	return RefractionIndex;
}

void ZEFixedMaterial::SetRefractionFactor(float Factor)
{
	RefractionFactor = Factor;
}

float ZEFixedMaterial::GetRefractionFactor() const
{
	return RefractionFactor;
}

void ZEFixedMaterial::SetDetailMapTiling(const ZEVector2& Tiling)
{
	DetailMapTiling = Tiling;
}

const ZEVector2& ZEFixedMaterial::GetDetailMapTiling() const
{
	return DetailMapTiling;
}

void ZEFixedMaterial::SetDetailDistance(float Factor)
{
	DetailDistance = Factor;
}

float ZEFixedMaterial::GetDetailDistance() const
{
	return DetailDistance;
}

void ZEFixedMaterial::SetDetailFadeDistance(float Factor)
{
	DetailFadeDistance = Factor;
}

float ZEFixedMaterial::GetDetailFadeDistance() const
{
	return DetailFadeDistance;
}

void ZEFixedMaterial::SetDetailBaseMapEnabled(bool Value)
{
	if (Components.DetailBaseMap != Value)
	{
		Components.DetailBaseMap = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetDetailBaseMapEnabled() const
{
	return Components.DetailBaseMap;
}

void ZEFixedMaterial::SetDetailBaseMap(ZETexture2D* Texture)
{
	if (Resources.DetailBaseMap != NULL)
	{
		Resources.DetailBaseMap->Release();
		Resources.DetailBaseMap = NULL;
	}

	Textures.DetailBaseMap = Texture;
}

ZETexture2D* ZEFixedMaterial::GetDetailBaseMap() const
{
	return Textures.DetailBaseMap;
}

void ZEFixedMaterial::SetDetailBaseMapFile(const char* Filename)
{
	if (Resources.DetailBaseMap != NULL)
	{
		Resources.DetailBaseMap->Release();
		Resources.DetailBaseMap = NULL;
	}

	ZETexture2DResource* Resource = ZETexture2DResource::LoadSharedResource(Filename);
	
	if (Resource != NULL)
	{
		Resources.DetailBaseMap = Resource;
		Textures.DetailBaseMap = Resource->GetTexture();
	}
	else
	{
		Textures.DetailBaseMap = NULL;
	}
}

const char* ZEFixedMaterial::GetDetailBaseMapFile() const
{
	if (Resources.DetailBaseMap != NULL)
		return Resources.DetailBaseMap->GetFileName();
	else
		return "";
}

void ZEFixedMaterial::SetDetailBaseMapAddressModeU(ZETextureAddressMode Mode)
{
	AdressingModes.DetailBaseMapU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailBaseMapAddressModeU() const
{
	return AdressingModes.DetailBaseMapU;
}

void ZEFixedMaterial::SetDetailBaseMapAddressModeV(ZETextureAddressMode Mode)
{
	AdressingModes.DetailBaseMapV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailBaseMapAddressModeV() const
{
	return AdressingModes.DetailBaseMapV;
}

void ZEFixedMaterial::SetDetailNormalMapEnabled(bool Value)
{
	if (Components.DetailNormalMap != Value)
	{
		Components.DetailNormalMap = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetDetailNormalMapEnabled() const
{
	return Components.DetailNormalMap;
}

void ZEFixedMaterial::SetDetailNormalMap(ZETexture2D* Texture)
{
	if (Resources.DetailNormalMap != NULL)
	{
		Resources.DetailNormalMap->Release();
		Resources.DetailNormalMap = NULL;
	}

	Textures.DetailNormalMap = Texture;
}

ZETexture2D* ZEFixedMaterial::GetDetailNormalMap() const
{
	return Textures.DetailNormalMap;
}

void ZEFixedMaterial::SetDetailNormalMapFile(const char* Filename)
{
	if (Resources.DetailNormalMap != NULL)
	{
		Resources.DetailNormalMap->Release();
		Resources.DetailNormalMap = NULL;
	}

	ZETexture2DResource* Resource = ZETexture2DResource::LoadSharedResource(Filename);
	
	if (Resource != NULL)
	{
		Resources.DetailNormalMap = Resource;
		Textures.DetailNormalMap = Resource->GetTexture();
	}
	else
	{
		Textures.DetailNormalMap = NULL;
	}
}

const char* ZEFixedMaterial::GetDetailNormalMapFile() const
{
	if (Resources.DetailNormalMap != NULL)
		return Resources.DetailNormalMap->GetFileName();
	else
		return "";
}

void ZEFixedMaterial::SetDetailNormalMapAddressModeU(ZETextureAddressMode Mode)
{
	AdressingModes.DetailNormalMapU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailNormalMapAddressModeU() const
{
	return AdressingModes.DetailNormalMapU;
}

void ZEFixedMaterial::SetDetailNormalMapAddressModeV(ZETextureAddressMode Mode)
{
	AdressingModes.DetailNormalMapV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDetailNormalMapAddressModeV() const
{
	return AdressingModes.DetailNormalMapV;
}

void ZEFixedMaterial::SetLightMapEnabled(bool Value)
{
	if (Components.LightMap != Value)
	{
		Components.LightMap = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetLightMapEnabled() const
{
	return Components.LightMap;
}

void ZEFixedMaterial::SetLightMapFactor(float Factor)
{
	LightMapFactor = Factor;
}

float ZEFixedMaterial::GetLightMapFactor() const
{
	return LightMapFactor;
}

void ZEFixedMaterial::SetLightMap(ZETexture2D* Texture)
{
	if (Resources.LightMap != NULL)
	{
		Resources.LightMap->Release();
		Resources.LightMap = NULL;
	}

	Textures.LightMap = Texture;
}

ZETexture2D* ZEFixedMaterial::GetLightMap() const
{
	return Textures.LightMap;
}

void ZEFixedMaterial::SetLightMapFile(const char* Filename)
{
	if (Resources.LightMap != NULL)
	{
		Resources.LightMap->Release();
		Resources.LightMap = NULL;
	}

	ZETexture2DResource* Resource = ZETexture2DResource::LoadSharedResource(Filename);
	
	if (Resource != NULL)
	{
		Resources.LightMap = Resource;
		Textures.LightMap = Resource->GetTexture();
	}
	else
	{
		Textures.LightMap = NULL;
	}
}

const char* ZEFixedMaterial::GetLightMapFile() const
{
	if (Resources.LightMap != NULL)
		return Resources.LightMap->GetFileName();
	else
		return "";
}

void ZEFixedMaterial::SetLightMapAddressModeU(ZETextureAddressMode Mode)
{
	AdressingModes.LightMapU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetLightMapAddressModeU() const
{
	return 	AdressingModes.LightMapU;
}

void ZEFixedMaterial::SetLightMapAddressModeV(ZETextureAddressMode Mode)
{
		AdressingModes.LightMapV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetLightMapAddressModeV() const
{
	return AdressingModes.LightMapV;
}

void ZEFixedMaterial::SetDistortionEnabled(bool Value)
{
	if (Components.DistortionMap != Value)
	{
		Components.DistortionMap = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetDistortionEnabled() const
{
	return Components.DistortionMap;
}

void ZEFixedMaterial::SetDistortionFactor(float Factor)
{
	DistortionFactor = Factor;
}

float ZEFixedMaterial::GetDistortionFactor() const
{
	return DistortionFactor;
}

void ZEFixedMaterial::SetDistortionMap(ZETexture2D* Texture)
{
	if (Resources.DistortionMap != NULL)
	{
		Resources.DistortionMap->Release();
		Resources.DistortionMap = NULL;
	}

	Textures.DistortionMap = Texture;
}

ZETexture2D* ZEFixedMaterial::GetDistortionMap() const
{
	return Textures.DistortionMap; 
}

void ZEFixedMaterial::SetDistortionMapFile(const char* Filename)
{
	if (Resources.DistortionMap != NULL)
	{
		Resources.DistortionMap->Release();
		Resources.DistortionMap = NULL;
	}

	ZETexture2DResource* Resource = ZETexture2DResource::LoadSharedResource(Filename);
	
	if (Resource != NULL)
	{
		Resources.DistortionMap = Resource;
		Textures.DistortionMap = Resource->GetTexture();
	}
	else
	{
		Textures.DistortionMap = NULL;
	}
}

const char* ZEFixedMaterial::GetDistortionMapFile() const
{
	if (Resources.DistortionMap != NULL)
		return Resources.DistortionMap->GetFileName();
	else
		return "";
}

void ZEFixedMaterial::SetDistortionMapAddressModeU(ZETextureAddressMode Mode)
{
	AdressingModes.DistortionMapU = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDistortionMapAddressModeU() const
{
	return AdressingModes.DistortionMapU;
}

void ZEFixedMaterial::SetDistortionMapAddressModeV(ZETextureAddressMode Mode)
{
	AdressingModes.DistortionMapV = Mode;
}

ZETextureAddressMode ZEFixedMaterial::GetDistortionMapAddressModeV() const
{
	return AdressingModes.DistortionMapV;
}

void ZEFixedMaterial::SetVertexColorEnabled(bool Value)
{
	if (Components.VertexColor != Value)
	{
		Components.VertexColor = Value;
		Recompile = true;
	} 
}

bool ZEFixedMaterial::GetVertexColorEnabled()
{
	return Components.VertexColor;
}

void ZEFixedMaterial::UpdateTransformations(const ZERenderCommand* RenderCommand)
{
	static ZEUInt LastFrameId = 0;
	ZEUInt CurrentFrameId = zeCore->GetFrameId();
	
	if (CurrentFrameId <= LastFrameId)
		return;

	LastFrameId = LastFrameId;
	ZECamera* Camera = zeScene->GetActiveCamera();

	// Update transformations
	ZEMatrix4x4 ViewProjMatrix;
	if (RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetViewProjectionTransform();
	}
	else if (RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetViewTransform();
	}
	else if (RenderCommand->Flags & ZE_ROF_ENABLE_PROJECTION_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetProjectionTransform();
	}
	else
	{
		ViewProjMatrix = ZEMatrix4x4::Identity;
	}
	

	if (RenderCommand->Flags & ZE_ROF_ENABLE_WORLD_TRANSFORM)
	{
		ZEMatrix4x4::Multiply(Transformations.WorldViewProj, ViewProjMatrix, RenderCommand->WorldMatrix);
		ZEMatrix4x4::Multiply(Transformations.WorldView, Camera->GetViewTransform(), RenderCommand->WorldMatrix);
	}
	else
	{
		Transformations.WorldViewProj = ViewProjMatrix;
		Transformations.WorldView = Camera->GetViewTransform();
	}

}

bool ZEFixedMaterial::SetupGeometryPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	UpdateMaterial();
	
	ZECamera* Camera = zeScene->GetActiveCamera();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetVertexShader(Shaders.GBufferPassVertexShader);
	Device->SetPixelShader(Shaders.GBufferPassPixelShader);
	/*
	// Setup Transformations
	ZEMatrix4x4 ViewProjMatrix;
	if (RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetViewProjectionTransform();
	}
	else if (RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetViewTransform();
	}
	else if (RenderCommand->Flags & ZE_ROF_ENABLE_PROJECTION_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetProjectionTransform();
	}
	else
	{
		ViewProjMatrix = ZEMatrix4x4::Identity;
	}
	
	ZEMatrix4x4 WorldViewProjMatrix;
	ZEMatrix4x4 WorldViewMatrix;
	if (RenderCommand->Flags & ZE_ROF_ENABLE_WORLD_TRANSFORM)
	{
		ZEMatrix4x4::Multiply(WorldViewProjMatrix, ViewProjMatrix, RenderCommand->WorldMatrix);
		ZEMatrix4x4::Multiply(WorldViewMatrix, Camera->GetViewTransform(), RenderCommand->WorldMatrix);
	}
	else
	{
		WorldViewProjMatrix = ViewProjMatrix;
		WorldViewMatrix = Camera->GetViewTransform();
	}
	*/

	FixedMaterialVertexShaderCommonData* VSData = NULL;
	Buffers.GBufferPassVertexShader->Lock((void**)&VSData);

		VSData->WorldViewMatrix = Transformations.WorldView;
		VSData->WorldViewInvTrpsMatrix = Transformations.WorldView;
		VSData->WorldViewProjMatrix = Transformations.WorldViewProj;
		VSData->EnableSkin = (RenderCommand->Flags & ZE_ROF_SKINNED) == ZE_ROF_SKINNED;

	Buffers.GBufferPassVertexShader->Unlock();
	Device->SetVertexShaderBuffer(0, Buffers.GBufferPassVertexShader);

	FixedMaterialGBufferPassPixelShaderData* PSData = NULL;
	Buffers.GBufferPassPixelShader->Lock((void**)&PSData);

		PSData->Opacity = Opacity;
		PSData->AlphaCullLimit = AlphaCullLimit;
		PSData->DetailDistance = DetailDistance;
		PSData->DetailMapTiling = DetailMapTiling;
		PSData->DetailFadeDistance = DetailFadeDistance;
		PSData->SpecularShininess = SpecularShininess;
		PSData->SubSurfaceScatteringFactor = SubSurfaceScatteringFactor;

	Buffers.GBufferPassPixelShader->Unlock();
	Device->SetPixelShaderBuffer(0, Buffers.GBufferPassPixelShader);


	ZEDeviceStateDepthStencil DepthStencilState;
	DepthStencilState.SetStencilTestEnable(false);
	DepthStencilState.SetZFunction(ZE_CF_LESS_EQUAL);
	DepthStencilState.SetZTestEnable((RenderCommand->Flags & ZE_ROF_ENABLE_Z_CULLING) == 0 ? false : true);
	DepthStencilState.SetZWriteEnable((RenderCommand->Flags & ZE_ROF_ENABLE_NO_Z_WRITE) == 0 ? true : false);
	Device->SetDepthStencilState(DepthStencilState);

	ZEDeviceStateRasterizer RasterizerState;
	RasterizerState.SetCullDirection(TwoSided ? ZE_CD_NONE : ZE_CD_COUNTER_CLOCKWISE);
	RasterizerState.SetFillMode(Wireframe ? ZE_FM_WIREFRAME : ZE_FM_SOLID);
	Device->SetRasterizerState(RasterizerState);

	/*
	// NOTE: Add bone transformation buffer to fixedmaterial.hlsl
	// Setup Bone Transforms
	if (RenderCommand->Flags & ZE_ROF_SKINNED && RenderCommand->BoneTransforms.GetCount() < 58)
	{
		GetDevice()->SetVertexShaderConstantF(32, (float*)RenderCommand->BoneTransforms.GetCArray(), (UINT)RenderCommand->BoneTransforms.GetCount() * 4);
		SkinEnabled = true;
	}
	*/

	if ((GetAlphaCullEnabled() || TransparancyMode != ZE_MTM_NONE) && OpacityComponent == ZE_MOC_BASE_MAP_ALPHA && Components.DetailBaseMap && Textures.BaseMap != NULL)
	{
		ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.BaseMapU);
		SamplerState.SetAddressV(AdressingModes.BaseMapV);

		Device->SetPixelShaderSampler(5, SamplerState);
		Device->SetPixelShaderTexture(5, Textures.BaseMap);
	}

	if (Components.NormalMap && Textures.NormalMap != NULL)
	{
		ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.NormalMapU);
		SamplerState.SetAddressV(AdressingModes.NormalMapV);

		Device->SetPixelShaderSampler(6, SamplerState);
		Device->SetPixelShaderTexture(6, Textures.NormalMap);
	}

	if (Components.ParallaxMap && Textures.ParallaxMap != NULL)
	{
		ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.ParallaxMapU);
		SamplerState.SetAddressV(AdressingModes.ParallaxMapV);

		Device->SetPixelShaderSampler(7, SamplerState);
		Device->SetPixelShaderTexture(7, Textures.ParallaxMap);
	}

	if (Components.SpecularMap && Textures.SpecularMap != NULL)
	{
		ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.SpecularMapU);
		SamplerState.SetAddressV(AdressingModes.SpecularMapV);

		Device->SetPixelShaderSampler(8, SamplerState);
		Device->SetPixelShaderTexture(8, Textures.SpecularMap);
	}

	if ((Components.AlphaCull || TransparancyMode != ZE_MTM_NONE) && OpacityComponent == ZE_MOC_OPACITY_MAP && Components.OpacityMap && Opacity != NULL && Textures.OpacityMap != NULL)
	{
		ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.OpacityMapU);
		SamplerState.SetAddressV(AdressingModes.OpacityMapV);
		
		Device->SetPixelShaderSampler(9, SamplerState);
		Device->SetPixelShaderTexture(9, Textures.OpacityMap);
	}

	if (Components.DetailNormalMap && Textures.DetailNormalMap != NULL)
	{
		ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.DetailNormalMapU);
		SamplerState.SetAddressV(AdressingModes.DetailNormalMapV);

		Device->SetPixelShaderSampler(15, SamplerState);
		Device->SetPixelShaderTexture(15, Textures.DetailNormalMap);
	}

	return true;
}

bool ZEFixedMaterial::SetupForwardPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	UpdateMaterial();

	ZECamera* Camera = zeScene->GetActiveCamera();
	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	Device->SetVertexShader(Shaders.ForwardPassVertexShader);
	Device->SetPixelShader(Shaders.ForwardPassPixelShader);
	/*
	// Setup Transformations
	ZEMatrix4x4 ViewProjMatrix;
	if (RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetViewProjectionTransform();
	}
	else if (RenderCommand->Flags & ZE_ROF_ENABLE_VIEW_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetViewTransform();
	}
	else if (RenderCommand->Flags & ZE_ROF_ENABLE_PROJECTION_TRANSFORM)
	{
		ViewProjMatrix = Camera->GetProjectionTransform();
	}
	else
	{
		ViewProjMatrix = ZEMatrix4x4::Identity;
	}

	ZEMatrix4x4 WorldViewProjMatrix;
	ZEMatrix4x4 WorldViewMatrix;
	if (RenderCommand->Flags & ZE_ROF_ENABLE_WORLD_TRANSFORM)
	{
		ZEMatrix4x4::Multiply(WorldViewProjMatrix, ViewProjMatrix, RenderCommand->WorldMatrix);
		ZEMatrix4x4::Multiply(WorldViewMatrix, Camera->GetViewTransform(), RenderCommand->WorldMatrix);
	}
	else
	{
		WorldViewProjMatrix = ViewProjMatrix;
		WorldViewMatrix = Camera->GetViewTransform();
	}
	*/
	FixedMaterialVertexShaderCommonData* VSData = NULL;
	Buffers.ForwardPassVertexShader->Lock((void**)&VSData);

		VSData->WorldMatrix = RenderCommand->WorldMatrix;
		VSData->WorldViewProjMatrix = Transformations.WorldViewProj;
		VSData->EnableSkin = (RenderCommand->Flags & ZE_ROF_SKINNED) == ZE_ROF_SKINNED;
		VSData->CameraPosition = ZEVector4(Camera->GetWorldPosition(), 1.0f);

	Buffers.ForwardPassVertexShader->Unlock();
	Device->SetVertexShaderBuffer(0, Buffers.ForwardPassVertexShader);

	FixedMaterialForwardPassPixelShaderData* PSData = NULL;
	Buffers.ForwardPassPixelShader->Lock((void**)&PSData);

		PSData->DetailMapTiling = DetailMapTiling;
		PSData->PixelSize = ZEVector2(1.0f / (float)zeGraphics->GetScreenWidth(), 1.0f / (float)zeGraphics->GetScreenHeight());
		PSData->Opacity = Opacity;
		PSData->AlphaCullLimit = AlphaCullLimit;
		PSData->DetailDistance = DetailDistance;
		PSData->DetailFadeDistance = DetailFadeDistance;
		PSData->AmbientColor = MaterialAmbientColor;
		PSData->ReflectionFactor = ReflectionFactor;
		PSData->SpecularColor = MaterialSpecularColor;
		PSData->RefractionFactor = RefractionFactor;
		PSData->DiffuseColor = MaterialDiffuseColor;
		PSData->RefractionIndex = RefractionIndex;
		PSData->EmmisiveColor = MaterialEmissiveColor;
		PSData->SubSurfaceScatteringFactor = SubSurfaceScatteringFactor;
		ZEMatrix4x4::Inverse(PSData->InvViewMatrix, Camera->GetViewTransform());

	Buffers.ForwardPassPixelShader->Unlock();
	Device->SetPixelShaderBuffer(0, Buffers.ForwardPassPixelShader);

	// Setup ZCulling
	ZEDeviceStateDepthStencil DepthStencilState;
	DepthStencilState.SetZFunction(ZE_CF_LESS_EQUAL);
	DepthStencilState.SetZTestEnable((RenderCommand->Flags & ZE_ROF_ENABLE_Z_CULLING) == 0 ? false : true);
	DepthStencilState.SetZWriteEnable((RenderCommand->Flags & ZE_ROF_ENABLE_NO_Z_WRITE) == 0 ? true : false);
	Device->SetDepthStencilState(DepthStencilState);

	ZEDeviceStateRasterizer RasterizerState;
	RasterizerState.SetCullDirection(TwoSided ? ZE_CD_NONE : ZE_CD_COUNTER_CLOCKWISE);
	RasterizerState.SetFillMode(Wireframe ? ZE_FM_WIREFRAME : ZE_FM_SOLID);
	Device->SetRasterizerState(RasterizerState);

	/*
	// Setup Bone Transforms
	if (RenderCommand->Flags & ZE_ROF_SKINNED && RenderCommand->BoneTransforms.GetCount() < 58)
	{
		GetDevice()->SetVertexShaderConstantF(32, (float*)RenderCommand->BoneTransforms.GetCArray(), (UINT)RenderCommand->BoneTransforms.GetCount() * 4);
	}
	*/

	// Setup Textures
	if (Components.BaseMap && Textures.BaseMap != NULL)
	{
		static ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.BaseMapU);
		SamplerState.SetAddressV(AdressingModes.BaseMapV);

		Device->SetPixelShaderSampler(5, SamplerState);
		Device->SetPixelShaderTexture(5, Textures.BaseMap);
	}

	if (Components.NormalMap && Textures.NormalMap != NULL)
	{
		static ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.NormalMapU);
		SamplerState.SetAddressV(AdressingModes.NormalMapV);

		Device->SetPixelShaderSampler(6, SamplerState);
		Device->SetPixelShaderTexture(6, Textures.NormalMap);
	}

	if (Components.ParallaxMap && Textures.ParallaxMap != NULL)
	{
		static ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.ParallaxMapU);
		SamplerState.SetAddressV(AdressingModes.ParallaxMapV);

		Device->SetPixelShaderSampler(7, SamplerState);
		Device->SetPixelShaderTexture(7, Textures.ParallaxMap);
	}

	if (Components.SpecularMap && Textures.SpecularMap != NULL)
	{
		static ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.SpecularMapU);
		SamplerState.SetAddressV(AdressingModes.SpecularMapV);

		Device->SetPixelShaderSampler(8, SamplerState);
		Device->SetPixelShaderTexture(8, Textures.SpecularMap);
	}

	if (Components.OpacityMap && Textures.OpacityMap != NULL)
	{
		static ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.OpacityMapU);
		SamplerState.SetAddressV(AdressingModes.OpacityMapV);

		Device->SetPixelShaderSampler(9, SamplerState);
		Device->SetPixelShaderTexture(9, Textures.OpacityMap);
	}

	if (Components.EmissiveMap && Textures.EmissiveMap != NULL)
	{
		static ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.EmissiveMapU);
		SamplerState.SetAddressV(AdressingModes.EmissiveMapV);

		Device->SetPixelShaderSampler(10, SamplerState);
		Device->SetPixelShaderTexture(10, Textures.EmissiveMap);
	}

	if ((Components.Reflection || Components.Refraction) && Textures.EnvironmentMap != NULL)
	{
		static ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.EnvironmentMapU);
		SamplerState.SetAddressV(AdressingModes.EnvironmentMapV);

		Device->SetPixelShaderSampler(11, SamplerState);
		Device->SetPixelShaderTexture(11, Textures.EnvironmentMap);
	}

	if (Components.LightMap && Textures.LightMap != NULL)
	{
		static ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.LightMapU);
		SamplerState.SetAddressV(AdressingModes.LightMapV);

		Device->SetPixelShaderSampler(12, SamplerState);
		Device->SetPixelShaderTexture(12, Textures.LightMap);
	}

	if (Components.DistortionMap && Textures.DistortionMap != NULL)
	{
		static ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.DistortionMapU);
		SamplerState.SetAddressV(AdressingModes.DistortionMapV);

		Device->SetPixelShaderSampler(13, SamplerState);
		Device->SetPixelShaderTexture(13, Textures.DistortionMap);
	}
	
	if (Components.DetailBaseMap && Textures.DetailBaseMap != NULL)
	{
		static ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.DetailBaseMapU);
		SamplerState.SetAddressV(AdressingModes.DetailBaseMapV);

		Device->SetPixelShaderSampler(14, SamplerState);
		Device->SetPixelShaderTexture(14, Textures.DetailBaseMap);
	}

	if (Components.DetailNormalMap && Textures.DetailNormalMap != NULL)
	{
		static ZEDeviceStateSampler SamplerState;
		SamplerState.SetMinFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMagFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetMipFilter(ZE_TFM_ANISOTROPY);
		SamplerState.SetAddressU(AdressingModes.DetailNormalMapU);
		SamplerState.SetAddressV(AdressingModes.DetailNormalMapV);

		Device->SetPixelShaderSampler(15, SamplerState);
		Device->SetPixelShaderTexture(15, Textures.DetailNormalMap);
	}

	return true;
}

bool ZEFixedMaterial::SetupTransparentPass(const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	UpdateMaterial();

	return true;
}

bool ZEFixedMaterial::SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	zeDebugCheck(PassId > 0, "Fixed material have single pass");

	// Update Buffers
	UpdateTransformations(RenderCommand);

	switch (Stage->GetStageFlags())
	{
		case ZE_RENDER_STAGE_GEOMETRY:
			SetupGeometryPass(Stage, RenderCommand);
			break;
		case ZE_RENDER_STAGE_FORWARD:
			SetupForwardPass(Stage, RenderCommand);
			break;
		case ZE_RENDER_STAGE_TRANSPARENT:
			SetupTransparentPass(Stage, RenderCommand);
			break;
	}

	return true;
}

void ZEFixedMaterial::UpdateMaterial()
{
	if (Recompile)
		CreateShaders();
}

ZEFixedMaterial* ZEFixedMaterial::CreateInstance()
{
	return new ZEFixedMaterial();
}

void ZEFixedMaterial::WriteToFile(ZEFile* File)
{
	/*
	ZEObjectDescription* ClassDescription = GetDescription();
	ZESSize PropertyCount = ClassDescription->GetPropertyCount();
	const ZEPropertyDescription* Properties = ClassDescription->GetProperties();

	ZEMLNode* RootNode = new ZEMLNode("Root");

	for(ZESize I = 0; I < PropertyCount; I++)
	{
		ZEVariant PropertyValue;
		GetProperty(Properties[I].Name, PropertyValue);
		RootNode->AddProperty(Properties[I].Name, PropertyValue);
	}

	RootNode->Write(File);
	delete RootNode;
	*/
}

void ZEFixedMaterial::ReadFromFile(ZEFile* File)
{
	/*
	ZEMLNode* RootNode = new ZEMLNode();
	RootNode->Read(File);

	ZEArray<ZEMLItem*> Props = RootNode->GetProperties();
	
	for (ZESize I = 0; I < Props.GetCount(); I++)
	{
		if(Props[I]->GetType() != ZEML_IT_OFFSET_DATA)
		{
			ZEMLProperty* CurrentProperty = ((ZEMLProperty*)Props[I]);

			if(CurrentProperty->GetValue().GetType() == ZE_VRT_STRING && ZEString(CurrentProperty->GetValue().GetString()).GetLength() != 0)
				SetProperty(Props[I]->GetName(), CurrentProperty->GetValue());
			else if(CurrentProperty->GetValue().GetType() != ZE_VRT_STRING)
				SetProperty(Props[I]->GetName(), CurrentProperty->GetValue());
		}
	}

	UpdateMaterial();
	*/
}
