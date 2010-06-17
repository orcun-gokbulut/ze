//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEParticleSystem.cpp
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

#include "ZEGraphics\ZEVertexBuffer.h"
#include "ZEGraphics\ZEVertexTypes.h"
#include "ZEGraphics\ZECamera.h"
#include "ZEParticleEmitter.h"
#include "ZEParticleEffect.h"
#include "ZEParticleSystem.h"
#include "ZEGame\ZEDrawParameters.h"
#include "ZEGraphics\ZESimpleMaterial.h"
#include "ZEGraphics\ZEFixedMaterial.h"

void ZEParticleSystem::UpdateVertexBuffer(ZEDrawParameters* DrawParameters) //Bitmedi
{
	if (BillboardType == ZE_PBT_WORLD_ORIENTED)
	{

	}
	else if (BillboardType == ZE_PBT_VIEWPLANE_ORIENTED)
	{
		unsigned int ParticleCount = GetTotalParticleCount();
		if (VertexBuffer == NULL)
			VertexBuffer = ZEStaticVertexBuffer::CreateInstance();

		if (VertexBuffer->GetBufferSize() != ParticleCount * sizeof(ZESimpleVertex) * 6)
			VertexBuffer->Create(ParticleCount * sizeof(ZESimpleVertex) * 6);
	
		RenderOrder.PrimitiveCount = 0;
		
		ZEMatrix4x4 InverseWorld;
		ZEMatrix4x4::Inverse(InverseWorld, Owner->GetWorldTransform());
		ZEMatrix4x4 Transform;
		ZEMatrix4x4::Multiply(Transform, DrawParameters->ViewPort->ViewTransform, InverseWorld);
		ZEVector3 CamDir;
		ZEMatrix4x4::Transform3x3(CamDir, Transform, ZEVector3(0.0f, 0.0f, 1.0f));

		ZESimpleVertex* Buffer = (ZESimpleVertex*)VertexBuffer->Lock();
		if (Buffer != NULL)
		{
			size_t VertexIndex = 0;
			for (size_t I = 0; I < EmitterArray.GetCount(); I++)
			{
				ZEParticleEmitter* CurrentEmmiter = EmitterArray[I];

				for (size_t N = 0; N < CurrentEmmiter->ParticlePool.GetCount(); N++)
				{
					ZEParticle* Particle = &CurrentEmmiter->ParticlePool[N];

					if (Particle->IsAlive)
					{
						ZEVector3 V, UP, U;
						ZEVector3::CrossProduct(V, Particle->UpVector, CamDir);
						ZEVector3::CrossProduct(UP, V, CamDir);
						ZEVector3::CrossProduct(U, CamDir, V); 
						float ParticleSize_2 = Particle->Size * 0.5f;

						ZEVector3 ParticlePosition;
						ZEVector3::Add(ParticlePosition, Particle->Position, CurrentEmmiter->GetPosition());

						ZEVector3 PU, NU, PV, NV;
						ZEVector3::Scale(PV, V, ParticleSize_2);
						ZEVector3::Scale(NV, V, -ParticleSize_2);
						ZEVector3::Scale(PU, U, ParticleSize_2);
						ZEVector3::Scale(NU, U, -ParticleSize_2);

						Buffer[VertexIndex].Position = ParticlePosition + NV + PU;
						Buffer[VertexIndex].Texcoord = ZEVector2(0.0f, 0.0f);

						Buffer[VertexIndex + 1].Position = ParticlePosition + PV + PU;
						Buffer[VertexIndex + 1].Texcoord = ZEVector2(1.0f, 0.0f);
						
						Buffer[VertexIndex + 2].Position = ParticlePosition + PV + NU;
						Buffer[VertexIndex + 2].Texcoord = ZEVector2(1.0f, 1.0f);
						
						Buffer[VertexIndex + 3].Position = Buffer[VertexIndex + 2].Position;
						Buffer[VertexIndex + 3].Texcoord = Buffer[VertexIndex + 2].Texcoord;
						
						Buffer[VertexIndex + 4].Position = ParticlePosition + NV + NU;
						Buffer[VertexIndex + 4].Texcoord = ZEVector2(0.0f, 1.0f);
						
						Buffer[VertexIndex + 5].Position = Buffer[VertexIndex].Position;
						Buffer[VertexIndex + 5].Texcoord = Buffer[VertexIndex].Texcoord;
						VertexIndex += 6;

						RenderOrder.PrimitiveCount += 2;
					}
				}
			}
			VertexBuffer->Unlock();
		}
	}
	else if (BillboardType == ZE_PBT_VIEWPOINT_ORIENTED)
	{
		unsigned int ParticleCount = GetTotalParticleCount();
		if (VertexBuffer == NULL)
			VertexBuffer = ZEStaticVertexBuffer::CreateInstance();

		if (VertexBuffer->GetBufferSize() != ParticleCount * sizeof(ZESimpleVertex) * 6)
			VertexBuffer->Create(ParticleCount * sizeof(ZESimpleVertex) * 6);
	
		RenderOrder.PrimitiveCount = 0;
		
		ZEVector3 CameraPosition = DrawParameters->ViewPort->Position - Owner->GetWorldPosition();

		ZESimpleVertex* Buffer = (ZESimpleVertex*)VertexBuffer->Lock();
		if (Buffer != NULL)
		{
			size_t VertexIndex = 0;
			for (size_t I = 0; I < EmitterArray.GetCount(); I++)
			{
				ZEParticleEmitter* CurrentEmmiter = EmitterArray[I];

				for (size_t N = 0; N < CurrentEmmiter->ParticlePool.GetCount(); N++)
				{
					ZEParticle* Particle = &CurrentEmmiter->ParticlePool[N];

					if (Particle->IsAlive)
					{
						ZEVector3 Direction;
						ZEVector3::Sub(Direction, CameraPosition, Particle->Position);
						ZEVector3::Normalize(Direction, Direction);
					
						ZEVector3 V, UP, U;
						ZEVector3::CrossProduct(V, Particle->UpVector, Direction);
						ZEVector3::CrossProduct(UP, V, Direction);
						ZEVector3::CrossProduct(U, Direction, V); 

						float ParticleSize_2 = Particle->Size * 0.5f;

						ZEVector3 PU, NU, PV, NV;
						ZEVector3::Scale(PV, V, ParticleSize_2);
						ZEVector3::Scale(NV, V, -ParticleSize_2);
						ZEVector3::Scale(PU, U, ParticleSize_2);
						ZEVector3::Scale(NU, U, -ParticleSize_2);

						ZEVector3 ParticlePosition;
						ZEVector3::Add(ParticlePosition, Particle->Position, CurrentEmmiter->GetPosition());

						Buffer[VertexIndex].Position = ParticlePosition + NV + PU;
						Buffer[VertexIndex].Texcoord = ZEVector2(0.0f, 0.0f);

						Buffer[VertexIndex + 1].Position = ParticlePosition + PV + PU;
						Buffer[VertexIndex + 1].Texcoord = ZEVector2(1.0f, 0.0f);
						
						Buffer[VertexIndex + 2].Position = ParticlePosition + PV + NU;
						Buffer[VertexIndex + 2].Texcoord = ZEVector2(1.0f, 1.0f);
						
						Buffer[VertexIndex + 3].Position = Buffer[VertexIndex + 2].Position;
						Buffer[VertexIndex + 3].Texcoord = Buffer[VertexIndex + 2].Texcoord;
						
						Buffer[VertexIndex + 4].Position = ParticlePosition + NV + NU;
						Buffer[VertexIndex + 4].Texcoord = ZEVector2(0.0f, 1.0f);
						
						Buffer[VertexIndex + 5].Position = Buffer[VertexIndex].Position;
						Buffer[VertexIndex + 5].Texcoord = Buffer[VertexIndex].Texcoord;
						VertexIndex += 6;

						RenderOrder.PrimitiveCount += 2;
					}
				}
			}
			VertexBuffer->Unlock();
		}
	}

	IsVertexBufferUpdated = true;
}

void ZEParticleSystem::Draw(ZEDrawParameters* DrawParameters)
{
	if (GetTotalParticleCount() == 0)
		return;

	UpdateVertexBuffer(DrawParameters);

	if (RenderOrder.PrimitiveCount == 0)
		return;

	if (VertexBuffer == NULL || ParticleMaterial == NULL)
		return;

	RenderOrder.WorldMatrix = GetOwner()->GetWorldTransform();
	RenderOrder.VertexBuffer = VertexBuffer;
	RenderOrder.Material = ParticleMaterial;
	DrawParameters->Renderer->AddToRenderList(&RenderOrder);
}

void ZEParticleSystem::Tick(float TimeElapsed)
{
	for(unsigned int I = 0; I < EmitterArray.GetCount(); I++)
	{
		EmitterArray[I]->Tick(TimeElapsed);
	}
}

unsigned int ZEParticleSystem::GetTotalParticleCount()
{
	unsigned int TotalParticleCount = 0;

	for(unsigned int I = 0; I < EmitterArray.GetCount(); I++)
	{
		TotalParticleCount += EmitterArray[I]->GetMaxParticleCount();
	}

	return TotalParticleCount;
}

void ZEParticleSystem::SetOwner(ZEParticleEffect* OwnerEffect)
{
	Owner = OwnerEffect;
}

ZEParticleEffect* ZEParticleSystem::GetOwner() const
{
	return Owner;
}

void ZEParticleSystem::AddParticleEmitter(ZEParticleEmitter* ParticleEmitter)
{
	ParticleEmitter->SetOwner(this);
	ParticleEmitter->InitializeParticlePool();
	EmitterArray.Add(ParticleEmitter);
}

void ZEParticleSystem::DeleteParticleEmitter(ZEParticleEmitter* ParticleEmitter)
{
	EmitterArray.DeleteValue(ParticleEmitter);
}

void ZEParticleSystem::SetName(const ZEString& SystemName)
{
	Name = SystemName;
}

const ZEString& ZEParticleSystem::GetName() const
{
	return Name;
}

void ZEParticleSystem::SetBillboardType(ZEParticleBillboardType Type)
{
	BillboardType = Type;
}

ZEParticleBillboardType ZEParticleSystem::GetBillboardType() const
{
	return BillboardType;
}

void ZEParticleSystem::SetMaterial(ZEMaterial *Material)
{
	ParticleMaterial = Material;
}

ZEMaterial* ZEParticleSystem::GetMaterial() const
{
	return ParticleMaterial;
}

const ZEArray<ZEParticleEmitter*>& ZEParticleSystem::GetEmitterArray()
{
	return EmitterArray;
}

ZEParticleSystem::ZEParticleSystem()
{
	Owner = NULL;
	VertexBuffer = NULL;
	ParticleMaterial = NULL;
	RenderOrder.SetZero();
	RenderOrder.Flags = ZE_ROF_ENABLE_VIEW_PROJECTION_TRANSFORM | ZE_ROF_ENABLE_WORLD_TRANSFORM | ZE_ROF_TRANSPARENT | ZE_ROF_ENABLE_Z_CULLING;
	RenderOrder.VertexDeclaration = ZESimpleVertex::GetVertexDeclaration();
	RenderOrder.PrimitiveType = ZE_ROPT_TRIANGLE;
	IsVertexBufferUpdated = false;
	BillboardType = ZEParticleBillboardType::ZE_PBT_VIEWPLANE_ORIENTED;
	ParticleMaterial = ZEFixedMaterial::CreateInstance();
	ParticleMaterial->SetZero();
}

ZEParticleSystem::~ZEParticleSystem()
{
	if (VertexBuffer != NULL)
		VertexBuffer->Destroy();
}

#include "ZEParticleSystem.h.zpp"




