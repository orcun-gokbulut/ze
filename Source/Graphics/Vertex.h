//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - Vertex.h
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
#ifndef	__ZE_VERTEX_H__
#define __ZE_VERTEX_H__

#include "ZEMath/ZEMath.h"
#include "Types.h"

#define ZE_VE_POSITION				0x00000008
#define ZE_VE_NORMAL				0x00000010
#define ZE_VE_TANGENT				0x00000020
#define ZE_VE_BINORMAL				0x00000040
#define ZE_VE_SKIN					0x00000080
#define ZE_VE_TEXCOORD(Count)		(0x00000004 && (Count))

enum ZEVertexType
{
	ZE_VT_NOTSET				= 0,
	ZE_VT_SIMPLEVERTEX			= 1,
	ZE_VT_MAPVERTEX				= 2,
	ZE_VT_MODELVERTEX			= 3,
	ZE_VT_SKINNEDMODELVERTEX	= 4,
	ZE_VT_GUIVERTEX				= 5
};

struct ZEVertexP
{
	ZEVector3			Position;
};

struct ZEVertexPN
{
	ZEVector3			Position;
	ZEVector3			Normal;
	ZEVector2			Texcoord;
};

struct ZEVertexPNT
{
	ZEVector3			Position;
	ZEVector3			Normal;
	ZEVector2			Texcoord;
};

struct ZEVertexPNBTT
{
	ZEVector3			Position;
	ZEVector3			Normal;
	ZEVector3			Tangent;
	ZEVector3			Binormal;
	ZEVector2			Texcoord;
};


struct ZESimpleVertex
{
	ZEVector3			Position;
	ZEVector3			Normal;
	ZEVector2			Texcoord;
};

struct ZEMapVertex
{
	ZEVector3			Position;
	ZEVector3			Normal;
	ZEVector3			Tangent;
	ZEVector3			Binormal;
	ZEVector2			Texcoord;
};

struct ZEModelVertex
{
	ZEVector3			Position;
	ZEVector3			Normal;
	ZEVector3			Tangent;
	ZEVector3			Binormal;
	ZEVector2			Texcoord;
};

struct ZESkinnedModelVertex
{
	ZEVector3			Position;
	ZEVector3			Normal;
	ZEVector3			Tangent;
	ZEVector3			Binormal;
	ZEVector2			Texcoord;
	unsigned char		BoneIndices[4];
	float				BoneWeights[4];
};

struct ZEGUIVertex
{
	ZEVector2			Position;
	ZEVector2			Texcoord;
};

size_t zeGetVertexSize(ZEDWORD VertexElements);
size_t zeGetVertexSize(ZEVertexType VertexType);


#endif
