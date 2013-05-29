
MeshAttributes = attributes ZEMeshAttributes
(
	parameters ZEMesh rollout:ZEMeshRollout
	(
		ZEType 					type:#string 				default:"Mesh" 												animatable:false
		Mesh_LOD				type:#integer 				default:0					ui:uiMesh_LOD					animatable:false
		Mesh_Visibility			type:#boolean				default:true				ui:uiMesh_Visibility			animatable:false
	)
	
	parameters ZEPhysicalBodyParameters rollout:ZEPhysicalBodyRollout
	(
		PhysicalBody_Type 						type:#integer					default:1				 			ui:uiPhysicalBody_Type  						animatable:false
		PhysicalBody_Enabled					type:#boolean					default:false						ui:uiPhysicalBody_Enabled						animatable:false
		PhysicalBody_Kinematic   				type:#boolean					default:false						ui:uiPhysicalBody_Kinematic						animatable:false
		PhysicalBody_Mass 						type:#float						default:0							ui:uiPhysicalBody_Mass							animatable:false
		PhysicalBody_CenterOfMass   			type:#node															ui:uiPhysicalBody_CenterOfMass					animatable:false
		PhysicalBody_LinearDamping  			type:#float						default:0							ui:uiPhysicalBody_LinearDamping					animatable:false
		PhysicalBody_AngularDamping				type:#float						default:0							ui:uiPhysicalBody_AngularDamping				animatable:false
		PhysicalBody_Shapes 					type:#nodeTab 					tabSizeVariable:true 																animatable:false
	)
	
	rollout ZEMeshRollout "ZE Mesh Parameters" rolledUp:false
	(
		/* LAYOUT DEFINITION */
		bitmap					uiLogo													fileName:"zineklogo.bmp"
		spinner 				uiMesh_LOD			"Mesh's Lod:" 						type:#integer
		checkbox 				uiMesh_Visibility 	"Is Visible"						toolTip:"Indicates whether the mesh is visible or not."
	)
	
	rollout ZEPhysicalBodyRollout "ZE Physical Body Parameters" rolledUp:true
	(
		/* PICK FILTERS */
		fn Shape_Filter obj =  isProperty obj "ZEPhysicalShapeAttributes" == true and obj != $
		fn PointHelper_Filter obj = (classof obj) == Point and (superclassof obj) == helper
		
		/* LAYOUT DEFINITION */
		dropdownlist 			uiPhysicalBody_Type 						"Type:" 						items:#("None", "Rigid Body", "Deformable", "Cloth") 								enabled:(Mesh_LOD == 0)
		checkbox				uiPhysicalBody_Enabled						"Enabled"																											enabled:(PhysicalBody_Type != 1 and Mesh_LOD == 0)
		checkbox 				uiPhysicalBody_Kinematic 					"Kinematic Body"				align:#left	tooltip:"Indicates whether the body is Kinematic or Dynamic actor."		enabled:(PhysicalBody_Type != 1 and Mesh_LOD == 0)
		label					uiPhysicalBody_CenterOfMass_Label			"Center Of Mass:"				align:#left																			enabled:(PhysicalBody_Type != 1 and Mesh_LOD == 0)
		pickbutton				uiPhysicalBody_CenterOfMass					"Pick"							type:#node  across:2 filter:PointHelper_Filter autoDisplay:true						enabled:(PhysicalBody_Type != 1 and Mesh_LOD == 0)
		button					uiPhysicalBody_CenterOfMass_Clear			"Clear"																												enabled:(PhysicalBody_Type != 1 and Mesh_LOD == 0)
		spinner 				uiPhysicalBody_Mass 						"Mass:"							type:#float																			enabled:(PhysicalBody_Type != 1 and Mesh_LOD == 0)
		spinner 				uiPhysicalBody_LinearDamping				"Linear Damp.:"					type:#float																			enabled:(PhysicalBody_Type != 1 and Mesh_LOD == 0)
		spinner 				uiPhysicalBody_AngularDamping				"Angular Damp.:"				type:#float																			enabled:(PhysicalBody_Type != 1 and Mesh_LOD == 0)
		group "Body Shapes"
		(
			listbox 			uiPhysicalBody_Shapes 						"Shapes:" 					items:(for o in PhysicalBody_Shapes collect o.name)										enabled:(PhysicalBody_Type != 1 and Mesh_LOD == 0)
			pickbutton 			uiPhysicalBody_Shape_Add					"Add" 						type:#node filter:Shape_Filter across:2													enabled:(PhysicalBody_Type != 1 and Mesh_LOD == 0)
			button 				uiPhysicalBody_Shape_Remove 				"Remove" 																											enabled:(PhysicalBody_Type != 1 and Mesh_LOD == 0)
		)		
		
		on uiPhysicalBody_CenterOFMass_Clear pressed do PhysicalBody_CenterOfMass = undefined
			
		/* PHYSICAL BODY SHAPES ADD*/
		on uiPhysicalBody_Shape_Add picked obj do
		( 
			fn AllreadyAdded NewObject =
			(
				for o in PhysicalBody_Shapes do
				(
					if  o == NewObject then return true
				)
				return false
			)
			
			if obj != undefined do
			(
				if not AllreadyAdded obj then
				(
					append  PhysicalBody_Shapes obj 
					uiPhysicalBody_Shapes.items = (for o in PhysicalBody_Shapes collect o.name)
				)
			)
		)	
		

		/* PHYSICAL BODY SHAPES REMOVE*/
		on uiPhysicalBody_Shape_Remove pressed do
		(
			if uiPhysicalBody_Shapes.selection != 0 then
			(
				deleteitem PhysicalBody_Shapes uiPhysicalBody_Shapes.selection
				uiPhysicalBody_Shapes.items = (for o in PhysicalBody_Shapes collect o.name)
			)
		)
				
		fn ManageLayout =
		(
			uiPhysicalBody_Enabled.enabled  					= PhysicalBody_Type != 1 and Mesh_LOD == 0	
			uiPhysicalBody_Kinematic.enabled  			 		= PhysicalBody_Type != 1 and Mesh_LOD == 0
			uiPhysicalBody_Mass.enabled  			 			= PhysicalBody_Type != 1 and Mesh_LOD == 0
			uiPhysicalBody_CenterOfMass_Label.enabled 			= PhysicalBody_Type != 1 and Mesh_LOD == 0
			uiPhysicalBody_CenterOfMass.enabled  				= PhysicalBody_Type != 1 and Mesh_LOD == 0
			uiPhysicalBody_CenterOfMass_Clear.enabled 			= PhysicalBody_Type != 1 and Mesh_LOD == 0
			uiPhysicalBody_LinearDamping.enabled  				= PhysicalBody_Type != 1 and Mesh_LOD == 0
			uiPhysicalBody_AngularDamping.enabled 				= PhysicalBody_Type != 1 and Mesh_LOD == 0	
			uiPhysicalBody_Shapes.enabled  						= PhysicalBody_Type != 1 and Mesh_LOD == 0
			uiPhysicalBody_Shape_Add.enabled 					= PhysicalBody_Type != 1 and Mesh_LOD == 0								
			uiPhysicalBody_Shape_Remove.enabled 				= PhysicalBody_Type != 1 and Mesh_LOD == 0	
		)	
		
		on uiPhysicalBody_Type selected arg do ManageLayout()

		on ZEPhysicalBodyRollout open do
		(
			ZEPhysicalBodyRollout.open = PhysicalBody_Type != 1
		)
	)
	
	/*ZE COORDINATES ROLLOUT DEFINITION*/
	
	rollout ZECoordinatesRollout "ZE Coordinates" rolledUp:true
	(
		group "Position"
		(
			edittext uitxtPositionXValue "X:" align: #left readOnly: true
			edittext uitxtPositionYValue "Y:" align: #left readOnly: true
			edittext uitxtPositionZValue "Z:" align: #left readOnly: true
		)
	
		group "Rotation"
		(
			edittext uitxtRotationXValue "X:" align: #left readOnly: true
			edittext uitxtRotationYValue "Y:" align: #left readOnly: true
			edittext uitxtRotationZValue "Z:" align: #left readOnly: true
		)
			
		group "Scale"
		(
			edittext uitxtScaleXValue "X:" align: #left readOnly: true
			edittext uitxtScaleYValue "Y:" align: #left readOnly: true
			edittext uitxtScaleZValue "Z:" align: #left readOnly: true
		)
		
		on ZECoordinatesRollout open do
		(
			for Current in selection do
			(
				if Current != undefined do
				(
					if isProperty Current "ZEMeshAttributes" do
					(
						UpdateZECoordinates Current
					)
				)
			)
		)
	)
	
)

if selection.count > 0 then
(
	when transform selection changes do
	(
		if selection.count == 1 AND isProperty $ "ZEMeshAttributes" do
			UpdateZECoordinates $
	)
	
	/* ADDING ATTRIBUTES */

	ArgFilterValue = 2
	
	CheckResult = CheckZEAttributes selection ArgFilterValue

	if CheckResult == false then
	(
		for Current in selection do
		(
			if Current != undefined then
			(	
				if isProperty Current "ZEMeshAttributes" == false and superclassof Current == GeometryClass do 
				(
					custAttributes.add Current MeshAttributes
					UpdateZECoordinates Current
				)
			)
		)
	)
	else
	(
		messagebox "Current selection already has Zinek Engine Attributes. Operation aborted."	
	)
	
)
else
(
	messagebox "No selection to add Zinek Engine Attributes. Operation aborted."
)




