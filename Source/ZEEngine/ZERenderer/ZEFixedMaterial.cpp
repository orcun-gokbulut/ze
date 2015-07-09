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

#include "ZEFixedMaterial.h"
#include <Memory.h>

#ifndef NULL
#define NULL 0
#endif

#include "ZEGraphics/ZEGRGraphicsModule.h"
#include "ZEMaterialComponents.h"
#include "ZETexture/ZETexture2DResource.h"
#include "ZETexture/ZETextureCubeResource.h"
#include "ZEGraphics/ZEGRTexture2D.h"
#include "ZEGraphics/ZEGRTextureCube.h"
#include "ZEML/ZEMLReader.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEFile/ZEFileInfo.h"

ZEFixedMaterial::ZEFixedMaterial()
{
	Constants.AmbientColor = ZEVector3::One;
	Constants.DiffuseColor = ZEVector3::One;
	Constants.SpecularColor = ZEVector3::One;
	Constants.EmmisiveColor = ZEVector3::One;
	Constants.DetailMapTiling = ZEVector2::One;
	Constants.Opacity = 1.0f;
	Constants.DistortionFactor = 1.0f;
	Constants.SpecularShininess = 1.0f;
	Constants.ReflectionFactor = 1.0f;
	Constants.RefractionIndex = 1.0f;
	Constants.RefractionFactor = 1.0f;
	Constants.LightMapFactor = 1.0f;
	Constants.SubSurfaceScatteringFactor = 0.0f;
	Constants.AlphaCullLimit = 1.0f;
	Constants.TwoSided = false;
	Constants.Wireframe = false;
	Constants.GlobalAmbientEnabled = true;

	TransparancyMode = ZE_MTM_NONE;
	AmbientFactor = 0.0f;
	DiffuseFactor = 1.0f;
	SpecularFactor = 0.0f;
	EmmisiveFactor = 0.0f;
	AmbientColor = ZEVector3::One;
	DiffuseColor = ZEVector3::One;
	SpecularColor = ZEVector3::One;
	EmmisiveColor = ZEVector3::One;
	OpacityComponent = ZE_MOC_BASE_MAP_ALPHA;
	
	VertexColorEnabled = false;
	AlphaCullEnabled = false;
	AmbientEnabled = true;
	DiffuseEnabled = true;
	SpecularEnabled = false;
	EmmisiveEnabled = false;
	NormalMapEnabled = false;
	DisplacementEnabled = false;
	OpacityEnabled = false;
	EnvironmentMapEnabled = false;
	ReflectionEnabled = false;
	RefractionEnabled = false;
	DetailBaseMapEnabled = false;
	DetailNormalMapEnabled = false;
	DistortionEnabled = false;
	SSAOEnabled = false;

	FileName = "";
}

ZEFixedMaterial::~ZEFixedMaterial()
{

}

void ZEFixedMaterial::SetName(const ZEString& Name)
{
	this->Name = Name;
}

const ZEString& ZEFixedMaterial::GetName() const
{
	return Name;
}

const ZEString&	ZEFixedMaterial::GetFileName() const
{
	return FileName;
}

void ZEFixedMaterial::SetTwoSided(bool Enable)
{
	Constants.TwoSided = Enable;
}

bool ZEFixedMaterial::GetTwoSided() const
{
	return Constants.TwoSided;
}

void ZEFixedMaterial::SetWireframe(bool Enable)
{
	Constants.Wireframe = Enable;
}

bool ZEFixedMaterial::GetWireframe() const
{
	return Constants.Wireframe;
}

void ZEFixedMaterial::SetTransparancyMode(ZEMaterialTransparancyMode Mode)
{
	TransparancyMode = Mode;
	Update();
}

ZEMaterialTransparancyMode ZEFixedMaterial::GetTransparancyMode() const
{
	return TransparancyMode;
}

void ZEFixedMaterial::SetAlphaCullEnabled(bool Enabled)
{
	if (AlphaCullEnabled == Enabled)
		return;

	AlphaCullEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetAlphaCullEnabled() const
{
	return AlphaCullEnabled;
}

void ZEFixedMaterial::SetAlphaCullLimit(float Limit)
{
	Constants.AlphaCullLimit = Limit;
}

float ZEFixedMaterial::GetAlphaCullLimit() const
{
	return Constants.AlphaCullLimit;
}

void ZEFixedMaterial::SetAmbientEnabled(bool Enabled)
{
	if (AmbientEnabled == Enabled)
		return;

	AmbientEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetAmbientEnabled() const
{
	return AmbientEnabled;
}

void ZEFixedMaterial::SetAmbientColor(const ZEVector3& Color)
{
	AmbientColor = Color;
	Constants.AmbientColor = AmbientFactor * AmbientColor;
}

const ZEVector3& ZEFixedMaterial::GetAmbientColor() const
{
	return AmbientColor;
}

void ZEFixedMaterial::SetAmbientFactor(float Factor)
{
	AmbientFactor = Factor;
	Constants.AmbientColor = AmbientFactor * AmbientColor;
}

float ZEFixedMaterial::GetAmbientFactor() const
{
	return AmbientFactor;
}

void ZEFixedMaterial::SetGlobalAmbientEnabled(bool Enabled)
{
	Constants.GlobalAmbientEnabled = Enabled;
}

bool ZEFixedMaterial::GetGlobalAmbientEnabled() const
{
	return Constants.GlobalAmbientEnabled;
}

void ZEFixedMaterial::SetDiffuseEnabled(bool Enabled)
{
	if (DiffuseEnabled == Enabled)
		return;

	DiffuseEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetDiffuseEnabled() const
{
	return DiffuseEnabled;
}

void ZEFixedMaterial::SetDiffuseColor(const ZEVector3& Color)
{
	DiffuseColor = Color;
	Constants.DiffuseColor = DiffuseFactor * DiffuseColor;
}

const ZEVector3& ZEFixedMaterial::GetDiffuseColor() const
{
	return DiffuseColor;
}

void ZEFixedMaterial::SetDiffuseFactor(float Factor)
{
	DiffuseFactor = Factor;
	Constants.DiffuseColor = DiffuseFactor * DiffuseColor;
}

float ZEFixedMaterial::GetDiffuseFactor() const
{
	return DiffuseFactor;
}

void ZEFixedMaterial::SetDiffuseSubSurfaceScatteringFactor(float Factor)
{
	Constants.SubSurfaceScatteringFactor = Factor;
}

float ZEFixedMaterial::GetDiffuseSubSurfaceScatteringFactor() const
{
	return Constants.SubSurfaceScatteringFactor;
}

void ZEFixedMaterial::SetSpecularEnabled(bool Enabled)
{
	if (SpecularEnabled == Enabled)
		return;

	SpecularEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetSpecularEnabled() const
{
	return SpecularEnabled;
}

void ZEFixedMaterial::SetSpecularColor(const ZEVector3& Color)
{
	SpecularColor = Color;
}

const ZEVector3& ZEFixedMaterial::GetSpecularColor() const
{
	return SpecularColor;
}

void ZEFixedMaterial::SetSpecularFactor(float Factor)
{
	SpecularFactor = Factor;
	Constants.SpecularColor = SpecularFactor * SpecularColor;
}

float ZEFixedMaterial::GetSpecularFactor() const
{
	return SpecularFactor;
}

void ZEFixedMaterial::SetSpecularShininess(float Shininess)
{
	Constants.SpecularShininess = Shininess;
}

float ZEFixedMaterial::GetSpecularShininess() const
{
	return Constants.SpecularShininess;
}

void ZEFixedMaterial::SetEmmisiveEnabled(bool Enabled)
{
	if (EmmisiveEnabled == Enabled)
		return;

	SpecularEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetEmmisiveEnabled() const
{
	return EmmisiveEnabled;
}

void ZEFixedMaterial::SetEmmisiveFactor(float Factor)
{
	EmmisiveFactor = Factor;
	Constants.EmmisiveColor = EmmisiveFactor * EmmisiveColor;
}

float ZEFixedMaterial::GetEmmisiveFactor() const
{
	return EmmisiveFactor;
}

void ZEFixedMaterial::SetEmmisiveColor(const ZEVector3& Color)
{
	EmmisiveColor = Color;
	Constants.EmmisiveColor = EmmisiveFactor * EmmisiveColor;
}

const ZEVector3& ZEFixedMaterial::GetEmmisiveColor() const
{
	return EmmisiveColor;
}

void ZEFixedMaterial::SetNormalMapEnabled(bool Enabled)
{
	if (NormalMapEnabled == Enabled)
		return;

	NormalMapEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetNormalMapEnabled() const
{
	return NormalMapEnabled;
}

void ZEFixedMaterial::SetDisplacementMapEnabled(bool Enabled)
{
	if (DisplacementEnabled == Enabled)
		return;

	DisplacementEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetDisplacementMapEnabled() const
{
	return DisplacementEnabled;
}

void ZEFixedMaterial::SetOpacityEnabled(bool Enabled)
{
	if (OpacityEnabled == Enabled)
		return;

	OpacityEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetOpacityEnabled() const
{
	return OpacityEnabled;
}

void ZEFixedMaterial::SetOpacity(float Value)
{
	Constants.Opacity = Value;
}

float ZEFixedMaterial::GetOpacity() const
{
	return Constants.Opacity;
}

void ZEFixedMaterial::SetOpacityComponent(ZEMaterialOpacityComponent Component)
{
	if (OpacityComponent == Component)
		return;

	OpacityComponent = Component;

	Update();
}

ZEMaterialOpacityComponent ZEFixedMaterial::GetOpacityComponent() const
{
	return OpacityComponent;
}

void ZEFixedMaterial::SetOpacityMap(const ZERNSampler& Texture)
{
	OpacityTexture = Texture;
}

const ZERNSampler& ZEFixedMaterial::GetOpacityMap() const
{
	return OpacityTexture;
}

void ZEFixedMaterial::SetEnvironmentMap(const ZERNSampler& Texture)
{
	EnvironmentTexture = Texture;
}

const ZERNSampler& ZEFixedMaterial::GetEnvironmentMap() const
{
	return EnvironmentTexture;
}

void ZEFixedMaterial::SetReflectionEnabled(bool Enabled)
{
	if (ReflectionEnabled == Enabled)
		return;

	ReflectionEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetReflectionEnabled() const
{
	return ReflectionEnabled;
}

void ZEFixedMaterial::SetReflectionFactor(float Factor)
{
	Constants.ReflectionFactor = Factor;
}

float ZEFixedMaterial::GetReflectionFactor() const
{
	return Constants.ReflectionFactor;
}

void ZEFixedMaterial::SetRefractionEnabled(bool Enabled)
{
	if (RefractionEnabled == Enabled)
		return;

	RefractionEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetRefractionEnabled() const
{
	return RefractionEnabled;
}

void ZEFixedMaterial::SetRefractionIndex(float Index)
{
	Constants.RefractionIndex = Index;
}

float ZEFixedMaterial::GetRefractionIndex() const
{
	return Constants.RefractionIndex;
}

void ZEFixedMaterial::SetRefractionFactor(float Factor)
{
	Constants.RefractionFactor = Factor;
}

float ZEFixedMaterial::GetRefractionFactor() const
{
	return Constants.RefractionFactor;
}

void ZEFixedMaterial::SetDetailMapTiling(const ZEVector2& Tiling)
{
	Constants.DetailMapTiling = Tiling;
}

const ZEVector2& ZEFixedMaterial::GetDetailMapTiling() const
{
	return Constants.DetailMapTiling;
}

void ZEFixedMaterial::SetDetailBaseMapEnabled(bool Enabled)
{
	if (DetailBaseMapEnabled == Enabled)
		return;

	DetailBaseMapEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetDetailBaseMapEnabled() const
{
	return DetailBaseMapEnabled;
}

void ZEFixedMaterial::SetDetailBaseMap(const ZERNSampler& Texture)
{
	DetailBaseTexture = Texture;
}

const ZERNSampler& ZEFixedMaterial::GetDetailBaseMap() const
{
	return DetailBaseTexture;
}

void ZEFixedMaterial::SetDetailNormalMapEnabled(bool Enabled)
{
	if (DetailNormalMapEnabled == Enabled)
		return;

	DetailBaseMapEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetDetailNormalMapEnabled() const
{
	return DetailNormalMapEnabled;
}

void ZEFixedMaterial::SetDetailNormalMap(const ZERNSampler& Texture)
{
	DetailNormalTexture = Texture;
}

const ZERNSampler& ZEFixedMaterial::GetDetailNormalMap() const
{
	return DetailNormalTexture;
}

void ZEFixedMaterial::SetDistortionMapEnabled(bool Enabled)
{
	if (DistortionEnabled == Enabled)
		return;

	DistortionEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetDistortionMapEnabled() const
{
	return DistortionEnabled;
}

void ZEFixedMaterial::SetDistortionFactor(float Factor)
{
	Constants.DistortionFactor = Factor;
}

float ZEFixedMaterial::GetDistortionFactor() const
{
	return Constants.DistortionFactor;
}

void ZEFixedMaterial::SetDistortionMap(const ZERNSampler& Texture)
{
	DistortionTexture = Texture;
}

const ZERNSampler& ZEFixedMaterial::GetDistortionMap() const
{
	return DistortionTexture; 
}

void ZEFixedMaterial::SetVertexColorEnabled(bool Enabled)
{
	if (VertexColorEnabled == Enabled)
		return;

	VertexColorEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetVertexColorEnabled()
{
	return VertexColorEnabled;
}

void ZEFixedMaterial::SetSSAOEnabled(bool Enabled)
{
	if (SSAOEnabled == Enabled)
		return;

	SSAOEnabled = Enabled;

	Update();
}

bool ZEFixedMaterial::GetSSAOEnabled() const
{
	return SSAOEnabled;
}

void ZEFixedMaterial::Tick(float ElapsedTime)
{

}

ZEFixedMaterial* ZEFixedMaterial::CreateInstance()
{
	return new ZEFixedMaterial;
}

void ZEFixedMaterial::WriteToFile(const ZEString& FilePath)
{
	ZEFile File;
	if(!File.Open(FilePath, ZE_FOM_WRITE, ZE_FCM_OVERWRITE))
		zeError("Can not open given file. File : %s", FilePath.ToCString());

	ZEMLWriter MaterialWriter;
	MaterialWriter.Open(&File);

	ZEMLWriterNode MaterialNode = MaterialWriter.WriteRootNode("ZEMaterial");

	MaterialNode.WriteString("Name", GetName().ToCString());

	ZEMLWriterNode ConfigurationNode = MaterialNode.OpenSubNode("Configuration");

	ConfigurationNode.WriteString("Name", "Default"); //Will be changed when configuration is implemented.
	ConfigurationNode.WriteBool("Wireframe", GetWireframe());
	ConfigurationNode.WriteBool("TwoSided", GetTwoSided());
	ConfigurationNode.WriteInt32("TransparencyMode", (ZEInt32)GetTransparancyMode());
	ConfigurationNode.WriteBool("LightningEnabled", GetLightningEnabled());
	ConfigurationNode.WriteBool("ShadowReceiver", GetShadowReceiver());
	ConfigurationNode.WriteBool("ShadowCaster", GetShadowCaster());

	ConfigurationNode.WriteBool("AmbientEnabled", GetAmbientEnabled());
	ConfigurationNode.WriteBool("GlobalAmbientEnabled", GetGlobalAmbientEnabled());
	ConfigurationNode.WriteBool("DiffuseEnabled", GetDiffuseEnabled());
	ConfigurationNode.WriteBool("NormalMapEnabled", GetNormalMapEnabled());
	ConfigurationNode.WriteBool("DisplacementMapEnabled", GetDisplacementMapEnabled());
	ConfigurationNode.WriteBool("SpecularEnabled", GetSpecularEnabled());
	ConfigurationNode.WriteBool("EmmisiveEnabled", GetEmmisiveEnabled());
	ConfigurationNode.WriteBool("OpacityEnabled", GetOpacityEnabled());
	ConfigurationNode.WriteBool("DetailBaseMapEnabled", GetDetailBaseMapEnabled());
	ConfigurationNode.WriteBool("DetailNormalMapEnabled", GetDetailNormalMapEnabled());
	ConfigurationNode.WriteBool("ReflectionEnabled", GetReflectionEnabled());
	ConfigurationNode.WriteBool("RefractionEnabled", GetRefractionEnabled());
	ConfigurationNode.WriteBool("DistortionMapEnabled", GetDistortionMapEnabled());
	ConfigurationNode.WriteBool("AlphaCullEnabled", GetAlphaCullEnabled());
	ConfigurationNode.WriteBool("VertexColorEnabled", GetVertexColorEnabled());

	ConfigurationNode.WriteFloat("Opacity", GetOpacity());
	ConfigurationNode.WriteInt32("OpacityComponent", (ZEInt32)GetOpacityComponent());
	ConfigurationNode.WriteFloat("AlphaCullLimit", GetAlphaCullLimit());
	ConfigurationNode.WriteVector2("DetailMapTiling", GetDetailMapTiling());
	ConfigurationNode.WriteFloat("SpecularShininess", GetSpecularShininess());
	ConfigurationNode.WriteFloat("SubSurfaceScatteringFactor", GetDiffuseSubSurfaceScatteringFactor());

	ConfigurationNode.WriteVector3("AmbientColor", GetAmbientColor());
	ConfigurationNode.WriteFloat("AmbientFactor", GetAmbientFactor());
	ConfigurationNode.WriteVector3("SpecularColor", GetSpecularColor());
	ConfigurationNode.WriteFloat("SpecularFactor", GetSpecularFactor());
	ConfigurationNode.WriteVector3("DiffuseColor", GetDiffuseColor());
	ConfigurationNode.WriteFloat("DiffuseFactor", GetDiffuseFactor());
	ConfigurationNode.WriteVector3("EmmisiveColor", GetEmmisiveColor());
	ConfigurationNode.WriteFloat("EmmisiveFactor", GetEmmisiveFactor());
	ConfigurationNode.WriteFloat("RefractionIndex", GetRefractionIndex());
	ConfigurationNode.WriteFloat("RefractionFactor", GetRefractionFactor());
	ConfigurationNode.WriteFloat("ReflectionFactor", GetReflectionFactor());

	ConfigurationNode.OpenSubNode("BaseMap"),
	BaseTexture.Write(ConfigurationNode);
	ConfigurationNode.CloseNode();

	ConfigurationNode.OpenSubNode("NormalMap");
	NormalTexture.Write(ConfigurationNode);
	ConfigurationNode.CloseNode();

	ConfigurationNode.OpenSubNode("DisplacementMap");
	ParallaxTexture.Write(ConfigurationNode);
	ConfigurationNode.CloseNode();

	ConfigurationNode.OpenSubNode("SpecularMap");
	SpecularTexture.Write(ConfigurationNode);
	ConfigurationNode.CloseNode();

	ConfigurationNode.OpenSubNode("EmmisiveMap");
	EmmisiveTexture.Write(ConfigurationNode);
	ConfigurationNode.CloseNode();

	ConfigurationNode.OpenSubNode("OpacityMap");
	OpacityTexture.Write(ConfigurationNode);
	ConfigurationNode.CloseNode();

	ConfigurationNode.OpenSubNode("DetailBaseMap");
	DetailBaseTexture;
	ConfigurationNode.CloseNode();

	ConfigurationNode.OpenSubNode("DetailNormalMap");
	DetailNormalTexture.Write(ConfigurationNode);
	ConfigurationNode.CloseNode();

	ConfigurationNode.OpenSubNode("EnvironmentMap");
	EnvironmentTexture.Write(ConfigurationNode);
	ConfigurationNode.CloseNode();

	ConfigurationNode.OpenSubNode("DistortionMap");
	DistortionTexture.Write(ConfigurationNode);
	ConfigurationNode.CloseNode();
	
	ConfigurationNode.CloseNode();
	MaterialNode.CloseNode();
	MaterialWriter.Close();

	if(File.IsOpen())
		File.Close();
}

void ZEFixedMaterial::ReadFromFile(const ZEString& FilePath)
{
	ZEFile File;
	if(!File.Open(FilePath, ZE_FOM_READ, ZE_FCM_NONE))
		zeError("Can not open given file. File : %s", FilePath.ToCString());


	ZEMLReader MaterialReader;
	if (!MaterialReader.Open(&File))
		return;

	ZEMLReaderNode MaterialNode = MaterialReader.GetRootNode();

	if(!MaterialNode.IsValid())
		zeError("Can not read material file.");

	if (MaterialNode.GetName() == "Material")
		zeWarning("Old depricated ZEMaterial file version detected. Please convert this file to new version for future compability. Current Version: 1.0. Detected Version: 0.0. File Name: \"%s\".", File.GetPath().ToCString());

	SetName(MaterialNode.ReadString("Name"));

	ZESize SubNodeCount = MaterialNode.GetSubNodeCount("Configuration");

	//When Material Configuration functionality is implemented this reading mechanism should be revised, now it just loads "Default" config.

	for (ZESize I = 0; I < SubNodeCount; I++)
	{
		ZEMLReaderNode ConfigurationNode = MaterialNode.GetSubNode("Configuration", I);

		if (!ConfigurationNode.IsValid())
		{
			zeError("Cannot read material configuration");
			return;
		}

		SetWireframe(ConfigurationNode.ReadBoolean("Wireframe"));
		SetTwoSided(ConfigurationNode.ReadBoolean("TwoSided"));
		SetLightningEnabled(ConfigurationNode.ReadBoolean("LightningEnabled"));
		SetTransparancyMode((ZEMaterialTransparancyMode)ConfigurationNode.ReadInt32("TransparencyMode"));

		SetAmbientEnabled(ConfigurationNode.ReadBoolean("AmbientEnabled"));
		SetGlobalAmbientEnabled(ConfigurationNode.ReadBoolean("GlobalAmbientEnabled"));
		SetDiffuseEnabled(ConfigurationNode.ReadBoolean("DiffuseEnabled"));
		SetNormalMapEnabled(ConfigurationNode.ReadBoolean("NormalMapEnabled"));
		SetDisplacementMapEnabled(ConfigurationNode.ReadBoolean("DisplacementEnabled"));
		SetSpecularEnabled(ConfigurationNode.ReadBoolean("SpecularEnabled"));
		SetEmmisiveEnabled(ConfigurationNode.ReadBoolean("EmmisiveEnabled"));
		SetOpacityEnabled(ConfigurationNode.ReadBoolean("OpacityEnabled"));
		SetDetailBaseMapEnabled(ConfigurationNode.ReadBoolean("DetailBaseMapEnabled"));
		SetDetailNormalMapEnabled(ConfigurationNode.ReadBoolean("DetailNormalMapEnabled"));
		SetReflectionEnabled(ConfigurationNode.ReadBoolean("ReflectionEnabled"));
		SetRefractionEnabled(ConfigurationNode.ReadBoolean("RefractionEnabled"));
		SetDistortionMapEnabled(ConfigurationNode.ReadBoolean("LightMapEnabled"));
		SetAlphaCullEnabled(ConfigurationNode.ReadBoolean("AlphaCullEnabled"));
		SetVertexColorEnabled(ConfigurationNode.ReadBoolean("VertexColorEnabled"));

		SetOpacity(ConfigurationNode.ReadFloat("Opacity", 1.0f));
		SetOpacityComponent((ZEMaterialOpacityComponent)ConfigurationNode.ReadInt32("OpacityComponent"));
		SetDetailMapTiling(ConfigurationNode.ReadVector2("DetailMapTiling", ZEVector2::One));
		SetAmbientColor(ConfigurationNode.ReadVector3("AmbientColor", ZEVector3::One));
		SetAmbientFactor(ConfigurationNode.ReadFloat("AmbientFactor", 1.0f));
		SetSpecularColor(ConfigurationNode.ReadVector3("SpecularColor", ZEVector3::One));
		SetSpecularFactor(ConfigurationNode.ReadFloat("SpecularFactor", 1.0f));
		SetDiffuseColor(ConfigurationNode.ReadVector3("DiffuseColor", ZEVector3::One));
		SetDiffuseFactor(ConfigurationNode.ReadFloat("DiffuseFactor", 1.0f));
		SetEmmisiveColor(ConfigurationNode.ReadVector3("EmmisiveColor", ZEVector3::One));
		SetEmmisiveFactor(ConfigurationNode.ReadFloat("EmmisiveFactor", 1.0f));
		SetRefractionIndex(ConfigurationNode.ReadFloat("RefractionIndex", 1.0f));
		SetRefractionFactor(ConfigurationNode.ReadFloat("RefractionFactor", 1.0f));
		SetReflectionFactor(ConfigurationNode.ReadFloat("ReflectionFactor", 1.0f));
		SetDiffuseSubSurfaceScatteringFactor(ConfigurationNode.ReadFloat("SubSurfaceScatteringFactor", 0.0f));
		SetAlphaCullLimit(ConfigurationNode.ReadFloat("AlphaCullLimit", 0.5f));
		SetSpecularShininess(ConfigurationNode.ReadFloat("SpecularShininess", 1.0f));

		ZEMLReaderNode Node = ConfigurationNode.GetSubNode("BaseMap");
		BaseTexture.Read(ConfigurationNode);
		

		Node = ConfigurationNode.GetSubNode("NormalMap");
		NormalTexture.Read(ConfigurationNode);

		Node = ConfigurationNode.GetSubNode("DisplacementMap");
		ParallaxTexture.Read(ConfigurationNode);

		Node = ConfigurationNode.GetSubNode("SpecularMap");
		SpecularTexture.Read(ConfigurationNode);

		Node = ConfigurationNode.GetSubNode("EmmisiveMap");
		EmmisiveTexture.Read(ConfigurationNode);

		Node = ConfigurationNode.GetSubNode("OpacityMap");
		OpacityTexture.Read(ConfigurationNode);

		Node = ConfigurationNode.GetSubNode("DetailBaseMap");
		DetailBaseTexture;

		Node = ConfigurationNode.GetSubNode("DetailNormalMap");
		DetailNormalTexture.Read(Node);

		Node = ConfigurationNode.GetSubNode("EnvironmentMap");
		EnvironmentTexture.Read(ConfigurationNode);

		Node = ConfigurationNode.GetSubNode("DistortionMap");
		DistortionTexture.Read(ConfigurationNode);
	}

	MaterialReader.Close();

	if(File.IsOpen())
		File.Close();

	FileName = FilePath;

	UpdateMaterial();
}
