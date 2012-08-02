//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEGLGraphicsDevice.cpp
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

#include "ZETypes.h"
#include "ZEGLGraphicsDevice.h"
#include "ZEGLShader.h"
#include "ZEGLRenderTarget.h"
#include "ZEGLDepthBuffer.h"
#include "ZEGLTexture2D.h"
#include "ZEGLTexture3D.h"
#include "ZEGLTextureCube.h"
#include "ZEGLVertexBuffer.h"
#include "ZEGLIndexBuffer.h"

inline GLenum ZEBlendOptionToGL(ZEBlendOption BlendOption)
{
	static const GLenum Values[] =
	{
		(GLenum)0,
		GL_ZERO,
		GL_ONE,
		GL_SRC_COLOR,
		GL_ONE_MINUS_SRC_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA,
		GL_DST_COLOR,
		GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA_SATURATE
	};
	return Values[BlendOption];
}

inline GLenum ZEBlendEquationToGL(ZEBlendEquation BlendEquation)
{
	static const GLenum Values[] =
	{
		(GLenum)0,
		GL_FUNC_ADD,
		GL_FUNC_SUBTRACT,
		GL_FUNC_REVERSE_SUBTRACT,
		GL_MIN,
		GL_MAX
	};
	return Values[BlendEquation];
}

inline GLenum ZEComparisionFunctionToGL(ZEComparisonFunction CompFunc)
{
	static const GLenum Values[] =
	{
		(GLenum)0, 
		GL_NEVER, 
		GL_LESS, GL_EQUAL, 
		GL_LEQUAL, 
		GL_GREATER, 
		GL_NOTEQUAL, 
		GL_GEQUAL, 
		GL_ALWAYS
	};
	return Values[CompFunc];
}

inline GLenum ZEStencilOperationToGL(ZEStencilOperation StencilOperation)
{
	static const GLenum Values[] = 
	{ 
		(GLenum)0, 
		GL_KEEP, 
		GL_ZERO, 
		GL_REPLACE,					
		GL_INCR_WRAP, 
		GL_DECR_WRAP, 
		GL_INVERT,
		GL_INCR, 
		GL_DECR
	};
	return Values[StencilOperation];
}

inline GLenum ZEFillModeToGL(ZEFillMode FillMode)
{
	static const GLenum Values[] = 
	{
		(GLenum)0,
		GL_LINE,
		GL_FILL,
	};
	return Values[FillMode];
}

inline GLenum ZECullDirectionToGL(ZECullDirection CullDirection)
{
	static const GLenum Values[] = 
	{
		(GLenum)0,
		GL_CW,
		GL_CCW
	};
	return Values[CullDirection];
}

inline GLenum ZETextureAdressModeToGL(ZETextureAdressMode TextureAddress)
{
	static const GLenum Values[] =
	{
		(GLenum)0,
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_CLAMP_TO_EDGE,
		GL_CLAMP_TO_BORDER 
	};
	return Values[TextureAddress];
}

inline GLenum ZETextureFilterModeToGL(ZETextureFilterMode TextureFilter)
{
	static const GLenum Values[] =
	{
		(GLenum)0,
		GL_NEAREST,
		GL_LINEAR,
		GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,
	};
	return Values[TextureFilter];
}

inline GLenum ZEPrimitiveTypeToGL(ZEROPrimitiveType PrimitiveType)
{
	static const GLenum Values[] = 
	{
		GL_POINTS,
		GL_LINES,
		GL_TRIANGLES,
		GL_TRIANGLE_FAN
	};
	return Values[PrimitiveType];
}

inline void ZEGLGraphicsDevice::InitDefaultState()
{
	// ZE standard Front Face is CW
	glFrontFace(GL_CW);

	// Only ZE_MAX_SAMPLER_ATTACHMENT amount, Sampler Object
	glGenSamplers(ZE_MAX_SAMPLER_ATTACHMENT, GLSamplerStateHandles);

	// Only One VAO, draw calls will be generated on the fly.
	glGenVertexArrays(1, &VertexArrayObject);
	glBindVertexArray(VertexArrayObject);

	// DummyFramebuffer Aside from Main Frame buffer
	glGenFramebuffers(1, &FramebufferObject);

	// TODO: 						   
	// Default Shaders				   
	//	.........					   
									   
	// Default Vertex Declaration	   
	// ..........					   
									   
	// RenderTargets				   
	// ..........
}

void ZEGLGraphicsDevice::ApplyRequestedBlendState()
{
	// Skip If Requested Blend State didn't changed.
	if(!RequestedBlendState.GetChanged())
		return;

	// Enable Disable
	if(RequestedBlendState.GetBlendEnable() != DeviceBlendState.GetBlendEnable())
	{
		DeviceBlendState.SetBlendEnable(RequestedBlendState.GetBlendEnable());
		if(DeviceBlendState.GetBlendEnable())
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);
	}
	// Alpha and Color Blending Options
	if(DeviceBlendState.GetBlendEnable())
	{
		bool ColorBlendOpChanged = false;
		if(RequestedBlendState.GetSourceColorBlendOption() != ZE_BO_CURRENT && 
			RequestedBlendState.GetSourceColorBlendOption() != DeviceBlendState.GetSourceColorBlendOption())
		{
			DeviceBlendState.SetSourceColorBlendOption(RequestedBlendState.GetSourceColorBlendOption());
			ColorBlendOpChanged = true;
		}

		if(RequestedBlendState.GetDestinationColorBlendOption() != ZE_BO_CURRENT &&
			RequestedBlendState.GetDestinationColorBlendOption() != DeviceBlendState.GetDestinationColorBlendOption())
		{
			DeviceBlendState.SetDestinationColorBlendOption(RequestedBlendState.GetDestinationColorBlendOption());
			ColorBlendOpChanged = true;
		}

		if(RequestedBlendState.GetSourceAlphaBlendOption() != ZE_BO_CURRENT &&
			RequestedBlendState.GetSourceAlphaBlendOption() != DeviceBlendState.GetSourceAlphaBlendOption())
		{
			DeviceBlendState.SetSourceAlphaBlendOption(RequestedBlendState.GetSourceAlphaBlendOption());
			ColorBlendOpChanged = true;
		}

		if(RequestedBlendState.GetDestinationAlphaBlendOption() != ZE_BO_CURRENT &&
			RequestedBlendState.GetDestinationAlphaBlendOption() != DeviceBlendState.GetDestinationAlphaBlendOption())
		{
			DeviceBlendState.SetDestinationAlphaBlendOption(RequestedBlendState.GetDestinationAlphaBlendOption());
			ColorBlendOpChanged = true;
		}

		if(ColorBlendOpChanged)
			glBlendFuncSeparate(ZEBlendOptionToGL(DeviceBlendState.GetSourceColorBlendOption()),
								ZEBlendOptionToGL(DeviceBlendState.GetDestinationColorBlendOption()),
								ZEBlendOptionToGL(DeviceBlendState.GetSourceAlphaBlendOption()),
								ZEBlendOptionToGL(DeviceBlendState.GetDestinationAlphaBlendOption()));


		if(RequestedBlendState.GetBlendEquation() != ZE_BE_CURRENT &&
			RequestedBlendState.GetBlendEquation() != DeviceBlendState.GetBlendEquation())
		{
			DeviceBlendState.SetBlendEquation(RequestedBlendState.GetBlendEquation());
			glBlendEquation(ZEBlendEquationToGL(DeviceBlendState.GetBlendEquation()));
		}
	}
	//ColorMask
	if(RequestedBlendState.GetColorChannelMask() != DeviceBlendState.GetColorChannelMask())
	{
		DeviceBlendState.SetColorChannelMask(RequestedBlendState.GetColorChannelMask());
		ZEColorChannelMask ChannelMask = DeviceBlendState.GetColorChannelMask();
		glColorMask(ChannelMask.GetFlags(ZE_CCM_RED),
					ChannelMask.GetFlags(ZE_CCM_GREEN),
					ChannelMask.GetFlags(ZE_CCM_BLUE),
					ChannelMask.GetFlags(ZE_CCM_ALPHA)
					);
	}
	// Until Next Apply;
	// If nothing changes on this state block, this should remain false
	RequestedBlendState.SetChanged(false);
}

void ZEGLGraphicsDevice::ApplyRequestedSamplerStates()
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
				glSamplerParameteri(GLSamplerStateHandles[i], GL_TEXTURE_WRAP_S, ZETextureAdressModeToGL(DeviceSamplerStates[i].GetAddressU()));
				if(DeviceSamplerStates[i].GetAddressU() == ZE_TAM_BORDER)
					AdressesHaveBorderColor = true;
			}
			if(RequestedSamplerStates[i].GetAddressV() != ZE_TAM_CURRENT &&
				RequestedSamplerStates[i].GetAddressV() != DeviceSamplerStates[i].GetAddressV())
			{
				DeviceSamplerStates[i].SetAddressV(RequestedSamplerStates[i].GetAddressV());
				glSamplerParameteri(GLSamplerStateHandles[i], GL_TEXTURE_WRAP_T, ZETextureAdressModeToGL(DeviceSamplerStates[i].GetAddressV()));
				if(DeviceSamplerStates[i].GetAddressV() == ZE_TAM_BORDER)
					AdressesHaveBorderColor = true;
			}
			if(RequestedSamplerStates[i].GetAddressW() != ZE_TAM_CURRENT &&
				RequestedSamplerStates[i].GetAddressW() != DeviceSamplerStates[i].GetAddressW())
			{
				DeviceSamplerStates[i].SetAddressW(RequestedSamplerStates[i].GetAddressW());
				glSamplerParameteri(GLSamplerStateHandles[i], GL_TEXTURE_WRAP_R, ZETextureAdressModeToGL(DeviceSamplerStates[i].GetAddressW()));
				if(DeviceSamplerStates[i].GetAddressW() == ZE_TAM_BORDER)
					AdressesHaveBorderColor = true;
			}
			// Filters
			if(RequestedSamplerStates[i].GetMagFilter() != ZE_TFM_CURRENT &&
				RequestedSamplerStates[i].GetMagFilter() != DeviceSamplerStates[i].GetMagFilter())
			{
				DeviceSamplerStates[i].SetMagFilter(RequestedSamplerStates[i].GetMagFilter());
				if(DeviceSamplerStates[i].GetMagFilter() == ZE_TFM_ANISOTROPY)
				{
					glTexParameteri(GLSamplerStateHandles[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					FiltersHaveAnisotrophy = true;
				}
				else
					glSamplerParameteri(GLSamplerStateHandles[i], GL_TEXTURE_MAG_FILTER, ZETextureFilterModeToGL(DeviceSamplerStates[i].GetMagFilter()));
			}
			// Min Mip is set in one go in GL
			if(RequestedSamplerStates[i].GetMinFilter() != ZE_TFM_CURRENT &&
				RequestedSamplerStates[i].GetMinFilter() != DeviceSamplerStates[i].GetMinFilter())
			{
				DeviceSamplerStates[i].SetMinFilter(RequestedSamplerStates[i].GetMinFilter());
				if(DeviceSamplerStates[i].GetMinFilter() == ZE_TFM_ANISOTROPY)
					FiltersHaveAnisotrophy = true;
			}
			if(RequestedSamplerStates[i].GetMipFilter() != ZE_TFM_CURRENT &&
				RequestedSamplerStates[i].GetMipFilter() != DeviceSamplerStates[i].GetMipFilter())
			{
				DeviceSamplerStates[i].SetMipFilter(RequestedSamplerStates[i].GetMipFilter());
				if(DeviceSamplerStates[i].GetMipFilter() == ZE_TFM_ANISOTROPY)
					FiltersHaveAnisotrophy = true;
			}
			// Assigning Texture Filtering Mode
			ZETextureFilterMode DeviceMinFilterMode = DeviceSamplerStates[i].GetMinFilter();
			ZETextureFilterMode DeviceMipFilterMode = DeviceSamplerStates[i].GetMipFilter();
			if(DeviceMinFilterMode != ZE_TFM_ANISOTROPY)
			{
				switch (DeviceMipFilterMode)
				{
					case ZE_TFM_NONE :
						glSamplerParameteri(GLSamplerStateHandles[i], GL_TEXTURE_MIN_FILTER, ZETextureFilterModeToGL(DeviceMinFilterMode));
						break;
					case ZE_TFM_LINEAR :
						if(DeviceMinFilterMode == ZE_TFM_LINEAR)
							glSamplerParameteri(GLSamplerStateHandles[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						else if (DeviceMinFilterMode ==  ZE_TFM_POINT)
							glSamplerParameteri(GLSamplerStateHandles[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
						break;
					case ZE_TFM_POINT :
						if(DeviceMinFilterMode == ZE_TFM_LINEAR)
							glSamplerParameteri(GLSamplerStateHandles[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
						else if (DeviceMinFilterMode ==  ZE_TFM_POINT)
							glSamplerParameteri(GLSamplerStateHandles[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
						break;
				}
			}
			// Rest
			if(FiltersHaveAnisotrophy &&
				RequestedSamplerStates[i].GetMaxAnisotrophy() != DeviceSamplerStates[i].GetMaxAnisotrophy())
			{
				DeviceSamplerStates[i].SetMaxAnisotrophy(RequestedSamplerStates[i].GetMaxAnisotrophy());
				glSamplerParameterf(GLSamplerStateHandles[i], GL_TEXTURE_MAX_ANISOTROPY_EXT, (int)RequestedSamplerStates[i].GetMaxAnisotrophy());
			}
			if(AdressesHaveBorderColor &&
				RequestedSamplerStates[i].GetBorderColor() != DeviceSamplerStates[i].GetBorderColor())
			{	
				DeviceSamplerStates[i].SetBorderColor(RequestedSamplerStates[i].GetBorderColor());
				glSamplerParameterfv(GLSamplerStateHandles[i], GL_TEXTURE_BORDER_COLOR, DeviceSamplerStates[i].GetBorderColor().M);
			}
			if(RequestedSamplerStates[i].GetMaxLOD() != DeviceSamplerStates[i].GetMaxLOD())
			{
				DeviceSamplerStates[i].SetMaxLOD(RequestedSamplerStates[i].GetMaxLOD());
				glSamplerParameterf(GLSamplerStateHandles[i], GL_TEXTURE_MAX_LOD, DeviceSamplerStates[i].GetMaxLOD());
			}
			if(RequestedSamplerStates[i].GetTexture() != DeviceSamplerStates[i].GetTexture())
			{
				DeviceSamplerStates[i].SetCurrentTexture(RequestedSamplerStates[i].GetTexture());
				glActiveTexture(GL_TEXTURE0 + i);
				ZETextureType TexType = DeviceSamplerStates[i].GetTexture()->GetTextureType();
				switch (TexType)
				{
					case ZE_TT_2D :
						glBindSampler(GLSamplerStateHandles[i], ((ZEGLTexture2D*)DeviceSamplerStates[i].GetTexture())->Texture2D);
						glBindTexture(GL_TEXTURE_2D, ((ZEGLTexture2D*)DeviceSamplerStates[i].GetTexture())->Texture2D);
						break;
					case ZE_TT_3D :
						glBindSampler(GLSamplerStateHandles[i], ((ZEGLTexture3D*)DeviceSamplerStates[i].GetTexture())->Texture3D);
						glBindTexture(GL_TEXTURE_3D, ((ZEGLTexture3D*)DeviceSamplerStates[i].GetTexture())->Texture3D);
						break;
					case ZE_TT_CUBE :
						glBindSampler(GLSamplerStateHandles[i], ((ZEGLTextureCube*)DeviceSamplerStates[i].GetTexture())->TextureCube);
						glBindTexture(GL_TEXTURE_CUBE_MAP, ((ZEGLTextureCube*)DeviceSamplerStates[i].GetTexture())->TextureCube);
				}
			}
			// Until Next Apply;
			// If nothing changes on this state block, this should remain false
			RequestedSamplerStates[i].SetChanged(false);
		}
	}
}

void ZEGLGraphicsDevice::ApplyRequestedShaders()
{
	// if RequestedPixelShader is NULL that means use current shader
	if(RequestedPixelShader != NULL)
	{
		// Check the pointers if same address then consider them as same shader
		if(RequestedPixelShader != DevicePixelShader)
		{
			DevicePixelShader = RequestedPixelShader;
			((ZEGLPixelShader*)DevicePixelShader)->BindShader();
		}
	}
	if(RequestedVertexShader != NULL)
	{
		if(RequestedVertexShader != DeviceVertexShader)
		{
			DeviceVertexShader = RequestedVertexShader;
			((ZEGLVertexShader*)DeviceVertexShader)->BindShader();
		}
	}
}

void ZEGLGraphicsDevice::ApplyRequestedStencilZState()
{
	// Skip Entire Function if Requested State is same from before
	if(!RequestedStencilZState.GetChanged())
		return;

	// Enable, Disable and Mask
	if(RequestedStencilZState.GetZTestEnable() != DeviceStencilZState.GetZTestEnable())
	{
		DeviceStencilZState.SetZTestEnable(RequestedStencilZState.GetZTestEnable());
		if(DeviceStencilZState.GetZTestEnable())
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}
	if(RequestedStencilZState.GetStencilTestEnable() != DeviceStencilZState.GetStencilTestEnable())
	{	
		DeviceStencilZState.SetStencilTestEnable(RequestedStencilZState.GetStencilTestEnable());
		if(DeviceStencilZState.GetStencilTestEnable())
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
	}
	if(RequestedStencilZState.GetZWriteEnable() != DeviceStencilZState.GetZWriteEnable())
	{
		DeviceStencilZState.SetZWriteEnable(RequestedStencilZState.GetZWriteEnable());
		glDepthMask(DeviceStencilZState.GetZWriteEnable());
	}
	if(RequestedStencilZState.GetStencilWriteMask() != DeviceStencilZState.GetStencilWriteMask())
	{
		DeviceStencilZState.SetStencilWriteMask(RequestedStencilZState.GetStencilWriteMask());
		glStencilMask(DeviceStencilZState.GetStencilWriteMask());
	}

	// Z Function
	if(DeviceStencilZState.GetZTestEnable() && RequestedStencilZState.GetZFunction() != ZE_CF_CURRENT &&
		RequestedStencilZState.GetZFunction() != DeviceStencilZState.GetZFunction())
	{
		DeviceStencilZState.SetZFunction(RequestedStencilZState.GetZFunction());
		glDepthFunc(ZEComparisionFunctionToGL(DeviceStencilZState.GetZFunction()));
	}
	// Front Stencil Operation
	if(DeviceStencilZState.GetStencilTestEnable())
	{
		bool FrontStencilChanged = false;
		bool BackStencilChanged = false;
		// Front Stencil Operation
		if(RequestedStencilZState.GetFrontStencilFail() != ZE_CF_CURRENT &&
			RequestedStencilZState.GetFrontStencilFail() != DeviceStencilZState.GetFrontStencilFail())
		{
			DeviceStencilZState.SetFrontStencilFail(RequestedStencilZState.GetFrontStencilFail());
			FrontStencilChanged = true;
		}
		if(DeviceStencilZState.GetFrontZFail() != ZE_SO_CURRENT &&
			RequestedStencilZState.GetFrontZFail() != DeviceStencilZState.GetFrontZFail())
		{
			DeviceStencilZState.SetFrontZFail(RequestedStencilZState.GetFrontZFail());
			FrontStencilChanged = true;
		}
		if(RequestedStencilZState.GetFrontStencilPass() != ZE_SO_CURRENT &&
			RequestedStencilZState.GetFrontStencilPass() != DeviceStencilZState.GetFrontStencilPass())
		{
			DeviceStencilZState.SetFrontStencilPass(RequestedStencilZState.GetFrontStencilPass());
			FrontStencilChanged = true;
		}
		if(RequestedStencilZState.GetFrontStencilFunction() != ZE_SO_CURRENT &&
			RequestedStencilZState.GetFrontStencilFunction() != DeviceStencilZState.GetFrontStencilFunction())
		{
			DeviceStencilZState.SetFrontStencilFunction(RequestedStencilZState.GetFrontStencilFunction());
			glStencilFuncSeparate(GL_FRONT, ZEComparisionFunctionToGL(DeviceStencilZState.GetFrontStencilFunction()), 0, 0xFFFFFFFF);
		}

		if(FrontStencilChanged)
			glStencilOpSeparate(GL_FRONT,
								DeviceStencilZState.GetFrontStencilFail(),
								DeviceStencilZState.GetFrontZFail(),
								DeviceStencilZState.GetFrontStencilPass());

		// Back Stencil Operation
		if(RequestedStencilZState.GetBackStencilFail() != ZE_CF_CURRENT &&
			RequestedStencilZState.GetBackStencilFail() != DeviceStencilZState.GetBackStencilFail())
		{
			DeviceStencilZState.SetBackStencilFail(RequestedStencilZState.GetBackStencilFail());
			BackStencilChanged = true;
		}
		if(RequestedStencilZState.GetBackZFail() != ZE_SO_CURRENT &&
			RequestedStencilZState.GetBackZFail() != DeviceStencilZState.GetBackZFail())
		{
			DeviceStencilZState.SetBackZFail(RequestedStencilZState.GetBackZFail());
			BackStencilChanged = true;
		}
		if(RequestedStencilZState.GetBackStencilPass() != ZE_SO_CURRENT &&
			RequestedStencilZState.GetBackStencilPass() != DeviceStencilZState.GetBackStencilPass())
		{
			DeviceStencilZState.SetBackStencilPass(RequestedStencilZState.GetBackStencilPass());
			BackStencilChanged = true;
		}
		if(RequestedStencilZState.GetBackStencilFunction() != ZE_SO_CURRENT &&
			RequestedStencilZState.GetBackStencilFunction() != DeviceStencilZState.GetBackStencilFunction())
		{
			DeviceStencilZState.SetBackStencilFunction(RequestedStencilZState.GetBackStencilFunction());
			glStencilFuncSeparate(GL_BACK, ZEComparisionFunctionToGL(DeviceStencilZState.GetBackStencilFunction()), 0, 0xFFFFFFFF);
		}

		if(BackStencilChanged)
			glStencilOpSeparate(GL_BACK,
								DeviceStencilZState.GetBackStencilFail(),
								DeviceStencilZState.GetBackZFail(),
								DeviceStencilZState.GetBackStencilPass());
	}
	// Until Next Apply;
	// If nothing changes on this state block, this should remain false
	RequestedStencilZState.SetChanged(false);
}

void ZEGLGraphicsDevice::ApplyRequestedRasterizerState()
{
	// Return if it did not changed
	if(!RequestedRasterizerState.GetChanged())
		return;

	// Cull Side and Cull Enable
	if(RequestedRasterizerState.GetCullEnable())
	{
		glEnable(GL_CULL_FACE);
		if(RequestedRasterizerState.GetCullDirection() != ZE_CD_CURRENT &&
			RequestedRasterizerState.GetCullDirection() != DeviceRasterizerState.GetCullDirection())
		{
			DeviceRasterizerState.SetCullDirection(RequestedRasterizerState.GetCullDirection());
			if(DeviceRasterizerState.GetCullDirection() == ZE_CD_CLOCKWISE)
				glCullFace(GL_FRONT);
			else
				glCullFace(GL_BACK);
		}
	}	
	else
		glDisable(GL_CULL_FACE);

	// Fill Mode
	if(RequestedRasterizerState.GetFillMode() !=  ZE_FM_CURRENT &&
		RequestedRasterizerState.GetFillMode() != DeviceRasterizerState.GetFillMode())
	{
		DeviceRasterizerState.SetFillMode(RequestedRasterizerState.GetFillMode());
		glPolygonMode(GL_FRONT_AND_BACK, ZEFillModeToGL(DeviceRasterizerState.GetFillMode()));
	}
	// Until Next Apply;
	// If nothing changes on this state block, this should remain false
	RequestedRasterizerState.SetChanged(false);
}

void ZEGLGraphicsDevice::ApplyRequestedVertexBuffer()
{
	if(RequestedVertexBuffer != NULL && RequestedVertexBuffer == DeviceVertexBuffer)
	{
		DeviceVertexBuffer = RequestedVertexBuffer;
		glBindBuffer(GL_ARRAY_BUFFER, ((ZEGLStaticVertexBuffer*)DeviceVertexBuffer)->VertexBuffer);
	}
	// If We Bind Here we can only use one vertex buffer.
}

void ZEGLGraphicsDevice::ApplyRequestedIndexBuffer()
{
	if(RequestedIndexBuffer != NULL && RequestedIndexBuffer == DeviceIndexBuffer)
	{
		DeviceIndexBuffer = RequestedIndexBuffer;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ((ZEGLIndexBuffer*)DeviceIndexBuffer)->IndexBuffer);
	}
}


void ZEGLGraphicsDevice::ApplyRequestedRenderTargets()
{
	if(ScreenWriteEnable)
	{
		// Ignore Any other RenderTarget
		// Switch to Screen FBO 
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glDrawBuffer(GL_BACK);
		return;
	}
	else
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FramebufferObject);
		GLenum DrawBufferEnable[ZE_MAX_RENDER_TARGETS];
		int EnabledRenderBufferSize = 0;
		for(int i = 0; i < ZE_MAX_RENDER_TARGETS; i++)
		{
			if(RequestedRenderTargets[i] != NULL)
			{
				if(RequestedRenderTargets[i] != DeviceRenderTargets[i])
				{
					DeviceRenderTargets[i] = RequestedRenderTargets[i];
					glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, ((ZEGLRenderTarget*)DeviceRenderTargets[i])->RenderTarget, 0);
				}
				DrawBufferEnable[EnabledRenderBufferSize] = GL_COLOR_ATTACHMENT0 + i;
				EnabledRenderBufferSize++;
			}
		}
		// Open Only non-NULL RenderTargets
		if(EnabledRenderBufferSize != 0)
			glDrawBuffers(EnabledRenderBufferSize, DrawBufferEnable);
		else
			glDrawBuffer(GL_NONE);	// Don't Write on to Anything. This only implies color(Depth/Stencil Buffer Writes Still Apply)

		// Now Set A DepthStencilBuffer
		if(RequestedDepthBuffer != NULL && RequestedDepthBuffer != DeviceDepthBuffer)
		{
			DeviceDepthBuffer = RequestedDepthBuffer;
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, ((ZEGLDepthBuffer*)DeviceDepthBuffer)->DepthBuffer, 0);
		}
	}
}

void ZEGLGraphicsDevice::ApplyAllRequestedStates()
{
	ApplyRequestedBlendState();
	ApplyRequestedSamplerStates();
	ApplyRequestedShaders();
	ApplyRequestedStencilZState();
	ApplyRequestedRasterizerState();
	ApplyRequestedRenderTargets();
	ApplyRequestedVertexBuffer();
}

void ZEGLGraphicsDevice::Draw(ZEROPrimitiveType PrimitiveType, ZEUInt32 StartVertex, ZEUInt32 VertexCount) 
{
	// Commit State
	ApplyAllRequestedStates();

	// Create Series of GL commands from Vertex Declaration
	// Assuming Vertex Size is Generated
	((ZEGLStaticVertexBuffer*)DeviceVertexBuffer)->PrepareForDrawCall();

	// DrawCall
	glDrawArrays(ZEPrimitiveTypeToGL(PrimitiveType), StartVertex, VertexCount);

	// Disabling VertexAttribArray
	((ZEGLStaticVertexBuffer*)DeviceVertexBuffer)->RestoreAttribArrays();
}

void ZEGLGraphicsDevice::DrawIndexed(ZEROPrimitiveType PrimitiveType, ZEInt BaseVertexIndex, ZEUInt32 MinIndex, 
	ZEUInt32 VertexCount, ZEUInt32 StartIndex, ZEUInt32 PrimitiveCount)
{
	// Commit State
	ApplyAllRequestedStates();
	ApplyRequestedIndexBuffer();

	// Create Series of GL commands from Vertex Declaration
	// Assuming Vertex Size is Generated
	((ZEGLStaticVertexBuffer*)DeviceVertexBuffer)->PrepareForDrawCall();

	// DrawCall
	glDrawRangeElementsBaseVertex(ZEPrimitiveTypeToGL(PrimitiveType), MinIndex, ZE_UINT_MAX, VertexCount, GL_UNSIGNED_INT, (void*)&StartIndex, BaseVertexIndex);

	// Disabling VertexAttribArray
	((ZEGLStaticVertexBuffer*)DeviceVertexBuffer)->RestoreAttribArrays();
}

ZEGLGraphicsDevice::ZEGLGraphicsDevice() : ZEGraphicsDevice()
{
	InitDefaultState();
}

ZEGLGraphicsDevice::~ZEGLGraphicsDevice()
{
	glDeleteSamplers(ZE_MAX_SAMPLER_ATTACHMENT, GLSamplerStateHandles);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VertexArrayObject);
	glDeleteFramebuffers(1, &FramebufferObject);
}

