//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFLTResourceRecords.h
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

#pragma once
#ifndef __ZE_FLT_RESOURCE_RECORDS_H__
#define __ZE_FLT_RESOURCE_RECORDS_H__

#include "ZEMath/ZEVector.h"
#include "ZEEndian.h"
#include "ZEPacking.h"

ZEPackStruct(
struct ZEFLTRecordBase
{
	ZEBigEndian<ZEInt16> Opcode;
	ZEBigEndian<ZEUInt16> Length;
};)

/* PRIMARY RECORDS */

ZEPackStruct(
struct ZEFLTRecordHeader : ZEFLTRecordBase
{
	char						ID[8];

	ZEBigEndian<ZEInt32>		FormatRevisionLevel;
	ZEBigEndian<ZEInt32>		EditRevisionLevel;
	char						DateTimeofLastRevision[32];

	ZEBigEndian<ZEInt16>		NextGroupNodeIDNumber;
	ZEBigEndian<ZEInt16>		NextLODNodeIDNumber;
	ZEBigEndian<ZEInt16>		NextObjectNodeIDNumber;
	ZEBigEndian<ZEInt16>		NextFaceNodeIDNumber;

	ZEBigEndian<ZEInt16>		UnitMultiplier; //Always 1
	ZEInt8						VertexCoordinateUnits;

	ZEInt8						TextWhiteOnNewFaces;
	ZEBigEndian<ZEInt32>		Flags;

	ZEBigEndian<ZEInt32>		Reserved1[6];

	ZEBigEndian<ZEInt32>		ProjectionType;

	ZEBigEndian<ZEInt32>		Reserved2[7];

	ZEBigEndian<ZEInt16>		NextDOFNodeIDNumber;

	ZEBigEndian<ZEInt16>		VertexStorageType;

	ZEBigEndian<ZEInt32>		DatabaseOrigin;

	ZEBigEndian<double>			SouthWestDatabaseCoordinateX;
	ZEBigEndian<double>			SouthWestDatabaseCoordinateY;
	ZEBigEndian<double>			DeltaXToPlaceDatabase;
	ZEBigEndian<double>			DeltaYToPlaceDatabase;

	ZEBigEndian<ZEInt16>		NextSoundNodeIDNumber;
	ZEBigEndian<ZEInt16>		NextPathNodeIDNumber;
	ZEBigEndian<ZEInt32>		Reserved3[2];
	ZEBigEndian<ZEInt16>		NextClipNodeIDNumber;
	ZEBigEndian<ZEInt16>		NextTextNodeIDNumber;
	ZEBigEndian<ZEInt16>		NextBSPNodeIDNumber;
	ZEBigEndian<ZEInt16>		NextSwitchNodeIDNumber;

	ZEBigEndian<ZEInt32>		Reserved4;

	ZEBigEndian<double>			SouthWestCornerLatitude;
	ZEBigEndian<double>			SouthWestCornerLongitude;
	ZEBigEndian<double>			NorthEastCornerLatitude;
	ZEBigEndian<double>			NorthEastCornerLongitude;
	ZEBigEndian<double>			OriginLatitude;
	ZEBigEndian<double>			OriginLongitude;
	ZEBigEndian<double>			LambertUpperLatitude;
	ZEBigEndian<double>			LambertLowerLatitude;

	ZEBigEndian<ZEInt16>		NextLightSourceNodeIDNumber;
	ZEBigEndian<ZEInt16>		NextLightPointNodeIDNumber;
	ZEBigEndian<ZEInt16>		NextRoadNodeIDNumber;
	ZEBigEndian<ZEInt16>		NextCATNodeIDNumber;

	ZEBigEndian<ZEInt16>		Reserved5;
	ZEBigEndian<ZEInt16>		Reserved6;
	ZEBigEndian<ZEInt16>		Reserved7;
	ZEBigEndian<ZEInt16>		Reserved8;

	ZEBigEndian<ZEInt32>		EarthElipsoidModel;

	ZEBigEndian<ZEInt16>		NextAdaptiveNodeIDNumber;
	ZEBigEndian<ZEInt16>		NextCurveNodeIDNumber;

	ZEBigEndian<ZEInt16>		UTMZone;
	char						Reserved9[6];

	ZEBigEndian<double>			DeltaZtoPlaceDatabase;
	ZEBigEndian<double>			Radius; //Distance from database origin to farthest corner
	
	ZEBigEndian<ZEUInt16>		NextMeshNodeIDNumber;
	ZEBigEndian<ZEUInt16>		NextLightPointSystemIDNumber;

	ZEBigEndian<ZEInt32>		Reserved10;

	ZEBigEndian<double>			EarthMajorAxis;
	ZEBigEndian<double>			EarthMinorAxis;

};)

ZEPackStruct(
struct ZEFLTRecordGroup : ZEFLTRecordBase
{
	char					ID[8];
	ZEBigEndian<ZEInt16>	RelativePriority;
	ZEBigEndian<ZEInt16>	Reserved1;

	ZEBigEndian<ZEInt32>	Flags;

	ZEBigEndian<ZEInt16>	SpecialEffectID1;
	ZEBigEndian<ZEInt16>	SpecialEffectID2;

	ZEBigEndian<ZEInt16>	Significance;

	ZEInt8					LayerCode;

	ZEInt8					Reserved2;
	ZEBigEndian<ZEInt32>	Reserved3;

	ZEBigEndian<ZEInt32>	LoopCount;
	ZEBigEndian<float>		LoopDurationInSeconds;
	ZEBigEndian<float>		LastFrameDurationInSeconds;

};)

ZEPackStruct(
struct ZEFLTRecordObject : ZEFLTRecordBase
{
	char						ID[8];

	ZEBigEndian<ZEInt32>		Flags;

	ZEBigEndian<ZEInt16>		RelativePriority;

	ZEBigEndian<ZEInt16>		Transparency;

	ZEBigEndian<ZEInt16>		SpecialEffectID1;
	ZEBigEndian<ZEInt16>		SpecialEffectID2;

	ZEBigEndian<ZEInt16>		Significance;
	ZEBigEndian<ZEInt16>		Reserved;

};)

ZEPackStruct(
struct ZEFLTRecordFace : ZEFLTRecordBase
{
	char						ID[8];

	ZEBigEndian<ZEInt32>		IRColorCode;

	ZEBigEndian<ZEInt16>		RelativePriority;

	ZEInt8						DrawType;

	ZEInt8						TextureWhite;
	ZEBigEndian<ZEUInt16>		ColorNameIndex;
	ZEBigEndian<ZEUInt16>		AlternateColorNameIndex;

	ZEInt8						Reserved1;

	ZEInt8						Template;

	ZEBigEndian<ZEInt16>		DetailTexturePatternIndex;
	ZEBigEndian<ZEInt16>		TexturePatternIndex;
	ZEBigEndian<ZEInt16>		MaterialIndex;

	ZEBigEndian<ZEInt16>		SurfaceMaterialCode;
	ZEBigEndian<ZEInt16>		FeatureID;

	ZEBigEndian<ZEInt32>		IRMaterialCode;

	ZEBigEndian<ZEUInt16>		Transparency;

	ZEUInt8						LODGenerationControl;

	ZEUInt8						LineStyleIndex;

	ZEBigEndian<ZEInt32>		Flags;

	ZEUInt8						LightMode;

	char						Reserved2[7];

	ZEBigEndian<ZEUInt32>		PackedColorPrimary;
	ZEBigEndian<ZEUInt32>		PackedColorAlternate;

	ZEBigEndian<ZEInt16>		TextureMappingIndex;

	ZEBigEndian<ZEInt16>		Reserved3;

	ZEBigEndian<ZEUInt32>		PrimaryColorIndex;
	ZEBigEndian<ZEUInt32>		AlternateColorIndex;

	ZEBigEndian<ZEInt16>		Reserved4;

	ZEBigEndian<ZEInt16>		ShaderIndex;

};)

ZEPackStruct(
struct ZEFLTRecordMesh : ZEFLTRecordBase
{
	char						ID[8];
	
	ZEBigEndian<ZEInt32>		Reserved1;

	ZEBigEndian<ZEInt32>		IRColorCode;

	ZEBigEndian<ZEInt16>		RelativePriority;

	ZEInt8						DrawType;

	ZEInt8						TextureWhite;

	ZEBigEndian<ZEUInt16>		ColorNameIndex;
	ZEBigEndian<ZEUInt16>		AlternateColorNameIndex;

	ZEInt8						Reserved2;

	ZEInt8						Template;

	ZEBigEndian<ZEInt16>		DetailTexturePatternIndex;
	ZEBigEndian<ZEInt16>		TexturePatternIndex;
	ZEBigEndian<ZEInt16>		MaterialIndex;

	ZEBigEndian<ZEInt16>		SurfaceMaterialCode;
	ZEBigEndian<ZEInt16>		FeatureID;

	ZEBigEndian<ZEInt32>		IRMaterialCode;

	ZEBigEndian<ZEUInt16>		Transparency;

	ZEUInt8						LODGenerationControl;

	ZEUInt8						LineStyleIndex;

	ZEBigEndian<ZEInt32>		Flags;

	ZEUInt8						LightMode;

	char						Reserved3[7];

	ZEBigEndian<ZEUInt32>		PackedColorPrimary;
	ZEBigEndian<ZEUInt32>		PackedColorAlternate;

	ZEBigEndian<ZEInt16>		TextureMappingIndex;

	ZEBigEndian<ZEInt16>		Reserved4;

	ZEBigEndian<ZEUInt32>		PrimaryColorIndex;
	ZEBigEndian<ZEUInt32>		AlternateColorIndex;

	ZEBigEndian<ZEInt16>		Reserved5;

	ZEBigEndian<ZEInt16>		ShaderIndex;

};)

ZEPackStruct(
struct ZEFLTRecordLocalVertexPoolVertex
{
	ZEBigEndian<double>			Coordinate[3];
	ZEBigEndian<ZEUInt32>		Color;

	ZEBigEndian<float>			Normal[3];
	ZEBigEndian<float>			UVBase[2];
	ZEBigEndian<float>			UV1[2];
	ZEBigEndian<float>			UV2[2];
	ZEBigEndian<float>			UV3[2];
	ZEBigEndian<float>			UV4[2];
	ZEBigEndian<float>			UV5[2];
	ZEBigEndian<float>			UV6[2];
	ZEBigEndian<float>			UV7[2];

};)

ZEPackStruct(
struct ZEFLTRecordLocalVertexPool : ZEFLTRecordBase
{
	ZEBigEndian<ZEUInt32>						NumberOfVertices;
	ZEBigEndian<ZEUInt32>						AttributeMask;

	ZEArray<ZEFLTRecordLocalVertexPoolVertex>	Vertices;
};)

ZEPackStruct(
struct ZEFLTRecordMeshPrimitive : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt16>			PrimitiveType;
	ZEBigEndian<ZEUInt16>			IndexSize;
	ZEBigEndian<ZEUInt32>			VertexCount;

	ZEArray<ZEBigEndian<ZEInt32>>	Indices;

};)

ZEPackStruct(
struct ZEFLTRecordIndexedLightPoint : ZEFLTRecordBase
{
	char						ID[8];

	ZEBigEndian<ZEInt32>		AppearanceIndex;
	ZEBigEndian<ZEInt32>		AnimationIndex;
	ZEBigEndian<ZEInt32>		DrawOrder;
	ZEBigEndian<ZEInt32>		Reserved;
};)

ZEPackStruct(
struct ZEFLTRecordLightPoint : ZEFLTRecordBase
{
	char						ID[8];

	ZEBigEndian<ZEInt16>		SurfaceMaterialCode;
	ZEBigEndian<ZEInt16>		FeatureID;

	ZEBigEndian<ZEUInt32>		BackColorForBidirectionalPoints;
	ZEBigEndian<ZEInt32>		DisplayMode;

	ZEBigEndian<float>			Intensity;
	ZEBigEndian<float>			BackIntensity;
	ZEBigEndian<float>			MinimumDefocus;
	ZEBigEndian<float>			MaximumDefocus;

	ZEBigEndian<ZEInt32>		FadingMode;
	ZEBigEndian<ZEInt32>		FogPunchMode;
	ZEBigEndian<ZEInt32>		DirectionalMode;
	ZEBigEndian<ZEInt32>		RangeMode;

	ZEBigEndian<float>			MinPixelSize;
	ZEBigEndian<float>			MaxPixelSize;
	ZEBigEndian<float>			ActualSize;

	ZEBigEndian<float>			TransparentFalloffPixelSize;
	ZEBigEndian<float>			TransparentFalloffExponent;
	ZEBigEndian<float>			TransparentFalloffScalar;
	ZEBigEndian<float>			TransparentFalloffClamp;

	ZEBigEndian<float>			FogScalar;

	ZEBigEndian<float>			Reserved;

	ZEBigEndian<float>			SizeDifferenceThreshold;

	ZEBigEndian<ZEInt32>		Directionality;

	ZEBigEndian<float>			HorizontalLobeAngle;
	ZEBigEndian<float>			VerticalLobeAngle;
	ZEBigEndian<float>			RollLobeAngle;

	ZEBigEndian<float>			DirectionalFalloffExponent;
	ZEBigEndian<float>			DirectionalAmbientIntensity;

	ZEBigEndian<float>			AnimationPeriod;
	ZEBigEndian<float>			AnimationPhaseDelay;
	ZEBigEndian<float>			AnimationEnabledPeriod;
	ZEBigEndian<float>			Significance;

	ZEBigEndian<ZEInt32>		CalligraphicDrawOrder;

	ZEBigEndian<ZEInt32>		Flags;

	ZEBigEndian<float>			AxisOfRotation[3];

};)

ZEPackStruct(
struct ZEFLTRecordLightPointSystem : ZEFLTRecordBase
{
	char						ID[8];
	ZEBigEndian<float>			Intensity;
	ZEBigEndian<ZEInt32>		AnimationState;
	ZEBigEndian<ZEInt32>		Flags;

};)

ZEPackStruct(
struct ZEFLTRecordDegreeOfFreedom : ZEFLTRecordBase
{
	char						ID[8];
	ZEBigEndian<ZEInt32>		Reserved1;

	ZEBigEndian<double>			Origin[3];
	ZEBigEndian<double>			PointOnXAxis[3];
	ZEBigEndian<double>			PointInXYPlane[3];

	ZEBigEndian<double>			MinPosZ;
	ZEBigEndian<double>			MaxPosZ;
	ZEBigEndian<double>			CurrentPosZ;
	ZEBigEndian<double>			IncrementPosZ;

	ZEBigEndian<double>			MinPosY;
	ZEBigEndian<double>			MaxPosY;
	ZEBigEndian<double>			CurrentPosY;
	ZEBigEndian<double>			IncrementPosY;

	ZEBigEndian<double>			MinPosX;
	ZEBigEndian<double>			MaxPosX;
	ZEBigEndian<double>			CurrentPosX;
	ZEBigEndian<double>			IncrementPosX;

	ZEBigEndian<double>			MinPitch;
	ZEBigEndian<double>			MaxPitch;
	ZEBigEndian<double>			CurrentPitch;
	ZEBigEndian<double>			IncrementPitch;

	ZEBigEndian<double>			MinRoll;
	ZEBigEndian<double>			MaxRoll;
	ZEBigEndian<double>			CurrentRoll;
	ZEBigEndian<double>			IncrementRoll;

	ZEBigEndian<double>			MinYaw;
	ZEBigEndian<double>			MaxYaw;
	ZEBigEndian<double>			CurrentYaw;
	ZEBigEndian<double>			IncrementYaw;

	ZEBigEndian<double>			MinScaleZ;
	ZEBigEndian<double>			MaxScaleZ;
	ZEBigEndian<double>			CurrentScaleZ;
	ZEBigEndian<double>			IncrementScaleZ;

	ZEBigEndian<double>			MinScaleY;
	ZEBigEndian<double>			MaxScaleY;
	ZEBigEndian<double>			CurrentScaleY;
	ZEBigEndian<double>			IncrementScaleY;

	ZEBigEndian<double>			MinScaleX;
	ZEBigEndian<double>			MaxScaleX;
	ZEBigEndian<double>			CurrentScaleX;
	ZEBigEndian<double>			IncrementScaleX;

	ZEBigEndian<ZEInt32>		Flags;
	ZEBigEndian<ZEInt32>		Reserved2;

};)

ZEPackStruct(
struct ZEFLTRecordVertexList : ZEFLTRecordBase
{
	ZEArray<ZEBigEndian<ZEInt32>>	VertexOffsets;
};)

ZEPackStruct(
struct ZEFLTRecordMorphListVertex
{
	ZEBigEndian<ZEInt32>		Offset0;
	ZEBigEndian<ZEInt32>		Offset100;
};)

ZEPackStruct(
struct ZEFLTRecordMorphList : ZEFLTRecordBase
{
	ZEArray<ZEFLTRecordMorphListVertex>		VertexOffsets;
};)

ZEPackStruct(
struct ZEFLTRecordBinarySeparatingPlane : ZEFLTRecordBase
{
	char						ID[8];
	ZEBigEndian<ZEInt32>		Reserved;
	ZEBigEndian<double>			PlaneEquationCoefficients[4];

};)

ZEPackStruct(
struct ZEFLTRecordExternalReference : ZEFLTRecordBase
{
	char						Path[200];
	ZEBigEndian<ZEInt32>		Reserved1;
	ZEBigEndian<ZEInt32>		Flags;
	ZEBigEndian<ZEInt16>		ViewAsBoundingBox;
	ZEBigEndian<ZEInt16>		Reserved2;
};)

ZEPackStruct(
struct ZEFLTRecordLevelOfDetail : ZEFLTRecordBase
{
	char						ID[8];
	ZEBigEndian<ZEInt32>		Reserved;

	ZEBigEndian<double>			SwitchInDistance;
	ZEBigEndian<double>			SwitchOutDistance;

	ZEBigEndian<ZEInt16>		SpecialEffectID1;
	ZEBigEndian<ZEInt16>		SpecialEffectID2;

	ZEBigEndian<ZEInt32>		Flags;

	ZEBigEndian<double>			LODCenterPosX;
	ZEBigEndian<double>			LODCenterPosY;
	ZEBigEndian<double>			LODCenterPosZ;

	ZEBigEndian<double>			TransitionRange;
	ZEBigEndian<double>			SignificantSize;

};)

ZEPackStruct(
struct ZEFLTRecordSound : ZEFLTRecordBase
{
	char						ID[8];
	ZEBigEndian<ZEInt32>		Reserved1;

	ZEBigEndian<ZEInt32>		SoundPaletteIndex;
	ZEBigEndian<ZEInt32>		Reserved2;

	ZEBigEndian<double>			OffsetFromLocalOrigin[3];
	ZEVector3					SoundDirection;

	ZEBigEndian<float>			Amplitude;
	ZEBigEndian<float>			PitchBend;
	ZEBigEndian<float>			Priority;
	ZEBigEndian<float>			Falloff;
	ZEBigEndian<float>			LobeWidth;

	ZEBigEndian<ZEInt32>		Flags;
	ZEBigEndian<ZEInt32>		Reserved3;

};)

ZEPackStruct(
struct ZEFLTRecordLightSource : ZEFLTRecordBase
{
	char						ID[8];
	ZEBigEndian<ZEInt32>		Reserved1;

	ZEBigEndian<ZEInt32>		LightPaletteIndex;
	ZEBigEndian<ZEInt32>		Reserved2;

	ZEBigEndian<ZEInt32>		Flags;
	ZEBigEndian<ZEInt32>		Reserved3;

	ZEBigEndian<double>			Position[3];
	ZEBigEndian<float>			Yaw;
	ZEBigEndian<float>			Pitch;
};)

ZEPackStruct(
struct ZEFLTRecordClipRegion : ZEFLTRecordBase
{
	char						ID[8];
	ZEBigEndian<ZEInt32>		Reserved1;
	ZEBigEndian<ZEInt16>		Reserved2;

	char						Flags[5];
	char						Reserved3;

	ZEBigEndian<double>			ClipRegionCoordinate1[3];
	ZEBigEndian<double>			ClipRegionCoordinate2[3];
	ZEBigEndian<double>			ClipRegionCoordinate3[3];
	ZEBigEndian<double>			ClipRegionCoordinate4[3];

	ZEBigEndian<double>			PlaneEquationCoefficients[20];

};)

ZEPackStruct(
struct ZEFLTRecordText : ZEFLTRecordBase
{
	char						ID[8];
	ZEBigEndian<ZEInt32>		Reserved1;
	ZEBigEndian<ZEInt32>		Reserved2;

	ZEBigEndian<ZEInt32>		Type;
	ZEBigEndian<ZEInt32>		DrawType;
	ZEBigEndian<ZEInt32>		Justification;

	ZEBigEndian<double>			FloatingPointValue;
	ZEBigEndian<ZEInt32>		IntegerValue;

	ZEBigEndian<ZEInt32>		Reserved3[5];

	ZEBigEndian<ZEInt32>		Flags;

	ZEBigEndian<ZEInt32>		Color1;
	ZEBigEndian<ZEInt32>		Color2;
	ZEBigEndian<ZEInt32>		Material;

	ZEBigEndian<ZEInt32>		Reserved4;

	ZEBigEndian<ZEInt32>		MaximumNumberOfLines;
	ZEBigEndian<ZEInt32>		MaximumNumberOfCharacters;
	ZEBigEndian<ZEInt32>		CurrentLengthOfText;
	ZEBigEndian<ZEInt32>		NextLineNumberAvailable;
	ZEBigEndian<ZEInt32>		LineNumberAtTopOfDisplay;
	
	ZEBigEndian<ZEInt32>		LowHighValuesForIntegers[2];
	ZEBigEndian<double>			LowHighValuesForFloats[2];
	ZEBigEndian<double>			TextRectangleLowerLeftCorner[3];
	ZEBigEndian<double>			TextRectangleUpperRightCorner[3];

	char						FontName[120];
	ZEBigEndian<ZEInt32>		DrawVertical;
	ZEBigEndian<ZEInt32>		DrawItalic;
	ZEBigEndian<ZEInt32>		DrawBold;
	ZEBigEndian<ZEInt32>		DrawUnderline;
	ZEBigEndian<ZEInt32>		LineStyle;

	ZEBigEndian<ZEInt32>		Reserved5;
};)

ZEPackStruct(
struct ZEFLTRecordSwitch : ZEFLTRecordBase //Mask words size is variable.. Solve it.
{
	char							ID[8];
	ZEBigEndian<ZEInt32>			Reserved;

	ZEBigEndian<ZEInt32>			CurrentMask;
	ZEBigEndian<ZEInt32>			NumberOfMasks;
	ZEBigEndian<ZEInt32>			NumberOfWordsPerMask;
	ZEArray<ZEBigEndian<ZEUInt32>>	MaskWords;
};)

ZEPackStruct(
struct ZEFLTRecordExtension : ZEFLTRecordBase //Last member Extended Data has variable size..
{
	char		ID[8];
	char		SiteID[8];

	ZEInt8		Reserved;
	ZEInt8		Revision;
	ZEInt8		RecordCode;
};)

/*ANCILLARY RECORDS*/

ZEPackStruct(
struct ZEFLTRecordMultitextureLayer
{
	ZEBigEndian<ZEUInt16>		TextureIndex;
	ZEBigEndian<ZEUInt16>		MultitextureEffect;
	ZEBigEndian<ZEUInt16>		TextureMappingIndex;
	ZEBigEndian<ZEUInt16>		TextureData;
};)

ZEPackStruct(
struct ZEFLTRecordMultitexture : ZEFLTRecordBase // Face/Mesh anch.
{
	ZEBigEndian<ZEInt32>					AttributeMask;
	ZEArray<ZEFLTRecordMultitextureLayer>	Layers;
};)

ZEPackStruct(
struct ZEFLTRecordUVListItemVertex
{
	ZEBigEndian<float>		TextureCoordinateU;
	ZEBigEndian<float>		TextureCoordinateV;
};)

ZEPackStruct(
struct ZEFLTRecordUVListItemMorph
{
	ZEBigEndian<float>		TextureCoordinateU0;
	ZEBigEndian<float>		TextureCoordinateV0;
	ZEBigEndian<float>		TextureCoordinateU100;
	ZEBigEndian<float>		TextureCoordinateV100;
};)

ZEPackStruct(
template <class T>
struct ZEFLTRecordUVList : ZEFLTRecordBase // VertexList/MorphList anch.
{
	ZEBigEndian<ZEInt32>		AttributeMask;
	ZEArray<T>					TextureCoordinates;
};)

ZEPackStruct(
struct ZEFLTRecordReplicate : ZEFLTRecordBase //Group/Face/LightPoint anch.
{
	ZEBigEndian<ZEInt16>		NumberOfReplications;
	ZEBigEndian<ZEInt16>		Reserved;
};)

ZEPackStruct(
struct ZEFLTRecordMatrix : ZEFLTRecordBase
{
	ZEBigEndian<float>		Matrix[16]; //Row-major
};)

ZEPackStruct(
struct ZEFLTRecordRotateAboutEdge : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		Reserved1;

	ZEBigEndian<double>			EdgePoint1[3];
	ZEBigEndian<double>			EdgePoint2[3];

	ZEBigEndian<float>			RotationAngle;
	ZEBigEndian<ZEInt32>		Reserved2;
};)

ZEPackStruct(
struct ZEFLTRecordTranslate : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		Reserved;

	ZEBigEndian<double>			FromPoint[3];
	ZEBigEndian<double>			DeltaToTranslate[3];
};)

ZEPackStruct(
struct ZEFLTRecordScale : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		Reserved1;

	ZEBigEndian<double>			ScaleCenter[3];

	ZEBigEndian<float>			ScaleX;
	ZEBigEndian<float>			ScaleY;
	ZEBigEndian<float>			ScaleZ;

	ZEBigEndian<ZEInt32>		Reserved2;
};)

ZEPackStruct(
struct ZEFLTRecordRotateAboutPoint : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		Reserved;

	ZEBigEndian<double>			RotationCenter[3];

	ZEBigEndian<float>			RotationAxisI;
	ZEBigEndian<float>			RotationAxisJ;
	ZEBigEndian<float>			RotationAxisK;
	ZEBigEndian<float>			RotationAngle;
};)

ZEPackStruct(
struct ZEFLTRecordRotateScaleToPoint : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		Reserved1;

	ZEBigEndian<double>			ScaleCenter[3];
	ZEBigEndian<double>			ReferencePoint[3];
	ZEBigEndian<double>			ToPoint[3];

	ZEBigEndian<float>			OverallScale;
	ZEBigEndian<float>			ScaleInAxisDirection;
	ZEBigEndian<float>			RotationAngle;

	ZEBigEndian<ZEInt32>		Reserved2;
};)

ZEPackStruct(
struct ZEFLTRecordPut : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		Reserved;

	ZEBigEndian<double>			FromOrigin[3];
	ZEBigEndian<double>			FromAlign[3];
	ZEBigEndian<double>			FromTrack[3];

	ZEBigEndian<double>			ToOrigin[3];
	ZEBigEndian<double>			ToAlign[3];
	ZEBigEndian<double>			ToTrack[3];
};)

ZEPackStruct(
struct ZEFLTRecordGeneralMatrix : ZEFLTRecordBase
{
	ZEBigEndian<float>		Matrix[16]; //Row-major
};)

ZEPackStruct(
struct ZEFLTRecordVector : ZEFLTRecordBase
{
	ZEBigEndian<float>		ComponentI;
	ZEBigEndian<float>		ComponentJ;
	ZEBigEndian<float>		ComponentK;

};)

ZEPackStruct(
struct ZEFLTRecordBoundingBox : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		Reserved;

	ZEBigEndian<double>			MinCornerX;
	ZEBigEndian<double>			MinCornerY;
	ZEBigEndian<double>			MinCornerZ;

	ZEBigEndian<double>			MaxCornerX;
	ZEBigEndian<double>			MaxCornerY;
	ZEBigEndian<double>			MaxCornerZ;

};)

ZEPackStruct(
struct ZEFLTRecordBoundingSphere : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>	Reserved;
	ZEBigEndian<double>		Radius;
};)

ZEPackStruct(
struct ZEFLTRecordBoundingCylinder : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>	Reserved;
	ZEBigEndian<double>		Radius;
	ZEBigEndian<double>		Height;
};)

ZEPackStruct(
struct ZEFLTRecordBoundingConvexHullTriangles
{
	ZEBigEndian<double>		Vertex1ComponentX;
	ZEBigEndian<double>		Vertex1ComponentY;
	ZEBigEndian<double>		Vertex1ComponentZ;

	ZEBigEndian<double>		Vertex2ComponentX;
	ZEBigEndian<double>		Vertex2ComponentY;
	ZEBigEndian<double>		Vertex2ComponentZ;

	ZEBigEndian<double>		Vertex3ComponentX;
	ZEBigEndian<double>		Vertex3ComponentY;
	ZEBigEndian<double>		Vertex3ComponentZ;
};)

ZEPackStruct(
struct ZEFLTRecordBoundingConvexHull : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		NumberOfTriangles;

	ZEArray<ZEFLTRecordBoundingConvexHullTriangles>	Triangles;
};)

ZEPackStruct(
struct ZEFLTRecordBoundingVolumeCenter : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		Reserved;

	ZEBigEndian<double>			CenterX;
	ZEBigEndian<double>			CenterY;
	ZEBigEndian<double>			CenterZ;

};)

ZEPackStruct(
struct ZEFLTRecordBoundingVolumeOrientation : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		Reserved;

	ZEBigEndian<double>			YawAngle;
	ZEBigEndian<double>			PitchAngle;
	ZEBigEndian<double>			RollAngle;

};)

ZEPackStruct(
struct ZEFLTRecordExtensionFieldBoolean : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		GUIDPaletteIndex;
	ZEBigEndian<ZEInt32>		BooleanValue;
};)

ZEPackStruct(
struct ZEFLTRecordExtensionFieldInteger : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		GUIDPaletteIndex;
	ZEBigEndian<ZEInt32>		IntegerValue;
};)

ZEPackStruct(
struct ZEFLTRecordExtensionFieldFloat : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		GUIDPaletteIndex;
	ZEBigEndian<float>			FloatValue;
};)

ZEPackStruct(
struct ZEFLTRecordExtensionFieldDouble : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		GUIDPaletteIndex;
	ZEBigEndian<double>			DoubleValue;
};)

ZEPackStruct(
struct ZEFLTRecordExtensionFieldString : ZEFLTRecordBase //Last Member char array size is variable
{
	ZEBigEndian<ZEInt32>		GUIDPaletteIndex;
	ZEBigEndian<ZEInt32>		StringLength;
};)

ZEPackStruct(
struct ZEFLTRecordExtensionFieldXML : ZEFLTRecordBase //Last Member char array size is variable
{
	ZEBigEndian<ZEInt32>		GUIDPaletteIndex;
	ZEBigEndian<ZEInt32>		StringLength;
};)

ZEPackStruct(
template <class T> 
struct ZEFLTRecordContinuation : ZEFLTRecordBase
{
	ZEArray<T> Data;
};)

ZEPackStruct(
struct ZEFLTRecordPaletteVertex : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>		LengthOfWholePalette;
};)

ZEPackStruct(
struct ZEFLTRecordVertexWithColor : ZEFLTRecordBase
{
	ZEBigEndian<ZEUInt16>		ColorNameIndex;
	ZEBigEndian<ZEInt16>		Flags;
	ZEBigEndian<double>			VertexCoordinate[3];
	ZEBigEndian<ZEInt32>		PackedColor;
	ZEBigEndian<ZEUInt32>		VertexColorIndex;
};)

ZEPackStruct(
struct ZEFLTRecordVertexWithColorNormal : ZEFLTRecordBase
{
	ZEBigEndian<ZEUInt16>		ColorNameIndex;
	ZEBigEndian<ZEInt16>		Flags;
	ZEBigEndian<double>			VertexCoordinate[3];
	ZEBigEndian<float>			VertexNormal[3];
	ZEBigEndian<ZEInt32>		PackedColor;
	ZEBigEndian<ZEUInt32>		VertexColorIndex;
	ZEBigEndian<ZEInt32>		Reserved;
};)

ZEPackStruct(
struct ZEFLTRecordVertexWithColorUV : ZEFLTRecordBase
{
	ZEBigEndian<ZEUInt16>		ColorNameIndex;
	ZEBigEndian<ZEInt16>		Flags;
	ZEBigEndian<double>			VertexCoordinate[3];
	ZEBigEndian<float>			TextureCoordinate[2];
	ZEBigEndian<ZEInt32>		PackedColor;
	ZEBigEndian<ZEUInt32>		VertexColorIndex;
};)

ZEPackStruct(
struct ZEFLTRecordVertexWithColorNormalUV : ZEFLTRecordBase
{
	ZEBigEndian<ZEUInt16>		ColorNameIndex;
	ZEBigEndian<ZEInt16>		Flags;
	ZEBigEndian<double>			VertexCoordinate[3];
	ZEBigEndian<float>			VertexNormal[3];
	ZEBigEndian<float>			TextureCoordinate[2];
	ZEBigEndian<ZEInt32>		PackedColor;
	ZEBigEndian<ZEUInt32>		VertexColorIndex;
	ZEBigEndian<ZEInt32>		Reserved;
};)

ZEPackStruct(
struct ZEFLTRecordColorName
{
	ZEBigEndian<ZEUInt16>		Length;
	ZEBigEndian<ZEInt16>		Reserved1;
	ZEBigEndian<ZEInt16>		ColorIndex;
	ZEBigEndian<ZEInt16>		Reserved2;
	char						ColorName[80];
};)

ZEPackStruct(
struct ZEFLTRecordPaletteColor : ZEFLTRecordBase
{
	char							Reserved[128];
	ZEBigEndian<ZEInt32>			BrightestRGBOfColors[1024];
	ZEBigEndian<ZEInt32>			NumberofColorNames;

	ZEArray<ZEFLTRecordColorName>	ColorNames;
};)

ZEPackStruct(
struct ZEFLTRecordNameTableEntry
{
	ZEBigEndian<ZEInt32>			Length;
	ZEBigEndian<ZEUInt16>			NameIndex;
	char							NameString[80];
};)

ZEPackStruct(
struct ZEFLTRecordNameTable : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>				NumberOfNames;
	ZEBigEndian<ZEUInt16>				NextAvailableNameIndex;
	ZEArray<ZEFLTRecordNameTableEntry>	Names;
};)

ZEPackStruct(
struct ZEFLTRecordPaletteMaterial : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>	MaterialIndex;
	char					MaterialName[12];
	ZEBigEndian<ZEInt32>	Flags;

	ZEBigEndian<float>		AmbientComponent[3];
	ZEBigEndian<float>		DiffuseComponent[3];
	ZEBigEndian<float>		SpecularComponent[3];
	ZEBigEndian<float>		EmissiveComponent[3];

	ZEBigEndian<float>		Shininess;
	ZEBigEndian<float>		Alpha;
	ZEBigEndian<ZEInt32>	Reserved;
};)

ZEPackStruct(
struct ZEFLTRecordExtendedMaterialHeader : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>	MaterialIndex;
	char					MaterialName[12];
	ZEBigEndian<ZEInt32>	Flags;
	ZEBigEndian<ZEInt32>	ShadeModel;
};)

ZEPackStruct(
struct ZEFLTRecordExtendedMaterialAmbient : ZEFLTRecordBase
{
	ZEBigEndian<float>		AmbientColor[3];
	ZEBigEndian<ZEInt32>	Layer0TextureIndex;
	ZEBigEndian<ZEInt32>	Layer0UVSet;
	ZEBigEndian<ZEInt32>	Layer1TextureIndex;
	ZEBigEndian<ZEInt32>	Layer1UVSet;
	ZEBigEndian<ZEInt32>	Layer2TextureIndex;
	ZEBigEndian<ZEInt32>	Layer2UVSet;
	ZEBigEndian<ZEInt32>	Layer3TextureIndex;
	ZEBigEndian<ZEInt32>	Layer3UVSet;

};)

ZEPackStruct(
struct ZEFLTRecordExtendedMaterialDiffuse : ZEFLTRecordBase
{
	ZEBigEndian<float>		DiffuseColor[3];
	ZEBigEndian<ZEInt32>	Layer0TextureIndex;
	ZEBigEndian<ZEInt32>	Layer0UVSet;
	ZEBigEndian<ZEInt32>	Layer1TextureIndex;
	ZEBigEndian<ZEInt32>	Layer1UVSet;
	ZEBigEndian<ZEInt32>	Layer2TextureIndex;
	ZEBigEndian<ZEInt32>	Layer2UVSet;
	ZEBigEndian<ZEInt32>	Layer3TextureIndex;
	ZEBigEndian<ZEInt32>	Layer3UVSet;

};)

ZEPackStruct(
struct ZEFLTRecordExtendedMaterialSpecular : ZEFLTRecordBase
{
	ZEBigEndian<float>		Shininness;
	ZEBigEndian<float>		SpecularColor[3];
	ZEBigEndian<ZEInt32>	Layer0TextureIndex;
	ZEBigEndian<ZEInt32>	Layer0UVSet;
	ZEBigEndian<ZEInt32>	Layer1TextureIndex;
	ZEBigEndian<ZEInt32>	Layer1UVSet;
	ZEBigEndian<ZEInt32>	Layer2TextureIndex;
	ZEBigEndian<ZEInt32>	Layer2UVSet;
	ZEBigEndian<ZEInt32>	Layer3TextureIndex;
	ZEBigEndian<ZEInt32>	Layer3UVSet;

};)

ZEPackStruct(
struct ZEFLTRecordExtendedMaterialEmissive : ZEFLTRecordBase
{
	ZEBigEndian<float>		EmissiveColor[3];
	ZEBigEndian<ZEInt32>	Layer0TextureIndex;
	ZEBigEndian<ZEInt32>	Layer0UVSet;
	ZEBigEndian<ZEInt32>	Layer1TextureIndex;
	ZEBigEndian<ZEInt32>	Layer1UVSet;
	ZEBigEndian<ZEInt32>	Layer2TextureIndex;
	ZEBigEndian<ZEInt32>	Layer2UVSet;
	ZEBigEndian<ZEInt32>	Layer3TextureIndex;
	ZEBigEndian<ZEInt32>	Layer3UVSet;

};)

ZEPackStruct(
struct ZEFLTRecordExtendedMaterialAlpha : ZEFLTRecordBase
{
	ZEBigEndian<float>		Alpha;
	ZEBigEndian<ZEInt32>	Layer0TextureIndex;
	ZEBigEndian<ZEInt32>	Layer0UVSet;
	ZEBigEndian<ZEInt32>	Layer1TextureIndex;
	ZEBigEndian<ZEInt32>	Layer1UVSet;
	ZEBigEndian<ZEInt32>	Layer2TextureIndex;
	ZEBigEndian<ZEInt32>	Layer2UVSet;
	ZEBigEndian<ZEInt32>	Layer3TextureIndex;
	ZEBigEndian<ZEInt32>	Layer3UVSet;
	ZEBigEndian<ZEInt32>	Quality;
};)

ZEPackStruct(
struct ZEFLTRecordExtendedMaterialLightMap : ZEFLTRecordBase
{
	ZEBigEndian<float>		MaximumIntensity;
	ZEBigEndian<ZEInt32>	TextureIndex;
	ZEBigEndian<ZEInt32>	UVSet;
};)

ZEPackStruct(
struct ZEFLTRecordExtendedMaterialNormalMap : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>	TextureIndex;
	ZEBigEndian<ZEInt32>	UVSet;
};)

ZEPackStruct(
struct ZEFLTRecordExtendedMaterialBumpMap : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>	TextureIndex;
	ZEBigEndian<ZEInt32>	UVSet;
	ZEBigEndian<ZEInt32>	UVSetForTangentVectors;
	ZEBigEndian<ZEInt32>	UVSetForBinormalVectors;
};)

ZEPackStruct(
struct ZEFLTRecordExtendedMaterialShadowMap : ZEFLTRecordBase
{
	ZEBigEndian<float>		MaximumIntensity;
	ZEBigEndian<ZEInt32>	TextureIndex;
	ZEBigEndian<ZEInt32>	UVSet;
};)

ZEPackStruct(
struct ZEFLTRecordExtendedMaterialReflectionMap : ZEFLTRecordBase
{
	ZEBigEndian<float>		TintColor[3];
	ZEBigEndian<ZEInt32>	ReflectionMapTextureIndex;
	ZEBigEndian<ZEInt32>	ReflectionMapUVSet;
	ZEBigEndian<ZEInt32>	EnvironmentMapTextureIndex;
	ZEBigEndian<ZEInt32>	Reserved;
};)

ZEPackStruct(
struct ZEFLTRecordPaletteTexture : ZEFLTRecordBase
{
	char					FileName[200];
	ZEBigEndian<ZEInt32>	TexturePatternIndex;
	ZEBigEndian<ZEInt32>	TexturePaletteLocation[2];

};)

ZEPackStruct(
struct ZEFLTRecordEyePoint
{
	ZEBigEndian<double>		RotationCenter[3];
	ZEBigEndian<float>		RotationAngles[3]; // Yaw Pitch Roll
	ZEBigEndian<float>		RotationMatrix[16]; // Row Major

	ZEBigEndian<float>		FieldOfView;
	ZEBigEndian<float>		Scale;
	ZEBigEndian<float>		NearClippingPlane;
	ZEBigEndian<float>		FarClippingPlane;

	ZEBigEndian<float>		FlyThroughMatrix[16]; // Row major
	ZEBigEndian<float>		EyePointPosition[3];
	ZEBigEndian<float>		FlyThroughYaw;
	ZEBigEndian<float>		FlyThroughPitch;

	ZEBigEndian<float>		EyePointDirection[3];
	ZEBigEndian<ZEInt32>	NoFlyThrough;
	ZEBigEndian<ZEInt32>	OrthoView;
	ZEBigEndian<ZEInt32>	ValidEyePoint;

	ZEBigEndian<ZEInt32>	ImageOffsetX;
	ZEBigEndian<ZEInt32>	ImageOffsetY;
	ZEBigEndian<ZEInt32>	ImageZoom;

	ZEBigEndian<ZEInt32>	Reserved1[8];
	ZEBigEndian<ZEInt32>	Reserved2;
};)

ZEPackStruct(
struct ZEFLTRecordTrackPlane
{
	ZEBigEndian<ZEInt32>	ValidTrackplane;
	ZEBigEndian<ZEInt32>	Reserved1;

	ZEBigEndian<double>		Origin[3];
	ZEBigEndian<double>		Alignment[3];
	ZEBigEndian<double>		Plane[3];

	bool					GridVisible;

	ZEInt8					GridTypeFlag;
	ZEInt8					GridUnderFlag;

	ZEInt8					Reserved2;

	ZEBigEndian<float>		GridAngleForRadialGrid;
	ZEBigEndian<double>		GridSpacingX;
	ZEBigEndian<double>		GridSpacingY;

	ZEInt8					RadialGridSpacingDirectionControl;
	ZEInt8					RectangularGridSpacingDirectionControl;
	bool					SnapCursor;

	ZEInt8					Reserved3;
	ZEBigEndian<ZEInt32>	Reserved4;

	ZEBigEndian<double>		GridSize;
	ZEBigEndian<ZEInt32>	Mask;
	ZEBigEndian<ZEInt32>	Reserved5;

};)

ZEPackStruct(
struct ZEFLTRecordPaletteEyePointsAndTrackPlanes : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>			Reserved;

	ZEArray<ZEFLTRecordEyePoint>	EyePoints;
	ZEArray<ZEFLTRecordTrackPlane>	TrackPlanes;
};)

ZEPackStruct(
struct ZEFLTRecordPaletteSoundIndex
{
	ZEBigEndian<ZEInt32>	SoundIndex;
	ZEBigEndian<ZEInt32>	Reserved;
	ZEBigEndian<ZEInt32>	FileNameOffset;
};)

ZEPackStruct(
struct ZEFLTRecordPaletteSoundHeader : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>					Subtype;
	ZEBigEndian<ZEInt32>					MaxNumber;
	ZEBigEndian<ZEInt32>					ActualNumber;
	ZEBigEndian<ZEInt32>					TotalLength;

	ZEBigEndian<ZEInt32>					Reserved[3];

	ZEArray<ZEFLTRecordPaletteSoundIndex>	Indices;
};)

ZEPackStruct(
struct ZEFLTRecordPaletteSoundData : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>	Subtype;
	ZEBigEndian<ZEInt32>	TotalLength;

	ZEArray<char>			PackedFileNames;
};)

ZEPackStruct(
struct ZEFLTRecordPaletteLightSource : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>	LightSourceIndex;
	ZEBigEndian<ZEInt32>	Reserved1[2];

	char					LightSourceName[20];

	ZEBigEndian<ZEInt32>	Reserved2;

	ZEBigEndian<float>		AmbientComponent[4];
	ZEBigEndian<float>		DiffuseComponent[4];
	ZEBigEndian<float>		SpecularComponent[4];

	ZEBigEndian<ZEInt32>	LightType;

	ZEBigEndian<ZEInt32>	Reserved3[10];

	ZEBigEndian<float>		SpotExponentialDropOffTerm;
	ZEBigEndian<float>		SpotCutoffAngle; //In degrees
	ZEBigEndian<float>		Yaw;
	ZEBigEndian<float>		Pitch;

	ZEBigEndian<float>		ConstantAttenuationCoefficient;
	ZEBigEndian<float>		LinearAttenuationCoefficient;
	ZEBigEndian<float>		QuadraticAttenuationCoefficient;

	ZEBigEndian<ZEInt32>	ModelingLight;

	ZEBigEndian<ZEInt32>	Reserved4[19];
};)

ZEPackStruct(
struct ZEFLTRecordPaletteLightPointAppearance : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>	Reserved1;

	char					Name[256];
	ZEBigEndian<ZEInt32>	Index;

	ZEBigEndian<ZEInt16>	SurfaceMaterialCode;
	ZEBigEndian<ZEInt16>	FeatureID;

	ZEBigEndian<ZEUInt32>	BackColor;
	ZEBigEndian<ZEInt32>	DisplayMode;

	ZEBigEndian<float>		FrontColorIntensity;
	ZEBigEndian<float>		BackColorIntensity;

	ZEBigEndian<float>		MinimumDefocus;
	ZEBigEndian<float>		MaximumDefocus;

	ZEBigEndian<ZEInt32>	FadingMode;
	ZEBigEndian<ZEInt32>	FogPunchMode;
	ZEBigEndian<ZEInt32>	DirectionalMode;
	ZEBigEndian<ZEInt32>	RangeMode;

	ZEBigEndian<float>		MinPixelSize;
	ZEBigEndian<float>		MaxPixelSize;

	ZEBigEndian<float>		ActualSize;

	ZEBigEndian<float>		TransparentFalloffPixelSize;
	ZEBigEndian<float>		TransparentFalloffExponent;
	ZEBigEndian<float>		TransparentFalloffScalar;
	ZEBigEndian<float>		TransparentFalloffClamp;

	ZEBigEndian<float>		FogScalar;
	ZEBigEndian<float>		FogIntensity;

	ZEBigEndian<float>		SizeDifferenceThreshold;

	ZEBigEndian<ZEInt32>	Directionality;
	ZEBigEndian<float>		HorizontalLobeAngle;
	ZEBigEndian<float>		VerticalLobeAngle;
	ZEBigEndian<float>		LobeRollAngle;

	ZEBigEndian<float>		DirectionalFalloffExponent;
	ZEBigEndian<float>		DirectionalAmbientIntensity;
	ZEBigEndian<float>		Significance;

	ZEBigEndian<ZEInt32>	Flags;

	ZEBigEndian<float>		VisibilityRange;
	ZEBigEndian<float>		FadeRangeRatio;
	ZEBigEndian<float>		FadeInDuration;
	ZEBigEndian<float>		FadeOutDuration;
	ZEBigEndian<float>		LODRangeRatio;
	ZEBigEndian<float>		LODScale;

	ZEBigEndian<ZEInt16>	TexturePatternIndex;
	ZEBigEndian<ZEInt16>	Reserved2;

};)

ZEPackStruct(
struct ZEFLTRecordPaletteLightPointAnimationSequence
{
	ZEBigEndian<ZEUInt32>	State;
	ZEBigEndian<float>		Duration;
	ZEBigEndian<ZEUInt32>	Color;
};)

ZEPackStruct(
struct ZEFLTRecordPaletteLightPointAnimation : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>	Reserved1;

	char					Name[256];
	ZEBigEndian<ZEInt32>	Index;

	ZEBigEndian<float>		PeriodInSeconds;
	ZEBigEndian<float>		PhaseDelayInSeconds;
	ZEBigEndian<float>		EnabledPeriodInSeconds;
	ZEBigEndian<float>		RotationAxis[3];

	ZEBigEndian<ZEInt32>	Flags;
	ZEBigEndian<ZEInt32>	AnimationType;

	ZEBigEndian<ZEInt32>	MorseCodeTiming;
	ZEBigEndian<ZEInt32>	WordRate;
	ZEBigEndian<ZEInt32>	CharacterRate;
	char					MorseCodeString[1024];
	ZEBigEndian<ZEInt32>	NumberOfSequences;

	ZEArray<ZEFLTRecordPaletteLightPointAnimationSequence>	Sequences;
};)

struct ZEFLTRecordPaletteTextureMappingParameterBase {};

ZEPackStruct(
struct ZEFLTRecordPaletteTextureMapping3PointPut : ZEFLTRecordPaletteTextureMappingParameterBase
{
	ZEBigEndian<ZEInt32>	StateOfPutTextureTool;
	ZEBigEndian<ZEInt32>	ActiveGeometryPoint;

	ZEBigEndian<double>		BBoxLowerLeftCorner[3];
	ZEBigEndian<double>		BBoxUpperRightCorner[3];

	ZEBigEndian<ZEInt32>	UseReadWorldSizeFlags[3];
	ZEBigEndian<ZEInt32>	Reserved1;

	ZEBigEndian<double>		TextureOriginPoint[3];
	ZEBigEndian<double>		TextureAlignmentPoint[3];
	ZEBigEndian<double>		TextureShearPoint[3];

	ZEBigEndian<double>		GeometryOriginPoint[3];
	ZEBigEndian<double>		GeometryAlignmentPoint[3];
	ZEBigEndian<double>		GeometryShearPoint[3];

	ZEBigEndian<ZEInt32>	ActiveTexturePoint;
	ZEBigEndian<ZEInt32>	UVDisplayType;
	
	ZEBigEndian<float>		URepetition;
	ZEBigEndian<float>		VRepetition;
};)

ZEPackStruct(
struct ZEFLTRecordPaletteTextureMapping4PointPut : ZEFLTRecordPaletteTextureMappingParameterBase
{
	ZEBigEndian<ZEInt32>	StateOfPutTextureTool;
	ZEBigEndian<ZEInt32>	ActiveGeometryPoint;

	ZEBigEndian<double>		BBoxLowerLeftCorner[3];
	ZEBigEndian<double>		BBoxUpperRightCorner[3];

	ZEBigEndian<ZEInt32>	UseReadWorldSizeFlags[3];
	ZEBigEndian<ZEInt32>	Reserved1;

	ZEBigEndian<double>		TextureOriginPoint[3];
	ZEBigEndian<double>		TextureAlignmentPoint[3];
	ZEBigEndian<double>		TextureShearPoint[3];
	ZEBigEndian<double>		TexturePerspectivePoint[3];

	ZEBigEndian<double>		GeometryOriginPoint[3];
	ZEBigEndian<double>		GeometryAlignmentPoint[3];
	ZEBigEndian<double>		GeometryShearPoint[3];
	ZEBigEndian<double>		GeometryPerspectivePoint[3];

	ZEBigEndian<ZEInt32>	ActiveTexturePoint;
	ZEBigEndian<ZEInt32>	UVDisplayType;

	ZEBigEndian<float>		DepthScaleFactor;
	ZEBigEndian<ZEInt32>	Reserved2;

	ZEBigEndian<double>		TransformationMatrix[16];

	ZEBigEndian<float>		URepetition;
	ZEBigEndian<float>		VRepetition;
};)

ZEPackStruct(
struct ZEFLTRecordPaletteTextureMappingSphericalProject : ZEFLTRecordPaletteTextureMappingParameterBase
{
	ZEBigEndian<float>		Scale;
	ZEBigEndian<ZEInt32>	Reserved1;
	ZEBigEndian<double>		CenterOfProjectSphere[3];
	ZEBigEndian<float>		CurrentMaximumDimensionOfBoundingBox;
	ZEBigEndian<float>		FirstMaximumDimensionOfBoundingBox;
};)

ZEPackStruct(
struct ZEFLTRecordPaletteTextureMappingRadialProject : ZEFLTRecordPaletteTextureMappingParameterBase
{
	ZEBigEndian<ZEInt32>	ActiveGeometryPoint;
	ZEBigEndian<ZEInt32>	Reserved;
	ZEBigEndian<float>		RadialScale;
	ZEBigEndian<float>		HeightScale;
	
	ZEBigEndian<double>		TransformationMatrix[16]; //Trackplane to XY plane

	ZEBigEndian<double>		CylinderCenterLinePoint1[3];
	ZEBigEndian<double>		CylinderCenterLinePoint2[3];

};)

ZEPackStruct(
struct ZEFLTRecordPaletteTextureMappingWarped
{
	ZEBigEndian<ZEInt32>	ActiveGeometryPoint;
	ZEBigEndian<ZEInt32>	WarpToolState;

	ZEBigEndian<ZEInt32>	Reserved1[2];

	ZEBigEndian<double>		FromPoints[8][2];
	ZEBigEndian<double>		ToPoints[8][2];
};)

ZEPackStruct(
struct ZEFLTRecordPaletteTextureMapping : ZEFLTRecordBase
{
	ZEBigEndian<ZEInt32>	Reserved1;

	ZEBigEndian<ZEInt32>	Index;
	char					Name[20];
	ZEBigEndian<ZEInt32>	Type;
	ZEBigEndian<ZEInt32>	WarpedFlag;

	ZEBigEndian<double>		TransformationMatrix[16]; // row major

	ZEFLTRecordPaletteTextureMappingParameterBase*	Parameters;
	ZEFLTRecordPaletteTextureMappingWarped*			WarpedParameters;

};)

#endif
