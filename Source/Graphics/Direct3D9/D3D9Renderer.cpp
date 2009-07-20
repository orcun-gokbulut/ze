//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - D3D9Renderer.cpp
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
#include "Core/Core.h"
#include "D3D9Renderer.h"
#include "D3D9VertexBuffer.h"
#include "D3D9Texture.h"
#include "Direct3D9Module.h"
#include "D3D9Common.h"
#pragma warning(disable:4267)

LPDIRECT3DVERTEXDECLARATION9 ZED3D9RendererBase::VertexDeclarations[8];

bool ZED3D9RendererBase::InitializeVertexDeclarations()
{
	D3DVERTEXELEMENT9 SimpleVertexDecl[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	Device->CreateVertexDeclaration(SimpleVertexDecl, &VertexDeclarations[ZE_VT_SIMPLEVERTEX]);

	D3DVERTEXELEMENT9 MapVertexDecl[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
		{0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	Device->CreateVertexDeclaration(MapVertexDecl, &VertexDeclarations[ZE_VT_MAPVERTEX]);

	D3DVERTEXELEMENT9 ModelVertexDecl[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
		{0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	Device->CreateVertexDeclaration(ModelVertexDecl, &VertexDeclarations[ZE_VT_MODELVERTEX]);

	D3DVERTEXELEMENT9 SkinnedModelVertexDecl[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0},
		{0, 48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 56, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
		{0, 60, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
		D3DDECL_END()
	};
	Device->CreateVertexDeclaration(SkinnedModelVertexDecl, &VertexDeclarations[ZE_VT_SKINNEDMODELVERTEX]);

	D3DVERTEXELEMENT9 UIVertexDecl[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	Device->CreateVertexDeclaration(UIVertexDecl, &VertexDeclarations[ZE_VT_GUIVERTEX]);
	return true;
}

void ZED3D9RendererBase::DeinitializeVertexDeclarations()
{
	for (int I = 0; I < 8; I++)
		ZED3D_RELEASE(VertexDeclarations[I]);
}

void ZED3D9RendererBase::PumpStreams(ZERenderList* RenderList)
{
	ZED3D9StaticVertexBuffer* VertexBuffer = (ZED3D9StaticVertexBuffer*)RenderList->VertexBuffer;
	D3DPRIMITIVETYPE PrimitiveType;
	switch(RenderList->PrimitiveType)
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

	if (RenderList->IndexBuffer != NULL)
	{
		/*
		Device->SetIndices(RenderList->IndexBuffer);
		if (VertexBuffer->IsStatic())
		{
			Device->SetStreamSource(0, StaticVertexBuffers[VertexBuffer, 0, RenderL VertexBuffer->GetVertexSize());
			Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, VertexBuffer->GetVertexCount(), 0, VertexBuffer->GetPolygonCount());
		}
		else
			Device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, VertexBuffer->GetVertexCount(), VertexBuffer->GetPolygonCount(), VertexBuffer->GetIndexBuffer(), D3DFMT_INDEX32, VertexBuffer->GetVertexBuffer(), VertexBuffer->GetVertexSize());
		*/
	} 
	else
	{
		if (VertexBuffer->IsStatic())
		{
			Device->SetStreamSource(0, VertexBuffer->StaticBuffer, RenderList->VertexBufferOffset, zeGetVertexSize(RenderList->VertexType));
			Device->DrawPrimitive(PrimitiveType, 0, RenderList->PrimitiveCount);
		}
		else
			Device->DrawPrimitiveUP(PrimitiveType, RenderList->PrimitiveCount, ((ZEDynamicVertexBuffer*)VertexBuffer)->GetVertexBuffer(), zeGetVertexSize(RenderList->VertexType));
	}
}

void ZED3D9RendererBase::SetShaderPass(ZED3D9ShaderPass* Pass, bool Skinned)
{
	if (Skinned)
		Device->SetVertexShader(Pass->SkinnedVertexShader);
	else
		Device->SetVertexShader(Pass->VertexShader);

	Device->SetPixelShader(Pass->PixelShader);
}

void ZED3D9RendererBase::DrawSM2(ZERenderList* RenderList, const ZEViewPoint& ViewPoint)
{
	const ZEMaterial* Material = RenderList->Material;
	
	ZED3D9Shader* Shader = (ZED3D9Shader*)RenderList->Material->GetShader();

	if (RenderList->Flags & ZE_RLF_SKINNED)
		Device->SetVertexShaderConstantF(32, (float*)RenderList->BoneTransforms.GetCArray(), RenderList->BoneTransforms.GetCount() * 4);

	//((ZEDefaultMaterial*)RenderList->Material)->AmbientColor = ZEVector3(0.5, 0.5, 0.5);
	/*if (RenderList->IndexBuffer != NULL)
		if (RenderList->IndexBuffer IsStaticIndexBuffer())
			Device->SetIndices(StaticIndexBuffers[RenderList->GetStaticIndexBufferId()]);	*/

	int TextureCount;
	const ZETextureBase** Textures = Material->GetTextures(&TextureCount);
	
	for (int I = 0; I < TextureCount; I++)
		if (Textures[I] != NULL)
			switch(Textures[I]->GetTextureType())
			{
				case ZE_TT_SURFACE:
					Device->SetTexture(I, ((ZED3D9Texture*)Textures[I])->Texture);
					break;
				case ZE_TT_CUBE:
					Device->SetTexture(I, ((ZED3D9CubeTexture*)Textures[I])->CubeTexture);
					break;
			}

	int VertexShaderParameterCount;
	const ZEVector4* VertexShaderParameters =	Material->GetVertexShaderConstants(&VertexShaderParameterCount);
	if (VertexShaderParameterCount != 0)
		Device->SetVertexShaderConstantF(12, (const float*)VertexShaderParameters, VertexShaderParameterCount);

	int PixelShaderParameterCount;
	const ZEVector4* PixelShaderParameters = Material->GetPixelShaderConstants(&PixelShaderParameterCount);
	if (PixelShaderParameterCount != 0)
		Device->SetPixelShaderConstantF(0, (const float*)PixelShaderParameters, PixelShaderParameterCount);
	
	if (RenderList->Flags & ZE_RLF_ENABLE_VIEWPROJECTION_TRANSFORM)
	{
		ZEMatrix4x4 WorldViewProjMatrix;
		ZEMatrix4x4::Multiply(WorldViewProjMatrix, RenderList->WorldMatrix, ViewPoint.ViewProjMatrix);
		Device->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);
	}
	else
		Device->SetVertexShaderConstantF(0, (float*)&RenderList->WorldMatrix, 4);

	Device->SetVertexShaderConstantF(4, (float*)&RenderList->WorldMatrix, 4);
	Device->SetVertexShaderConstantF(8, (float*)&RenderList->WorldMatrix, 4);
	Device->SetVertexShaderConstantF(16, (float*)&ZEVector4(ViewPoint.ViewPosition, 1.0f), 1);

	if (RenderList->Flags & ZE_RLF_ENABLE_ZCULLING)
	{
		Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		if (RenderList->Flags & (ZE_RLF_TRANSPARENT | ZE_RLF_IMPOSTER))
			Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		else
			Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	}
	else
		Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	
	if (Material->TwoSided)
		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	else
		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	if (Material->Wireframe)
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	
	if (Material->TransparancyMode != ZE_TM_NOTRANSPARACY)
	{
		Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		Device->SetRenderState(D3DRS_ALPHAREF, Material->TransparancyCullLimit);
		Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		switch(Material->TransparancyMode)
		{
			case ZE_TM_ADDAPTIVE:
				Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
			case ZE_TM_SUBTRACTIVE:
				Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_SUBTRACT);
				Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
			case ZE_TM_REGULAR:
				Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
				Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
			case ZE_TM_ALPHACULL:
				break;
		}

	}
	else
	{
		Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}

	Device->SetVertexDeclaration(VertexDeclarations[RenderList->VertexType]);
	SetShaderPass(&Shader->PreLightPass, RenderList->Flags & ZE_RLF_SKINNED);
	PumpStreams(RenderList);



	if (Material->LightningEnabled && RenderList->Lights.GetCount() != 0)
	{
		Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL);
		Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		for (size_t I = 0; I < RenderList->Lights.GetCount(); I++)
		{			
			const ZERLLight* CurrentLight = RenderList->Lights[I];

			switch(CurrentLight->Type)
			{
				case ZE_RLLT_POINT:
					Device->SetVertexShaderConstantF(24, (const float*)&CurrentLight->Position, 1);
					Device->SetVertexShaderConstantF(25, (const float*)&CurrentLight->Attenuation, 1);

					Device->SetPixelShaderConstantF(12, (const float*)&CurrentLight->Color, 1);
					Device->SetPixelShaderConstantF(13, (const float*)&ZEVector4(CurrentLight->Intensity, CurrentLight->Range, 0.0f, 0.0f), 1);

					if (CurrentLight->ShadowMap != NULL && Material->RecivesShadow)
					{
						SetShaderPass(&Shader->ShadowedPointLightPass, RenderList->Flags & ZE_RLF_SKINNED);
						Device->SetTexture(8, ((ZED3D9CubeTexture*)CurrentLight->CubeShadowMap)->CubeTexture);
					}
					else
						SetShaderPass(&Shader->PointLightPass, RenderList->Flags & ZE_RLF_SKINNED);
					PumpStreams(RenderList);
					break;

				case ZE_RLLT_DIRECTIONAL:
					Device->SetVertexShaderConstantF(24, (const float*)&CurrentLight->Direction, 1);

					Device->SetPixelShaderConstantF(12, (const float*)&CurrentLight->Color, 1);
					Device->SetPixelShaderConstantF(13, (const float*)&ZEVector4(CurrentLight->Intensity, 0.0f, 0.0f, 0.0f), 1);

					if (CurrentLight->ShadowMap != NULL && Material->RecivesShadow)
					{
						SetShaderPass(&Shader->ShadowedDirectionalLightPass, RenderList->Flags & ZE_RLF_SKINNED);
						Device->SetTexture(8, ((ZED3D9Texture*)CurrentLight->ShadowMap)->Texture);
					}
					else
						SetShaderPass(&Shader->DirectionalLightPass, RenderList->Flags & ZE_RLF_SKINNED);
					PumpStreams(RenderList);
					break;

				case ZE_RLLT_PROJECTIVE:
					Device->SetVertexShaderConstantF(24, (const float*)&CurrentLight->Position, 1);
					Device->SetVertexShaderConstantF(25, (const float*)&CurrentLight->Attenuation, 1);
					Device->SetVertexShaderConstantF(28, (float*)&CurrentLight->LightViewProjMatrix, 4);

					Device->SetPixelShaderConstantF(12, (const float*)&CurrentLight->Color, 1);
					Device->SetPixelShaderConstantF(13, (const float*)&ZEVector4(CurrentLight->Intensity, 0.0f, 0.0f, 0.0f), 1);

					Device->SetSamplerState(9, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
					Device->SetSamplerState(9, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
					Device->SetSamplerState(9, D3DSAMP_BORDERCOLOR, 0x00);
					if (CurrentLight->ProjectionMap != NULL)
						Device->SetTexture(9, ((ZED3D9Texture*)CurrentLight->ProjectionMap)->Texture);

					if (CurrentLight->ShadowMap != NULL && Material->RecivesShadow)
					{ 
						Device->SetSamplerState(8, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
						Device->SetSamplerState(8, D3DSAMP_MINFILTER, D3DTEXF_POINT);
						Device->SetSamplerState(8, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
						Device->SetSamplerState(8, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
						Device->SetSamplerState(8, D3DSAMP_BORDERCOLOR, 0x00);	
						Device->SetTexture(8, ((ZED3D9Texture*)CurrentLight->ShadowMap)->Texture);
						Device->SetPixelShaderConstantF(14, 
							(const float*)&ZEVector4(1.0f / ((ZED3D9Texture*)CurrentLight->ShadowMap)->GetWidth(), 
							1.0f / ((ZED3D9Texture*)CurrentLight->ShadowMap)->GetHeight(), 0.0f, 0.0f),1);
						SetShaderPass(&Shader->ShadowedProjectiveLightPass, RenderList->Flags & ZE_RLF_SKINNED);
					}
					else
						SetShaderPass(&Shader->ProjectiveLightPass, RenderList->Flags & ZE_RLF_SKINNED);

					PumpStreams(RenderList);
					break;

				case ZE_RLLT_OMNIPROJECTIVE:
					Device->SetVertexShaderConstantF(24, (const float*)&CurrentLight->Position, 1);
					Device->SetVertexShaderConstantF(25, (const float*)&CurrentLight->Attenuation, 1);
					Device->SetVertexShaderConstantF(28, (float*)&CurrentLight->LightRotationMatrix, 4);

					Device->SetPixelShaderConstantF(12, (const float*)&CurrentLight->Color, 1);
					Device->SetPixelShaderConstantF(13, (const float*)&ZEVector4(CurrentLight->Intensity, 0.0f, 0.0f, 0.0f), 1);

					Device->SetSamplerState(9, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
					Device->SetSamplerState(9, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
					Device->SetSamplerState(9, D3DSAMP_BORDERCOLOR, 0x00);
					if (CurrentLight->CubeProjectionMap != NULL)
						Device->SetTexture(9, ((ZED3D9CubeTexture*)CurrentLight->CubeProjectionMap)->CubeTexture);

					if (CurrentLight->ShadowMap != NULL && Material->RecivesShadow)
					{
						SetShaderPass(&Shader->ShadowedProjectiveLightPass, RenderList->Flags & ZE_RLF_SKINNED);
						Device->SetTexture(8, ((ZED3D9Texture*)CurrentLight->ShadowMap)->Texture);
					}
					else
						SetShaderPass(&Shader->OmniProjectiveLightPass, RenderList->Flags & ZE_RLF_SKINNED);

					PumpStreams(RenderList);
					break;
			}
		}
	}
}

bool ZED3D9RendererBase::BaseInitialize()
{
	if (!InitializeVertexDeclarations())
		return false;

	return true;
}

void ZED3D9RendererBase::BaseDeinitialize()
{
	Device = NULL;
	Module = NULL;

	DeinitializeVertexDeclarations();
}

bool ZED3D9RendererBase::Initialize()
{
	return true;
}

void ZED3D9RendererBase::Deinitialize()
{
	ZED3D_RELEASE(RenderZBufferRT);
	ZED3D_RELEASE(RenderColorBufferRT);
}

void ZED3D9RendererBase::Destroy()
{
	Module->Renderers.DeleteValue((ZED3D9RendererBase*)this);
	delete this;
}

void ZED3D9RendererBase::SetViewPoint(const ZEViewPoint& ViewPoint)
{
	this->ViewPoint = ViewPoint;
}

void ZED3D9RendererBase::ClearList()
{
	Imposter.Clear(true);
	Transparent.Clear(true);
	NonTransparent.Clear(true);
}

void ZED3D9RendererBase::AddToRenderList(ZERenderList* RenderList)
{
	#ifdef ZEDEBUG_ENABLED
		if (RenderList->Material == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderList's material does not have valid material.");
			return;
		}

		if (RenderList->Material->GetShader() == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderList's material does not have valid shader.");
			return;
		}

		if (RenderList->VertexType == ZE_VT_NOTSET)
		{
			zeError("Direct3D9 Renderer", "RenderList's vertex type is invalid.");
			return;
		}

		if (RenderList->PrimitiveType != ZE_RLPT_POINT &&
			RenderList->PrimitiveType != ZE_RLPT_LINE &&
			RenderList->PrimitiveType != ZE_RLPT_TRIANGLE &&
			RenderList->PrimitiveType != ZE_RLPT_TRIANGLESTRIPT)
		{
			zeError("Direct3D9 Renderer", "RenderList's primitive type is not valid.");
			return;
		}

		if (RenderList->VertexBuffer == NULL)
		{
			zeError("Direct3D9 Renderer", "RenderList's vertex buffer is not valid.");
			return;
		}

		if (RenderList->PrimitiveCount == 0)
		{
			zeError("Direct3D9 Renderer", "RenderList's primitive count is zero.");
			return;
		}
	#endif

/*	if (RenderList->Lights.GetCount() != 0 && RenderList->Lights[0]->ShadowMap != NULL)
		return;*/
	if (RenderList->Flags & ZE_RLF_IMPOSTER)
		Imposter.Add(*RenderList);
	if (RenderList->Flags & ZE_RLF_TRANSPARENT)
		Transparent.Add(*RenderList);
	else
		NonTransparent.Add(*RenderList);
}

void ZED3D9RendererBase::Render(float ElaspedTime)
{

	if (!Module->IsEnabled() || Module->IsDeviceLost)
		return;

	Device->SetRenderTarget(0, RenderColorBufferRT);
	Device->SetDepthStencilSurface(RenderZBufferRT);

	Device->SetRenderState(D3DRS_DEPTHBIAS, 0);
	Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);

	Device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00, 1.0f, 0x00000000);

	for (int I = 0; I < 10; I++)
	{
		if (I < 8)
		{
			Device->SetSamplerState(I, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			Device->SetSamplerState(I, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			Device->SetSamplerState(I, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		} 
		else
		{
			Device->SetSamplerState(I, D3DSAMP_BORDERCOLOR, 0x0f);
			Device->SetSamplerState(I, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			Device->SetSamplerState(I, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			Device->SetSamplerState(I, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		}

		if (I == 8)
		{
			Device->SetSamplerState(I, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			Device->SetSamplerState(I, D3DSAMP_MAGFILTER, D3DTEXF_POINT);	
			Device->SetSamplerState(I, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
		}
		else
		{
			Device->SetSamplerState(I, D3DSAMP_MINFILTER, (Module->GetAnisotropicFilter() ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR));
			Device->SetSamplerState(I, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			Device->SetSamplerState(I, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		}


	}

	Device->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

	Device->BeginScene();
	for (size_t I = 0; I < NonTransparent.GetCount(); I++)
		DrawSM2(&NonTransparent[I], ViewPoint);

	for (size_t I = 0; I < Transparent.GetCount(); I++)
		DrawSM2(&Transparent[I], ViewPoint);

	for (size_t I = 0; I < Imposter.GetCount(); I++)
		DrawSM2(&Imposter[I], ViewPoint);

	Device->EndScene();

}


ZED3D9RendererBase::ZED3D9RendererBase()
{
	RenderColorBufferRT = NULL;
	RenderZBufferRT = NULL;	
};

ZED3D9RendererBase::~ZED3D9RendererBase()
{
	Deinitialize();
}

// Framebuffer renderer
bool ZED3D9FrameBufferRenderer::SetOutput(ZECubeTexture* Texture, ZECubeTextureFace Face)
{
	ZEASSERT(true, "You can not set output of a frame buffer renderer.");
	return false;
}

bool ZED3D9FrameBufferRenderer::SetOutput(ZETexture* Texture)
{
	ZEASSERT(true, "You can not set output of a frame buffer renderer.");
	return false;
}

void ZED3D9FrameBufferRenderer::DeviceLost()
{
	Deinitialize();
}

bool ZED3D9FrameBufferRenderer::DeviceRestored()
{
	return Initialize();
}

bool ZED3D9FrameBufferRenderer::Initialize()
{
	if (Module->FrameColorBuffer == NULL || Module->FrameZBuffer == NULL)
	{
		zeError("D3D9 Renderer", "Can not intialize renderer. Direct3D9 module is not initialized.");
		return false;
	}

	Deinitialize();
	Module->FrameColorBuffer->AddRef();
	RenderColorBufferRT = Module->FrameColorBuffer;
	Module->FrameZBuffer->AddRef();
	RenderZBufferRT = Module->FrameZBuffer;
	return true;
}

// Offscreen
bool ZED3D9TextureRenderer::SetOutput(ZECubeTexture* Texture, ZECubeTextureFace Face)
{
	if (!Texture->IsRenderTarget())
	{
		zeError("D3D9 Offscreen Renderer", "Can not set output texture becouse texture is not a render target.");
		return false;
	}

	OutputCubeTexture = Texture;
	OutputCubeTextureFace = Face;
	OutputTexture = NULL;

	LPDIRECT3DCUBETEXTURE9 D3DTexture = ((ZED3D9CubeTexture*)Texture)->CubeTexture;
	CreateDepthRenderTarget(&RenderZBufferRT, Texture->GetEdgeLenght(), Texture->GetEdgeLenght());

	if (RenderColorBufferRT != NULL)
		RenderColorBufferRT->Release();
	
	D3DTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)Face, 0, &RenderColorBufferRT);
	return true;
}

bool ZED3D9TextureRenderer::SetOutput(ZETexture* Texture)
{
	if (!Texture->IsRenderTarget())
	{
		zeError("D3D9 Offscreen Renderer", "Can not set output texture becouse texture is not a render target.");
		return false;
	}

	OutputCubeTexture = NULL;
	OutputTexture = Texture;

	LPDIRECT3DTEXTURE9 D3DTexture = ((ZED3D9Texture*)Texture)->Texture;
	CreateDepthRenderTarget(&RenderZBufferRT, Texture->GetWidth(), Texture->GetHeight());

	if (RenderColorBufferRT != NULL)
		RenderColorBufferRT->Release();

	D3DTexture->GetSurfaceLevel(0, &RenderColorBufferRT);	
	return true;
}

void ZED3D9TextureRenderer::DeviceLost()
{
	Deinitialize();
}

bool ZED3D9TextureRenderer::DeviceRestored()
{
	if (OutputCubeTexture != NULL)
		return SetOutput(OutputCubeTexture, OutputCubeTextureFace);
	else if (OutputTexture != NULL)
		return SetOutput(OutputTexture);
}

ZED3D9TextureRenderer::ZED3D9TextureRenderer()
{
	OutputTexture = NULL;
	OutputCubeTexture = NULL;
}

// Shadow Renderer
#define ZE_MAX_SHADER_SOURCE_SIZE 32768

LPDIRECT3DVERTEXSHADER9 ShadowMapVS = NULL;
LPDIRECT3DVERTEXSHADER9 ShadowMapTextVS = NULL;
LPDIRECT3DPIXELSHADER9 ShadowMapPS = NULL;
LPDIRECT3DPIXELSHADER9 ShadowMapOpasityPS = NULL;
LPDIRECT3DPIXELSHADER9 ShadowMapDiffuseAlphaPS = NULL;
	
bool ZED3D9ShadowRenderer::BaseInitialize()
{
	char SourceVS[ZE_MAX_SHADER_SOURCE_SIZE], SourcePS[ZE_MAX_SHADER_SOURCE_SIZE];
	ZEArray<D3DXMACRO> Macros;

	ZEResourceFile::ReadTextFile("Shaders\\ShadowPass.vs", SourceVS, ZE_MAX_SHADER_SOURCE_SIZE);
	ZEResourceFile::ReadTextFile("Shaders\\ShadowPass.ps", SourcePS, ZE_MAX_SHADER_SOURCE_SIZE);
	
	CompileVertexShader(SourceVS, &ShadowMapVS, &Macros);
	CompilePixelShader(SourcePS, &ShadowMapPS, &Macros);

	Macros.Add();
	Macros[0].Name = "ZESHADER_TRANSPARANT";
	Macros[0].Definition = "";
	Macros.Add();
	Macros[1].Name = NULL;
	Macros[1].Definition = NULL;
	CompileVertexShader(SourceVS, &ShadowMapTextVS, &Macros);

	Macros[1].Definition = "";
	Macros[1].Name = "ZESHADER_OPASITYMAP";
	Macros.Add();
	Macros[2].Name = NULL;
	Macros[2].Definition = NULL;
	CompilePixelShader(SourcePS, &ShadowMapOpasityPS, &Macros);

	Macros.Add();
	Macros[1].Definition = "";
	Macros[1].Name = "ZESHADER_DIFFUSEMAP";
	CompilePixelShader(SourcePS, &ShadowMapDiffuseAlphaPS, &Macros);

	return true;
}

void ZED3D9ShadowRenderer::BaseDeinitialize()
{
	ZED3D_RELEASE(ShadowMapVS);
	ZED3D_RELEASE(ShadowMapTextVS);
	ZED3D_RELEASE(ShadowMapPS);
	ZED3D_RELEASE(ShadowMapOpasityPS);
	ZED3D_RELEASE(ShadowMapDiffuseAlphaPS);
}

void ZED3D9ShadowRenderer::DrawSM2(ZERenderList* RenderList, const ZEViewPoint& ViewPoint)
{
	const ZEMaterial* Material = RenderList->Material;

	if (RenderList->Flags & ZE_RLF_SKINNED)
		Device->SetVertexShaderConstantF(32, (float*)RenderList->BoneTransforms.GetCArray(), RenderList->BoneTransforms.GetCount() * 4);

	/*if (RenderList->IndexBuffer != NULL)
		if (RenderList->IndexBuffer IsStaticIndexBuffer())
			Device->SetIndices(StaticIndexBuffers[RenderList->GetStaticIndexBufferId()]);	*/

	if (RenderList->Flags & ZE_RLF_ENABLE_VIEWPROJECTION_TRANSFORM)
	{
		ZEMatrix4x4 WorldViewProjMatrix;
		ZEMatrix4x4::Multiply(WorldViewProjMatrix, RenderList->WorldMatrix, ViewPoint.ViewProjMatrix);
		Device->SetVertexShaderConstantF(0, (float*)&WorldViewProjMatrix, 4);
	}
	else
		Device->SetVertexShaderConstantF(0, (float*)&RenderList->WorldMatrix, 4);


	Device->SetVertexShaderConstantF(4, (float*)&RenderList->WorldMatrix, 4);
	Device->SetVertexShaderConstantF(8, (float*)&RenderList->WorldMatrix, 4);
	Device->SetVertexShaderConstantF(16, (float*)&ZEVector4(ViewPoint.ViewPosition, 1.0f), 1);

	/*if (RenderList->Flags & ZE_RLF_ENABLE_ZCULLING)
	{
		Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
	else
		Device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);*/
	
	if (Material->TwoSided)
		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	else
		Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	if (Material->Wireframe)
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	else
		Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	Device->SetVertexDeclaration(VertexDeclarations[RenderList->VertexType]);
	PumpStreams(RenderList);
}

bool ZED3D9ShadowRenderer::SetOutput(ZECubeTexture* Texture, ZECubeTextureFace Face)
{
	if (!Texture->IsRenderTarget())
	{
		zeError("D3D9 Shadow Renderer", "Can not set output texture becouse texture is not render target.");
		return false;
	}

	
	OutputCubeTexture = Texture;
	OutputCubeTextureFace = Face;
	OutputTexture = NULL;

	if (Texture->GetPixelFormat() != ZE_TPF_DEPTH && Texture->GetPixelFormat() != ZE_TPF_SHADOWMAP)
	{
		zeError("D3D9 Shadow Renderer", "Can not set output texture becouse texture is not a depth texture.");
		return false;
	}

	if (RenderZBufferRT != NULL)
	{
		RenderZBufferRT->Release();
		RenderZBufferRT = NULL;
	}

	LPDIRECT3DCUBETEXTURE9 D3DTexture = ((ZED3D9CubeTexture*)Texture)->CubeTexture;
	if (D3DTexture->GetCubeMapSurface((D3DCUBEMAP_FACES)Face, 0, &RenderZBufferRT) != D3D_OK || RenderZBufferRT == NULL)
	{
		zeError("D3D9 Shadow Renderer", "Can not get shadow map surface.");
		return false;
	}


	if (!CreateRenderTarget(&RenderColorBufferRT, Texture->GetEdgeLenght(), Texture->GetEdgeLenght(), ZE_TPF_ARGB32))
		return false;
	
	return true;
}

bool ZED3D9ShadowRenderer::SetOutput(ZETexture* Texture)
{
	if (!Texture->IsRenderTarget())
	{
		zeError("D3D9 Shadow Renderer", "Can not set output texture becouse texture is not render target.");
		return false;
	}

	OutputCubeTexture = NULL;
	OutputTexture = Texture;

	if (Texture->GetPixelFormat() != ZE_TPF_DEPTH && Texture->GetPixelFormat() != ZE_TPF_SHADOWMAP)
	{
		zeError("D3D9 Shadow Renderer", "Can not set output texture becouse texture is not a depth texture.");
		return false;
	}

	LPDIRECT3DTEXTURE9 D3DTexture = ((ZED3D9Texture*)Texture)->Texture;
	if (RenderZBufferRT != NULL)
	{
		RenderZBufferRT->Release();
		RenderZBufferRT = NULL;
	}

	if (D3DTexture->GetSurfaceLevel(0, &RenderZBufferRT) != D3D_OK || RenderZBufferRT == NULL)
	{
		zeError("D3D9 Shadow Renderer", "Can not get shadow map surface.");
		return false;
	}

	if (!CreateRenderTarget(&RenderColorBufferRT, Texture->GetWidth(), Texture->GetHeight(), ZE_TPF_ARGB32))
		return false;
	
	return true;
}

void ZED3D9ShadowRenderer::DeviceLost()
{
	Deinitialize();
}

bool ZED3D9ShadowRenderer::DeviceRestored()
{
	if (OutputCubeTexture != NULL)
		return SetOutput(OutputCubeTexture, OutputCubeTextureFace);
	else if (OutputTexture != NULL)
		return SetOutput(OutputTexture);

	return true;
}

void ZED3D9ShadowRenderer::Render(float ElaspedTime)
{
	if (!Module->IsEnabled() || Module->IsDeviceLost)
		return;

	Device->SetRenderTarget(0, RenderColorBufferRT);
	Device->SetDepthStencilSurface(RenderZBufferRT);
	
	Device->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xFFFFFFFF, 1.0f, 0x00000000);

	float BiasValue;
	BiasValue = 0.0001f;//2.0f / 16777215.0f;;
	Device->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&BiasValue);

	BiasValue = 0.002f;//2.0f / 16777215.0f;;
	Device->SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)&BiasValue);

	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	Device->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00);
	Device->SetRenderState(D3DRS_ZENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	
	Device->SetVertexShader(ShadowMapVS);
	Device->SetPixelShader(ShadowMapPS);

	Device->BeginScene();
		for (size_t I = 0; I < NonTransparent.GetCount(); I++)
			DrawSM2(&NonTransparent[I], ViewPoint);

		if (Transparent.GetCount() != 0)
		{
			Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
			Device->SetRenderState(D3DRS_ALPHAREF, 0x01);

			Device->SetVertexShader(ShadowMapTextVS);
			Device->SetPixelShader(ShadowMapOpasityPS);
			for (size_t I = 0; I < Transparent.GetCount(); I++)
			{
				if (((ZEDefaultMaterial*)Transparent[I].Material)->OpacityMap != NULL)
					DrawSM2(&Transparent[I], ViewPoint);
			}


			Device->SetPixelShader(ShadowMapDiffuseAlphaPS);
			for (size_t I = 0; I < Transparent.GetCount(); I++)
			{
				if (((ZEDefaultMaterial*)Transparent[I].Material)->OpacityMap == NULL)
					DrawSM2(&Transparent[I], ViewPoint);
			}
		}
	Device->EndScene();
//	Module->CurrentFrameId++;
}

ZED3D9ShadowRenderer::ZED3D9ShadowRenderer()
{
	OutputTexture = NULL;
	OutputCubeTexture = NULL;
}
