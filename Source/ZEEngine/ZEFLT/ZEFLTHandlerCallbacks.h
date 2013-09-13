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

void ReadRecordHeader(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordGroup(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordObject(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordFace(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordMesh(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordLocalVertexPool(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordMeshPrimitive(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordIndexedLightPoint(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordLightPoint(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordLightPointSystem(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordDegreeOfFreedom(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordVertexList(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordMorphList(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordBinarySeparatingPlane(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExternalReference(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordLevelOfDetail(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordSound(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordLightSource(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordClipRegion(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordText(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordSwitch(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtension(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);

void ReadRecordMultitexture(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordUVList(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordReplicate(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordMatrix(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordRotateAboutEdge(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordTranslate(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordScale(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordRotateAboutPoint(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordRotateScaleToPoint(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordPut(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordGeneralMatrix(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordVector(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordBoundingBox(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordBoundingSphere(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordBoundingCylinder(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordBoundingConvexHull(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordBoundingVolumeCenter(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordBoundingVolumeOrientation(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtensionFieldBoolean(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtensionFieldInteger(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtensionFieldFloat(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtensionFieldDouble(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtensionFieldString(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtensionFieldXML(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordContinuation(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);

void ReadRecordVertexPalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordVertexWithColor(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordVertexWithColorNormal(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordVertexWithColorUV(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordVertexWithColorNormalUV(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordColorPalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordNameTable(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordMaterialPalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtendedMaterialHeader(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtendedMaterialAmbient(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtendedMaterialDiffuse(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtendedMaterialSpecular(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtendedMaterialEmissive(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtendedMaterialAlpha(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtendedMaterialLightMap(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtendedMaterialNormalMap(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtendedMaterialBumpMap(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtendedMaterialShadowMap(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordExtendedMaterialReflectionMap(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordTexturePalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordEyepointTrackPlanePalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordSoundPalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordLightSourcePalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordLightPointAppearancePalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordLightPointAnimationPalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordTextureMappingPalette(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);

void ReadRecordPushLevel(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordPopLevel(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordPushSubface(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordPopSubface(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordPushExtension(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordPopExtension(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordInstanceReference(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordInstanceDefinition(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordPushAttribute(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);
void ReadRecordPopAttribute(ZEFile* File, ZEFLTResourceNode** Node, const ZEInt16& PreviousOpCode, const ZEUInt16& PreviousLength, const ZEInt16& CurrentOpCode, const ZEUInt16& CurrentLength);

#endif
