//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - VertexDeclaration.h
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
#ifndef	__ZE_VERTEX_DECLARATION_H__
#define __ZE_VERTEX_DECLARATION_H__

#include "ZEDS/Array.h"
#include "Types.h"

#define ZE_VE_POSITION				0x00000008
#define ZE_VE_NORMAL				0x00000010
#define ZE_VE_TANGENT				0x00000020
#define ZE_VE_BINORMAL				0x00000040
#define ZE_VE_SKIN					0x00000080
#define ZE_VE_TEXCOORD(Count)		(0x00000004 && (Count))

enum ZEVertexElementSemantic
{
	ZE_VES_POSITION		= 1,
	ZE_VES_NORMAL		= 2,
	ZE_VES_BINORMAL		= 3,
	ZE_VES_TANGENT		= 4,
	ZE_VES_TEXTCOORD	= 5,
	ZE_VES_COLOR		= 6,
	ZE_VES_BLENDINDEX	= 7,
	ZE_VES_BLENDWEIGHT	= 8
};

enum ZEVertexElementType
{
	ZE_VET_FLOAT		= 1,
	ZE_VET_FLOAT2		= 2,
	ZE_VET_FLOAT3		= 3,
	ZE_VET_FLOAT4		= 4,
	ZE_VET_SHORT2		= 5,
	ZE_VET_SHORT4		= 6,
	ZE_VET_BYTE4		= 7,
	ZE_VET_COLOR		= 8,
};

struct ZEVertexElement
{
	/*public:	*/
		ZEVertexElementSemantic						Semantic;
		ZEVertexElementType							Type;
		unsigned int								Index;
};

class ZEResourceFile;
class ZEVertexDeclaration
{
	protected:
													ZEVertexDeclaration();
		virtual										~ZEVertexDeclaration();

	public:
		virtual const ZEArray<ZEVertexElement>&		GetVertexElements() = 0;
		virtual size_t								GetVertexSize() = 0;

		virtual bool								Create(const ZEArray<ZEVertexElement>& VertexElements);
		virtual bool								Create(const ZEVertexElement* Elements, size_t ElementCount) = 0;
		virtual void								Release() = 0;
		virtual void								Destroy();

		virtual void								SetupVertexDeclaration() = 0;

		static ZEVertexDeclaration*					LoadFromFile(const char* FileName);
		static ZEVertexDeclaration*					LoadFromFile(ZEResourceFile* ResourceFile);
		static ZEVertexDeclaration*					CreateInstance();
};

enum ZEVertexType
{
	ZE_VT_NOTSET				= 0,
	ZE_VT_SIMPLEVERTEX			= 1,
	ZE_VT_MAPVERTEX				= 2,
	ZE_VT_MODELVERTEX			= 3,
	ZE_VT_SKINNEDMODELVERTEX	= 4,
	ZE_VT_GUIVERTEX				= 5
};
#endif
