//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVertexDeclaration.cpp
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

#include "ZEVertexDeclaration.h"
#include "ZEDefinitions.h"
#include "ZECore\ZEError.h"
#include "ZECore\ZEResourceFile.h"
#include "ZEGraphics\ZEGraphicsModule.h"

#define ZE_FILE_FORMAT_MAKEVERSION(Major, Minor)		((ZEDWORD)((((ZEDWORD)(Major)) << 16) + (ZEDWORD)(Minor)))
#define ZE_FILE_FORMAT_GETMINORVERSION(x)				((ZEDWORD)((x) & 0x0000FFFF))
#define ZE_FILE_FORMAT_GETMAJORVERSION(x)				((ZEDWORD)(((x) & 0xFFFF0000) >> 16))

#define ZE_VERTEX_DECLARATION_FILE_VERSION				ZE_FILE_FORMAT_MAKEVERSION(0,1)
#define ZE_VERTEX_DECLARATION_FILE_HEADER_CHUNK_ID		(ZEDWORD)'ZEVD'

struct ZEVertexDeclarationFileHeaderChunk
{
	ZEDWORD		ChunkIndentifier;
	ZEDWORD		Version;
	ZEDWORD		VertexElementCount;
};

struct ZEVertexDeclarationFileElementChunk
{
	ZEDWORD		Semantic;
	ZEDWORD		Type;
	ZEDWORD		Index;
};
ZEVertexDeclaration::ZEVertexDeclaration()
{
	
}

ZEVertexDeclaration::~ZEVertexDeclaration()
{

}		

bool ZEVertexDeclaration::Create(const ZEArray<ZEVertexElement>& VertexElements)
{
	return Create(VertexElements.GetConstCArray(), VertexElements.GetCount());
}

void ZEVertexDeclaration::Destroy()
{
	delete this;
}

ZEVertexDeclaration* ZEVertexDeclaration::LoadFromFile(const char* FileName)
{
	ZEResourceFile File;
	if (!File.Open(FileName))
	{
		zeError("Vertex Declaration", "Can not open zeVertexDecl file. (FileName : \"%s\")", FileName);
		return NULL;
	}
	
	ZEVertexDeclaration* VertexDeclaration = ZEVertexDeclaration::LoadFromFile(&File);
	
	File.Close();
	
	return VertexDeclaration;
}


ZEVertexDeclaration* ZEVertexDeclaration::LoadFromFile(ZEResourceFile* ResourceFile)
{
	ZEVertexDeclarationFileHeaderChunk HeaderChunk;
	ResourceFile->Read(&HeaderChunk, sizeof(ZEVertexDeclarationFileHeaderChunk), 1);
	if (HeaderChunk.ChunkIndentifier != ZE_VERTEX_DECLARATION_FILE_HEADER_CHUNK_ID)
	{
		zeError("Vertex Declaration", "Unknown zeVertex file format. (FileName : \"%s\")", ResourceFile->GetFileName());
		return NULL;
	}

	if(HeaderChunk.Version != ZE_VERTEX_DECLARATION_FILE_VERSION)
	{	
		zeError("Map Resource", "ZEMap file version mismatched. (FileName : \"%s\")", ResourceFile->GetFileName());
		return false;
	}
	
	ZEArray<ZEVertexElement> VertexElements;
	VertexElements.SetCount(HeaderChunk.VertexElementCount);
	
	ZEVertexDeclarationFileElementChunk ElementChunk;
	for (size_t I = 0; I < HeaderChunk.VertexElementCount; I++)
	{
		ResourceFile->Read(&ElementChunk, sizeof(ZEVertexDeclarationFileElementChunk), 1);
		VertexElements[I].Semantic = (ZEVertexElementSemantic)ElementChunk.Semantic;
		VertexElements[I].Type = (ZEVertexElementType)ElementChunk.Type;
		VertexElements[I].Index = ElementChunk.Index;
	}

	ZEVertexDeclaration* VertexDeclaration = ZEVertexDeclaration::CreateInstance();
	if (!VertexDeclaration->Create(VertexElements))
	{
		VertexDeclaration->Release();
		return NULL;
	}

	return VertexDeclaration;
}

ZEVertexDeclaration* ZEVertexDeclaration::CreateInstance()
{
	return zeGraphics->CreateVertexDeclaration();
}





