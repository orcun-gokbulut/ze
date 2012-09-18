macroScript ZEPhysicalShapeAttribute_Add
	category:"ZEModel"
	toolTip:"Add Physical Body Shape Attributes"
	icon:#("ZEToolbarIcons", 3)
(

	/*ZE COORDINATES CONVERSION FUNCTION*/
	
	struct ZETransform 
	(
		PositionValue,
		RotationValue,
		ScaleValue
	)
	
	fn MAX_TO_ZE obj  = 
	(
		Result = ZETransform PositionValue: [0.0, 0.0, 0.0] RotationValue: (eulerAngles 0.0 0.0 0.0) ScaleValue: [0.0, 0.0, 0.0]
		
		Threshold = 0.0001
		
		Result.PositionValue.x = if abs(obj.position.x) < Threshold then 0.0 else obj.position.x
		Result.PositionValue.y = if abs(obj.position.z) < Threshold then 0.0 else obj.position.z
		Result.PositionValue.z = -(if abs(obj.position.y) < Threshold then 0.0 else obj.position.y)

		TempValues = (obj.rotation) as eulerAngles
		
		Result.RotationValue.x = if abs(TempValues.x) < Threshold then 0.0 else TempValues.x
		Result.RotationValue.y = if abs(TempValues.z) < Threshold then 0.0 else TempValues.z
		Result.RotationValue.z = if abs(TempValues.y) < Threshold then 0.0 else TempValues.y
		
		Result.ScaleValue.x = if abs(obj.scale.x) < Threshold then 0.0 else obj.scale.x
		Result.ScaleValue.y = if abs(obj.scale.z) < Threshold then 0.0 else obj.scale.z
		Result.ScaleValue.z = if abs(obj.scale.y) < Threshold then 0.0 else obj.scale.y
		
		return Result		
	)
	
	/*ZE COORDINATES UPDATE FUNCTION */
	
	fn UpdateZECoordinates CurrentObject =
	(
		TransformValue = MAX_TO_ZE CurrentObject
		
		CurrentObject.ZECoordinatesRollout.uitxtPositionXValue.text = TransformValue.PositionValue.x as string
		CurrentObject.ZECoordinatesRollout.uitxtPositionYValue.text = TransformValue.PositionValue.y as string
		CurrentObject.ZECoordinatesRollout.uitxtPositionZValue.text = TransformValue.PositionValue.z as string
		
		CurrentObject.ZECoordinatesRollout.uitxtRotationXValue.text = TransformValue.RotationValue.x as string
		CurrentObject.ZECoordinatesRollout.uitxtRotationYValue.text = TransformValue.RotationValue.y as string
		CurrentObject.ZECoordinatesRollout.uitxtRotationZValue.text = TransformValue.RotationValue.z as string
		
		CurrentObject.ZECoordinatesRollout.uitxtScaleXValue.text = TransformValue.ScaleValue.x  as string
		CurrentObject.ZECoordinatesRollout.uitxtScaleYValue.text = TransformValue.ScaleValue.y  as string
		CurrentObject.ZECoordinatesRollout.uitxtScaleZValue.text = TransformValue.ScaleValue.z  as string
	)

	
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
	
	when transform selection changes do
	(
		if selection.count == 1 AND isProperty $ "ZEPhysicalShapeAttributes" do
			UpdateZECoordinates $
	)
	
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

macroScript ZEPhysicalShapeAttributeRemove
	category:"ZEModel"
	toolTip:"Remove Physical Body Shape Attributes"
	icon:#("ZEToolbarIcons", 6)
(
	for Current in Selection do
	(
		if Current != undefined then	
		(
			if isProperty Current "ZEPhysicalShapeAttributes" == true then 
			(
				for i = 1 to custAttributes.count Current  do 
				(
					if (custAttributes.getdef Current i).name == #ZEPhysicalShapeAttributes then custAttributes.delete Current i
				)
			)
		)
	)
)
