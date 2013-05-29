//ZE_SOURCE_PROCESSOR_START(License, 1.0)
/*******************************************************************************
 Zinek Engine - ZEFoliage.h
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

#ifndef ZE_FOLIAGE_H_
#define ZE_FOLIAGE_H_

#include "ZEEntity.h"
#include "ZEDS\ZEArray.h"

class HaltonSequence
{
private:
	double Value;
	double InverseBase;

public:
	double GetNext()
	{
		double Radius = 1.0 - Value - 0.0000000001;
		if (InverseBase < Radius) 
		{
			Value += InverseBase;
		}
		else
		{
			double h = InverseBase, hh;
			do
			{
				hh = h;
				h *= InverseBase;

			} while ( h >= Radius );

			Value += hh + h - 1.0;
		}

		return Value;
	}

	HaltonSequence(double Base)
	{
		Value = 0;
		InverseBase = 1.0f / Base;
	}

	~HaltonSequence()
	{

	}
};

class ZEModel;
class ZEModelResource;

ZE_META_ENTITY_DESCRIPTION(ZEFoliage);

class ZEFoliage : public ZEEntity
{
	ZE_META_ENTITY(ZEFoliage)

	private:
		float						BrushDensity;
		ZEVector2					BrushSize;
		float						MinScale;
		float						MaxScale;

		ZEModel*					Ground;

// 		ZEModelResource*			Tree1;
// 		ZEModelResource*			Tree2;
// 		ZEModelResource*			Tree3;
// 		ZEModelResource*			Tree4;

		ZEArray<ZEModelResource*>	Trees;


	protected:
		virtual bool				InitializeSelf();
		virtual bool				DeinitializeSelf();

									ZEFoliage();
		virtual						~ZEFoliage();

	public:

		void						SetSurfaceEntityName(const char* EntityName);
		const char*					GetSurfaceEntityName() const;

		void						SetBrushSize(const ZEVector2& Size);
		ZEVector2					GetBrushSize() const;

		void						SetBrushDensity(float Density);
		float						GetBrushDensity() const;

		void						SetMinScale(float MinScale);
		float						GetMinScale() const;

		void						SetMaxScale(float MaxScale);
		float						GetMaxScale() const;

		void						SetGround(ZEModel* Ground);
		ZEModel*					GetGround() const;

		virtual void				Tile();
		virtual void				Realign();

		virtual void				SetPosition(const ZEVector3& NewPosition);

/*		virtual void				Draw(ZEDrawParameters* DrawParameters);*/

		static ZEFoliage*			CreateInstance();

};

/*
ZE_POST_PROCESSOR_START(Meta)
<zinek>
	<meta> 
		<class name="ZEFoliage" parent="ZEEntity" description="Foliage">		
			<description>Tiles Trees</description>
			<property name="SurfaceEntityName" type="string" setfunction="SetSurfaceEntityName" getfunction="GetSurfaceEntityName" description="Surface Entities Name"/>	
			<property name="MinScale" type="float" autogetset="true" description="..."/>
			<property name="MaxScale" type="float" autogetset="true" description="..."/>
			<property name="BrushDensity" type="float" autogetset="true" description="..."/>
			<property name="BrushSize" type="ZEVector2" autogetset="true" description="..."/>
		</class>
	</meta>
</zinek>
ZE_POST_PROCESSOR_END()
*/

#endif
