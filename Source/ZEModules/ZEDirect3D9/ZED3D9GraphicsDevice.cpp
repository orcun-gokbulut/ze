//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZED3D9GraphicsDevice.cpp
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

#include "ZED3D9GraphicsDevice.h"
#include "ZED3D9Shader.h"
#include "ZED3D9VertexDeclaration.h"
#include "ZED3D9RenderTarget.h"
#include "ZED3D9DepthBuffer.h"
#include "ZED3D9Texture2D.h"
#include "ZED3D9Texture3D.h"
#include "ZED3D9TextureCube.h"
#include "ZED3D9VertexBuffer.h"
#include "ZED3D9IndexBuffer.h"
#include "ZED3D9GraphicsModule.h"


inline static bool CheckResult(HRESULT Hr)
{
	if (FAILED(Hr))
	{
		zeError("Cannot set device state.");
		return false;
	}

	return true;
}
		

inline D3DBLEND ZEBlendOptionToDX9(ZEBlendOption BlendOption)
{
	static const D3DBLEND Values[] =
	{
		(D3DBLEND)0,
		D3DBLEND_ZERO,
		D3DBLEND_ONE,
		D3DBLEND_SRCCOLOR,
		D3DBLEND_INVSRCCOLOR,
		D3DBLEND_SRCALPHA,
		D3DBLEND_INVSRCALPHA,
		D3DBLEND_DESTALPHA,
		D3DBLEND_INVDESTALPHA,
		D3DBLEND_DESTCOLOR,
		D3DBLEND_INVDESTCOLOR,
		D3DBLEND_SRCALPHASAT
	};
	return Values[BlendOption];
}

inline D3DBLENDOP ZEBlendEquationToDX9(ZEBlendEquation BlendEquation)
{
	static const D3DBLENDOP Values[] =
	{
		(D3DBLENDOP)0,
		D3DBLENDOP_ADD,
		D3DBLENDOP_SUBTRACT,
		D3DBLENDOP_REVSUBTRACT,
		D3DBLENDOP_MIN,
		D3DBLENDOP_MAX
	};
	return Values[BlendEquation];
}

inline D3DCMPFUNC ZEComparisionFunctionToDX9(ZEComparisonFunction CompFunc)
{
	static const D3DCMPFUNC Values[] =
	{
		(D3DCMPFUNC)0, 
		D3DCMP_NEVER, 
		D3DCMP_LESS,
		D3DCMP_EQUAL, 
		D3DCMP_LESSEQUAL, 
		D3DCMP_GREATER, 
		D3DCMP_NOTEQUAL, 
		D3DCMP_GREATEREQUAL, 
		D3DCMP_ALWAYS
	};
	return Values[CompFunc];
}

inline D3DSTENCILOP ZEStencilOperationToDX9(ZEStencilOperation StencilOperation)
{
	static const D3DSTENCILOP Values[] = 
	{ 
		(D3DSTENCILOP)0, 
		D3DSTENCILOP_KEEP, 
		D3DSTENCILOP_ZERO, 
		D3DSTENCILOP_REPLACE,					
		D3DSTENCILOP_INCRSAT, 
		D3DSTENCILOP_DECRSAT, 
		D3DSTENCILOP_INVERT,
		D3DSTENCILOP_INCR, 
		D3DSTENCILOP_DECR
	};
	return Values[StencilOperation];
}

inline D3DFILLMODE ZEFillModeToDX9(ZEFillMode FillMode)
{
	static const D3DFILLMODE Values[] = 
	{
		(D3DFILLMODE)0,
		D3DFILL_WIREFRAME,
		D3DFILL_SOLID,
	};
	return Values[FillMode];
}

inline D3DCULL ZECullDirectionToDX9(ZECullDirection CullDirection)
{
	static const D3DCULL Values[] = 
	{
		(D3DCULL)0,
		D3DCULL_NONE,
		D3DCULL_CW,
		D3DCULL_CCW
	};
	return Values[CullDirection];
}

inline D3DTEXTUREADDRESS ZETextureAdressModeToDX9(ZETextureAddressMode TextureAddress)
{
	static const D3DTEXTUREADDRESS Values[] =
	{
		(D3DTEXTUREADDRESS)0,
		D3DTADDRESS_WRAP,
		D3DTADDRESS_MIRROR,
		D3DTADDRESS_CLAMP,
		D3DTADDRESS_BORDER
	};
	return Values[TextureAddress];
}

inline D3DTEXTUREFILTERTYPE ZETextureFilterModeToDX9(ZETextureFilterMode TextureFilter)
{
	static const D3DTEXTUREFILTERTYPE Values[] =
	{
		D3DTEXF_NONE,
		D3DTEXF_POINT,
		D3DTEXF_LINEAR,
		D3DTEXF_ANISOTROPIC
	};
	return Values[TextureFilter];
}

inline D3DFORMAT ZEIndexBufferFormatToD3D9(ZEIndexBufferFormat BufferFormat)
{
	static const D3DFORMAT Values[] = 
	{
		D3DFMT_INDEX16,
		D3DFMT_INDEX32
	};
	return Values[BufferFormat];
}

inline D3DPRIMITIVETYPE ZEPrimitiveTypeToDX9(ZEPrimitiveType PrimitiveType)
{
	static const D3DPRIMITIVETYPE Values[] = 
	{
		D3DPT_POINTLIST,
		D3DPT_LINELIST,
		D3DPT_TRIANGLELIST,
		D3DPT_TRIANGLESTRIP
	};
	return Values[PrimitiveType];
}

void ZED3D9GraphicsDevice::ApplyStates()
{
	// Apply Blend States
	if(BlendState.GetChanged())
	{
		if (BlendState.GetAlphaTestEnable() != OldBlendState.GetAlphaTestEnable())
			CheckResult(Device->SetRenderState(D3DRS_ALPHATESTENABLE, BlendState.GetAlphaTestEnable()));

		if(BlendState.GetAlphaTestEnable())
			CheckResult(Device->SetRenderState(D3DRS_ALPHAREF, BlendState.GetAlphaReferance()));

		// Check if alpha blending enabled/disabled
		if(BlendState.GetAlphaBlendEnable() != OldBlendState.GetAlphaBlendEnable())
			CheckResult(Device->SetRenderState(D3DRS_ALPHABLENDENABLE, BlendState.GetAlphaBlendEnable()));
		
		// If alpha blending is enabled
		if(BlendState.GetAlphaBlendEnable())
		{
			// Three more stated must be checked
			if(BlendState.GetSourceBlendOption() != ZE_BO_CURRENT && BlendState.GetSourceBlendOption() != OldBlendState.GetSourceBlendOption())
				CheckResult(Device->SetRenderState(D3DRS_SRCBLEND, ZEBlendOptionToDX9(BlendState.GetSourceBlendOption())));
			
			if(BlendState.GetDestinationBlendOption() != ZE_BO_CURRENT && BlendState.GetDestinationBlendOption() != OldBlendState.GetDestinationBlendOption())
				CheckResult(Device->SetRenderState(D3DRS_DESTBLEND, ZEBlendOptionToDX9(BlendState.GetDestinationBlendOption())));
		
			if(BlendState.GetBlendEquation() != ZE_BE_CURRENT && BlendState.GetBlendEquation() != OldBlendState.GetBlendEquation())
				CheckResult(Device->SetRenderState(D3DRS_BLENDOP, ZEBlendEquationToDX9(BlendState.GetBlendEquation())));
		}

		// Check if separate alpha blending enabled/disabled
		if (BlendState.GetSeperateAlphaBlendEnable() != OldBlendState.GetSeperateAlphaBlendEnable())
			CheckResult(Device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, BlendState.GetAlphaBlendEnable()));

		// If separate alpha blending is enabled
		if (BlendState.GetSeperateAlphaBlendEnable())
		{
			// Three more stated must be checked
			if(BlendState.GetSourceAlphaBlendOption() != ZE_BO_CURRENT && BlendState.GetSourceAlphaBlendOption() != OldBlendState.GetSourceAlphaBlendOption())
				CheckResult(Device->SetRenderState(D3DRS_SRCBLENDALPHA, ZEBlendOptionToDX9(BlendState.GetSourceAlphaBlendOption())));

			if(BlendState.GetDestinationAlphaBlendOption() != ZE_BO_CURRENT && BlendState.GetDestinationAlphaBlendOption() != OldBlendState.GetDestinationAlphaBlendOption())
				CheckResult(Device->SetRenderState(D3DRS_DESTBLENDALPHA, ZEBlendOptionToDX9(BlendState.GetDestinationAlphaBlendOption())));

			if(BlendState.GetAlphaBlendEquation() != ZE_BE_CURRENT && BlendState.GetAlphaBlendEquation() != OldBlendState.GetAlphaBlendEquation())
				CheckResult(Device->SetRenderState(D3DRS_BLENDOPALPHA, ZEBlendEquationToDX9(BlendState.GetAlphaBlendEquation())));
		}

		// Check if Color channel mask is changed
		if(BlendState.GetColorChannelMask() != OldBlendState.GetColorChannelMask())
			CheckResult(Device->SetRenderState(D3DRS_COLORWRITEENABLE, BlendState.GetColorChannelMask().Value));

		// Mark as not changed
		BlendState.SetChanged(false);

		// Make a copy
		OldBlendState = BlendState;
	}

	// Apply Raster States
	if(RasterizerState.GetChanged())
	{
		if(RasterizerState.GetCullDirection() != ZE_CD_CURRENT && RasterizerState.GetCullDirection() != OldRasterizerState.GetCullDirection())
			CheckResult(Device->SetRenderState(D3DRS_CULLMODE, ZECullDirectionToDX9(RasterizerState.GetCullDirection())));
		
		// Fill Mode
		if(RasterizerState.GetFillMode() !=  ZE_FM_CURRENT && RasterizerState.GetFillMode() != OldRasterizerState.GetFillMode())
			CheckResult(Device->SetRenderState(D3DRS_FILLMODE, ZEFillModeToDX9(RasterizerState.GetFillMode())));

		// Mark as not changed
		RasterizerState.SetChanged(false);

		// Make a copy
		OldRasterizerState = RasterizerState;
	}
	
	// Apply Stencil States
	if(StencilZState.GetChanged())
	{
		// Z Write
		if(StencilZState.GetZWriteEnable() != OldStencilZState.GetZWriteEnable())
			CheckResult(Device->SetRenderState(D3DRS_ZWRITEENABLE, StencilZState.GetZWriteEnable()));
		
		// Z Test
		if(StencilZState.GetZTestEnable() != OldStencilZState.GetZTestEnable())
			CheckResult(Device->SetRenderState(D3DRS_ZENABLE, StencilZState.GetZTestEnable()));
		
		// Z Function
		if(StencilZState.GetZTestEnable())
			if(StencilZState.GetZFunction() != ZE_CF_CURRENT && StencilZState.GetZFunction() != OldStencilZState.GetZFunction())
				CheckResult(Device->SetRenderState(D3DRS_ZFUNC, ZEComparisionFunctionToDX9(StencilZState.GetZFunction())));

		// Stencil Test
		if(StencilZState.GetStencilTestEnable() != OldStencilZState.GetStencilTestEnable())
			CheckResult(Device->SetRenderState(D3DRS_STENCILENABLE, StencilZState.GetStencilTestEnable()));

		// Stencil mask
		if(StencilZState.GetStencilWriteMask() != OldStencilZState.GetStencilWriteMask())
			CheckResult(Device->SetRenderState(D3DRS_STENCILWRITEMASK, StencilZState.GetStencilWriteMask()));

		// Front Stencil Operation
		if(StencilZState.GetStencilTestEnable())
		{
			// Front Stencil Operation
			if(StencilZState.GetFrontStencilFail() != ZE_CF_CURRENT && StencilZState.GetFrontStencilFail() != OldStencilZState.GetFrontStencilFail())
				CheckResult(Device->SetRenderState(D3DRS_STENCILFAIL, ZEStencilOperationToDX9(StencilZState.GetFrontStencilFail())));

			if(OldStencilZState.GetFrontZFail() != ZE_SO_CURRENT && StencilZState.GetFrontZFail() != OldStencilZState.GetFrontZFail())
				CheckResult(Device->SetRenderState(D3DRS_STENCILZFAIL, ZEStencilOperationToDX9(OldStencilZState.GetFrontZFail())));

			if(StencilZState.GetFrontStencilPass() != ZE_SO_CURRENT && StencilZState.GetFrontStencilPass() != OldStencilZState.GetFrontStencilPass())
				CheckResult(Device->SetRenderState(D3DRS_STENCILPASS, ZEStencilOperationToDX9(StencilZState.GetFrontStencilPass())));

			if(StencilZState.GetFrontStencilFunction() != ZE_SO_CURRENT && StencilZState.GetFrontStencilFunction() != OldStencilZState.GetFrontStencilFunction())
				CheckResult(Device->SetRenderState(D3DRS_STENCILFUNC, ZEComparisionFunctionToDX9(StencilZState.GetFrontStencilFunction())));

			// Back Stencil Operation
			if(StencilZState.GetBackStencilFail() != ZE_CF_CURRENT && StencilZState.GetBackStencilFail() != OldStencilZState.GetBackStencilFail())
				CheckResult(Device->SetRenderState(D3DRS_CCW_STENCILFAIL, ZEStencilOperationToDX9(StencilZState.GetBackStencilFail())));

			if(StencilZState.GetBackZFail() != ZE_SO_CURRENT && StencilZState.GetBackZFail() != OldStencilZState.GetBackZFail())
				CheckResult(Device->SetRenderState(D3DRS_CCW_STENCILZFAIL, ZEStencilOperationToDX9(StencilZState.GetBackZFail())));

			if(StencilZState.GetBackStencilPass() != ZE_SO_CURRENT && StencilZState.GetBackStencilPass() != OldStencilZState.GetBackStencilPass())
				CheckResult(Device->SetRenderState(D3DRS_CCW_STENCILPASS, ZEStencilOperationToDX9(StencilZState.GetBackStencilPass())));

			if(StencilZState.GetBackStencilFunction() != ZE_SO_CURRENT && StencilZState.GetBackStencilFunction() != OldStencilZState.GetBackStencilFunction())
				CheckResult(Device->SetRenderState(D3DRS_CCW_STENCILFUNC, ZEComparisionFunctionToDX9(StencilZState.GetBackStencilFunction())));
		}

		// Mark as not changed
		StencilZState.SetChanged(false);

		// Make a copy
		OldStencilZState = StencilZState;
	}

	// Apply Sampler States
	bool BorderColorUsed = false;
	bool AnisotrophyUsed = false;

	for(ZESize I = 0; I < ZE_MAX_SAMPLER_ATTACHMENT; ++I)
	{
		ZESamplerState* SamplerState = &SamplerStates[I];
		ZESamplerState* OldSamplerState = &OldSamplerStates[I];

		// If Changed
		if(SamplerState->GetChanged())
		{
			// Addressing Modes
			if(SamplerState->GetAddressU() != ZE_TAM_CURRENT && SamplerState->GetAddressU() != OldSamplerState->GetAddressU())
			{
				CheckResult(Device->SetSamplerState((DWORD)I, D3DSAMP_ADDRESSU, ZETextureAdressModeToDX9(SamplerState->GetAddressU())));
				if(SamplerState->GetAddressU() == ZE_TAM_BORDER)
					BorderColorUsed = true;
			}
			if(SamplerState->GetAddressV() != ZE_TAM_CURRENT && SamplerState->GetAddressV() != OldSamplerState->GetAddressV())
			{
				CheckResult(Device->SetSamplerState((DWORD)I, D3DSAMP_ADDRESSV, ZETextureAdressModeToDX9(SamplerState->GetAddressV())));
				if(SamplerState->GetAddressV() == ZE_TAM_BORDER)
					BorderColorUsed = true;
			}
			if(SamplerState->GetAddressW() != ZE_TAM_CURRENT && SamplerState->GetAddressW() != OldSamplerState->GetAddressW())
			{
				CheckResult(Device->SetSamplerState((DWORD)I, D3DSAMP_ADDRESSW, ZETextureAdressModeToDX9(SamplerState->GetAddressW())));
				if(SamplerState->GetAddressW() == ZE_TAM_BORDER)
					BorderColorUsed = true;
			}

			// Filters
			if(SamplerState->GetMinFilter() != ZE_TFM_CURRENT && SamplerState->GetMinFilter() != OldSamplerState->GetMinFilter())
			{
				CheckResult(Device->SetSamplerState((DWORD)I, D3DSAMP_MINFILTER, ZETextureFilterModeToDX9(SamplerState->GetMinFilter())));
				if(SamplerState->GetMinFilter() == ZE_TFM_ANISOTROPY)
					AnisotrophyUsed = true;
			}
			if(SamplerState->GetMagFilter() != ZE_TFM_CURRENT && SamplerState->GetMagFilter() != OldSamplerState->GetMagFilter())
			{
				CheckResult(Device->SetSamplerState((DWORD)I, D3DSAMP_MAGFILTER, ZETextureFilterModeToDX9(SamplerState->GetMagFilter())));
				if(SamplerState->GetMagFilter() == ZE_TFM_ANISOTROPY)
					AnisotrophyUsed = true;
			}
			if(SamplerState->GetMipFilter() != ZE_TFM_CURRENT && SamplerState->GetMipFilter() != OldSamplerState->GetMipFilter())
			{
				CheckResult(Device->SetSamplerState((DWORD)I, D3DSAMP_MIPFILTER, ZETextureFilterModeToDX9(SamplerState->GetMipFilter())));
				if(SamplerState->GetMipFilter() == ZE_TFM_ANISOTROPY)
					AnisotrophyUsed = true;
			}
			
			// Rest
			if(AnisotrophyUsed && SamplerState->GetMaxAnisotrophy() != OldSamplerState->GetMaxAnisotrophy())
			{
				CheckResult(Device->SetSamplerState((DWORD)I, D3DSAMP_MAXANISOTROPY, SamplerState->GetMaxAnisotrophy()));
			}
			
			if(BorderColorUsed && SamplerState->GetBorderColor() != OldSamplerState->GetBorderColor())
			{	
				// Convert To RGBA
				ZEInt ColorRGBA[4];
				ZEVector4 Color = SamplerStates->GetBorderColor().Clamp(0.0f, 1.0f);
				ColorRGBA[0] = (ZEInt)(Color.x * 255.0f);
				ColorRGBA[1] = (ZEInt)(Color.y * 255.0f);
				ColorRGBA[2] = (ZEInt)(Color.z * 255.0f);
				ColorRGBA[3] = (ZEInt)(Color.w * 255.0f);

				CheckResult(Device->SetSamplerState((DWORD)I, D3DSAMP_BORDERCOLOR, D3DCOLOR_RGBA(ColorRGBA[0], ColorRGBA[1], ColorRGBA[2], ColorRGBA[3])));
			}
			
			if(SamplerState->GetMaxLOD() != OldSamplerState->GetMaxLOD())
			{
				CheckResult(Device->SetSamplerState((DWORD)I, D3DSAMP_MAXMIPLEVEL, (ZEInt)SamplerState->GetMaxLOD()));
			}
			
			if(SamplerState->GetTexture() != OldSamplerState->GetTexture())
			{
				ZETextureType TexType = OldSamplerState->GetTexture()->GetTextureType();
				switch (TexType)
				{
					case ZE_TT_2D :
						CheckResult(Device->SetTexture((DWORD)I, (((ZED3D9Texture2D*)SamplerState->GetTexture())->Texture)));
						break;
					case ZE_TT_3D :
						CheckResult(Device->SetTexture((DWORD)I, (((ZED3D9Texture3D*)SamplerState->GetTexture())->VolumeTexture)));
						break;
					case ZE_TT_CUBE :
						CheckResult(Device->SetTexture((DWORD)I, (((ZED3D9TextureCube*)SamplerState->GetTexture())->CubeTexture)));
						break;
				}

			}

			// Mark as not changed
			SamplerState->SetChanged(false);

			// Make a copy
			*SamplerState = *OldSamplerState;
		}
	}

	// Apply Render Targets
	if(ScreenWriteEnable)
	{
		if (RenderTargets[0] != NULL && RenderTargets[0] != OldRenderTargets[0])
		{	
			// Now Only Set RT0 Which Should Have Back buffer
			CheckResult(Device->SetRenderTarget(0, ((ZED3D9RenderTarget*)RenderTargets[0])->Surface));
			OldRenderTargets[0] = RenderTargets[0];
		}
	}
	else
	{
		for(ZESize I = 0; I < ZE_MAX_RENDER_TARGETS; ++I)
		{
			if (RenderTargets[I] != NULL && RenderTargets[I] != OldRenderTargets[I])
			{		
				CheckResult(Device->SetRenderTarget((DWORD)I, ((ZED3D9RenderTarget*)RenderTargets[I])->Surface));
				OldRenderTargets[I] = RenderTargets[I];
			}
		}
	}

	if(DepthStencilBuffer != NULL && DepthStencilBuffer != OldDepthStencilBuffer)
	{
		CheckResult(Device->SetDepthStencilSurface(((ZED3D9DepthBuffer*)DepthStencilBuffer)->DepthSurface));
		OldDepthStencilBuffer = DepthStencilBuffer;
	}

	// Apply static Vertex Buffer
	if (VertexBuffer != NULL && VertexBuffer != OldVertexBuffer)
	{
		if (VertexBuffer->IsStatic())
			CheckResult(Device->SetStreamSource(0, ((ZED3D9StaticVertexBuffer*)VertexBuffer)->StaticBuffer, 0, (UINT)((ZED3D9StaticVertexBuffer*)VertexBuffer)->GetVertexSize()));

		OldVertexBuffer = VertexBuffer;
	}

	// Apply static Index Buffers
	if (IndexBuffer != NULL && IndexBuffer != OldIndexBuffer)
	{
		if (IndexBuffer->IsStatic())
			CheckResult(Device->SetIndices(((ZED3D9StaticIndexBuffer*)IndexBuffer)->StaticBuffer));

		OldIndexBuffer = IndexBuffer;
	}

	// Apply Shaders
	if(PixelShader != NULL && PixelShader != OldPixelShader)
	{
		CheckResult(Device->SetPixelShader(((ZED3D9PixelShader*)PixelShader)->GetPixelShader()));
		OldPixelShader = PixelShader;
	}

	if(VertexShader != NULL && VertexShader != OldVertexShader)
	{
		CheckResult(Device->SetVertexShader(((ZED3D9VertexShader*)VertexShader)->GetVertexShader()));
		OldVertexShader = VertexShader;
	}
}

void ZED3D9GraphicsDevice::ResetToDefaultStates()
{
	BlendState = DefaultBlendState;
	RasterizerState = DefaultRasterStates;
	StencilZState = DefaultStencilZState;

	for (ZESize I = 0; I < ZE_MAX_SAMPLER_ATTACHMENT; ++I)
		SamplerStates[I] = DefaultSamplerState;
	
	for (ZESize I = 0; I < ZE_MAX_RENDER_TARGETS; ++I)
		RenderTargets[I] = NULL;

	VertexBuffer = NULL;
	IndexBuffer = NULL;
	VertexShader = NULL;
	PixelShader = NULL;
	DepthStencilBuffer = NULL;
	
	ScreenWriteEnable = false;
}

void ZED3D9GraphicsDevice::Draw(ZEPrimitiveType PrimitiveType, ZEUInt32 StartVertex, ZEUInt32 PrimitiveCount) 
{
	// Commit State
	ApplyStates();

	if (VertexBuffer->IsStatic())
	{
		// NOTE: vertex declaration creation should not be here in the future.
		ZED3D9VertexDeclaration VertexDeclaration;
		if (!VertexDeclaration.Create(OldVertexBuffer->GetVertexElements().GetConstCArray(), OldVertexBuffer->GetVertexElements().GetSize()))
		{
			zeError("Unable To Generate Vertex Declaration!!");
		}

		VertexDeclaration.SetupVertexDeclaration();

		if (!CheckResult(Device->DrawPrimitive(ZEPrimitiveTypeToDX9(PrimitiveType), StartVertex, PrimitiveCount)));
		{
			//Device lost check
			//if ()
			//	Module->RestoreDevice();
		}

		VertexDeclaration.Release();
	}
	else
	{
		ZEDynamicVertexBuffer* Buffer = (ZEDynamicVertexBuffer*)VertexBuffer;
		
		// NOTE: vertex size calculation should not be here in the future.
		Buffer->GenerateVertexSize();

		if (!CheckResult(Device->DrawPrimitiveUP(ZEPrimitiveTypeToDX9(PrimitiveType), PrimitiveCount, Buffer->GetVertexBuffer(), Buffer->GetVertexSize())))
		{
			//Device lost check
			//if ()
			//	Module->RestoreDevice();
		}
	}
	
}

void ZED3D9GraphicsDevice::DrawIndexed(ZEPrimitiveType PrimitiveType, ZEInt BaseVertexIndex, ZEUInt32 MinIndex, ZEUInt32 VertexCount, ZEUInt32 StartIndex, ZEUInt32 PrimitiveCount)
{
	ApplyStates();

	if (VertexBuffer->IsStatic())
	{
		// NOTE: vertex declaration creation should not be here in the future.
		ZED3D9VertexDeclaration VertexDeclaration;
		if (!VertexDeclaration.Create(OldVertexBuffer->GetVertexElements().GetConstCArray(), OldVertexBuffer->GetVertexElements().GetSize()))
		{
			zeError("Unable To Generate Vertex Declaration!!");
		}

		VertexDeclaration.SetupVertexDeclaration();

		if (!CheckResult(Device->DrawIndexedPrimitive(ZEPrimitiveTypeToDX9(PrimitiveType), BaseVertexIndex, MinIndex, VertexCount, StartIndex, PrimitiveCount)))
		{
			// Device lost
			//if ()
			//	Module->RestoreDevice();
		}

		VertexDeclaration.Release();
	}
	else
	{
		if (IndexBuffer->IsStatic())
		{
			zeError("Dynamic index buffer is expected instead of a static one");
			return;
		}

		ZEDynamicIndexBuffer* IndBuff = (ZEDynamicIndexBuffer*)IndexBuffer;
		ZEDynamicVertexBuffer* VertBuff = (ZEDynamicVertexBuffer*)VertexBuffer;
		
		// NOTE: vertex size calculation should not be here in the future.
		VertBuff->GenerateVertexSize();

		if (!CheckResult(Device->DrawIndexedPrimitiveUP(ZEPrimitiveTypeToDX9(PrimitiveType), MinIndex, VertexCount, PrimitiveCount, IndBuff->GetIndexBuffer(), ZEIndexBufferFormatToD3D9(IndBuff->GetBufferFormat()), VertBuff->GetVertexBuffer(), VertBuff->GetVertexSize())))
		{
			// Device lost
			//if ()
			//	Module->RestoreDevice();
		}
	}

	
}

bool ZED3D9GraphicsDevice::Present()
{
	return CheckResult(Device->Present(NULL, NULL, NULL, NULL));
}

bool ZED3D9GraphicsDevice::EndScene()
{
	return CheckResult(Device->EndScene());
}

bool ZED3D9GraphicsDevice::BeginScene()
{
	return CheckResult(Device->BeginScene());
}

bool ZED3D9GraphicsDevice::Clear(bool RenderTarget, bool ZBuffer, bool StencilBuffer, const ZEVector4& Color, float ZClearValue, ZEUInt32 StencilClearValue)
{
	DWORD Flags = 0;
	// Set surfaces before cleaning
	if (RenderTarget)
	{
		Flags |= D3DCLEAR_TARGET;

		for(ZESize I = 0; I < ZE_MAX_RENDER_TARGETS; ++I)
		{
			if (RenderTargets[I] != NULL && RenderTargets[I] != OldRenderTargets[I])
			{		
				CheckResult(Device->SetRenderTarget((DWORD)I, ((ZED3D9RenderTarget*)RenderTargets[I])->Surface));
				OldRenderTargets[I] = RenderTargets[I];
			}
		}
	}
	if (ZBuffer || StencilBuffer)
	{
		Flags |= ZBuffer ? D3DCLEAR_ZBUFFER : 0;
		Flags |= StencilBuffer ? D3DCLEAR_STENCIL : 0;

		if(DepthStencilBuffer != NULL && DepthStencilBuffer != OldDepthStencilBuffer)
		{
			CheckResult(Device->SetDepthStencilSurface(((ZED3D9DepthBuffer*)DepthStencilBuffer)->DepthSurface));
			OldDepthStencilBuffer = DepthStencilBuffer;
		}
	}

	D3DCOLOR ClearColor = D3DCOLOR_ARGB((ZEUInt8)(Color.w * 255.0f),
										(ZEUInt8)(Color.x * 255.0f),
										(ZEUInt8)(Color.y * 255.0f),
										(ZEUInt8)(Color.z * 255.0f));

	return CheckResult(Device->Clear(0, NULL, Flags, ClearColor, ZClearValue, StencilClearValue));
}

ZED3D9GraphicsDevice::ZED3D9GraphicsDevice() : ZEGraphicsDevice()
{
	// Reset states to d3d defaults
	DefaultRasterStates.SetFillMode(ZE_FM_FILL);
	DefaultRasterStates.SetCullDirection(ZE_CD_COUNTER_CLOCKWISE);
	
	DefaultBlendState.SetAlphaBlendEnable(false);
	DefaultBlendState.SetSourceBlendOption(ZE_BO_ONE);
	DefaultBlendState.SetDestinationBlendOption(ZE_BO_ZERO);
	DefaultBlendState.SetSeperateAlphaBlendEnable(false);
	DefaultBlendState.SetSourceAlphaBlendOption(ZE_BO_ONE);
	DefaultBlendState.SetDestinationAlphaBlendOption(ZE_BO_ZERO);
	DefaultBlendState.SetBlendEquation(ZE_BE_ADD);

	DefaultStencilZState.SetZWriteEnable(true);
	DefaultStencilZState.SetZTestEnable(false);
	DefaultStencilZState.SetZFunction(ZE_CF_LESS_EQUAL);
	DefaultStencilZState.SetStencilWriteMask(0);
	DefaultStencilZState.SetStencilTestEnable(false);
	DefaultStencilZState.SetFrontStencilFunction(ZE_CF_ALWAYS);
	DefaultStencilZState.SetFrontStencilFail(ZE_SO_KEEP);
	DefaultStencilZState.SetFrontStencilPass(ZE_SO_KEEP);
	DefaultStencilZState.SetFrontZFail(ZE_SO_KEEP);
	DefaultStencilZState.SetBackStencilFunction(ZE_CF_ALWAYS);
	DefaultStencilZState.SetBackStencilFail(ZE_SO_KEEP);
	DefaultStencilZState.SetBackStencilPass(ZE_SO_KEEP);
	DefaultStencilZState.SetBackZFail(ZE_SO_KEEP);

	DefaultSamplerState.SetAddressU(ZE_TAM_WRAP);
	DefaultSamplerState.SetAddressV(ZE_TAM_WRAP);
	DefaultSamplerState.SetAddressW(ZE_TAM_WRAP);
	DefaultSamplerState.SetBorderColor(ZEVector4::Zero);
	DefaultSamplerState.SetMagFilter(ZE_TFM_POINT);
	DefaultSamplerState.SetMinFilter(ZE_TFM_POINT);
	DefaultSamplerState.SetMipFilter(ZE_TFM_NONE);
	DefaultSamplerState.SetMaxAnisotrophy(1);
	DefaultSamplerState.SetMaxLOD(0);
	DefaultSamplerState.SetTexture(NULL);
}

ZED3D9GraphicsDevice::~ZED3D9GraphicsDevice()
{

}
