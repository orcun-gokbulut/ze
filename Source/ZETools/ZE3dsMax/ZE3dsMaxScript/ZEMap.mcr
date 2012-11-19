/*ZEHEADER_START*/
//////////////////////////////////////////////////////////////////////////////////////////////////
//																								//
//	ZEInterior.ms - Zinek Engine 3ds Max ZEInterior Exporter GUI Frontend v0.03 Source Code		//                              
//		-------------------------------------------------------------------------------			//
//		Copyright (c) 2007-2008 Zinek Engine group.												//
//		All rights reserved.																	//
//																								//
//		               READ TERMS BELLOW BEFORE TAKING ANY ACTION !								//
//																								//
//		These coded instructions, statements, and computer programs contain						//
//		unpublished proprietary information written by Zinek Engine group and they are			//
//		protected by international copyright laws. This file is intellectual property			//
//		of the Zinek Engine group and author(s). Storing this file, using this file,			//
//		compiling this file, publishing this file to anykind media or network, editing			//
//		this file, selling this file, reading this file is probited without notice and			//
//		written permision of Y. Orcun Gokbulut. These terms at above can only be				//
//		changed by Y. Orcun GOKBULUT.															//
//		author(s).																				//
//*																							   *//
//		DESCRIPTION         :																	//
//		AUTHOR(S)           : Y. Orcun GOKBULUT													//
//*																							   *//
//////////////////////////////////////////////////////////////////////////////////////////////////
/*ZEHEADER_END*/

macroScript AddZEInteriorRoomParameters
	category:"ZEInterior"
	toolTip:"Add ZEInteriorRoom Parameters"
	icon:#("ZEToolbarIcons", 7)
(
	RoomAttributes = attributes ZEInteriorRoomAttributes
	(
		parameters ZEInteriorRoom rollout:ZEInteriorRoomRollout
		(
			ZEType 					type:#string 		default:"Room" 										animatable:false
			PhysicalMeshExists 		type:#boolean 		default:false		ui:uiPhysicalMeshExists  		animatable:false
			PhysicalMeshEnabled		type:#boolean		default:true		ui:uiPhysicalMeshEnabled		animatable:false
			PhysicalMeshUseSelf 	type:#boolean  		default:true 		ui:uiPhysicalMeshUseSelf		animatable:false
			PhysicalMesh 			type:#node 																animatable:false
		)
		
		rollout ZEInteriorRoomRollout "ZEInteriorRoom Parameters" rolledUp:false
		(
			/* PICK FILTERS */
			fn PhysicalMeshFilter obj = (classof obj == Editable_mesh or classof obj == Editable_Poly) and obj != $

			/* LAYOUT DEFINITION */
			bitmap		uiLogo					fileName:"zineklogo.bmp"
			checkbox 	uiPhysicalMeshExists 	"Export Physical Mesh" 
			
			group "Physical Mesh"
			(
				checkbox	uiPhysicalMeshEnabled	"Physical Mesh Enabled"																										enabled:PhysicalMeshExists
				checkbox 	uiPhysicalMeshUseSelf 	"Use self geometry" 																										enabled:PhysicalMeshExists
				edittext 	uiPhysicalMesh 			"Physical Mesh" 		readOnly:true 				text:(if (PhysicalMesh != undefined) then PhysicalMesh.name else "") 	enabled:PhysicalMeshExists
				pickbutton 	uiPhysicalMeshSelect 	"Pick" 					filter:PhysicalMeshFilter 	across:2																enabled:(PhysicalMeshExists and not PhysicalMeshUseSelf) 
				button 		uiPhysicalMeshClean 	"Clean" 																													enabled:(PhysicalMeshExists and not PhysicalMeshUseSelf)
			)
			
			function ManageRollout =
			(
				uiPhysicalMeshEnabled.enabled 	= PhysicalMeshExists
				uiPhysicalMeshUseSelf.enabled 	= PhysicalMeshExists
				uiPhysicalMesh.enabled 			= (PhysicalMeshExists and not PhysicalMeshUseSelf)
				uiPhysicalMeshSelect.enabled 	= (PhysicalMeshExists and not PhysicalMeshUseSelf)
				uiPhysicalMeshClean.enabled 	= (PhysicalMeshExists and not PhysicalMeshUseSelf)
			)
			
			on uiPhysicalMeshSelect picked obj do
			(
				if obj != undefined do
				(
					uiPhysicalMesh.text = obj.name
					PhysicalMesh = obj
				)
			)
			
			on uiPhysicalMeshClean pressed do
			(
				uiPhysicalMesh.text = ""
				PhysicalMesh = undefined
			)
			
			on uiPhysicalMeshExists changed arg do ManageRollout()
			
			on uiPhysicalMeshEnabled changed arg do ManageRollout()
					
			on uiPhysicalMeshUseSelf changed Enabled do 
			(
				uiPhysicalMesh.text = ""
				PhysicalMesh = undefined
				ManageRollout()			
			)
		)
	)
	
	/* ADDING ATTRIBUTES */
	for obj in selection do
	(
		if obj != undefined then
		(
			if (isProperty obj "ZEInteriorRoomAttributes" == false and superclassof obj == GeometryClass) do
			(
				custAttributes.add obj RoomAttributes
			)
		)

	)
)

macroScript RemoveZEInteriorRoomParameters
	category:"ZEInterior"
	toolTip:"Remove ZEInteriorRoom Parameters"
	icon:#("ZEToolbarIcons", 9)
(
	for obj in selection do
	(
		if obj != undefined then
		(
			if isProperty obj "ZEInteriorRoomAttributes" == true then
			(
				for i = 1 to custAttributes.count obj  do
				(
					if (custAttributes.getdef obj i).name == #ZEInteriorRoomAttributes then custAttributes.delete obj i
				)
			)
		)
	)
)

macroScript AddZEInteriorDoorParameters
	category:"ZEInterior"
	toolTip:"Add ZEInteriorDoor Parameters"
	icon:#("ZEToolbarIcons", 8)
(
	DoorAttributes = attributes ZEInteriorDoorAttributes
	(
		parameters ZEInteriorDoor rollout:ZEInteriorDoorRollout
		(
			ZEType 	type:#string 	default:"Door" 					animatable:false
			RoomA 	type:#node 										animatable:false
			RoomB 	type:#node 										animatable:false
			IsOpen 	type:#boolean 	default:true 	ui:uiIsOpen  	animatable:false
		)
		
		rollout ZEInteriorDoorRollout "ZEInteriorDoor Parameters" rolledUp:false
		(
			/* PICK FILTERS */
			fn RoomFilter obj =  isProperty obj "ZEInteriorRoomAttributes" == true and $ != obj

			/* LAYOUT DEFINITION */
			bitmap		uiLogo		fileName:"zineklogo.bmp"
			checkbox 	uiIsOpen 	"Is Open"	toolTip:"Indicates whether the door is open or not."
			
			group "Room A"
			(
				edittext 		uiRoomA 		"Room :" 	readOnly:true text:(if (RoomA != undefined) then RoomA.name else "") 
				pickbutton 		uiRoomASelect 	"Pick" 		filter:RoomFilter across:2
				button 			uiRoomAClean 	"Clean"
			)

			group "Room B"
			(
				edittext 		uiRoomB 		"Room :" 	readOnly:true text:(if (RoomB != undefined) then RoomB.name else "") 
				pickbutton 		uiRoomBSelect 	"Pick" 		filter:RoomFilter across:2
				button 			uiRoomBClean 	"Clean"
			)
			
			on uiRoomASelect picked obj do
			(
				if obj != undefined do
				(
					uiRoomA.text = obj.name
					RoomA = obj
				)
			)
			
			on uiRoomAClean pressed do
			(
				uiRoomA.text = ""
				RoomA = undefined
			)
			
			on uiRoomBSelect picked obj do
			(
				if obj != undefined do
				(
					uiRoomB.text = obj.name
					RoomB = obj
				)
			)
			
			on uiRoomBClean pressed do
			(
				uiRoomB.text = ""
				RoomB = undefined
			)		
		)
	)
	
	/* ADDING ATTRIBUTES */
	for obj in selection do
	(
		if obj != undefined then
		(
			if isProperty obj "ZEInteriorDoorAttributes" == false and classof obj == plane do
			(
				custAttributes.add obj DoorAttributes	
			)
		)
	)
)

macroScript RemoveZEInteriorDoorParameters
	category:"ZEInterior"
	toolTip:"Remove ZEInteriorDoor Parameters"
	icon:#("ZEToolbarIcons", 10)
(
	for obj in selection do
	(
		if obj != undefined then
		(
			if isProperty obj "ZEInteriorDoorAttributes" == true then
			(
				for i = 1 to custAttributes.count obj  do
				(
					if (custAttributes.getdef obj i).name == #ZEInteriorDoorAttributes then custAttributes.delete obj i
				)
			)
		)
	)
)

macroScript RemoveZEPortalMapParameters
	category:"ZEInterior"
	toolTip:"Remove All Old ZEPortalMap Parameters"
(
	for obj in selection do
	(
		if obj != undefined then
		(
			if isProperty obj "ZEMapPortalParameters" == true then
			(
				for i = 1 to custAttributes.count obj  do
				(
					if (custAttributes.getdef obj i).name == #ZEMapPortalParameters then custAttributes.delete obj i
				)
			)
			if isProperty obj "ZEMapPortalDoorParameters" == true then
			(
				for i = 1 to custAttributes.count obj  do
				(
					if (custAttributes.getdef obj i).name == #ZEMapPortalDoorParameters then custAttributes.delete obj i
				)
			)
		)
	)
)

