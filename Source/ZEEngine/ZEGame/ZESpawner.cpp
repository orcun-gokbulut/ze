//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESpawner.cpp
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

#include "ZESpawner.h"

#include "ZEError.h"
#include "ZEMeta/ZEMTProvider.h"
#include "ZEScene.h"

void ZESpawner::UpdateSpawns(ZESize Index)
{
	const ZEArray<ZEEntity*> ChildEntities = GetChildEntities();
	Spawns.LockWrite();
	ChildEntities.LockRead();
	{
		Spawns[Index].Entity = ChildEntities[Index];
		Spawns[Index].Position = ChildEntities[Index]->GetPosition();
		Spawns[Index].Rotation = ChildEntities[Index]->GetRotation();
		Spawns[Index].Scale = ChildEntities[Index]->GetScale();
		Spawns[Index].Visible = ChildEntities[Index]->GetVisible();
		Spawns[Index].Enabled = ChildEntities[Index]->GetEnabled();
	}
	ChildEntities.UnlockRead();
	Spawns.UnlockWrite();
}

ZEEntityResult ZESpawner::InitializeInternal()
{
	ZE_ENTITY_INITIALIZE_CHAIN(ZEEntity);
	
	
	if (AutoSpawn && AutoSpawnDelay == 0.0f)
		Spawn();
	
	return ZE_ER_DONE;
}

ZEEntityResult ZESpawner::DeinitializeInternal()
{
	Despawn();



	ZE_ENTITY_DEINITIALIZE_CHAIN(ZEEntity);

	return ZE_ER_DONE;
}

ZEEntityResult ZESpawner::LoadInternal()
{
	ZE_ENTITY_LOAD_CHAIN(ZEEntity);

	if (GetWrapper() == NULL)
	{
		const ZEArray<ZEEntity*>& ChildEntities = GetChildEntities();
		for (ZESize I = 0; I < ChildEntities.GetCount(); I++)
			ZEEntity::RemoveChildEntity(ChildEntities[I]);
	}

	return ZE_ER_DONE;
}

ZEEntityResult ZESpawner::UnloadInternal()
{
	ZE_ENTITY_UNLOAD_CHAIN(ZEEntity);

	if (GetWrapper() == NULL)
	{
		for (ZESize I = 0; I < Spawns.GetCount(); I++)
			ZEEntity::AddChildEntity(Spawns[I].Entity);
	}

	return ZE_ER_DONE;
}

ZESpawner::ZESpawner()
{
	Spawned = false;
	AutoSpawn = false;
	AutoRespawn = false;
	AutoSpawnDelay = 0.0f;
}

ZESpawner::~ZESpawner()
{
	Unload();
}

const ZEArray<ZESpawn>& ZESpawner::GetSpawns() const
{
	return Spawns;
}

void ZESpawner::SetAutoSpawn(bool AutoSpawn)
{
	this->AutoSpawn = AutoSpawn;
}

bool ZESpawner::GetAutoSpawn() const
{
	return AutoSpawn;
}

void ZESpawner::SetAutoSpawnDelay(float Seconds)
{
	if (AutoSpawnDelay = Seconds)
		return;

	AutoSpawnDelay = Seconds;

	if (AutoSpawnDelay > 0.0f)
		SetEntityFlags(GetEntityFlags() | ZE_EF_TICKABLE);
	else
		SetEntityFlags(GetEntityFlags() & ~ZE_EF_TICKABLE);
}

float ZESpawner::GetAutoSpawnDelay() const
{
	return AutoSpawnDelay;
}

bool ZESpawner::AddChildEntity(ZEEntity* Entity)
{
	Spawns.LockWriteNested();
	{
		for (ZESize I = 0; I < Spawns.GetCount(); I++)
		{
			if (Spawns[I].Entity == Entity)
			{
				zeError("Cannot add entity. Entitiy is alread added as spawn. Parent Entity Name: \"%s\", Child Entity Name: \"%s\".", 
					GetName().ToCString(), Entity->GetName().ToCString());
				return false;
			}
		}

		ZESpawn NewSpawn;
		NewSpawn.Entity = Entity;
		NewSpawn.Visible = Entity->GetVisible();
		NewSpawn.Enabled = Entity->GetEnabled();
		NewSpawn.Spawned = false;

		if (ZEEntity::AddChildEntity(Entity))
		{
			NewSpawn.Position = Entity->GetWorldPosition();
			NewSpawn.Rotation = Entity->GetWorldRotation();
			NewSpawn.Scale = Entity->GetWorldScale();
			Spawns.Add(NewSpawn);
		}
		else
		{
			Spawns.UnlockWrite();
			return false;
		}
	}
	Spawns.UnlockWrite();

	return true;
}

void ZESpawner::RemoveChildEntity(ZEEntity* Entity)
{
	Spawns.LockWriteNested();
	{
		for (ZESize I = 0; I < Spawns.GetCount(); I++)
		{
			if (Spawns[I].Entity == Entity)
			{
				if (!Spawns[I].Spawned)
					ZEEntity::RemoveChildEntity(Entity);

				Spawns.Remove(I);
			}
		}
	}
	Spawns.LockWrite();
}

bool ZESpawner::IsSpawned()
{
	return Spawned;
}

void ZESpawner::SetAutoRespawn(bool Respawn)
{
	AutoRespawn = Respawn;
}

bool ZESpawner::GetAutoRespawn() const
{
	return AutoRespawn;
}

bool ZESpawner::Spawn()
{
	if (GetScene() == NULL)
		return false;

	if (!IsInitialized())
		return false;

	Spawns.LockWrite();
	{
		if (Spawned)
		{
			Spawns.UnlockWrite();
			return true;
		}

		for (ZESize I = 0; I < Spawns.GetCount(); I++)
		{			
			if (Spawns[I].Entity->GetParent() == this)
				ZEEntity::RemoveChildEntity(Spawns[I].Entity);

			Spawns[I].Entity->SetEnabled(Spawns[I].Enabled);
			Spawns[I].Entity->SetVisible(Spawns[I].Visible);

			if (GetParent() != NULL)
				GetParent()->ZEEntity::AddChildEntity(Spawns[I].Entity);
			else
				GetScene()->AddEntity(Spawns[I].Entity);

			Spawns[I].Entity->SetWorldPosition(Spawns[I].Position);
			Spawns[I].Entity->SetWorldRotation(Spawns[I].Rotation);
			Spawns[I].Entity->SetWorldScale(Spawns[I].Scale);
			Spawns[I].Spawned = true;
			Spawns[I].Entity->Reinitialize();
		}

		Spawned = true;
	}
	Spawns.UnlockWrite();

	return true;
}

void ZESpawner::Despawn()
{
	Spawns.LockWrite();
	{
		if (!Spawned)
		{
			Spawns.UnlockWrite();
			return;
		}

		SpawnTimer = 0.0f;
		Spawned = false;

		for (ZESize I = 0; I < Spawns.GetCount(); I++)
		{
			if (Spawns[I].Spawned && IsDestroyed())
				continue;

			if (Spawns[I].Entity->GetParent() != NULL)
				Spawns[I].Entity->GetParent()->ZEEntity::RemoveChildEntity(Spawns[I].Entity);
			else
				Spawns[I].Entity->GetScene()->RemoveEntity(Spawns[I].Entity);

			Spawns[I].Entity->Deinitialize();
			Spawns[I].Entity->SetPosition(Spawns[I].Position);
			Spawns[I].Entity->SetRotation(Spawns[I].Rotation);
			Spawns[I].Entity->SetScale(Spawns[I].Scale);	
			Spawns[I].Entity->SetEnabled(Spawns[I].Enabled);
			Spawns[I].Entity->SetVisible(Spawns[I].Visible);
			Spawns[I].Spawned = false;
		}
	}
	Spawns.UnlockWrite();

	if (AutoRespawn && AutoSpawnDelay <= 0.0f)
		Spawn();
}

bool ZESpawner::Respawn()
{
	Despawn();
	return Spawn();
}

void ZESpawner::Tick(float ElapsedTime)
{
	if (IsSpawned())
		return;

	SpawnTimer += ElapsedTime;
	if (SpawnTimer > AutoSpawnDelay)
		Spawn();
}

ZESpawner* ZESpawner::CreateInstance()
{
	return new ZESpawner();
}
