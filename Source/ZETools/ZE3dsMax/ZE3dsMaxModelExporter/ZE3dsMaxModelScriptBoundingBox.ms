
BoundingBoxAttributes = attributes ZEBoundingBoxAttributes
(
	parameters ZEHelper rollout:ZEHelperRollout
	(
		ZEType 					type:#string 				default:"BoundingBox" 		animatable:false
	)
		
	rollout ZEHelperRollout "ZEModel Bounding Box Parameters" rolledUp:false
	(
		/* LAYOUT DEFINITION */
		bitmap				uiLogo							fileName:"zineklogo.bmp"
		
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
					if isProperty Current "ZEBoundingBoxAttributes" do
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
	if selection.count == 1 then
	(
		Uniqueness = true
		
		for Current in geometry do
		(
			if Current != undefined then
			(	
				if classof Current == Box and isProperty Current "ZEBoundingBoxAttributes" == true do 
				(
					Uniqueness = false
				)
			)
		)
		
		if Uniqueness == true then
		(
			when transform selection changes do
			(
				if selection.count == 1 AND isProperty $ "ZEBoundingBoxAttributes" do
					UpdateZECoordinates $
			)
		
			/* ADDING ATTRIBUTES */

			ArgFilterValue = 7
			
			CheckResult = CheckZEAttributes selection ArgFilterValue

			if CheckResult == false then
			(
				for Current in selection do
				(
					if Current != undefined then
					(	
						if isProperty Current "ZEBoundingBoxAttributes" == false and classof Current == Box do 
						(
							custAttributes.add Current BoundingBoxAttributes
							Current.xray = true
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
			messagebox "Only single object with ZEModel Bounding Box Attribute is allowed in scene. Operation aborted."
		)
	)
	else
	(
		messagebox "Current selection has multiple objects selected. Please select a single object. Operation aborted."
	)
)
else
(
	messagebox "No selection to add Zinek Engine Attributes. Operation aborted."
)