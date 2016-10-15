//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEMDVertex.h
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

#include "ZETypes.h"
#include "ZEPacking.h"
#include "ZEMath/ZEVector.h"
#include "ZEMath/ZEMatrix.h"

#include "ZEGraphics/ZEGRVertexElementType.h"

class ZEGRVertexLayout;

enum ZEMDVertexType
{
	ZEMD_VT_NORMAL		= 0,
	ZEMD_VT_SKINNED		= 1
};

enum ZEMDVertexIndexType
{
	ZEMD_VIT_NONE		= 0,
	ZEMD_VIT_16BIT		= 16,
	ZEMD_VIT_32BIT		= 32
};

ZEPackStruct(
struct ZEMDVertex
{
	ZEVector3						Position;
	float							Reserved0;
	ZEInt16_2						Normal;
	ZEInt16_2						Tangent;
	ZEVector2						Texcoords;

	static const ZEGRVertexLayout&	GetVertexLayout();
});

ZEPackStruct(
struct ZEMDVertexInstance
{
	ZEVector3						Position;
	float							Reserved0;
	ZEInt16_2						Normal;
	ZEInt16_2						Tangent;
	ZEVector2						Texcoords;

	ZEMatrix4x4						WorldTransform;
	ZEMatrix4x4						WorldTransformInverseTranspose;

	static const ZEGRVertexLayout&	GetVertexLayout();
});

ZEPackStruct(
struct ZEMDVertexSkin
{
	ZEVector3						Position;
	ZEUInt8_4						Indices;
	ZEInt16_2						Normal;
	ZEInt16_2						Tangent;
	ZEVector2						Texcoords;
	ZEVector4						Weights;

	static const ZEGRVertexLayout&	GetVertexLayout();
});
