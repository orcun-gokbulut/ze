/*ZEHEADER_START*/
//////////////////////////////////////////////////////////////////////////////////////
//                                                                                  //
//  ZEMap.ms - Zinek Engine 3ds Max zeMap Exporter GUI Frontend v0.03 Source Code   //                              
// -------------------------------------------------------------------------------- //
//  Copyright (c) 2007-2008 Zinek Engine group.                                     //
//  All rights reserved.                                                            //
//                                                                                  //
//                 READ TERMS BELLOW BEFORE TAKING ANY ACTION !                     //
//                                                                                  //
//  These coded instructions, statements, and computer programs contain             //
//  unpublished proprietary information written by Zinek Engine group and they are  //
//  protected by international copyright laws. This file is intellectual property   //
//  of the Zinek Engine group and author(s). Storing this file, using this file,    //
//  compiling this file, publishing this file to anykind media or network, editing  //
//  this file, selling this file, reading this file is probited without notice and  //
//  written permision of Y. Orcun Gokbulut. These terms at above can only be        //
//  changed by Y. Orcun GOKBULUT.                                                   //
//  author(s).                                                                      //
//*                                                                                *//
//  DESCRIPTION         :                                                           //
//  AUTHOR(S)           : Y. Orcun GOKBULUT                                         //
//*                                                                                *//
//////////////////////////////////////////////////////////////////////////////////////
/*ZEHEADER_END*/

macroScript AddZEMapPortalParameters
	category:"ZEMap"
	toolTip:"Add ZEMapPortal parameters"
	icon:#("ZEToolbarIcons", 7)
(
	PortalParameters = attributes ZEMapPortalParameters
	(
		parameters ZEMapPortal rollout:ZEMapPortalRollout
		(
			ZEType type:#string default:"Portal" animatable:false
			PhysicalMeshEnabled type:#boolean ui:uiPhysicalMeshEnabled default:false animatable:false
			PhysicalMeshUseSelf type:#boolean ui:uiPhysicalMeshUseSelf default:true animation:false
			PhysicalMesh type:#node animatable:false
			GenerateOctree type:#boolean ui:uiGenerateOctree default:true animatable:false
			MaxOctreeDepth type:#integer ui:uiMaxOctreeDepth default:3 animatable:false
			--Doors type:#nodeTab tabSizeVariable:true animatable:false
		)
		
		rollout ZEMapPortalRollout "ZEMapPortal Parameters" rolledUp:false
		(
			--fn DoorFilter obj =  isProperty obj "ZEMapPortalDoor" == true and $ != obj
			fn PhysicalMeshFilter obj = (classof obj == Editable_mesh or classof obj == Editable_Poly) 

			checkbox uiPhysicalMeshEnabled "Enable Physical Mesh" 
			
			group "Physical Mesh"
			(
				checkbox uiPhysicalMeshUseSelf "Use self geometry" enabled:PhysicalMeshEnabled
				edittext uiPhysicalMesh "Physical Mesh" readOnly:true text:(if (PhysicalMesh != undefined) then PhysicalMesh.name else "") enabled:PhysicalMeshEnabled
				pickbutton uiPhysicalMeshSelect "Pick" filter:PhysicalMeshFilter enabled:(PhysicalMeshEnabled and not PhysicalMeshUseSelf) across:2
				button uiPhysicalMeshClean "Clean" enabled:(PhysicalMeshEnabled and not PhysicalMeshUseSelf)
			)
			
			group "Octree"
			(
				checkbox uiGenerateOctree "Generate Octree" 
				spinner uiMaxOctreeDepth "Max Depth" enabled:GenerateOctree
			)	
			
			/*group "Doors"
			(
				listbox uiPortalDoors "Portal Doors"
				pickbutton uiAddPortalDoor "Add" type:#node filter:DoorFilter across:2
				pickbutton uiRemovePortalDoor "Remove"
			)*/
			
			function ManageRollout =
			(
				uiPhysicalMeshUseSelf.enabled = PhysicalMeshEnabled
				uiPhysicalMesh.enabled = (PhysicalMeshEnabled and not PhysicalMeshUseSelf)
				uiPhysicalMeshSelect.enabled = (PhysicalMeshEnabled and not PhysicalMeshUseSelf)
				uiPhysicalMeshClean.enabled = (PhysicalMeshEnabled and not PhysicalMeshUseSelf)
				uiMaxOctreeDepth.enabled = GenerateOctree
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
			
			on uiPhysicalMeshEnabled changed arg do ManageRollout()
			
			on uiGenerateOctree changed Enabled do ManageRollout()
			
			on uiPhysicalMeshUseSelf changed Enabled do 
			(
				uiPhysicalMesh.text = ""
				PhysicalMesh = undefined
				ManageRollout()			
			)
			/*
			on uiAddPortalDoor picked obj do
			( 
				fn AllreadyAdded NewObject =
				(
					for o in Doors do
						if  o == NewObject then return true
					return false
				)
				
				if obj != undefined do
					if not AllreadyAdded obj then
					(
						append  Doors obj 
						uiPortalDoors.items = (for o in Doors collect o.name)
					)
			)	
			
			on uiRemovePortalDoor pressed do
			(
				if uiPortalDoors.selection != 0 then
				(
					deleteitem Doors uiPortalDoors.selection
					uiPortalDoors.items = (for o in Doors collect o.name)
				)
			)*/
		)
	)

	for obj in selection do
		if isProperty obj "PortalAttributes" == false and superclassof obj  == GeometryClass then 
			custAttributes.add obj PortalParameters
)

macroScript RemoveZEMapPortalParameters
	category:"ZEMap"
	toolTip:"Remove ZEMapPortal parameters"
	icon:#("ZEToolbarIcons", 9)
(
	for obj in selection do
		if obj != undefined then
			if isProperty obj "ZEMapPortalParameters" == true then
				for i = 1 to custAttributes.count obj  do
					if (custAttributes.getdef obj i).name == #ZEMapPortalParameters then custAttributes.delete obj i
)

macroScript AddZEMapPortalDoorParameters
	category:"ZEMap"
	toolTip:"Add ZEMapPortalDoor parameters"
	icon:#("ZEToolbarIcons", 8)
(
	PortalDoorParameters = attributes ZEMapPortalDoorParameters
	(
		parameters ZEMapPortalDoor rollout:ZEMapPortalDoorRollout
		(
			ZEType type:#string default:"PortalDoor" animatable:false
			PortalA type:#node animatable:false
			PortalB type:#node animatable:false
			IsOpen ui:uiIsOpen type:#boolean default:true animatable:false
		)
		
		rollout ZEMapPortalDoorRollout "ZEMapPortalDoor Parameters" rolledUp:false
		(
			fn PortalFilter obj =  isProperty obj "ZEMapPortalParameters" == true and $ != obj

			checkbox uiIsOpen "Portal door is open"
			group "Portal A"
			(
				edittext uiPortalA "Portal A" readOnly:true text:(if (PortalA != undefined) then PortalA.name else "") 
				pickbutton uiPortalASelect "Pick" filter:PortalFilter across:2
				button uiPortalAClean "Clean"
			)

			group "Portal B"
			(
				edittext uiPortalB "Portal B" readOnly:true text:(if (PortalB != undefined) then PortalB.name else "") 
				pickbutton uiPortalBSelect "Pick" filter:PortalFilter across:2
				button uiPortalBClean "Clean"
			)
			
			on uiPortalASelect picked obj do
			(
				if obj != undefined do
				(
					uiPortalA.text = obj.name
					PortalA = obj
				)
			)
			
			on uiPortalAClean pressed do
			(
				uiPortalA.text = ""
				PortalA = undefined
			)
			
			on uiPortalBSelect picked obj do
			(
				if obj != undefined do
				(
					uiPortalB.text = obj.name
					PortalB = obj
				)
			)
			
			on uiPortalBClean pressed do
			(
				uiPortalB.text = ""
				PortalB = undefined
			)		
		)
	)

	for obj in selection do
		if isProperty obj "PortalDoorParamters" == false and classof obj  == plane then 
			custAttributes.add obj PortalDoorParameters
)

macroScript RemoveZEMapPortalDoorParameters
	category:"ZEMap"
	toolTip:"Remove ZEMapPortalDoor paramters"
	icon:#("ZEToolbarIcons", 10)
(
	for obj in selection do
		if obj != undefined then
			if isProperty obj "ZEMapPortalDoorParameters" == true then
				for i = 1 to custAttributes.count obj  do
					if (custAttributes.getdef obj i).name == #ZEMapPortalDoorParameters then custAttributes.delete obj i
)