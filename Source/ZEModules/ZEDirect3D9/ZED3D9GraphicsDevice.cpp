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
		D3DCMP_LESS, D3DCMP_EQUAL, 
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
		D3DCULL_CW,
		D3DCULL_CCW
	};
	return Values[CullDirection];
}

inline D3DTEXTUREADDRESS ZETextureAdressModeToDX9(ZETextureAdressMode TextureAddress)
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

inline D3DPRIMITIVETYPE ZEPrimitiveTypeToDX9(ZEROPrimitiveType PrimitiveType)
{
	static const D3DPRIMITIVETYPE Values[] = 
	{
		D3DPT_POINTLIST,
		D3DPT_LINELIST,
		D3DPT_TRIANGLELIST,
		D3DPT_TRIANGLEFAN
	};
	return Values[PrimitiveType];
}

inline void ZED3D9GraphicsDevice::InitDefaultState()
{
	// TODO: 
	D3DDevice9->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, true);

	// Default Shaders
	//	.........

	// Default Vertex Declaration
	// ..........

	// RenderTargets
	// ..........
}

void ZED3D9GraphicsDevice::ApplyRequestedBlendState()
{
	// Skip If Requested Blend State didn't changed.
	if(!RequestedBlendState.GetChanged())
		return;

	// Enable Disable
	if(RequestedBlendState.GetBlendEnable() != DeviceBlendState.GetBlendEnable())
	{
		DeviceBlendState.SetBlendEnable(RequestedBlendState.GetBlendEnable());
		D3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, DeviceBlendState.GetBlendEnable());
	}
	// Alpha and Color Blending Options
	if(DeviceBlendState.GetBlendEnable())
	{
		if(RequestedBlendState.GetSourceColorBlendOption() != ZE_BO_CURRENT && 
			RequestedBlendState.GetSourceColorBlendOption() != DeviceBlendState.GetSourceColorBlendOption())
		{
			DeviceBlendState.SetSourceColorBlendOption(RequestedBlendState.GetSourceColorBlendOption()); 
			D3DDevice9->SetRenderState(D3DRS_SRCBLEND, ZEBlendOptionToDX9(DeviceBlendState.GetSourceColorBlendOption()));
		}

		if(RequestedBlendState.GetDestinationColorBlendOption() != ZE_BO_CURRENT &&
			RequestedBlendState.GetDestinationColorBlendOption() != DeviceBlendState.GetDestinationColorBlendOption())
		{
			DeviceBlendState.SetDestinationColorBlendOption(RequestedBlendState.GetDestinationColorBlendOption()); 
			D3DDevice9->SetRenderState(D3DRS_DESTBLEND, ZEBlendOptionToDX9(DeviceBlendState.GetDestinationColorBlendOption()));
		}
		
		if(RequestedBlendState.GetSourceAlphaBlendOption() != ZE_BO_CURRENT &&
			RequestedBlendState.GetSourceAlphaBlendOption() != DeviceBlendState.GetSourceAlphaBlendOption())
		{
			DeviceBlendState.SetSourceAlphaBlendOption(RequestedBlendState.GetSourceAlphaBlendOption());
			D3DDevice9->SetRenderState(D3DRS_SRCBLENDALPHA, ZEBlendOptionToDX9(DeviceBlendState.GetSourceAlphaBlendOption()));
		}

		if(RequestedBlendState.GetDestinationAlphaBlendOption() != ZE_BO_CURRENT &&
			RequestedBlendState.GetDestinationAlphaBlendOption() != DeviceBlendState.GetDestinationAlphaBlendOption())
		{
			DeviceBlendState.SetDestinationAlphaBlendOption(RequestedBlendState.GetDestinationAlphaBlendOption());
			D3DDevice9->SetRenderState(D3DRS_DESTBLENDALPHA, ZEBlendOptionToDX9(DeviceBlendState.GetDestinationAlphaBlendOption()));
		}
	
		if(RequestedBlendState.GetBlendEquation() != ZE_BE_CURRENT &&
			RequestedBlendState.GetBlendEquation() != DeviceBlendState.GetBlendEquation())
		{
			DeviceBlendState.SetBlendEquation(RequestedBlendState.GetBlendEquation());
			D3DDevice9->SetRenderState(D3DRS_BLENDOP, ZEBlendEquationToDX9(DeviceBlendState.GetBlendEquation()));
			D3DDevice9->SetRenderState(D3DRS_BLENDOPALPHA, ZEBlendEquationToDX9(DeviceBlendState.GetBlendEquation()));
		}
	}
	//ColorMask
	if(RequestedBlendState.GetColorChannelMask() != DeviceBlendState.GetColorChannelMask())
	{
		DeviceBlendState.SetColorChannelMask(RequestedBlendState.GetColorChannelMask());
		D3DDevice9->SetRenderState(D3DRS_COLORWRITEENABLE, DeviceBlendState.GetColorChannelMask());
	}
	// Until Next Apply;
	// If nothing changes on this state block, this should remain false
	RequestedBlendState.SetChanged(false);
}

void ZED3D9GraphicsDevice::ApplyRequestedSamplerStates()
{
	bool AdressesHaveBorderColor = false;
	bool FiltersHaveAnisotrophy = false;

	for(int i = 0; i < ZE_MAX_SAMPLER_ATTACHMENT; i++)
	{
		// If Requested Not Changed, Skip
		if(RequestedSamplerStates[i].GetChanged())
		{
			// Addressing Modes
			if(RequestedSamplerStates[i].GetAddressU() != ZE_TAM_CURRENT &&
				RequestedSamplerStates[i].GetAddressU() != DeviceSamplerStates[i].GetAddressU())
			{
				DeviceSamplerStates[i].SetAddressU(RequestedSamplerStates[i].GetAddressU());
				D3DDevice9->SetSamplerState(i, D3DSAMP_ADDRESSU, ZETextureAdressModeToDX9(DeviceSamplerStates[i].GetAddressU()));
				if(DeviceSamplerStates[i].GetAddressU() == ZE_TAM_BORDER)
					AdressesHaveBorderColor = true;
			}
			if(RequestedSamplerStates[i].GetAddressV() != ZE_TAM_CURRENT &&
				RequestedSamplerStates[i].GetAddressV() != DeviceSamplerStates[i].GetAddressV())
			{
				DeviceSamplerStates[i].SetAddressV(RequestedSamplerStates[i].GetAddressV());
				D3DDevice9->SetSamplerState(i, D3DSAMP_ADDRESSU, ZETextureAdressModeToDX9(DeviceSamplerStates[i].GetAddressV()));
				if(DeviceSamplerStates[i].GetAddressV() == ZE_TAM_BORDER)
					AdressesHaveBorderColor = true;
			}
			if(RequestedSamplerStates[i].GetAddressW() != ZE_TAM_CURRENT &&
				RequestedSamplerStates[i].GetAddressW() != DeviceSamplerStates[i].GetAddressW())
			{
				DeviceSamplerStates[i].SetAddressU(RequestedSamplerStates[i].GetAddressW());
				D3DDevice9->SetSamplerState(i, D3DSAMP_ADDRESSU, ZETextureAdressModeToDX9(DeviceSamplerStates[i].GetAddressW()));
				if(DeviceSamplerStates[i].GetAddressW() == ZE_TAM_BORDER)
					AdressesHaveBorderColor = true;
			}
			// Filters
			if(RequestedSamplerStates[i].GetMinFilter() != ZE_TFM_CURRENT &&
				RequestedSamplerStates[i].GetMinFilter() != DeviceSamplerStates[i].GetMinFilter())
			{
				DeviceSamplerStates[i].SetMinFilter(RequestedSamplerStates[i].GetMinFilter());
				D3DDevice9->SetSamplerState(i, D3DSAMP_MINFILTER, ZETextureFilterModeToDX9(DeviceSamplerStates[i].GetMinFilter()));
				if(DeviceSamplerStates[i].GetMinFilter() == ZE_TFM_ANISOTROPY)
					FiltersHaveAnisotrophy = true;
			}
			if(RequestedSamplerStates[i].GetMagFilter() != ZE_TFM_CURRENT &&
				RequestedSamplerStates[i].GetMagFilter() != DeviceSamplerStates[i].GetMagFilter())
			{
				DeviceSamplerStates[i].SetMagFilter(RequestedSamplerStates[i].GetMagFilter());
				D3DDevice9->SetSamplerState(i, D3DSAMP_MINFILTER, ZETextureFilterModeToDX9(DeviceSamplerStates[i].GetMagFilter()));
				if(DeviceSamplerStates[i].GetMagFilter() == ZE_TFM_ANISOTROPY)
					FiltersHaveAnisotrophy = true;
			}
			if(RequestedSamplerStates[i].GetMipFilter() != ZE_TFM_CURRENT &&
				RequestedSamplerStates[i].GetMipFilter() != DeviceSamplerStates[i].GetMipFilter())
			{
				DeviceSamplerStates[i].SetMipFilter(RequestedSamplerStates[i].GetMipFilter());
				D3DDevice9->SetSamplerState(i, D3DSAMP_MINFILTER, ZETextureFilterModeToDX9(DeviceSamplerStates[i].GetMipFilter()));
				if(DeviceSamplerStates[i].GetMipFilter() == ZE_TFM_ANISOTROPY)
					FiltersHaveAnisotrophy = true;
			}
			// Rest
			if(FiltersHaveAnisotrophy &&
				RequestedSamplerStates[i].GetMaxAnisotrophy() != DeviceSamplerStates[i].GetMaxAnisotrophy() )
			{
				DeviceSamplerStates[i].SetMaxAnisotrophy(RequestedSamplerStates[i].GetMaxAnisotrophy());
				D3DDevice9->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, DeviceSamplerStates[i].GetMaxAnisotrophy());
			}
			if(AdressesHaveBorderColor &&
				RequestedSamplerStates[i].GetBorderColor() != DeviceSamplerStates[i].GetBorderColor())
			{	
				DeviceSamplerStates[i].SetBorderColor(RequestedSamplerStates[i].GetBorderColor());
				D3DDevice9->SetSamplerState(i, D3DSAMP_BORDERCOLOR, *(RequestedSamplerStates[i].GetBorderColor().M));
			}
			if(RequestedSamplerStates[i].GetMaxLOD() != RequestedSamplerStates[i].GetMaxLOD())
			{
				DeviceSamplerStates[i].SetMaxLOD(RequestedSamplerStates[i].GetMaxLOD());
				D3DDevice9->SetSamplerState(i, D3DSAMP_MAXMIPLEVEL, DeviceSamplerStates[i].GetMaxLOD());
			}
			if(RequestedSamplerStates[i].GetTexture() != DeviceSamplerStates[i].GetTexture())
			{
				DeviceSamplerStates[i].SetCurrentTexture(RequestedSamplerStates[i].GetTexture());

				ZETextureType TexType = DeviceSamplerStates[i].GetTexture()->GetTextureType();
				switch (TexType)
				{
					case ZE_TT_2D :
						D3DDevice9->SetTexture(i, (((ZED3D9Texture2D*)DeviceSamplerStates[i].GetTexture())->Texture));
						break;
					case ZE_TT_3D :
						D3DDevice9->SetTexture(i, (((ZED3D9Texture3D*)DeviceSamplerStates[i].GetTexture())->VolumeTexture));
						break;
					case ZE_TT_CUBE :
						D3DDevice9->SetTexture(i, (((ZED3D9TextureCube*)DeviceSamplerStates[i].GetTexture())->CubeTexture));
				}

			}
			// Until Next Apply;
			// If nothing changes on this state block, this should remain false
			RequestedSamplerStates[i].SetChanged(false);
		}
	}
}

void ZED3D9GraphicsDevice::ApplyRequestedShaders()
{
	// if RequestedPixelShader is NULL that means use current shader
	if(RequestedPixelShader != NULL)
	{
		// Check the pointers if same address then consider them as same shader
		if(RequestedPixelShader != DevicePixelShader)
		{
			DevicePixelShader = RequestedPixelShader;
			D3DDevice9->SetPixelShader(((ZED3D9PixelShader*)DevicePixelShader)->GetPixelShader());
		}
	}
	if(RequestedVertexShader != NULL)
	{
		if(RequestedVertexShader != DeviceVertexShader)
		{
			DeviceVertexShader = RequestedVertexShader;
			D3DDevice9->SetVertexShader(((ZED3D9VertexShader*)DeviceVertexShader)->GetVertexShader());
		}
	}
}

void ZED3D9GraphicsDevice::ApplyRequestedStencilZState()
{
	// Skip Entire Function if Requested State is same from before
	if(!RequestedStencilZState.GetChanged())
		return;

	// Enable, Disable and Mask
	if(RequestedStencilZState.GetZTestEnable() != DeviceStencilZState.GetZTestEnable())
	{
		DeviceStencilZState.SetZTestEnable(RequestedStencilZState.GetZTestEnable());
		D3DDevice9->SetRenderState(D3DRS_ZENABLE, DeviceStencilZState.GetZTestEnable());
	}
	if(RequestedStencilZState.GetStencilTestEnable() != DeviceStencilZState.GetStencilTestEnable())
	{	
		DeviceStencilZState.SetStencilTestEnable(RequestedStencilZState.GetStencilTestEnable());
		D3DDevice9->SetRenderState(D3DRS_STENCILENABLE, DeviceStencilZState.GetStencilTestEnable());
	}
	if(RequestedStencilZState.GetZWriteEnable() != DeviceStencilZState.GetZWriteEnable())
	{
		DeviceStencilZState.SetZWriteEnable(RequestedStencilZState.GetZWriteEnable());
		D3DDevice9->SetRenderState(D3DRS_ZWRITEENABLE, DeviceStencilZState.GetZWriteEnable());
	}
	if(RequestedStencilZState.GetStencilWriteMask() != DeviceStencilZState.GetStencilWriteMask())
	{
		DeviceStencilZState.SetStencilWriteMask(RequestedStencilZState.GetStencilWriteMask());
		D3DDevice9->SetRenderState(D3DRS_STENCILWRITEMASK, DeviceStencilZState.GetStencilWriteMask());
	}
	// Z Function
	if(DeviceStencilZState.GetZTestEnable() && RequestedStencilZState.GetZFunction() != ZE_CF_CURRENT &&
		RequestedStencilZState.GetZFunction() != DeviceStencilZState.GetZFunction())
	{
		DeviceStencilZState.SetZFunction(RequestedStencilZState.GetZFunction());
		D3DDevice9->SetRenderState(D3DRS_ZFUNC, ZEComparisionFunctionToDX9(DeviceStencilZState.GetZFunction()));
	}
	// Front Stencil Operation
	if(DeviceStencilZState.GetStencilTestEnable())
	{
		// Front Stencil Operation
		if(RequestedStencilZState.GetFrontStencilFail() != ZE_CF_CURRENT &&
			RequestedStencilZState.GetFrontStencilFail() != DeviceStencilZState.GetFrontStencilFail())
		{
			DeviceStencilZState.SetFrontStencilFail(RequestedStencilZState.GetFrontStencilFail());
			D3DDevice9->SetRenderState(D3DRS_STENCILFAIL, ZEStencilOperationToDX9(DeviceStencilZState.GetFrontStencilFail()));
		}
		if(DeviceStencilZState.GetFrontZFail() != ZE_SO_CURRENT &&
			RequestedStencilZState.GetFrontZFail() != DeviceStencilZState.GetFrontZFail())
		{
			DeviceStencilZState.SetFrontZFail(RequestedStencilZState.GetFrontZFail());
			D3DDevice9->SetRenderState(D3DRS_STENCILZFAIL, ZEStencilOperationToDX9(DeviceStencilZState.GetFrontZFail()));
		}
		if(RequestedStencilZState.GetFrontStencilPass() != ZE_SO_CURRENT &&
			RequestedStencilZState.GetFrontStencilPass() != DeviceStencilZState.GetFrontStencilPass())
		{
			DeviceStencilZState.SetFrontStencilPass(RequestedStencilZState.GetFrontStencilPass());
			D3DDevice9->SetRenderState(D3DRS_STENCILPASS, ZEStencilOperationToDX9(DeviceStencilZState.GetFrontStencilPass()));
		}
		if(RequestedStencilZState.GetFrontStencilFunction() != ZE_SO_CURRENT &&
			RequestedStencilZState.GetFrontStencilFunction() != DeviceStencilZState.GetFrontStencilFunction())
		{
			DeviceStencilZState.SetFrontStencilFunction(RequestedStencilZState.GetFrontStencilFunction());
			D3DDevice9->SetRenderState(D3DRS_STENCILFUNC, ZEComparisionFunctionToDX9(DeviceStencilZState.GetFrontStencilFunction()));
		}
		// Back Stencil Operation
		if(RequestedStencilZState.GetBackStencilFail() != ZE_CF_CURRENT &&
			RequestedStencilZState.GetBackStencilFail() != DeviceStencilZState.GetBackStencilFail())
		{
			DeviceStencilZState.SetBackStencilFail(RequestedStencilZState.GetBackStencilFail());
			D3DDevice9->SetRenderState(D3DRS_CCW_STENCILFAIL, ZEStencilOperationToDX9(DeviceStencilZState.GetBackStencilFail()));
		}
		if(RequestedStencilZState.GetBackZFail() != ZE_SO_CURRENT &&
			RequestedStencilZState.GetBackZFail() != DeviceStencilZState.GetBackZFail())
		{
			DeviceStencilZState.SetBackZFail(RequestedStencilZState.GetBackZFail());
			D3DDevice9->SetRenderState(D3DRS_CCW_STENCILZFAIL, ZEStencilOperationToDX9(DeviceStencilZState.GetBackZFail()));
		}
		if(RequestedStencilZState.GetBackStencilPass() != ZE_SO_CURRENT &&
			RequestedStencilZState.GetBackStencilPass() != DeviceStencilZState.GetBackStencilPass())
		{
			DeviceStencilZState.SetBackStencilPass(RequestedStencilZState.GetBackStencilPass());
			D3DDevice9->SetRenderState(D3DRS_CCW_STENCILPASS, ZEStencilOperationToDX9(DeviceStencilZState.GetBackStencilPass()));
		}
		if(RequestedStencilZState.GetBackStencilFunction() != ZE_SO_CURRENT &&
			RequestedStencilZState.GetBackStencilFunction() != DeviceStencilZState.GetBackStencilFunction())
		{
			DeviceStencilZState.SetBackStencilFunction(RequestedStencilZState.GetBackStencilFunction());
			D3DDevice9->SetRenderState(D3DRS_CCW_STENCILFUNC, ZEComparisionFunctionToDX9(DeviceStencilZState.GetBackStencilFunction()));
		}
	}
	// Until Next Apply;
	// If nothing changes on this state block, this should remain false
	RequestedStencilZState.SetChanged(false);
}

void ZED3D9GraphicsDevice::ApplyRequestedRasterizerState()
{
	// Return if it did not changed
	if(!RequestedRasterizerState.GetChanged())
		return;

	// Cull Side and Cull Enable
	if(RequestedRasterizerState.GetCullEnable())
	{
		if(RequestedRasterizerState.GetCullDirection() != ZE_CD_CURRENT &&
			RequestedRasterizerState.GetCullDirection() != DeviceRasterizerState.GetCullDirection())
		{
			DeviceRasterizerState.SetCullDirection(RequestedRasterizerState.GetCullDirection());
			D3DDevice9->SetRenderState(D3DRS_CULLMODE, ZECullDirectionToDX9(DeviceRasterizerState.GetCullDirection()));
		}
	}	
	else
		D3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// Fill Mode
	if(RequestedRasterizerState.GetFillMode() !=  ZE_FM_CURRENT &&
		RequestedRasterizerState.GetFillMode() != DeviceRasterizerState.GetFillMode())
	{
		DeviceRasterizerState.SetFillMode(RequestedRasterizerState.GetFillMode());
		D3DDevice9->SetRenderState(D3DRS_FILLMODE, ZEFillModeToDX9(DeviceRasterizerState.GetFillMode()));
	}
	// Until Next Apply;
	// If nothing changes on this state block, this should remain false
	RequestedRasterizerState.SetChanged(false);
}

void ZED3D9GraphicsDevice::ApplyRequestedVertexDeclaration()
{
	// If Requested Device is NULL it means Use Current
	if(RequestedVertexDeclaration != NULL)
	{
		// If Pointer Address not same consider it changed
		if(RequestedVertexDeclaration != DeviceVertexDeclaration)
		{
			DeviceVertexDeclaration = RequestedVertexDeclaration;
			DeviceVertexDeclaration->SetupVertexDeclaration();
		}
	}
}

void ZED3D9GraphicsDevice::ApplyRequestedRenderTargets()
{
	for(int i = 0; i < ZE_MAX_RENDER_TARGETS; i++)
	{
		// If NULL skip this render target
		if(RequestedRenderTargets[i] != NULL)
		{		
			// If Pointer Address not same consider it changed
			if(RequestedRenderTargets[i] != DeviceRenderTargets[i])
			{
				DeviceRenderTargets[i] = RequestedRenderTargets[i];
				D3DDevice9->SetRenderTarget(i, ((ZED3D9RenderTarget*)DeviceRenderTargets[i])->FrameBuffer);
			}
		}
	}

	if(RequestedDepthBuffer != NULL && RequestedDepthBuffer != DeviceDepthBuffer)
	{
		DeviceDepthBuffer = RequestedDepthBuffer;
		D3DDevice9->SetDepthStencilSurface(((ZED3D9DepthBuffer*)RequestedDepthBuffer)->DepthSurface);
	}
}

void ZED3D9GraphicsDevice::ApplyAllRequestedStates()
{
	ApplyRequestedBlendState();
	ApplyRequestedSamplerStates();
	ApplyRequestedShaders();
	ApplyRequestedStencilZState();
	ApplyRequestedRasterizerState();
	ApplyRequestedRenderTargets();
	ApplyRequestedVertexDeclaration();
}

void ZED3D9GraphicsDevice::Draw(ZEROPrimitiveType PrimitiveType, ZEUInt32 StartVertex, ZEUInt32 VertexCount) 
{
	// Commit State
	ApplyAllRequestedStates();
	// Then Draw
	D3DDevice9->DrawPrimitive(ZEPrimitiveTypeToDX9(PrimitiveType), StartVertex, VertexCount);
}

void ZED3D9GraphicsDevice::DrawIndexed(ZEROPrimitiveType PrimitiveType, ZEInt BaseVertexIndex, ZEUInt32 MinIndex, 
										ZEUInt32 VertexCount, ZEUInt32 StartIndex, ZEUInt32 PrimitiveCount)
{
	// Commit State
	ApplyAllRequestedStates();
	// Then Draw
	D3DDevice9->DrawIndexedPrimitive(ZEPrimitiveTypeToDX9(PrimitiveType), BaseVertexIndex, MinIndex,
										VertexCount, StartIndex, PrimitiveCount);
}

ZED3D9GraphicsDevice::ZED3D9GraphicsDevice() : ZEGraphicsDevice()
{
	D3DDevice9 = GetDevice();
	InitDefaultState();
}

ZED3D9GraphicsDevice::~ZED3D9GraphicsDevice()
{

}
