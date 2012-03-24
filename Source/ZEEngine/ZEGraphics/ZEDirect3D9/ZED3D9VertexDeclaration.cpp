//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9VertexDeclaration.cpp
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

#include "ZED3D9VertexDeclaration.h"
#include "ZEError.h"
#include <D3D9.h>

const ZEArray<ZEVertexElement>& ZED3D9VertexDeclaration::GetVertexElements()
{
	return VertexElements;
}

ZESize ZED3D9VertexDeclaration::GetVertexSize()
{
	return VertexSize;
}

void ZED3D9VertexDeclaration::SetupVertexDeclaration()
{
	GetDevice()->SetVertexDeclaration(VertexDeclaration);
}

bool ZED3D9VertexDeclaration::CreateVertexDeclaration(const ZEVertexElement* Elements, ZESize ElementCount)
{
	if (ElementCount > 12)
	{
		zeError("Vertex element count in vertex declaration is too high. (Max Vertex Element Count : 12, Vertex Element Count : %d)", ElementCount);
		return false;
	}

	VertexSize = 0;


	static D3DVERTEXELEMENT9 D3DElements[13];	
	for (ZESize I = 0; I < ElementCount; I++)
	{
		D3DElements[I].Stream = 0;
		D3DElements[I].Offset = (WORD)VertexSize;
		D3DElements[I].UsageIndex = Elements[I].Index;
		D3DElements[I].Method = D3DDECLMETHOD_DEFAULT;
		switch (Elements[I].Semantic)
		{
			case ZE_VES_POSITION:
				D3DElements[I].Usage = D3DDECLUSAGE_POSITION;
				break;
			case ZE_VES_NORMAL:
				D3DElements[I].Usage = D3DDECLUSAGE_NORMAL;
				break;
			case ZE_VES_BINORMAL:
				D3DElements[I].Usage = D3DDECLUSAGE_BINORMAL;
				break;
			case ZE_VES_TANGENT:
				D3DElements[I].Usage = D3DDECLUSAGE_TANGENT;
				break;
			case ZE_VES_TEXTCOORD:
				D3DElements[I].Usage = D3DDECLUSAGE_TEXCOORD;
				break;
			case ZE_VES_COLOR:
				D3DElements[I].Usage = D3DDECLUSAGE_COLOR;
				break;
			case ZE_VES_BLENDINDEX:
				D3DElements[I].Usage = D3DDECLUSAGE_BLENDINDICES;
				break;
			case ZE_VES_BLENDWEIGHT:
				D3DElements[I].Usage = D3DDECLUSAGE_BLENDWEIGHT;
				break;
			default:
				zeError("Wrong Vertex Element Semantic. (Index : %d)", I);
				return false;
				break;
		}

		switch (Elements[I].Type)
		{
			case ZE_VET_FLOAT:
				D3DElements[I].Type = D3DDECLTYPE_FLOAT1;
				VertexSize += 4;
				break;
			case ZE_VET_FLOAT2:
				D3DElements[I].Type = D3DDECLTYPE_FLOAT2;
				VertexSize += 8;
				break;
			case ZE_VET_FLOAT3:
				D3DElements[I].Type = D3DDECLTYPE_FLOAT3;
				VertexSize += 12;
				break;
			case ZE_VET_FLOAT4:
				D3DElements[I].Type = D3DDECLTYPE_FLOAT4;
				VertexSize += 16;
				break;
			case ZE_VET_COLOR:
				D3DElements[I].Type = D3DDECLTYPE_D3DCOLOR;
				VertexSize += 4;
				break;
			case ZE_VET_BYTE4:
				D3DElements[I].Type = D3DDECLTYPE_UBYTE4;
				VertexSize += 4;
				break;
			case ZE_VET_SHORT2:
				D3DElements[I].Type = D3DDECLTYPE_SHORT2;
				VertexSize += 4;
				break;
			case ZE_VET_SHORT4:
				D3DElements[I].Type = D3DDECLTYPE_SHORT4;
				VertexSize += 8;
				break;
			default:
				zeError("Wrong Vertex Element Type. (Index : %d)", I);
				return false;
		}
	}

	D3DElements[ElementCount].Stream = 0xFF;
	D3DElements[ElementCount].Offset = 0;
	D3DElements[ElementCount].Type = D3DDECLTYPE_UNUSED;
	D3DElements[ElementCount].Method = 0;
	D3DElements[ElementCount].Usage = 0;
	D3DElements[ElementCount].UsageIndex = 0;

	if (GetDevice()->CreateVertexDeclaration(D3DElements, &VertexDeclaration) != D3D_OK)
	{
		zeError("Can not create vertex declaration.");
		Release();
		return false;
	}

	return true;
}

bool ZED3D9VertexDeclaration::Create(const ZEVertexElement* Elements, ZESize ElementCount)
{
	Release();

	if (!CreateVertexDeclaration(Elements, ElementCount))
		return false;

	VertexElements.MassAdd(Elements, ElementCount);
	return true;
}

void ZED3D9VertexDeclaration::Release()
{
	VertexElements.Clear();
	if (VertexDeclaration != NULL)
	{
		VertexDeclaration->Release();
		VertexDeclaration = NULL;
	}
}

ZED3D9VertexDeclaration::ZED3D9VertexDeclaration()
{
	VertexDeclaration = NULL;
}		

ZED3D9VertexDeclaration::~ZED3D9VertexDeclaration()
{
	Release();
}





