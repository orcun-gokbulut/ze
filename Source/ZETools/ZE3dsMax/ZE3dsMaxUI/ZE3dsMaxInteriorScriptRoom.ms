
RoomAttributes = attributes ZEInteriorRoomAttributes
(
	parameters ZEInteriorRoom rollout:ZEInteriorRoomRollout
	(
		ZEType 					type:#string 		default:"Room" 										animatable:false
		OctreeEnabled			type:#boolean		default:false		ui:uiGenerateOctree				animatable:false
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
		checkbox	uiGenerateOctree		"Generate Octree"
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
					if isProperty Current "ZEInteriorRoomAttributes" do
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
		if selection.count == 1 AND isProperty $ "ZEInteriorRoomAttributes" do
			UpdateZECoordinates $
	)
	
	/* ADDING ATTRIBUTES */

	ArgFilterValue = 5
	
	CheckResult = CheckZEAttributes selection ArgFilterValue

	if CheckResult == false then
	(
		for Current in selection do
		(
			if Current != undefined then
			(
				if isProperty Current "ZEInteriorRoomAttributes" == false and superclassof Current == GeometryClass do
				(
					custAttributes.add Current RoomAttributes
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


