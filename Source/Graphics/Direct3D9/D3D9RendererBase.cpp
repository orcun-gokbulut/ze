//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9RendererBase.cpp
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
#ifdef ZEDEBUG_ENABLED
#define D3D_DEBUG_INFO
#endif

#include "D3D9RendererBase.h"
#include "D3D9VertexBuffer.h"
#include "D3D9Texture2D.h"
#include "D3D9Texture3D.h"
#include "D3D9TextureCube.h"
#include "D3D9Module.h"
#include "D3D9CommonTools.h"
#include "D3D9VertexDeclaration.h"
#include "D3D9FixedMaterial.h"
#include "Graphics/RenderOrder.h"
#include "Core/Error.h"

#pragma warning(disable:4267)

void ZED3D9RendererBase::PumpStreams(ZERenderOrder* RenderOrder)
{
	ZEVertexBuffer* VertexBuffer = RenderOrder->VertexBuffer;

	// Figure out primitive type
	D3DPRIMITIVETYPE PrimitiveType;
	switch(RenderOrder->PrimitiveType)
	{
		default:
		case ZE_RLPT_POINT:
			PrimitiveType = D3DPT_POINTLIST;
			break;
		case ZE_RLPT_LINE:
			PrimitiveType = D3DPT_LINELIST;
			break;
		case ZE_RLPT_TRIANGLE:
			PrimitiveType = D3DPT_TRIANGLELIST;
			break;
	}

	// Make draw call
	// Check wheater render order is indexed or not
	if (RenderOrder->IndexBuffer != NULL)
	{
		/* NOT SUPPORTED !!! */
		/*
		// Check wheater render order has static vertex buffer or not
		if (VertexBuffer->IsStatic())
			GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, VertexBuffer->GetVertexCount(), 0, VertexBuffer->GetPolygonCount());
		else
			GetDevice()->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, VertexBuffer->GetVertexCount(), RenderOrder->PrimitiveCount, RenderOrder->IndexBuffer, D3DFMT_INDEX32, VertexBuffer->GetVertexBuffer(), RenderOrder->VertexDeclaration->GetVertexSize());
		*/
	} 
	else
	{
		// Check wheater render order has static vertex buffer or not
		if (VertexBuffer->IsStatic())
			GetDevice()->DrawPrimitive(PrimitiveType, 0, RenderOrder->PrimitiveCount);
		else
			GetDevice()->DrawPrimitiveUP(PrimitiveType, RenderOrder->PrimitiveCount, ((ZEDynamicVertexBuffer*)VertexBuffer)->GetVertexBuffer(),  RenderOrder->VertexDeclaration->GetVertexSize());
	}
}

void ZED3D9RendererBase::DrawRenderOrder(ZERenderOrder* RenderOrder, ZECamera* Camera)
{
	// Get Vertex buffer of render order
	ZED3D9StaticVertexBuffer* VertexBuffer = (ZED3D9StaticVertexBuffer*)RenderOrder->VertexBuffer;

	// Check render order has vertex buffer and one or more primitives.
	if (RenderOrder->VertexBuffer == NULL || RenderOrder->PrimitiveCount == 0)
		return;

	// Get material of the render order
	const ZEMaterial* Material = RenderOrder->Material;

	// Setup vertex declaration
	RenderOrder->VertexDeclaration->SetupVertexDeclaration();

	/* NOT INDEX BUFFERS SUPPORTED ANYMORE */
	/*
	// Setup index buffer if available
	if (RenderOrder->IndexBuffer != NULL)
		GetDevice()->SetIndices(RenderOrder->IndexBuffer);
	*/

	// If vertex buffer is static, setup vertex buffer
	if (VertexBuffer->IsStatic())
		GetDevice()->SetStreamSource(0, VertexBuffer->StaticBuffer, RenderOrder->VertexBufferOffset, RenderOrder->VertexDeclaration->GetVertexSize());

	// Setup Material
	Material->SetupMaterial(RenderOrder, Camera);

	// Do prelightning pass
	Material->SetupPreLightning();

	Material->DoPreLightningPass();
	PumpStreams(RenderOrder);

	// Do light passes
	if (Material->SetupLightning() && RenderOrder->Lights.GetCount() != 0)
	{
		// Sort the lights
		const ZERLLight* PointLightList[64];
		size_t PointLightCount = 0;
		const ZERLLight* DirectionalLightList[64];
		size_t DirectionalLightCount = 0;
		const ZERLLight* ProjectiveLightList[64];
		size_t ProjectiveLightCount = 0;
		const ZERLLight* OmniProjectiveLightList[64];
		size_t OmniProjectiveLightCount = 0;
		const ZERLLight* ShadowedPointLightList[64];
		size_t ShadowedPointLightCount = 0;
		const ZERLLight* ShadowedDirectionalLightList[64];
		size_t ShadowedDirectionalLightCount = 0;
		const ZERLLight* ShadowedProjectiveLightList[64];
		size_t ShadowedProjectiveLightCount = 0;
		const ZERLLight* ShadowedOmniProjectiveLightList[64];
		size_t ShadowedOmniProjectiveLightCount = 0;

		for (size_t I = 0; I < RenderOrder->Lights.GetCount(); I++)
		{			
			const ZERLLight* CurrentLight = RenderOrder->Lights[I];
			switch (CurrentLight->Type)
			{
				case ZE_RLLT_POINT:
					if (CurrentLight->ShadowMap != NULL)
					{
						PointLightList[PointLightCount] = CurrentLight;
						PointLightCount++;
					}
					else
					{
						ShadowedPointLightList[ShadowedPointLightCount] = CurrentLight;
						ShadowedPointLightCount++;
					}
					break;

				case ZE_RLLT_DIRECTIONAL:
					if (CurrentLight->ShadowMap != NULL)
					{
						DirectionalLightList[DirectionalLightCount] = CurrentLight;
						DirectionalLightCount++;
					}
					else
					{
						ShadowedDirectionalLightList[ShadowedDirectionalLightCount] = CurrentLight;
						ShadowedDirectionalLightCount++;
					}
					break;

				case ZE_RLLT_PROJECTIVE:
					if (CurrentLight->ShadowMap != NULL)
					{
						ProjectiveLightList[PointLightCount] = CurrentLight;
						ProjectiveLightCount++;
					}
					else
					{
						ShadowedProjectiveLightList[ShadowedPointLightCount] = CurrentLight;
						ShadowedProjectiveLightCount++;
					}
					break;

				case ZE_RLLT_OMNIPROJECTIVE:
					if (CurrentLight->ShadowMap != NULL)
					{
						OmniProjectiveLightList[OmniProjectiveLightCount] = CurrentLight;
						OmniProjectiveLightCount++;

					}
					else
					{
						ShadowedOmniProjectiveLightList[OmniProjectiveLightCount] = CurrentLight;
						ShadowedOmniProjectiveLightCount++;
					}
					break;
			}
		}
	
		// Draw point light passes
		if (PointLightCount != 0)
		{
			// Setup material for point light passes
			Material->SetupPointLightPass(false);			
			while (PointLightCount != 0)
			{
				// Setup point light pass and decrease processed light coun t
				//		Materials can have multiple lights per pass or multiple pass per light so DoPointLightPass 
				//		function returns how many light will be processed in a particular pass. If its returns zero
				//		it means renderer should do multiple passes per lights. On multiple pass per light situation
				//		DoPointLightPass will return 0 until light has been successfully processed. Then it will return
				//		one. (In multiple lights per multiple pass it will return more than zero) 
				// Decrease number of light count.

				PointLightCount -= Material->DoPointLightPass(PointLightList, PointLightCount);

				// Draw primitives
				PumpStreams(RenderOrder);
			}
		}

		// Draw shadowed point light passes
		if (ShadowedPointLightCount != 0)
		{
			Material->SetupPointLightPass(true);			
			while (PointLightCount != 0)
			{
				ShadowedPointLightCount -= Material->DoPointLightPass(ShadowedPointLightList, PointLightCount);
				PumpStreams(RenderOrder);
			}
		}

		// Draw directional light passes
		if (DirectionalLightCount != 0)
		{
			Material->SetupDirectionalLightPass(false);			
			while (DirectionalLightCount != 0)
			{
				DirectionalLightCount -= Material->DoDirectionalLightPass(DirectionalLightList, DirectionalLightCount);
				PumpStreams(RenderOrder);
			}
		}

		// Draw shadowed directional light passes
		if (ShadowedDirectionalLightCount != 0)
		{
			Material->SetupDirectionalLightPass(true);			
			while (ShadowedDirectionalLightCount != 0)
			{
				ShadowedDirectionalLightCount -= Material->DoDirectionalLightPass(ShadowedDirectionalLightList, ShadowedDirectionalLightCount);
				PumpStreams(RenderOrder);
			}
		}

		// Draw projective light passes
		if (ProjectiveLightCount != 0)
		{
			Material->SetupProjectiveLightPass(false);			
			while (ProjectiveLightCount != 0)
			{
				ProjectiveLightCount -= Material->DoProjectiveLightPass(ProjectiveLightList, ProjectiveLightCount);
				PumpStreams(RenderOrder);
			}
		}

		// Draw shadowed projective light passes		
		if (ShadowedProjectiveLightCount != 0)
		{
			Material->SetupProjectiveLightPass(true);			
			while (ShadowedProjectiveLightCount != 0)
			{
				ShadowedProjectiveLightCount -= Material->DoProjectiveLightPass(ShadowedProjectiveLightList, ShadowedProjectiveLightCount);
				PumpStreams(RenderOrder);
			}
		}
		
		// Draw omni projective light passes
		if (OmniProjectiveLightCount != 0)
		{
			Material->SetupOmniProjectiveLightPass(false);			
			while (OmniProjectiveLightCount != 0)
			{
				OmniProjectiveLightCount -= Material->DoOmniProjectivePass(OmniProjectiveLightList, OmniProjectiveLightCount);
				PumpStreams(RenderOrder);
			}
		}

		// Draw shadowed omni projective light passes
		if (ShadowedOmniProjectiveLightCount != 0)
		{
			Material->SetupOmniProjectiveLightPass(true);			
			while (ShadowedOmniProjectiveLightCount != 0)
			{
				ShadowedOmniProjectiveLightCount -= Material->DoOmniProjectivePass(ShadowedOmniProjectiveLightList, ShadowedOmniProjectiveLightCount);
				PumpStreams(RenderOrder);
			}
		}
	}
}



bool ZED3D9RendererBase::CheckRenderOrder(ZERenderOrder* RenderOrder)
{
	#ifdef ZEDEBUG_ENABLED
		// Check render order material is available
		if (RenderOrder->Material == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's material does not have valid material.");
			return false;
		}

		// Check render order material has shader
		/*if (RenderOrder->Material->GetShader() == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's material does not have valid shader.");
			return false;
		}*/

		// Check vertex declaration is available
		if (RenderOrder->VertexDeclaration == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's vertex declaration is invalid.");
			return false;
		}

		// Check render order has valid primitive type
		if (RenderOrder->PrimitiveType != ZE_RLPT_POINT &&
			RenderOrder->PrimitiveType != ZE_RLPT_LINE &&
			RenderOrder->PrimitiveType != ZE_RLPT_TRIANGLE &&
			RenderOrder->PrimitiveType != ZE_RLPT_TRIANGLESTRIPT)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's primitive type is not valid.");
			return false;
		}

		// Check render order has vertex buffer
		/*if (RenderOrder->VertexBuffer == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's vertex buffer is not valid.");
			return false;
		}*/

		// Check render order has one or more primitive
		/*if (RenderOrder->PrimitiveCount == 0)
		{
			zeError("Direct3D9 Renderer", "RenderOrder's primitive count is zero.");
			return false;
		}*/
	#else
		if (RenderOrder->PrimitiveCount == 0)
			return false;
	#endif

	return true;
}
