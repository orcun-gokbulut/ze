//////////////////////////////////////////////////////////////////////////////////////
//                                                                      	        //
//  ZEModelGUI.ms - Zinek Engine 3ds Max zeModel Exporter GUI Frontend              //
//                  v0.03 Source Code                                               //                              
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

macroScript ZEModelMeshAttributeAdd
	category:"ZEModel"
	toolTip:"Add ZEModelMesh parameters"
	icon:#("ZEToolbarIcons", 1)
(
	MeshAttributes = attributes ZEMeshAttributes
	(
		parameters ZEMesh rollout:ZEMeshRollout
		(
			ZEType type:#string default:"Mesh" animatable:false
			Lod type:#integer ui:uiLod default:0 animatable:false
			PhysicalEnabled type:#boolean ui:uiPhysicalEnabled default:false animatable:false
			PhysicalBodyType type:#integer ui:uiPhysicalBodyType default:1 animatable:false
			PhysicalShapes type:#nodeTab tabSizeVariable:true animatable:false
		)
		
		rollout ZEMeshRollout "ZEMesh Parameters" rolledUp:false
		(
			fn shape_filter obj =  isProperty obj "ZEPhysicalBodyShapeAttributes" == true and obj != $

			spinner uiLod "Mesh's Lod:" type:#integer
			checkbox uiPhysicalEnabled "Enable Physical Body"
			
			group "Physical Properties"
			(
				dropdownlist uiPhysicalBodyType "Type :" items:#("Rigid Body", "Deformable", "Cloth")  enabled:(PhysicalEnabled and Lod == 0)
				listbox uiPhysicalShapes "Custom Shapes:" items:(for o in PhysicalShapes collect o.name) enabled:(PhysicalEnabled and Lod == 0)
				pickbutton uiAddPhysicalShape "Add" type:#node filter:shape_filter across:2 enabled:(PhysicalEnabled and Lod == 0)
				button uiRemovePhysicalShape "Remove" enabled:(PhysicalEnabled and Lod == 0)
			)	
	
			
			function ManageRollout =
			(
				uiPhysicalEnabled.enabled =  Lod == 0
				uiPhysicalBodyType.enabled = PhysicalEnabled and Lod == 0
				uiPhysicalShapes.enabled = PhysicalEnabled and Lod == 0
				uiAddPhysicalShape.enabled = PhysicalEnabled and Lod == 0
				uiRemovePhysicalShape.enabled = PhysicalEnabled and Lod == 0
			)
			
			on uiLod changed arg do ManageRollout()
			
			on uiPhysicalEnabled changed Enabled do ManageRollout()
			
			on uiAddPhysicalShape picked obj do
			( 
				fn AllreadyAdded NewObject =
				(
					for o in PhysicalShapes do
					(
						if  o == NewObject then return true
					)
					return false
				)
				
				if obj != undefined do
				(
					if not AllreadyAdded obj then
					(
						append  PhysicalShapes obj 
						uiPhysicalShapes.items = (for o in PhysicalShapes collect o.name)
					)
				)
			)	
			
			on uiRemovePhysicalShape pressed do
			(
				if uiPhysicalShapes.selection != 0 then
				(
					deleteitem PhysicalShapes uiPhysicalShapes.selection
					uiPhysicalShapes.items = (for o in PhysicalShapes collect o.name)
				)
			)
			
			on uiPhysicalShapeMaterialEnabled changed arg do ManageRollout();

		)
	)
	
	if $ != undefined then
	(	
		if isProperty $ "ZEMeshAttributes" == false and superclassof $  == GeometryClass then custAttributes.add $ MeshAttributes
	)
)

macroScript ZEModelMeshAttributeDelete
	category:"ZEModel"
	toolTip:"Remove ZEModelMesh parameters"
	icon:#("ZEToolbarIcons", 4)
(
	if $ != undefined then
	(
		if isProperty $ "ZEMeshAttributes" == true then
		(
			for i = 1 to custAttributes.count $  do
			(
				print (custAttributes.getdef $ i).name
				if (custAttributes.getdef $ i).name == #ZEMeshAttributes then
				(
					custAttributes.delete $ i
				)
			)
		)
	)
)

macroScript ZEModelBoneAttributeAdd
	category:"ZEModel"
	toolTip:"Add ZEModel Bone parameters"
	icon:#("ZEToolbarIcons", 2)
(
	BoneAttributes = attributes ZEBoneAttributes
	(
		parameters ZEBone rollout:ZEBoneRollout
		(
			ZEType type:#string default:"Bone" animatable:false
			PhysicalEnabled type:#boolean ui:uiPhysicalEnabled default:false animatable:false
			PhysicalBodyType type:#integer ui:uiPhysicalBodyType default:1 animatable:false
			PhysicalShapes type:#nodeTab tabSizeVariable:true animatable:false
		)
		
		rollout ZEBoneRollout "ZEBone Parameters" rolledUp:false
		(
			fn shape_filter obj =  isProperty obj "ZEPhysicalBodyShapeAttributes" == true and obj != $

			checkbox uiPhysicalEnabled "Enable Physical Body"
			
			group "Physical Properties"
			(
				dropdownlist uiPhysicalBodyType "Type :" items:#("Rigid Body", "Deformable", "Cloth")  enabled:(PhysicalEnabled)
				listbox uiPhysicalShapes "Custom Shapes:" items:(for o in PhysicalShapes collect o.name) enabled:(PhysicalEnabled)
				pickbutton uiAddPhysicalShape "Add" type:#node filter:shape_filter across:2 enabled:(PhysicalEnabled)
				button uiRemovePhysicalShape "Remove" enabled:(PhysicalEnabled)
			)	
			
			function ManageRollout =
			(
				uiPhysicalBodyType.enabled = PhysicalEnabled 
				uiPhysicalShapes.enabled = PhysicalEnabled
				uiAddPhysicalShape.enabled = PhysicalEnabled
				uiRemovePhysicalShape.enabled = PhysicalEnabled
			)
			
			on uiPhysicalShapeMaterialEnabled changed arg do ManageRollout();
			
			on uiPhysicalEnabled changed Enabled do ManageRollout()
			
			on uiAddPhysicalBodyShape picked obj do
			( 
				fn AllreadyAdded NewObject =
				(
					for o in PhysicalShapes do
					(
						if  o == NewObject then return true
					)
					return false
				)
				
				if obj != undefined do
				(
					if not AllreadyAdded obj then
					(
						append  PhysicalShapes obj 
						uiPhysicalShapes.items = (for o in PhysicalShapes collect o.name)
					)
				)
			)	
			
			on uiRemovePhysicalBodyShape pressed do
			(
				if uiPhysicalShapes.selection != 0 then
				(
					deleteitem PhysicalShapes uiPhysicalShapes.selection
					uiPhysicalShapes.items = (for o in PhysicalShapes collect o.name)
				)
			)
		)
	)
	
	if $ != undefined then
	(	
		if isProperty $ "ZEBoneAttributes" == false and classof $  == BoneGeometry then custAttributes.add $ BoneAttributes
	)
)

macroScript ZEModelBoneAttributeDelete
	category:"ZEModel"
	toolTip:"Remove ZEModelBone parameters"
	icon:#("ZEToolbarIcons", 5)
(
	if $ != undefined then
	(
		if isProperty $ "ZEBoneAttributes" == true then
		(
			for i = 1 to custAttributes.count $  do
			(
				print (custAttributes.getdef $ i).name
				if (custAttributes.getdef $ i).name == #ZEBoneAttributes then
				(
					custAttributes.delete $ i
				)
			)
		)
	)
)

macroScript ZEPhysicalBodyShapeAttributeAdd
	category:"ZEModel"
	toolTip:"Add ZEModelPhysicalMesh parameters"
	icon:#("ZEToolbarIcons", 3)
(
	PhysicalBodyShapeAttributes = attributes ZEPhysicalBodyShapeAttributes
	(
		parameters ZEPhysicalBodyShape rollout:ZEPhysicalBodyShapeRollout
		(
			ZEType type:#string default:"PhysicalShape" animatable:false
			PhysicalShapeMass type:#float ui:uiPhysicalShapeMass default:1 animatable:false
			PhysicalShapeMaterialEnabled type:#boolean ui:uiPhysicalShapeMaterialEnabled default:false animatable:false
			PhysicalShapeMaterialRestitution ui:uiPhysicalShapeMaterialRestitution type:#float animatable:false
			PhysicalShapeMaterialDynamicFriction ui:uiPhysicalShapeMaterialDynamicFriction type:#float animatable:false
			PhysicalShapeMaterialStaticFriction ui:uiPhysicalShapeMaterialStaticFriction type:#float animatable:false
		)

		rollout ZEPhysicalBodyShapeRollout "ZEPhysicalBodyShape Pameters" rolledUp:false
		(
			spinner uiPhysicalShapeMass "Mass:" type:#float
			group "Material"
			(
				checkbox uiPhysicalShapeMaterialEnabled "Enable Material" type:#boolean
				spinner uiPhysicalShapeMaterialRestitution "Restitution" type:#float enabled:PhysicalShapeMaterialEnabled
				spinner uiPhysicalShapeMaterialDynamicFriction "Dynamic Friction" type:#float enabled:PhysicalShapeMaterialEnabled
				spinner uiPhysicalShapeMaterialStaticFriction "Static Friction" type:#float  enabled:PhysicalShapeMaterialEnabled
			)
			
			on uiPhysicalShapeMaterialEnabled changed param do
			(
				uiPhysicalShapeMaterialRestitution.enabled = PhysicalShapeMaterialEnabled
				uiPhysicalShapeMaterialDynamicFriction.enabled = PhysicalShapeMaterialEnabled
				uiPhysicalShapeMaterialStaticFriction.enabled = PhysicalShapeMaterialEnabled
			)
		)
	)
	
	if $ != undefined then
	(	
		fn shape_filter obj = (classof obj == Sphere or classof obj == Box or classof obj == Capsule or classof obj == Cylinder or classof obj == Editable_mesh or classof obj == Editable_Poly) 
		if isProperty $ "ZEPhysicalBodyShapeAttributes" == false and shape_filter $ == true then custAttributes.add $ PhysicalBodyShapeAttributes
	)
)

macroScript ZEPhysicalBodyShapeAttributeRemove
	category:"ZEModel"
	toolTip:"Remove ZEModelPhysicalMesh"
	icon:#("ZEToolbarIcons", 6)
(
	if $ != undefined then	
	(
		if isProperty $ "ZEPhysicalBodyShapeAttributes" == true then 
		(
			for i = 1 to custAttributes.count $  do
			(
				
				if (custAttributes.getdef $ i).name == #ZEPhysicalBodyShapeAttributes then	
				(
					custAttributes.delete $ i
				)
			)
		)
	)
)