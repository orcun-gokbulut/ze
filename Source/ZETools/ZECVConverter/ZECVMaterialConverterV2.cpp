//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZECVMaterialConverterV2.cpp
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

#include "ZECVMaterialConverterV2.h"

#include "ZEError.h"
#include "ZEFile/ZEPathManager.h"
#include "ZEFile/ZEFile.h"
#include "ZEML/ZEMLWriter.h"
#include "ZEML/ZEMLReader.h"

bool ZECVMaterialConverterV2::ConvertMaterial(ZEMLReaderNode* Unserializer, ZEMLWriterNode* Serializer)
{
	if (!Unserializer->IsValid())
		return false;

	ZEMLReaderNode ConfigurationNode = Unserializer->GetNode("Configuration");

	if (!ConfigurationNode.IsValid())
		return false;

	Serializer->WriteUInt8("VersionMajor", 2);
	Serializer->WriteUInt8("VersionMinor", 0);

	Serializer->WriteString("GUID", ZEGUID::Generate().ToString());
	Serializer->WriteString("Class", "ZERNStandardMaterial");

	ZEMLWriterNode PropertiesNode;
	Serializer->OpenNode("Properties", PropertiesNode);

	PropertiesNode.WriteString("Name", Unserializer->ReadString("Name"));
	PropertiesNode.WriteBool("ShadowCaster", ConfigurationNode.ReadBoolean("ShadowCaster", true));
	PropertiesNode.WriteBool("ShadowReceiver", ConfigurationNode.ReadBoolean("ShadowReceiver", true));
	PropertiesNode.WriteBool("Wireframe", ConfigurationNode.ReadBoolean("Wireframe", false));
	PropertiesNode.WriteBool("TwoSided", ConfigurationNode.ReadBoolean("TwoSided", false));
	PropertiesNode.WriteBool("SkinningEnabled", ConfigurationNode.ReadBoolean("SkinningEnabled", false));
	PropertiesNode.WriteBool("VertexColorEnabled", ConfigurationNode.ReadBoolean("VertexColorEnabled", false));
	PropertiesNode.WriteBool("ClippingPlanesEnabled", ConfigurationNode.ReadBoolean("ClippingPlanesEnabled", false));

	PropertiesNode.WriteBool("AlphaCullEnabled", ConfigurationNode.ReadBoolean("AlphaCullEnabled", false));
	PropertiesNode.WriteFloat("AlphaCullLimit", ConfigurationNode.ReadFloat("AlphaCullLimit", 0.1f));
	PropertiesNode.WriteBool("TransparencyEnabled", ConfigurationNode.ReadBoolean("TransparencyEnabled", ConfigurationNode.ReadBoolean("Transparant", false))); 
	PropertiesNode.WriteInt32("TransparencyMode", ConfigurationNode.ReadInt32("TransparencyMode", 0));

	PropertiesNode.WriteBool("DiffuseEnabled", ConfigurationNode.ReadBoolean("DiffuseEnabled", false));
	PropertiesNode.WriteVector3("DiffuseColor", ConfigurationNode.ReadVector3("DiffuseColor"));
	PropertiesNode.WriteFloat("DiffuseFactor", ConfigurationNode.ReadFloat("DiffuseFactor", 1.0f));

	PropertiesNode.WriteBool("AmbientEnabled", ConfigurationNode.ReadBoolean("AmbientEnabled", false));
	PropertiesNode.WriteBool("SceneAmbientEnabled", ConfigurationNode.ReadBoolean("SceneAmbientEnabled", true));
	PropertiesNode.WriteVector3("AmbientColor", ConfigurationNode.ReadVector3("AmbientColor"));
	PropertiesNode.WriteFloat("AmbientFactor", ConfigurationNode.ReadFloat("AmbientFactor", 1.0f));

	ZEString BaseMap = ConfigurationNode.ReadString("BaseMap");
	PropertiesNode.WriteBool("BaseMapEnabled", ConfigurationNode.ReadBoolean("BaseMapEnabled", !BaseMap.IsEmpty()));
	PropertiesNode.WriteString("BaseMap", BaseMap);

	PropertiesNode.WriteBool("SpecularEnabled", ConfigurationNode.ReadBoolean("SpecularEnabled", false));
	PropertiesNode.WriteVector3("SpecularColor", ConfigurationNode.ReadVector3("SpecularColor"));
	PropertiesNode.WriteFloat("SpecularFactor", ConfigurationNode.ReadFloat("SpecularFactor", 1.0f));
	PropertiesNode.WriteFloat("SpecularShininess", ConfigurationNode.ReadFloat("SpecularShininess", 1.0f));

	ZEString SpecularMap = ConfigurationNode.ReadString("SpecularMap");
	PropertiesNode.WriteBool("SpecularMapEnabled", ConfigurationNode.ReadBoolean("SpecularMapEnabled", !SpecularMap.IsEmpty()));
	PropertiesNode.WriteString("SpecularMap", SpecularMap);

	ZEString SpecularGlossMap = ConfigurationNode.ReadString("SpecularGlossMap");
	PropertiesNode.WriteBool("SpecularGlossMapEnabled", ConfigurationNode.ReadBoolean("SpecularGlossMapEnabled", !SpecularGlossMap.IsEmpty()));
	PropertiesNode.WriteString("SpecularGlossMap", SpecularGlossMap);

	ZEString NormalMap = ConfigurationNode.ReadString("NormalMap");
	PropertiesNode.WriteBool("NormalMapEnabled", ConfigurationNode.ReadBoolean("NormalMapEnabled", !NormalMap.IsEmpty()));
	PropertiesNode.WriteString("NormalMap", NormalMap);

	ZEString HeightMap = ConfigurationNode.ReadString("HeightMap");
	PropertiesNode.WriteBool("HeightMapEnabled", ConfigurationNode.ReadBoolean("HeightMapEnabled", !HeightMap.IsEmpty()));
	PropertiesNode.WriteString("HeightMap", HeightMap);
	PropertiesNode.WriteInt32("HeightMapTechnique", ConfigurationNode.ReadInt32("HeightMapTechnique", 0));
	PropertiesNode.WriteFloat("HeightMapOffset", ConfigurationNode.ReadFloat("HeightMapOffset", 0.0f));
	PropertiesNode.WriteFloat("HeightMapScale", ConfigurationNode.ReadFloat("HeightMapScale", 1.0f));

	PropertiesNode.WriteBool("EmissiveEnabled", ConfigurationNode.ReadBoolean("EmmisiveEnabled", false));
	PropertiesNode.WriteVector3("EmissiveColor", ConfigurationNode.ReadVector3("EmmisiveColor"));
	PropertiesNode.WriteFloat("EmissiveFactor", ConfigurationNode.ReadFloat("EmmisiveFactor", 1.0f));

	ZEString EmissiveMap = ConfigurationNode.ReadString("EmissiveMap");
	PropertiesNode.WriteBool("EmissiveMapEnabled", ConfigurationNode.ReadBoolean("EmissiveMapEnabled", !EmissiveMap.IsEmpty()));
	PropertiesNode.WriteString("EmissiveMap", EmissiveMap);

	PropertiesNode.WriteFloat("Opacity", ConfigurationNode.ReadFloat("Opacity", 1.0f));
	ZEString OpacityMap = ConfigurationNode.ReadString("OpacityMap");
	PropertiesNode.WriteBool("OpacityMapEnabled", ConfigurationNode.ReadBoolean("OpacityMapEnabled", !OpacityMap.IsEmpty()));
	PropertiesNode.WriteString("OpacityMap", OpacityMap);

	ZEString SubSurfaceScatteringMap = ConfigurationNode.ReadString("SubSurfaceScatteringMap");
	PropertiesNode.WriteBool("SubSurfaceScatteringMapEnabled", ConfigurationNode.ReadBoolean("SubSurfaceScatteringMapEnabled", !SubSurfaceScatteringMap.IsEmpty()));
	PropertiesNode.WriteString("SubSurfaceScatteringMap", SubSurfaceScatteringMap);
	PropertiesNode.WriteFloat("SubSurfaceScatteringFactor", ConfigurationNode.ReadFloat("SubSurfaceScatteringFactor", 0.0f));

	PropertiesNode.WriteBool("ReflectionEnabled", ConfigurationNode.ReadBoolean("ReflectionEnabled", false));
	PropertiesNode.WriteVector3("ReflectionColor", ConfigurationNode.ReadVector3("ReflectionColor"));
	PropertiesNode.WriteFloat("ReflectionFactor", ConfigurationNode.ReadFloat("ReflectionFactor", 1.0f));

	PropertiesNode.WriteBool("RefractionEnabled", ConfigurationNode.ReadBoolean("RefractionEnabled", false));
	PropertiesNode.WriteVector3("RefractionColor", ConfigurationNode.ReadVector3("RefractionColor"));
	PropertiesNode.WriteFloat("RefractionFactor", ConfigurationNode.ReadFloat("RefractionFactor", 1.0f));
	PropertiesNode.WriteFloat("RefractionIndex", ConfigurationNode.ReadFloat("RefractionIndex", 1.0f));

	ZEString EnvironmentMap = ConfigurationNode.ReadString("EnvironmentMap");
	PropertiesNode.WriteBool("EnvironmentMapEnabled", ConfigurationNode.ReadBoolean("EnvironmentMapEnabled", !EnvironmentMap.IsEmpty()));
	PropertiesNode.WriteString("EnvironmentMap", EnvironmentMap);

	ZEString DetailBaseMap = ConfigurationNode.ReadString("DetailBaseMap");
	PropertiesNode.WriteBool("DetailBaseMapEnabled", ConfigurationNode.ReadBoolean("DetailBaseMapEnabled", !DetailBaseMap.IsEmpty()));
	PropertiesNode.WriteString("DetailBaseMap", DetailBaseMap);
	PropertiesNode.WriteVector3("DetailBaseMapColor", ConfigurationNode.ReadVector3("DetailBaseMapColor", ZEVector3::One));
	PropertiesNode.WriteVector2("DetailBaseMapTiling", ConfigurationNode.ReadVector2("DetailBaseMapTiling", ZEVector2::One));
	PropertiesNode.WriteFloat("DetailBaseMapAttenuationStart", ConfigurationNode.ReadFloat("DetailBaseMapAttenuationStart", 10.0f));
	PropertiesNode.WriteFloat("DetailBaseMapAttenuationFactor", ConfigurationNode.ReadFloat("DetailBaseMapAttenuationFactor", 0.01f));

	ZEString DetailNormalMap = ConfigurationNode.ReadString("DetailNormalMap");
	PropertiesNode.WriteBool("DetailNormalMapEnabled", ConfigurationNode.ReadBoolean("DetailNormalMapEnabled", !DetailNormalMap.IsEmpty()));
	PropertiesNode.WriteString("DetailNormalMap", DetailNormalMap);
	PropertiesNode.WriteFloat("DetailNormalMapFactor", ConfigurationNode.ReadFloat("DetailNormalMapFactor", 1.0f));
	PropertiesNode.WriteVector2("DetailNormalMapTiling", ConfigurationNode.ReadVector2("DetailNormalMapTiling", ZEVector2::One));
	PropertiesNode.WriteFloat("DetailNormalMapAttenuationStart", ConfigurationNode.ReadFloat("DetailNormalMapAttenuationStart", 10.0f));
	PropertiesNode.WriteFloat("DetailNormalMapAttenuationFactor", ConfigurationNode.ReadFloat("DetailNormalMapAttenuationFactor", 0.01f));

	PropertiesNode.CloseNode();

	return true;
}

ZECVVersion ZECVMaterialConverterV2::GetSourceVersion() const
{
	ZECVVersion Version;
	Version.Major = 1;
	Version.Minor = 0;
	return Version;
}

ZECVVersion ZECVMaterialConverterV2::GetDestinationVersion() const
{
	ZECVVersion Version;
	Version.Major = 2;
	Version.Minor = 0;
	return Version;
}

ZECVResult ZECVMaterialConverterV2::Convert(const ZEString& SourceFileName, const ZEString& DestinationFileName)
{
	ZEMLReader Unserializer;
	if (!Unserializer.Open(SourceFileName))
	{
		zeError("Cannot open source material file. File Name: \"%s\".", SourceFileName.ToCString());
		return ZECV_R_FAILED;
	}

	ZEMLReaderNode SourceMaterialNode = Unserializer.GetRootNode();
	if (SourceMaterialNode.GetName() != "Material")
	{
		zeError("Cannot load material. Corrupted model file. File Name: \"%s\".", SourceFileName.ToCString());
		Unserializer.Close();
		return ZECV_R_FAILED;
	}

	ZEMLWriter Serializer;
	if (!Serializer.Open(DestinationFileName))
	{
		zeError("Cannot open destination material file. File Name: \"%s\".", DestinationFileName.ToCString());
		return ZECV_R_FAILED;
	}
	
	ZEMLWriterNode DestinationMaterialNode;
	Serializer.OpenRootNode("ZERNMaterial", DestinationMaterialNode);

	DestinationMaterialNode.WriteUInt8("MajorVersion", 2);
	DestinationMaterialNode.WriteUInt8("MinorVersion", 0);

	if (!ConvertMaterial(&SourceMaterialNode, &DestinationMaterialNode))
	{
		zeError("Conversation has failed. File Name: \"%s\".", DestinationFileName.ToCString());
		return ZECV_R_FAILED;
	}
	
	DestinationMaterialNode.CloseNode();

	Serializer.Close();
	Unserializer.Close();

	return ZECV_R_DONE;
}
