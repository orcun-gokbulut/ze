//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEVertexLayout.cpp
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

#include "ZEError.h"
#include "ZEShader.h"
#include "ZEStatePool.h"
#include "ZEFile/ZEFile.h"
#include "ZEDefinitions.h"
#include "ZEVertexLayout.h"
#include "ZEVertexBuffer.h"
#include "ZEDS/ZEHashGenerator.h"
#include "ZEGraphics/ZEGraphicsModule.h"

ZESize ZEVertexLayout::GetHash()
{
	if (Dirty)
	{
		Dirty = false;
		Hash = ZEHashGenerator::Hash(&StateData, sizeof(ZEVertexLayoutData));
	}

	return Hash;
}

bool ZEVertexLayout::IsEmpty() const
{
	return StateData.ElementCount == 0;
}

void ZEVertexLayout::SetToDefault()
{
	Hash = 0;
	Dirty = false; 
	
	memset(&StateData, 0, sizeof(ZEVertexLayoutData));
}

void ZEVertexLayout::SetLayout(const ZEVertexElement* VertexElements, ZEUInt ElementCount)
{
	zeDebugCheck(VertexElements == NULL, "NULL pointer");
	zeDebugCheck(ElementCount == 0, "Zero element count");
	zeDebugCheck(ElementCount >= ZE_MAX_VERTEX_LAYOUT_ELEMENT, "Too many elements");

	SetToDefault();

	StateData.ElementCount = ElementCount;
	for (ZESize I = 0; I < ElementCount; ++I)
	{
		zeDebugCheck(strnlen(VertexElements[I].Semantic, ZE_MAX_SHADER_VARIABLE_NAME) == ZE_MAX_SHADER_VARIABLE_NAME, 
						"Wrong vertex buffer element semantic");

		sprintf(StateData.VertexElements[I].Semantic, "%s", VertexElements[I].Semantic);
		StateData.VertexElements[I].Index = VertexElements[I].Index;
		StateData.VertexElements[I].Type = VertexElements[I].Type;
		StateData.VertexElements[I].Stream = VertexElements[I].Stream;
		StateData.VertexElements[I].Offset = VertexElements[I].Offset;
		StateData.VertexElements[I].Usage = VertexElements[I].Usage;
		StateData.VertexElements[I].InstanceCount = VertexElements[I].InstanceCount;
	}

	Dirty = true;
}

const ZEVertexElement* ZEVertexLayout::GetLayout() const
{
	return StateData.VertexElements;
}

ZEUInt ZEVertexLayout::GetElementCount() const
{
	return StateData.ElementCount;
}

const ZEVertexLayout& ZEVertexLayout::operator=(const ZEVertexLayout& State)
{
	Hash = State.Hash;
	Dirty = State.Dirty;

	memcpy(&StateData, &State.StateData, sizeof(ZEVertexLayoutData));

	return *this;
}

bool ZEVertexLayout::operator==(const ZEVertexLayout& State)
{
	if (State.StateData.ElementCount != StateData.ElementCount)
		return false;

	return memcmp(&StateData.VertexElements, &State.StateData.VertexElements, sizeof(ZEVertexElement) * (ZESize)StateData.ElementCount) == 0 ? true : false;
}

bool ZEVertexLayout::operator!=(const ZEVertexLayout& State)
{
	return !operator==(State);
}

// Registered Vertex buffer elements must have the same arrangement/order with shader inputs!
bool ZEVertexLayout::GenerateLayout(ZEVertexLayout& Output, ZEVertexBuffer* BufferArr[ZE_MAX_VERTEX_BUFFER_SLOT], const ZEShader* VertexShader)
{
	zeDebugCheck(VertexShader == NULL, "NUll pointer.");
	zeDebugCheck(VertexShader->GetShaderType() != ZE_ST_VERTEX, "Wrong shader type");

	const ZEShaderMetaTable* MetaTable = VertexShader->GetMetaTable();
	ZESize ShaderInputCount = MetaTable->Inputs.GetCount();
	if (ShaderInputCount == 0)
		return true;

	Output.SetToDefault();

	ZESize HashSearchList[ZE_MAX_VERTEX_LAYOUT_ELEMENT] = {0};
	
	ZEArray<ZEVertexElement> FinalLayout;
	FinalLayout.SetCount(ShaderInputCount);

	// Generate search list of hashes
	for (ZESize I = 0; I < ShaderInputCount; ++I)
		HashSearchList[I] = MetaTable->Inputs[I].Hash;

	// Iterate all buffers in list only once
	ZESize InputN = 0;
	ZESize BufferN = 0;
	ZESize ElementN = 0;
	for (; InputN < ShaderInputCount && BufferN != ZE_MAX_VERTEX_BUFFER_SLOT; )
	{
		for (; BufferN < ZE_MAX_VERTEX_BUFFER_SLOT; ++BufferN)
		{
			const ZEVertexBuffer* Buffer = BufferArr[BufferN];

			if (Buffer == NULL)
				continue;

			ZESize BufferElementCount = Buffer->GetElementCount();
			const ZEVertexBufferElement* BufferElements = Buffer->GetElements();

			for (; ElementN < BufferElementCount; ++ElementN)
			{
				if (HashSearchList[InputN] != Buffer->Hashes[ElementN])
					continue;
			
				const ZEVertexBufferElement* BufferElement = &BufferElements[ElementN];

				zeDebugCheck(BufferElement->Type != MetaTable->Inputs[InputN].ElementType,
								"Shader input type and registerd vertex buffer element type does not match!");
				
				// Match found
				// Generate layout for the buffer element
				strcpy(FinalLayout[InputN].Semantic, BufferElement->Semantic);
				FinalLayout[InputN].Index = BufferElement->Index;
				FinalLayout[InputN].Type = BufferElement->Type;
				FinalLayout[InputN].Stream = (ZEUInt8)BufferN;
				FinalLayout[InputN].Offset = BufferElement->Offset;
				FinalLayout[InputN].Usage = BufferElement->Usage;
				FinalLayout[InputN].InstanceCount = BufferElement->InstanceCount;

				InputN++;
			}
		}
	}

	if (InputN != ShaderInputCount)
		return false;

	Output.SetLayout(FinalLayout.GetConstCArray(), (ZEUInt)ShaderInputCount);
	return true;
}

ZEVertexLayout::ZEVertexLayout()
{
	SetToDefault();
}

ZEVertexLayout::~ZEVertexLayout()
{
	
}
