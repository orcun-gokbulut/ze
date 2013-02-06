
PhysicalShapeAttributes = attributes ZEPhysicalShapeAttributes
(
	parameters ZEPhysicalShapeParameters rollout:ZEPhysicalShapeRollout
	(
		ZEType											type:#string 					default:"PhysicalShape" 																					animatable:false
		PhysicalShape_MaterialRestitution 				type:#float						default:1.0								ui:uiPhysicalShape_MaterialRestitution 					animatable:false
		PhysicalShape_MaterialDynamicFriction	 		type:#float						default:1.0								ui:uiPhysicalShape_MaterialDynamicFriction 			animatable:false
		PhysicalShape_MaterialStaticFriction 	 		type:#float						default:1.0								ui:uiPhysicalShape_MaterialStaticFriction 				animatable:false
		PhysicalShape_GeometryType						type:#integer					default:1								ui:uiPhysicalShape_GeometryType						animatable:false
	)

	rollout ZEPhysicalShapeRollout "ZE Physical Body Shape Parameters" rolledUp:false
	(
		bitmap			uiLogo																						fileName:"zineklogo.bmp"
		dropdownlist	uiPhysicalShape_GeometryType				"Geometry Type"			items:(if (classof $ == Editable_Poly or classof $ == Editable_mesh or classof $ == Editable_Patch) then #("Primitive (Not Supported)", "Convex") else #("Primitive", "Convex"))
		group "Material"
		(
			spinner 	uiPhysicalShape_MaterialRestitution 		"Restitution:" 				type:#float 			range:[0, 1, 0.01]
			spinner 	uiPhysicalShape_MaterialDynamicFriction 	"Dynamic Friction:" 		type:#float 
			spinner 	uiPhysicalShape_MaterialStaticFriction		"Static Friction:" 			type:#float
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
					if isProperty Current "ZEPhysicalShapeAttributes" do
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
		if selection.count == 1 AND isProperty $ "ZEPhysicalShapeAttributes" do
			UpdateZECoordinates $
	)

	/* ADDING ATTRIBUTES */

	ArgFilterValue = 3
	
	CheckResult = CheckZEAttributes selection ArgFilterValue

	if CheckResult == false then
	(
		for Current in Selection do
		(
			if Current != undefined then
			(	
				fn shape_filter obj = (classof obj == Sphere or classof obj == Box or classof obj == Capsule or classof obj == Cylinder or classof obj == Editable_mesh or classof obj == Editable_Poly) 
				if isProperty Current "ZEPhysicalShapeAttributes" == false and isProperty Current "ZEType" == false and shape_filter Current == true do
				(
					custAttributes.add Current PhysicalShapeAttributes
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

