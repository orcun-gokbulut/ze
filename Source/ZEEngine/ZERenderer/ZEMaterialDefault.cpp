//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMaterialDefault.cpp
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

#include "ZERenderStage.h"
#include "ZEGame/ZEScene.h"
#include "ZEMath/ZEMatrix.h"
#include "ZERenderCommand.h"
#include "ZEMaterialDefault.h"
#include "ZEGraphics/ZEShader.h"
#include "ZEGraphics/ZETexture2D.h"
#include "ZEGraphics/ZETextureCube.h"
#include "ZEGraphics/ZEGraphicsDevice.h"
#include "ZEGraphics/ZEGraphicsModule.h"
#include "ZEGraphics/ZEShaderCompiler.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETextureCubeResource.h"
#include "ZEGraphics/ZEShaderCompileOptions.h"

#include "ZEML/ZEMLNode.h"
#include "ZEML/ZEMLItem.h"
#include "ZEDS/ZEString.h"
#include "ZEML/ZEMLProperty.h"
#include "ZEFile/ZEFileInfo.h"
#include "ZEFile/ZEPathUtils.h"
#include "ZEML/ZEMLSerialReader.h"

#define PROPERTY_BUFFER_SLOT			0
#define TRANSFORMATION_BUFFER_SLOT		0
#define SKINNING_BUFFER_SLOT			1

struct Transformations
{
	ZEMatrix4x4		WorldTransform;
};

void ZEMaterialDefault::DestroyShaders()
{
	ZE_DESTROY(ShaderShadowPassVS);
	ZE_DESTROY(ShaderShadowPassPS);
	ZE_DESTROY(ShaderGeometryPassVS);
	ZE_DESTROY(ShaderGeometryPassPS);
	ZE_DESTROY(ShaderAccumulationPassVS);
	ZE_DESTROY(ShaderAccumulationPassPS);
	ZE_DESTROY(ShaderTransparentPassVS);
	ZE_DESTROY(ShaderTransparentPassPS);
}

void ZEMaterialDefault::UpdateShaders()
{
	DestroyShaders();

	ZEShaderCompileOptions Options;
	Options.FileName = "MaterialDefault.hlsl";
	Options.Model = ZE_SM_4_0;

	ZEShaderParameter TempParameter;

	if (Components.GetFlags(ZE_MDC_AMBIENT))
	{
		TempParameter.Name = "ZE_SHADER_AMBIENT";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_DIFFUSE))
	{
		TempParameter.Name = "ZE_SHADER_DIFFUSE";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_BASE_MAP))
	{
		TempParameter.Name = "ZE_SHADER_BASE_MAP";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_NORMAL_MAP))
	{
		TempParameter.Name = "ZE_SHADER_NORMAL_MAP";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_PARALLAX_MAP))
	{
		TempParameter.Name = "ZE_SHADER_PARALLAX_MAP";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_SPECULAR))
	{
		TempParameter.Name = "ZE_SHADER_SPECULAR";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_SPECULAR_MAP))
	{
		TempParameter.Name = "ZE_SHADER_SPECULAR_MAP";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_EMISSIVE))
	{
		TempParameter.Name = "ZE_SHADER_EMISSIVE";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_EMISSIVE_MAP))
	{
		TempParameter.Name = "ZE_SHADER_EMISSIVE_MAP";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_OPACITY))
	{
		TempParameter.Name = "ZE_SHADER_OPACITY";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_OPACITY_BASE_ALPHA))
	{
		TempParameter.Name = "ZE_SHADER_OPACITY_BASE_ALPHA";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_OPACITY_CONSTANT))
	{
		TempParameter.Name = "ZE_SHADER_OPACITY_CONSTANT";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_OPACITY_MAP))
	{
		TempParameter.Name = "ZE_SHADER_OPACITY_MAP";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_DETAIL_BASE_MAP))
	{
		TempParameter.Name = "ZE_SHADER_DETAIL_BASE_MAP";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_DETAIL_NORMAL_MAP))
	{
		TempParameter.Name = "ZE_SHADER_DETAIL_NORMAL_MAP";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_REFLECTION))
	{
		TempParameter.Name = "ZE_SHADER_REFLECTION";
		Options.Parameters.Add(TempParameter);
	}	
	if (Components.GetFlags(ZE_MDC_REFRACTION))
	{
		TempParameter.Name = "ZE_SHADER_REFRACTION";
		Options.Parameters.Add(TempParameter);
	}	
	if (Components.GetFlags(ZE_MDC_LIGHT_MAP))
	{
		TempParameter.Name = "ZE_SHADER_LIGHT_MAP";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_ALPHA_CULL))
	{
		TempParameter.Name = "ZE_SHADER_ALPHA_CULL";
		Options.Parameters.Add(TempParameter);
	}
	if (Components.GetFlags(ZE_MDC_VERTEX_COLOR))
	{
		TempParameter.Name = "ZE_SHADER_VERTEX_COLOR";
		Options.Parameters.Add(TempParameter);
	}
	//if (Components.GetFlags(ZE_MDC_SKINNING))
	//{
	//	TempParameter.Name = "ZE_SHADER_SKIN_TRANSFORM";
	//	Options.Parameters.Add(TempParameter);

	//	TempParameter.Name = "ZE_SHADER_BONE_MATRIX_COUNT";
	//	TempParameter.Definition = ZEString::FromUInt32(BoneMatrixCount);
	//	Options.Parameters.Add(TempParameter);
	//}

	// Geometry pass shaders
	Options.Type = ZE_ST_VERTEX;
	Options.EntryPoint = "ZEMaterialDefault_GeometryPass_VertexShader";
	ShaderGeometryPassVS = ZEShaderCompiler::CompileShaderFromFile(&Options);

	Options.Type = ZE_ST_PIXEL;
	Options.EntryPoint = "ZEMaterialDefault_GeometryPass_PixelShader";
	ShaderGeometryPassPS = ZEShaderCompiler::CompileShaderFromFile(&Options);

	// Accumulation pass shaders
	Options.Type = ZE_ST_VERTEX;
	Options.EntryPoint = "ZEMaterialDefault_AccumulationPass_VertexShader";
	ShaderAccumulationPassVS = ZEShaderCompiler::CompileShaderFromFile(&Options);

	Options.Type = ZE_ST_PIXEL;
	Options.EntryPoint = "ZEMaterialDefault_AccumulationPass_PixelShader";
	ShaderAccumulationPassPS = ZEShaderCompiler::CompileShaderFromFile(&Options);

	// Transparent pass shaders
	Options.Type = ZE_ST_VERTEX;
	Options.EntryPoint = "ZEMaterialDefault_TransparentPass_VertexShader";
	//ShaderTransparentPassVertexShader = ZEShaderCompiler::CompileShaderFromFile(&Options);

	Options.Type = ZE_ST_PIXEL;
	Options.EntryPoint = "ZEMaterialDefault_TransparentPass_PixelShader";
	//ShaderTransparentPassPixelShader = ZEShaderCompiler::CompileShaderFromFile(&Options);

	// ShadowPass pass shaders
	Options.Type = ZE_ST_VERTEX;
	Options.EntryPoint = "ZEMaterialDefault_ShadowPass_VertexShader";
	//ShaderShadowPassVertexShader = ZEShaderCompiler::CompileShaderFromFile(&Options);

	Options.Type = ZE_ST_PIXEL;
	Options.EntryPoint = "ZEMaterialDefault_ShadowPasss_PixelShader";
	//ShaderShadowPassPixelShader = ZEShaderCompiler::CompileShaderFromFile(&Options);
}

void ZEMaterialDefault::DestroyBuffers()
{
	ZE_DESTROY(MaterialProperties);
}

void ZEMaterialDefault::InitializeBuffers()
{
	DestroyBuffers();

	MaterialProperties = ZEConstantBuffer::CreateInstance();
	MaterialProperties->Create(sizeof(ZEMaterialDefault::Properties));

	// Set default values
	ZEMaterialDefault::Properties* Properties = NULL;
	MaterialProperties->Lock((void**)&Properties);
	
		Properties->Opacity = 1.0f;
		Properties->DetailMapTiling = ZEVector2::One;
		Properties->DetailDistanceFull = 4.0f;
		Properties->DetailDistanceFade = 2.0f;
		Properties->AmbientColor = ZEVector3::One;
		Properties->AmbientFactor = 1.0f;
		Properties->SpecularColor = ZEVector3::One;
		Properties->SpecularFactor = 1.0f;
		Properties->DiffuseColor = ZEVector3::One;
		Properties->DiffuseFactor = 1.0f;
		Properties->EmissiveColor = ZEVector3::One;
		Properties->EmissiveFactor = 1.0f;
		Properties->RefractionIndex = 1.0f;
		Properties->RefractionFactor = 1.0f;
		Properties->ReflectionFactor = 1.0f;
		Properties->SubSurfScatFactor = 1.0f;
		Properties->AlphaCullLimit = 1.0f;
		Properties->SpecularShininess = 1.0f;
	
	MaterialProperties->Unlock();

	if (WorldTransform == NULL)
	{
		WorldTransform = ZEConstantBuffer::CreateInstance();
		WorldTransform->Create(sizeof(Transformations));

		// Set default values
		Transformations* Transforms = NULL;
		WorldTransform->Lock((void**)&Transforms);

			Transforms->WorldTransform = ZEMatrix4x4::Identity;

		WorldTransform->Unlock();
	}
}

bool ZEMaterialDefault::SetupPassShadow(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	if (!ShadowCaster)
		return false;

	zeCriticalError("Not implemented yet");

	// No more passes
	return false;
}

bool ZEMaterialDefault::SetupPassGeometry(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	if (TransparencyMode != ZE_TM_NONE)
		return false;

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();
	
	// SetShaders
	Device->SetVertexShader(ShaderGeometryPassVS);
	Device->SetPixelShader(ShaderGeometryPassPS);
	
	// Set Buffers
	Device->SetVertexShaderBuffer(TRANSFORMATION_BUFFER_SLOT, WorldTransform);
	Device->SetPixelShaderBuffer(PROPERTY_BUFFER_SLOT, MaterialProperties);

	if (RenderCommand->Skinned)
		Device->SetVertexShaderBuffer(SKINNING_BUFFER_SLOT, RenderCommand->SkinningBuffer);

	ZEDepthStencilState DepthStencilState;
	DepthStencilState.SetZTestEnable(true);
	DepthStencilState.SetZWriteEnable(true);
	DepthStencilState.SetStencilTestEnable(false);
	DepthStencilState.SetZFunction(ZE_CF_LESS_EQUAL);
	Device->SetDepthStencilState(DepthStencilState);

	ZERasterizerState RasterizerState;
	RasterizerState.SetFillMode(Wireframe ? ZE_FM_WIREFRAME : ZE_FM_SOLID);
	RasterizerState.SetCullDirection(TwoSided ? ZE_CD_NONE : ZE_CD_COUNTER_CLOCKWISE);
	Device->SetRasterizerState(RasterizerState);

	bool Opacity = Components.GetFlags(ZE_MDC_OPACITY);
	bool AlphaCull = Components.GetFlags(ZE_MDC_ALPHA_CULL);
	bool UseBaseMap = (OpacityComponent == ZE_OC_BASE_MAP_ALPHA) && Components.GetFlags(ZE_MDC_BASE_MAP) && (TextureBaseMap != NULL);
	bool UseOpacityMap = (OpacityComponent == ZE_OC_OPACITY_MAP) && Components.GetFlags(ZE_MDC_OPACITY_MAP) && (TextureOpacityMap != NULL);
	
	if ((AlphaCull || TransparencyMode != ZE_TM_NONE) && Opacity && UseBaseMap)
	{
		Device->SetPixelShaderSampler(5, SamplerBaseMap);
		Device->SetPixelShaderTexture(5, TextureBaseMap);
	}
	if ((AlphaCull || TransparencyMode != ZE_TM_NONE) && Opacity && UseOpacityMap)
	{		
		Device->SetPixelShaderSampler(9, SamplerOpacityMap);
		Device->SetPixelShaderTexture(9, TextureOpacityMap);
	}
	if (Components.GetFlags(ZE_MDC_NORMAL_MAP) && TextureNormalMap != NULL)
	{
		Device->SetPixelShaderSampler(6, SamplerNormalMap);
		Device->SetPixelShaderTexture(6, TextureNormalMap);
	}
	if (Components.GetFlags(ZE_MDC_PARALLAX_MAP) && TextureParallaxMap != NULL)
	{
		Device->SetPixelShaderSampler(7, SamplerParallaxMap);
		Device->SetPixelShaderTexture(7, TextureParallaxMap);
	}
	if (Components.GetFlags(ZE_MDC_SPECULAR_MAP) && TextureSpecularMap != NULL)
	{
		Device->SetPixelShaderSampler(8, SamplerSpecularMap);
		Device->SetPixelShaderTexture(8, TextureSpecularMap);
	}
	if (Components.GetFlags(ZE_MDC_DETAIL_NORMAL_MAP) && TextureDetailNormalMap != NULL)
	{
		Device->SetPixelShaderSampler(15, SamplerDetailNormalMap);
		Device->SetPixelShaderTexture(15, TextureDetailNormalMap);
	}

	// No more passes
	return false;
}

bool ZEMaterialDefault::SetupPassAccumulation(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	// No semi transparency
	if (TransparencyMode != ZE_TM_NONE)
		return false;

	ZEGraphicsDevice* Device = zeGraphics->GetDevice();

	// SetShaders
	Device->SetVertexShader(ShaderAccumulationPassVS);
	Device->SetPixelShader(ShaderAccumulationPassPS);
	
	// Set Buffers
	Device->SetVertexShaderBuffer(TRANSFORMATION_BUFFER_SLOT, WorldTransform);
	Device->SetPixelShaderBuffer(PROPERTY_BUFFER_SLOT, MaterialProperties);

	if (RenderCommand->Skinned)
		Device->SetVertexShaderBuffer(SKINNING_BUFFER_SLOT, RenderCommand->SkinningBuffer);

	// Setup ZCulling
	ZEDepthStencilState DepthStencilState;
	DepthStencilState.SetZTestEnable(true);
	DepthStencilState.SetZWriteEnable(false);
	DepthStencilState.SetZFunction(ZE_CF_LESS_EQUAL);
	Device->SetDepthStencilState(DepthStencilState);
	
	ZERasterizerState RasterizerState;
	RasterizerState.SetFillMode(Wireframe ? ZE_FM_WIREFRAME : ZE_FM_SOLID);
	RasterizerState.SetCullDirection(TwoSided ? ZE_CD_NONE : ZE_CD_COUNTER_CLOCKWISE);
	Device->SetRasterizerState(RasterizerState);

	// Setup Textures
	if (Components.GetFlags(ZE_MDC_BASE_MAP) && TextureBaseMap != NULL)
	{
		Device->SetPixelShaderSampler(5, SamplerBaseMap);
		Device->SetPixelShaderTexture(5, TextureBaseMap);
	}
	if (Components.GetFlags(ZE_MDC_PARALLAX_MAP) && TextureParallaxMap != NULL)
	{
		Device->SetPixelShaderSampler(7, SamplerParallaxMap);
		Device->SetPixelShaderTexture(7, TextureParallaxMap);
	}
	if (Components.GetFlags(ZE_MDC_SPECULAR_MAP) && TextureSpecularMap != NULL)
	{
		Device->SetPixelShaderSampler(8, SamplerSpecularMap);
		Device->SetPixelShaderTexture(8, TextureSpecularMap);
	}
	if (Components.GetFlags(ZE_MDC_OPACITY_MAP) && TextureOpacityMap != NULL)
	{
		Device->SetPixelShaderSampler(9, SamplerOpacityMap);
		Device->SetPixelShaderTexture(9, TextureOpacityMap);
	}
	if (Components.GetFlags(ZE_MDC_EMISSIVE_MAP) && TextureEmissiveMap != NULL)
	{
		Device->SetPixelShaderSampler(10, SamplerEmissiveMap);
		Device->SetPixelShaderTexture(10, TextureEmissiveMap);
	}
	if ((Components.GetFlags(ZE_MDC_REFLECTION) || Components.GetFlags(ZE_MDC_REFRACTION)) && TextureEnvironmentMap != NULL)
	{
		Device->SetPixelShaderSampler(11, SamplerEnvironmentMap);
		Device->SetPixelShaderTexture(11, TextureEnvironmentMap);
	}
	if (Components.GetFlags(ZE_MDC_DETAIL_BASE_MAP) && TextureDetailBaseMap != NULL)
	{
		Device->SetPixelShaderSampler(14, SamplerDetailBaseMap);
		Device->SetPixelShaderTexture(14, TextureDetailBaseMap);
	}
	if (Components.GetFlags(ZE_MDC_DETAIL_NORMAL_MAP) && TextureDetailNormalMap != NULL)
	{
		Device->SetPixelShaderSampler(15, SamplerDetailNormalMap);
		Device->SetPixelShaderTexture(15, TextureDetailNormalMap);
	}

	// No more passes
	return false;
}

bool ZEMaterialDefault::SetupPassTransparent(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	zeCriticalError("Not implemented yet");
		
	return false;
}

ZESize ZEMaterialDefault::GetHash() const
{
	ZESize Hash = ZEHashGenerator::Hash("ZEMaterialDefaultNew");
	ZEHashGenerator::Hash(Hash, (void*)&Components, sizeof(ZEMaterialDefaultComponents));
	return Hash;
}

void ZEMaterialDefault::WriteToFile(const ZEString& FilePath)
{
	zeCriticalError("Not implemented yet");
}

void ZEMaterialDefault::ReadFromFile(const ZEString& FilePath)
{
	ZEFile File;
	if(!File.Open(FilePath, ZE_FOM_READ, ZE_FCM_NONE))
		zeError("Can not open given file. File : %s", FilePath.ToCString());

	ZEMLSerialReader Reader(&File);

	if(!Reader.Read())
		zeError("Can not read material file.");

	ZEVariant Name;
	ZEMLSerialPointer Configuration;

	ZEMLSerialListItem Material[] = 
	{
		ZEML_LIST_NODE("Configuration",		Configuration,						true),
		ZEML_LIST_PROPERTY("Name",			Name,			ZE_VRT_STRING,		true),
	};

	if (!Reader.ReadPropertyList(Material, 2))
			zeError("Cannot read material configuration");

	Reader.SeekPointer(Configuration);

	// Simple rendering options
	ZEVariant WireFrame, TwoSided, ZCull, ZWrite;
	
	// Material properties
	ZEVariant Opacity, AlphaCullLimit, DetailMapTiling, DetailDistanceFull, 
				DetailDistanceFade, SpecularShininess, SubSurfScatFactor,
				AmbientColor, AmbientFactor, SpecularColor, SpecularFactor, 
				DiffuseColor, DiffuseFactor, EmissiveColor, EmissiveFactor,
				RefractionIndex, RefractionFactor, ReflectionFactor;
	
	// Textures
	ZEVariant BaseMap, NormalMap, ParallaxMap, SpecularMap, EmissiveMap, OpacityMap, 
				DetailBaseMap, DetailNormalMap, EnvironmentMap, LightMap;
	
	// Shader components
	ZEVariant AmbientEnabled, DiffuseEnabled, BaseMapEnabled, NormalMapEnabled,
				ParallaxMapEnabled, SpecularEnabled, SpecularMapEnabled, 
				EmissiveEnabled, EmissiveMapEnabled, OpacityEnabled,
				OpacityBaseAlphaEnabled, OpacityConstantEnabled, OpacityMapEnabled, 
				DetailEnabled, DetailBaseMapEnabled, DetailNormalMapEnabled, 
				ReflectionEnabled, RefractionEnabled, LightMapEnabled, 
				AlphaCullEnabled, VertexColorEnabled;
	
	ZEMLSerialListItem Properties[] = 
	{
		ZEML_LIST_PROPERTY("ZCull",						ZCull,					ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("ZWrite",					ZWrite,					ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("Wireframe",					WireFrame,				ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("TwoSided",					TwoSided,				ZE_VRT_BOOLEAN,		false),

		ZEML_LIST_PROPERTY("BaseMap",					BaseMap,				ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("NormalMap",					NormalMap,				ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("ParallaxMap",				ParallaxMap,			ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("SpecularMap",				SpecularMap,			ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("EmissiveMap",				EmissiveMap,			ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("OpacityMap",				OpacityMap,				ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("DetailNormalMap",			DetailBaseMap,			ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("DetailNormalMap",			DetailNormalMap,		ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("EnvironmentMap",			EnvironmentMap,			ZE_VRT_STRING,		false),
		ZEML_LIST_PROPERTY("LightMap",					LightMap,				ZE_VRT_STRING,		false),
	
		ZEML_LIST_PROPERTY("AmbientEnabled",			AmbientEnabled,			ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("DiffuseEnabled",			DiffuseEnabled,			ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("BaseMapEnabled",			BaseMapEnabled,			ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("NormalMapEnabled",			NormalMapEnabled,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("ParallaxMapEnabled",		ParallaxMapEnabled,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("SpecularEnabled",			SpecularEnabled,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("SpecularMapEnabled",		SpecularMapEnabled,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("EmissiveEnabled",			EmissiveEnabled,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("EmissiveMapEnabled",		EmissiveMapEnabled,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("OpacityEnabled",			OpacityEnabled,			ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("OpacityMapEnabled",			OpacityMapEnabled,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("OpacityConstantEnabled",	OpacityConstantEnabled,	ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("OpacityBaseAlphaEnabled",	OpacityBaseAlphaEnabled,ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("DetailBaseMapEnabled",		DetailBaseMapEnabled,	ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("DetailNormalMapEnabled",	DetailNormalMapEnabled,	ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("ReflectionEnabled",			ReflectionEnabled,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("RefractionEnabled",			RefractionEnabled,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("LightMapEnabled",			LightMapEnabled,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("AlphaCullEnabled",			AlphaCullEnabled,		ZE_VRT_BOOLEAN,		false),
		ZEML_LIST_PROPERTY("VertexColorEnabled",		VertexColorEnabled,		ZE_VRT_BOOLEAN,		false),

		ZEML_LIST_PROPERTY("Opacity",					Opacity,				ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("AlphaCullLimit",			AlphaCullLimit,			ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("DetailMapTiling",			DetailMapTiling,		ZE_VRT_VECTOR2,		false),
		ZEML_LIST_PROPERTY("DetailDistanceFull",		DetailDistanceFull,		ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("DetailDistanceFade",		DetailDistanceFade,		ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("SpecularShininess",			SpecularShininess,		ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("SubSurfScatFactor",			SubSurfScatFactor,		ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("AmbientColor",				AmbientColor,			ZE_VRT_VECTOR3,		false),
		ZEML_LIST_PROPERTY("AmbientFactor",				AmbientFactor,			ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("SpecularColor",				SpecularColor,			ZE_VRT_VECTOR3,		false),
		ZEML_LIST_PROPERTY("SpecularFactor",			SpecularFactor,			ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("DiffuseColor",				DiffuseColor,			ZE_VRT_VECTOR3,		false),
		ZEML_LIST_PROPERTY("DiffuseFactor",				DiffuseFactor,			ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("EmissiveColor",				EmissiveColor,			ZE_VRT_VECTOR3,		false),
		ZEML_LIST_PROPERTY("EmissiveFactor",			EmissiveFactor,			ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("RefractionIndex",			RefractionIndex,		ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("RefractionFactor",			RefractionFactor,		ZE_VRT_FLOAT,		false),
		ZEML_LIST_PROPERTY("ReflectionFactor",			ReflectionFactor,		ZE_VRT_FLOAT,		false)
	};

	if (!Reader.ReadPropertyList(Properties, 52))
			zeError("Cannot read material properties");

	// Load resources
	ZEString ResourcePath;
	Components.UnraiseFlags(0xFFFFFFFF);
	if (BaseMap.GetType() != ZE_VRT_UNDEFINED && !BaseMap.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + BaseMap.GetString();
		TextureBaseMap = ZETexture2DResource::LoadSharedResource(ResourcePath)->GetTexture();
		Components.RaiseFlags(ZE_MDC_BASE_MAP);
		Components.RaiseFlags(ZE_MDC_DIFFUSE);
	}
	if (NormalMap.GetType() != ZE_VRT_UNDEFINED && !NormalMap.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + NormalMap.GetString();
		TextureNormalMap = ZETexture2DResource::LoadSharedResource(ResourcePath)->GetTexture();
		Components.RaiseFlags(ZE_MDC_NORMAL_MAP);
	}
	if (ParallaxMap.GetType() != ZE_VRT_UNDEFINED && !ParallaxMap.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + ParallaxMap.GetString();
		TextureParallaxMap = ZETexture2DResource::LoadSharedResource(ResourcePath)->GetTexture();
		Components.RaiseFlags(ZE_MDC_PARALLAX_MAP);
	}
	if (SpecularMap.GetType() != ZE_VRT_UNDEFINED && !SpecularMap.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + SpecularMap.GetString();
		TextureSpecularMap = ZETexture2DResource::LoadSharedResource(ResourcePath)->GetTexture();
		Components.RaiseFlags(ZE_MDC_SPECULAR_MAP);
		Components.RaiseFlags(ZE_MDC_SPECULAR);
	}
	if (EmissiveMap.GetType() != ZE_VRT_UNDEFINED && !EmissiveMap.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + EmissiveMap.GetString();
		TextureEmissiveMap = ZETexture2DResource::LoadSharedResource(ResourcePath)->GetTexture();
		Components.RaiseFlags(ZE_MDC_EMISSIVE_MAP);
		Components.RaiseFlags(ZE_MDC_EMISSIVE);
	}
	if (OpacityMap.GetType() != ZE_VRT_UNDEFINED && !OpacityMap.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + OpacityMap.GetString();
		TextureOpacityMap = ZETexture2DResource::LoadSharedResource(ResourcePath)->GetTexture();
		Components.RaiseFlags(ZE_MDC_OPACITY_MAP);
		Components.RaiseFlags(ZE_MDC_OPACITY);
	}
	if (DetailBaseMap.GetType() != ZE_VRT_UNDEFINED && !DetailBaseMap.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + DetailBaseMap.GetString();
		TextureDetailBaseMap = ZETexture2DResource::LoadSharedResource(ResourcePath)->GetTexture();
		Components.RaiseFlags(ZE_MDC_DETAIL_BASE_MAP);
	}
	if (DetailNormalMap.GetType() != ZE_VRT_UNDEFINED && !DetailNormalMap.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + DetailNormalMap.GetString();
		TextureDetailNormalMap = ZETexture2DResource::LoadSharedResource(ResourcePath)->GetTexture();
		Components.RaiseFlags(ZE_MDC_DETAIL_NORMAL_MAP);
	}
	if (EnvironmentMap.GetType() != ZE_VRT_UNDEFINED && !EnvironmentMap.GetString().IsEmpty())
	{
		ResourcePath = ZEFileInfo::GetParentDirectory(FilePath) + ZEPathUtils::GetSeperator() + EnvironmentMap.GetString();
		TextureEnvironmentMap = ZETextureCubeResource::LoadSharedResource(ResourcePath)->GetTexture();
	}

	// Configure material
	Components.RaiseFlags(AmbientEnabled.GetType() != ZE_VRT_UNDEFINED && AmbientEnabled.GetBoolean() ? ZE_MDC_AMBIENT : 0);
	Components.RaiseFlags(DiffuseEnabled.GetType() != ZE_VRT_UNDEFINED && DiffuseEnabled.GetBoolean() ? ZE_MDC_DIFFUSE : 0);
	Components.RaiseFlags(BaseMapEnabled.GetType() != ZE_VRT_UNDEFINED && BaseMapEnabled.GetBoolean() ? ZE_MDC_BASE_MAP : 0);
	Components.RaiseFlags(NormalMapEnabled.GetType() != ZE_VRT_UNDEFINED && NormalMapEnabled.GetBoolean() ? ZE_MDC_NORMAL_MAP : 0);
	Components.RaiseFlags(ParallaxMapEnabled.GetType() != ZE_VRT_UNDEFINED && ParallaxMapEnabled.GetBoolean() ? ZE_MDC_PARALLAX_MAP : 0);
	Components.RaiseFlags(SpecularEnabled.GetType() != ZE_VRT_UNDEFINED && SpecularEnabled.GetBoolean() ? ZE_MDC_SPECULAR : 0);
	Components.RaiseFlags(SpecularMapEnabled.GetType() != ZE_VRT_UNDEFINED && SpecularMapEnabled.GetBoolean() ? ZE_MDC_SPECULAR_MAP : 0);
	Components.RaiseFlags(EmissiveEnabled.GetType() != ZE_VRT_UNDEFINED && EmissiveEnabled.GetBoolean() ? ZE_MDC_EMISSIVE : 0);
	Components.RaiseFlags(EmissiveMapEnabled.GetType() != ZE_VRT_UNDEFINED && EmissiveMapEnabled.GetBoolean() ? ZE_MDC_EMISSIVE_MAP : 0);
	Components.RaiseFlags(OpacityEnabled.GetType() != ZE_VRT_UNDEFINED && OpacityEnabled.GetBoolean() ? ZE_MDC_OPACITY : 0);
	Components.RaiseFlags(OpacityConstantEnabled.GetType() != ZE_VRT_UNDEFINED && OpacityConstantEnabled.GetBoolean() ? ZE_MDC_OPACITY_CONSTANT : 0);
	Components.RaiseFlags(OpacityBaseAlphaEnabled.GetType() != ZE_VRT_UNDEFINED && OpacityBaseAlphaEnabled.GetBoolean() ? ZE_MDC_OPACITY_BASE_ALPHA : 0);
	Components.RaiseFlags(OpacityMapEnabled.GetType() != ZE_VRT_UNDEFINED && OpacityMapEnabled.GetBoolean() ? ZE_MDC_OPACITY_MAP : 0);
	Components.RaiseFlags(DetailBaseMapEnabled.GetType() != ZE_VRT_UNDEFINED && DetailBaseMapEnabled.GetBoolean() ? ZE_MDC_DETAIL_BASE_MAP : 0);
	Components.RaiseFlags(DetailNormalMapEnabled.GetType() != ZE_VRT_UNDEFINED && DetailNormalMapEnabled.GetBoolean() ? ZE_MDC_DETAIL_NORMAL_MAP : 0);
	Components.RaiseFlags(ReflectionEnabled.GetType() != ZE_VRT_UNDEFINED && ReflectionEnabled.GetBoolean() ? ZE_MDC_REFLECTION : 0);
	Components.RaiseFlags(RefractionEnabled.GetType() != ZE_VRT_UNDEFINED && RefractionEnabled.GetBoolean() ? ZE_MDC_REFRACTION : 0);
	Components.RaiseFlags(LightMapEnabled.GetType() != ZE_VRT_UNDEFINED && LightMapEnabled.GetBoolean() ? ZE_MDC_LIGHT_MAP : 0);
	Components.RaiseFlags(AlphaCullEnabled.GetType() != ZE_VRT_UNDEFINED && AlphaCullEnabled.GetBoolean() ? ZE_MDC_ALPHA_CULL : 0);
	Components.RaiseFlags(VertexColorEnabled.GetType() != ZE_VRT_UNDEFINED && VertexColorEnabled.GetBoolean() ? ZE_MDC_VERTEX_COLOR : 0);

	// Set parameters
	ZEMaterialDefault::Properties* MaterialProps = NULL;
	MaterialProperties->Lock((void**)&MaterialProps);

		MaterialProps->Opacity				= Opacity.GetType() == ZE_VRT_UNDEFINED ? 1.0f : Opacity.GetFloat();
		MaterialProps->DetailMapTiling		= DetailMapTiling.GetType() == ZE_VRT_UNDEFINED ? ZEVector2::One : DetailMapTiling.GetVector2();
		MaterialProps->DetailDistanceFull	= DetailDistanceFull.GetType() == ZE_VRT_UNDEFINED ? 4.0f : DetailDistanceFull.GetFloat();
		MaterialProps->DetailDistanceFade	= DetailDistanceFade.GetType() == ZE_VRT_UNDEFINED ? 2.0f : DetailDistanceFade.GetFloat();
		MaterialProps->AmbientColor			= AmbientColor.GetType() == ZE_VRT_UNDEFINED ? ZEVector3::One : AmbientColor.GetVector3();
		MaterialProps->AmbientFactor		= AmbientFactor.GetType() == ZE_VRT_UNDEFINED ? 1.0f : AmbientFactor.GetFloat();
		MaterialProps->SpecularColor		= SpecularColor.GetType() == ZE_VRT_UNDEFINED ? ZEVector3::One : SpecularColor.GetVector3();
		MaterialProps->SpecularFactor		= SpecularFactor.GetType() == ZE_VRT_UNDEFINED ? 1.0f : SpecularFactor.GetFloat();
		MaterialProps->DiffuseColor			= DiffuseColor.GetType() == ZE_VRT_UNDEFINED ? ZEVector3::One : DiffuseColor.GetVector3();
		MaterialProps->DiffuseFactor		= DiffuseFactor.GetType() == ZE_VRT_UNDEFINED ? 1.0f : DiffuseFactor.GetFloat();
		MaterialProps->EmissiveColor		= EmissiveColor.GetType() == ZE_VRT_UNDEFINED ? ZEVector3::One : EmissiveColor.GetVector3();
		MaterialProps->EmissiveFactor		= EmissiveFactor.GetType() == ZE_VRT_UNDEFINED ? 1.0f : EmissiveFactor.GetFloat();
		MaterialProps->RefractionIndex		= RefractionIndex.GetType() == ZE_VRT_UNDEFINED ? 1.0f : RefractionIndex.GetFloat();
		MaterialProps->RefractionFactor		= RefractionFactor.GetType() == ZE_VRT_UNDEFINED ? 1.0f : RefractionFactor.GetFloat();
		MaterialProps->ReflectionFactor		= ReflectionFactor.GetType() == ZE_VRT_UNDEFINED ? 1.0f : ReflectionFactor.GetFloat();
		MaterialProps->SubSurfScatFactor	= SubSurfScatFactor.GetType() == ZE_VRT_UNDEFINED ? 1.0f : SubSurfScatFactor.GetFloat();
		MaterialProps->AlphaCullLimit		= AlphaCullLimit.GetType() == ZE_VRT_UNDEFINED ? 1.0f : AlphaCullLimit.GetFloat();
		MaterialProps->SpecularShininess	= SpecularShininess.GetType() == ZE_VRT_UNDEFINED ? 1.0f : SpecularShininess.GetFloat();

	MaterialProperties->Unlock();

	UpdateShaders();
}

bool ZEMaterialDefault::SetupPass(ZEUInt PassId, const ZERenderStage* Stage, const ZERenderCommand* RenderCommand)
{
	if(!ZEMaterial::SetupPass(PassId, Stage, RenderCommand))
		return false;

	if (Components != OldComponents)
	{
		UpdateShaders();
		OldComponents = Components;
	}

	// Copy world matrix to buffer
	Transformations* Transforms = NULL;
	WorldTransform->Lock((void**)&Transforms);

		Transforms->WorldTransform = RenderCommand->WorldTransform;

	WorldTransform->Unlock();

	bool Result = false;
	switch (Stage->GetStageType())
	{
		default:
		case ZE_RST_NONE:
			zeWarning("Cannot setup pass, unknown render stage type.");
			break;
		case ZE_RST_GEOMETRY:
			Result = SetupPassGeometry(PassId, Stage, RenderCommand);
			break;
		case ZE_RST_SHADOW:
			Result = SetupPassShadow(PassId, Stage, RenderCommand);
			break;
		case ZE_RST_ACCUMULATION:
			Result = SetupPassAccumulation(PassId, Stage, RenderCommand);
			break;
		case ZE_RST_TRANSPARENT:
			Result = SetupPassTransparent(PassId, Stage, RenderCommand);
			break;
	}

	return Result;
}

ZEMaterialDefault* ZEMaterialDefault::CreateInstance()
{
	return new ZEMaterialDefault();
}

ZEConstantBuffer* ZEMaterialDefault::WorldTransform = NULL;
ZEMaterialDefault::ZEMaterialDefault()
{
	ZCull = true;
	ZWrite = true;
	TwoSided = false;
	Wireframe = false;

	ShadowCaster = false;
	ShadowReciever = false;
	
	Components = 0;
	OldComponents = 0;

	TransparencyMode = ZE_TM_NONE;
	OpacityComponent = ZE_OC_CONSTANT;

	ShaderShadowPassVS = NULL;
	ShaderShadowPassPS = NULL;
	ShaderGeometryPassVS = NULL;
	ShaderGeometryPassPS = NULL;
	ShaderAccumulationPassVS = NULL;
	ShaderAccumulationPassPS = NULL;
	ShaderTransparentPassVS = NULL;
	ShaderTransparentPassPS = NULL;

	MaterialProperties = NULL;

	TextureBaseMap = NULL;
	TextureNormalMap = NULL;
	TextureParallaxMap = NULL;
	TextureSpecularMap = NULL;
	TextureEmissiveMap = NULL;
	TextureOpacityMap = NULL;
	TextureDetailBaseMap = NULL;
	TextureDetailNormalMap = NULL;
	TextureEnvironmentMap = NULL;

	SupportedStages = ZE_RST_SHADOW | ZE_RST_GEOMETRY | ZE_RST_ACCUMULATION | ZE_RST_TRANSPARENT | ZE_RST_SHADOW;
	EnableStage(SupportedStages);
	
	SamplerBaseMap.SetAddressU(ZE_TAM_WRAP);
	SamplerBaseMap.SetAddressV(ZE_TAM_WRAP);
	SamplerBaseMap.SetAddressW(ZE_TAM_WRAP);
	SamplerBaseMap.SetMagFilter(ZE_TFM_LINEAR);
	SamplerBaseMap.SetMinFilter(ZE_TFM_LINEAR);
	SamplerBaseMap.SetMipFilter(ZE_TFM_ANISOTROPY);
	
	SamplerNormalMap.SetAddressU(ZE_TAM_WRAP);
	SamplerNormalMap.SetAddressV(ZE_TAM_WRAP);
	SamplerNormalMap.SetAddressW(ZE_TAM_WRAP);
	SamplerNormalMap.SetMagFilter(ZE_TFM_LINEAR);
	SamplerNormalMap.SetMinFilter(ZE_TFM_LINEAR);
	SamplerNormalMap.SetMipFilter(ZE_TFM_ANISOTROPY);

	SamplerParallaxMap.SetAddressU(ZE_TAM_WRAP);
	SamplerParallaxMap.SetAddressV(ZE_TAM_WRAP);
	SamplerParallaxMap.SetAddressW(ZE_TAM_WRAP);
	SamplerParallaxMap.SetMagFilter(ZE_TFM_LINEAR);
	SamplerParallaxMap.SetMinFilter(ZE_TFM_LINEAR);
	SamplerParallaxMap.SetMipFilter(ZE_TFM_ANISOTROPY);

	SamplerSpecularMap.SetAddressU(ZE_TAM_WRAP);
	SamplerSpecularMap.SetAddressV(ZE_TAM_WRAP);
	SamplerSpecularMap.SetAddressW(ZE_TAM_WRAP);
	SamplerSpecularMap.SetMagFilter(ZE_TFM_LINEAR);
	SamplerSpecularMap.SetMinFilter(ZE_TFM_LINEAR);
	SamplerSpecularMap.SetMipFilter(ZE_TFM_ANISOTROPY);

	SamplerEmissiveMap.SetAddressU(ZE_TAM_WRAP);
	SamplerEmissiveMap.SetAddressV(ZE_TAM_WRAP);
	SamplerEmissiveMap.SetAddressW(ZE_TAM_WRAP);
	SamplerEmissiveMap.SetMagFilter(ZE_TFM_LINEAR);
	SamplerEmissiveMap.SetMinFilter(ZE_TFM_LINEAR);
	SamplerEmissiveMap.SetMipFilter(ZE_TFM_ANISOTROPY);

	SamplerOpacityMap.SetAddressU(ZE_TAM_WRAP);
	SamplerOpacityMap.SetAddressV(ZE_TAM_WRAP);
	SamplerOpacityMap.SetAddressW(ZE_TAM_WRAP);
	SamplerOpacityMap.SetMagFilter(ZE_TFM_LINEAR);
	SamplerOpacityMap.SetMinFilter(ZE_TFM_LINEAR);
	SamplerOpacityMap.SetMipFilter(ZE_TFM_ANISOTROPY);

	SamplerDetailBaseMap.SetAddressU(ZE_TAM_WRAP);
	SamplerDetailBaseMap.SetAddressV(ZE_TAM_WRAP);
	SamplerDetailBaseMap.SetAddressW(ZE_TAM_WRAP);
	SamplerDetailBaseMap.SetMagFilter(ZE_TFM_LINEAR);
	SamplerDetailBaseMap.SetMinFilter(ZE_TFM_LINEAR);
	SamplerDetailBaseMap.SetMipFilter(ZE_TFM_ANISOTROPY);

	SamplerDetailNormalMap.SetAddressU(ZE_TAM_WRAP);
	SamplerDetailNormalMap.SetAddressV(ZE_TAM_WRAP);
	SamplerDetailNormalMap.SetAddressW(ZE_TAM_WRAP);
	SamplerDetailNormalMap.SetMagFilter(ZE_TFM_LINEAR);
	SamplerDetailNormalMap.SetMinFilter(ZE_TFM_LINEAR);
	SamplerDetailNormalMap.SetMipFilter(ZE_TFM_ANISOTROPY);
	
	SamplerEnvironmentMap.SetAddressU(ZE_TAM_WRAP);
	SamplerEnvironmentMap.SetAddressV(ZE_TAM_WRAP);
	SamplerEnvironmentMap.SetAddressW(ZE_TAM_WRAP);
	SamplerEnvironmentMap.SetMagFilter(ZE_TFM_LINEAR);
	SamplerEnvironmentMap.SetMinFilter(ZE_TFM_LINEAR);
	SamplerEnvironmentMap.SetMipFilter(ZE_TFM_ANISOTROPY);
	
	InitializeBuffers();
}

ZEMaterialDefault::~ZEMaterialDefault()
{
	DestroyBuffers();
	DestroyShaders();
}
