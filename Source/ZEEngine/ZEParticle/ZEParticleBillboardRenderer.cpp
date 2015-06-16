//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleBillboardRenderer.cpp
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

#include "ZEParticleBillboardRenderer.h"
#include "ZEParticle\ZEParticleEffect.h"
#include "ZEParticle\ZEParticleSystem.h"
#include "ZEGraphics\ZEVertexBuffer.h"
#include "ZEGraphics\ZEVertexTypes.h"
#include "ZEGame\ZEDrawParameters.h"
#include "ZERenderer\ZECamera.h"

void ZEParticleBillboardRenderer::DrawParticle(ZESimpleVertex* Buffer, const ZEParticle* Particle, const ZEVector3& Right, const ZEVector3& Up)
{
	ZEVector2 ParticleSize_2 = Particle->Size2D * 0.5f;

	ZEVector3 PU, NU, PV, NV;
	ZEVector3::Scale(PV, Right, ParticleSize_2.y);
	ZEVector3::Scale(NV, Right, -ParticleSize_2.y);
	ZEVector3::Scale(PU, Up, ParticleSize_2.x);
	ZEVector3::Scale(NU, Up, -ParticleSize_2.x);


	Buffer[0].Position = Particle->Position + NV + PU;
	Buffer[0].Texcoord = ZEVector2(0.0f, 0.0f);
	Buffer[0].Color = Particle->Color;

	Buffer[1].Position = Particle->Position + PV + PU;
	Buffer[1].Texcoord = ZEVector2(1.0f, 0.0f);
	Buffer[1].Color = Particle->Color;

	Buffer[2].Position = Particle->Position + PV + NU;
	Buffer[2].Texcoord = ZEVector2(1.0f, 1.0f);
	Buffer[2].Color = Particle->Color;

	Buffer[3].Position = Buffer[2].Position;
	Buffer[3].Texcoord = Buffer[2].Texcoord;
	Buffer[3].Color = Particle->Color;

	Buffer[4].Position = Particle->Position + NV + NU;
	Buffer[4].Texcoord = ZEVector2(0.0f, 1.0f);
	Buffer[4].Color = Particle->Color;

	Buffer[5].Position = Buffer[0].Position;
	Buffer[5].Texcoord = Buffer[0].Texcoord;
	Buffer[5].Color = Particle->Color;
}

void ZEParticleBillboardRenderer::UpdateVertexBuffer(ZEDrawParameters* DrawParameters)
{
	ZESize ParticleCount = Particles.GetCount();

	if (VertexBuffer == NULL)
		VertexBuffer = ZEStaticVertexBuffer::CreateInstance();

	if (VertexBuffer->GetBufferSize() != ParticleCount * sizeof(ZESimpleVertex) * 6)
	{
		if (!VertexBuffer->Create(ParticleCount * sizeof(ZESimpleVertex) * 6))
		{
			zeError("Could not create particle vertex buffer.");
			return;
		}
	}

	RenderCommand.PrimitiveCount = 0;

	ZESimpleVertex* Buffer = (ZESimpleVertex*)VertexBuffer->Lock();
	if (Buffer == NULL)
	{
		zeError("Could not lock particle vertex buffer.");
		return;
	}

	ZEVector3 CameraRight = DrawParameters->View->Camera->GetWorldRight();
	ZEVector3 CameraUp = DrawParameters->View->Camera->GetWorldUp();
	ZEVector3 CameraLook = DrawParameters->View->Camera->GetWorldFront();

	if (BillboardType == ZE_PBT_AXIS_ORIENTED)
	{
		ZESize VertexIndex = 0;
		for (ZESize N = 0; N < ParticleCount; N++)
		{
			ZEParticle Particle = Particles[ParticleCount - N - 1];
			if (Particle.State != ZE_PAS_DEAD)
			{
				ZEVector3 RightVec;
				ZEVector3::CrossProduct(RightVec, ZEVector3(Particle.Cos_NegSin, 0.0f), AxisOfOrientation);
				DrawParticle(Buffer + VertexIndex, &Particle, RightVec, ZEVector3(Particle.Cos_NegSin, 0.0f));			
				VertexIndex += 6;
				RenderCommand.PrimitiveCount += 2;
			}
		}
	}
	else if (BillboardType == ZE_PBT_SCREEN_ALIGNED)
	{
		ZESize VertexIndex = 0;

		for (ZESize N = 0; N < ParticleCount; N++)
		{
			ZEParticle Particle = Particles[ParticleCount - N - 1];

			if (Particle.State != ZE_PAS_DEAD)
			{
				ZEVector3 NegLook = -CameraLook;
				ZEVector3 Up,RightVec;


				float ZxZ = NegLook.z * NegLook.z;
				float YxY = NegLook.y * NegLook.y;
				float XxX = NegLook.x * NegLook.x;

				float XY = NegLook.x * NegLook.y;
				float XZ = NegLook.x * NegLook.z;
				float YZ = NegLook.y * NegLook.z;

				ZEMatrix3x3 L(	 0.0f,		 NegLook.z, -NegLook.y,
					-NegLook.z,  0.0f,		 NegLook.x,
					NegLook.y,	-NegLook.x,  0.0f);

				ZEMatrix3x3 LL(	-ZxZ - YxY,  XY,			XZ,
					XY,			-ZxZ - XxX, YZ,
					XZ,			 YZ,		-YxY - XxX);

				Up = (ZEMatrix3x3::Identity + Particle.Cos_NegSin.y * L + ((1 - Particle.Cos_NegSin.x) * LL)) * CameraUp;

				ZEVector3::CrossProduct(RightVec, Up, CameraLook);
				DrawParticle(Buffer + VertexIndex, &Particle, RightVec, Up);
				VertexIndex += 6;
				RenderCommand.PrimitiveCount += 2;
			}
		}
	}
	else if (BillboardType == ZE_PBT_VIEW_POINT_ORIENTED)
	{
		ZESize VertexIndex = 0;

		for (ZESize N = 0; N < ParticleCount; N++)
		{
			ZEParticle Particle = Particles[ParticleCount - N - 1];

			ZEVector3 ParticleFaceDirection = DrawParameters->View->Camera->GetWorldPosition() - Particle.Position;
			ParticleFaceDirection.NormalizeSelf();

			if (Particle.State != ZE_PAS_DEAD)
			{
				ZEVector3 InitialUp, FinalUp, RightVec;

				float ZxZ = ParticleFaceDirection.z * ParticleFaceDirection.z;
				float YxY = ParticleFaceDirection.y * ParticleFaceDirection.y;
				float XxX = ParticleFaceDirection.x * ParticleFaceDirection.x;

				float XY = ParticleFaceDirection.x * ParticleFaceDirection.y;
				float XZ = ParticleFaceDirection.x * ParticleFaceDirection.z;
				float YZ = ParticleFaceDirection.y * ParticleFaceDirection.z;

				ZEMatrix3x3 L(	0.0f,						 ParticleFaceDirection.z,	 -ParticleFaceDirection.y,
								-ParticleFaceDirection.z,	 0.0f,						 ParticleFaceDirection.x,
								ParticleFaceDirection.y,	 -ParticleFaceDirection.x,	 0.0f);

				ZEMatrix3x3 LL(	-ZxZ - YxY,	XY,			 XZ,
								XY,			-ZxZ - XxX,	 YZ,
								XZ,			YZ,			 -YxY - XxX);


				ZEVector3::CrossProduct(InitialUp, CameraRight, ParticleFaceDirection);
				FinalUp = (ZEMatrix3x3::Identity + Particle.Cos_NegSin.y * L + ((1.0f - Particle.Cos_NegSin.x) * LL)) * InitialUp; 

				ZEVector3::CrossProduct(RightVec, ParticleFaceDirection, FinalUp);
				DrawParticle(Buffer + VertexIndex, &Particle, RightVec, FinalUp);
				VertexIndex += 6;
				RenderCommand.PrimitiveCount += 2;
			}
		}
	}

	VertexBuffer->Unlock();
}

void ZEParticleBillboardRenderer::Draw(ZEDrawParameters* DrawParameters)
{
	ZEParticleRenderer::Draw(DrawParameters);

	if (GetOwner()->GetParticlePool().GetCount() == 0)
		return;

	UpdateVertexBuffer(DrawParameters);

	if (RenderCommand.PrimitiveCount == 0)
		return;

	if (VertexBuffer == NULL || Material == NULL)
		return;

	ZEVector3 OwnerWorldPos = GetOwner()->GetOwner()->GetWorldPosition();
	ZEVector3 OwnerScale = GetOwner()->GetOwner()->GetScale();

 	if(GetParticlesLocal())
 	{
		ZEMatrix4x4 ScaleMatrix, TranslationMatrix;

		ZEMatrix4x4::CreateScale(ScaleMatrix, OwnerScale);
		ZEMatrix4x4::CreateTranslation(TranslationMatrix, OwnerWorldPos);

		RenderCommand.WorldMatrix = ScaleMatrix * TranslationMatrix; //May be bug swap multiplication order.
	}
 	else
	{
		ZEMatrix4x4 ScaleMatrix;
		ZEMatrix4x4::CreateScale(ScaleMatrix, OwnerScale);
		RenderCommand.WorldMatrix = ScaleMatrix;
	}

	RenderCommand.VertexBuffer = VertexBuffer;
	RenderCommand.Material = Material;
	DrawParameters->Renderer->AddToRenderList(&RenderCommand);
}

void ZEParticleBillboardRenderer::SetAxixOfOrientation(const ZEVector3& AxisOfOrientation)
{
	this->AxisOfOrientation = AxisOfOrientation;
}

const ZEVector3& ZEParticleBillboardRenderer::GetAxisOfOrientation() const
{
	return AxisOfOrientation;
}

void ZEParticleBillboardRenderer::SetBillboardType(ZEParticleBillboardType Type)
{
	BillboardType = Type;
}

ZEParticleBillboardType ZEParticleBillboardRenderer::GetBillboardType() const
{
	return BillboardType;
}

ZEParticleBillboardRenderer* ZEParticleBillboardRenderer::CreateInstance()
{
	return new ZEParticleBillboardRenderer();
}

ZEParticleBillboardRenderer::ZEParticleBillboardRenderer()
{
	AxisOfOrientation = ZEVector3::UnitZ;
}
