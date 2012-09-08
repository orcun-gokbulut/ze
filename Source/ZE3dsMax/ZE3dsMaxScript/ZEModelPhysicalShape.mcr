macroScript ZEPhysicalShapeAttribute_Add
	category:"ZEModel"
	toolTip:"Add Physical Body Shape Attributes"
	icon:#("ZEToolbarIcons", 3)
(
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
	)
	
	for Current in Selection do
	(
		if Current != undefined then
		(	
			fn shape_filter obj = (classof obj == Sphere or classof obj == Box or classof obj == Capsule or classof obj == Cylinder or classof obj == Editable_mesh or classof obj == Editable_Poly) 
			if isProperty Current "ZEPhysicalShapeAttributes" == false and isProperty Current "ZEType" == false and shape_filter Current == true then custAttributes.add Current PhysicalShapeAttributes
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
