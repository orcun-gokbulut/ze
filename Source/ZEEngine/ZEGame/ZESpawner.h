//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZESpawner.h
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

#pragma once

#include "ZEEntity.h"

struct ZESpawn
{
	ZEEntity*							Entity;
	ZEVector3							Postion;
	ZEQuaternion						Rotation;
	ZEVector3							Scale;
	bool								Enabled;
	bool								Visible;
};

class ZESpawner : public ZEEntity
{
	ZE_OBJECT
	private:
		ZEArray<ZESpawn>				Spawns;

		bool							AutoSpawn;
		bool							AutoRespawn;
		float							AutoSpawnDelay;
		bool							Spawned;

		float							SpawnTimer;

		void							UpdateSpawns(ZESize Index);

		virtual ZEEntityResult			InitializeInternal();
		virtual ZEEntityResult			DeinitializeInternal();

		virtual ZEEntityResult			LoadInternal();
		virtual ZEEntityResult			UnloadInternal();

										ZESpawner();
		virtual							~ZESpawner();

	public:
		const ZEArray<ZESpawn>&			GetSpawns() const;

		void							SetAutoSpawn(bool AutoSpawn);
		bool							GetAutoSpawn() const;

		void							SetAutoRespawn(bool Respawn);
		bool							GetAutoRespawn() const;

		void							SetAutoSpawnDelay(float Seconds);
		float							GetAutoSpawnDelay() const;

		virtual bool					AddChildEntity(ZEEntity* Entity) override;
		virtual void					RemoveChildEntity(ZEEntity* Entity) override;

		bool							IsSpawned();

		bool							Spawn();
		void							Despawn();
		bool							Respawn();

		virtual void					Tick(float ElapsedTime);

		static ZESpawner*				CreateInstance();
};
