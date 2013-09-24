//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFLTHandlerCallbacks.h
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
#ifndef __ZE_FLT_HANDLER_CALLBACKS_H__
#define __ZE_FLT_HANDLER_CALLBACKS_H__

#include "ZETypes.h"

class ZEFile;
class ZEFLTResourceNode;

void ReadRecordHeader(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordGroup(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordObject(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordFace(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordMesh(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordLocalVertexPool(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordMeshPrimitive(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordIndexedLightPoint(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordLightPoint(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordLightPointSystem(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordDegreeOfFreedom(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordVertexList(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordMorphList(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordBinarySeparatingPlane(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExternalReference(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordLevelOfDetail(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordSound(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordLightSource(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordClipRegion(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordText(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordSwitch(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtension(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);

void ReadRecordMultitexture(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordUVList(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordReplicate(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordMatrix(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordRotateAboutEdge(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordTranslate(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordScale(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordRotateAboutPoint(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordRotateScaleToPoint(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordPut(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordGeneralMatrix(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordVector(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordBoundingBox(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordBoundingSphere(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordBoundingCylinder(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordBoundingConvexHull(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordBoundingVolumeCenter(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordBoundingVolumeOrientation(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtensionFieldBoolean(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtensionFieldInteger(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtensionFieldFloat(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtensionFieldDouble(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtensionFieldString(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtensionFieldXML(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordContinuation(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);

void ReadRecordVertexPalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordVertexWithColor(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordVertexWithColorNormal(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordVertexWithColorUV(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordVertexWithColorNormalUV(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordColorPalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordNameTable(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordMaterialPalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtendedMaterialHeader(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtendedMaterialAmbient(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtendedMaterialDiffuse(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtendedMaterialSpecular(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtendedMaterialEmissive(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtendedMaterialAlpha(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtendedMaterialLightMap(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtendedMaterialNormalMap(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtendedMaterialBumpMap(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtendedMaterialShadowMap(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordExtendedMaterialReflectionMap(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordTexturePalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordEyepointTrackPlanePalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordSoundPalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordLightSourcePalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordLightPointAppearancePalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordLightPointAnimationPalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordTextureMappingPalette(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);

void ReadRecordPushLevel(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordPopLevel(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordPushSubface(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordPopSubface(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordPushExtension(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordPopExtension(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordInstanceReference(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordInstanceDefinition(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordPushAttribute(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);
void ReadRecordPopAttribute(char* BufferAtNode, ZEFLTResourceNode** Node, const ZEInt16& PreceedingOpCode, const ZEUInt16& NodeLength);

#endif
