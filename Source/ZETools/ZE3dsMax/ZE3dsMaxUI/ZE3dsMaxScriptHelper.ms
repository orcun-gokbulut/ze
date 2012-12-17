
HelperAttributes = attributes ZEHelperAttributes
(
	parameters ZEHelper rollout:ZEHelperRollout
	(
		ZEType 					type:#string 				default:"Helper" 			animatable:false
		Owner 					type:#node 												animatable:false
	)
		
	rollout ZEHelperRollout "ZE Helper Parameters" rolledUp:false
	(
		/* PICK FILTERS */
		fn HelperFilter obj = (isProperty obj "ZEMeshAttributes" == true or isProperty obj "ZEBoneAttributes" == true) and $ != obj
		
		/* LAYOUT DEFINITION */
		bitmap				uiLogo							fileName:"zineklogo.bmp"
		edittext 			uiHelper 			"Owner :" 	readOnly:true text:(if (Owner != undefined) then Owner.name else "") 
		pickbutton 			uiHelperSelect 		"Pick" 		filter:HelperFilter across:2
		button 				uiHelperClean 		"Clean"
		
		on uiHelperSelect picked obj do
		(
			if obj != undefined do
			(
				uiHelper.text = obj.name
				Owner = obj
			)
		)
		
		on uiHelperClean pressed do
		(
			uiHelper.text = ""
			Owner = undefined
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
					if isProperty Current "ZEHelperAttributes" do
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
		if selection.count == 1 AND isProperty $ "ZEHelperAttributes" do
			UpdateZECoordinates $
	)
	
	/* ADDING ATTRIBUTES */

	ArgFilterValue = 6
	
	CheckResult = CheckZEAttributes selection ArgFilterValue

	if CheckResult == false then
	(
		for Current in selection do
		(
			if Current != undefined then
			(	
				if isProperty Current "ZEHelperAttributes" == false and superclassof Current == helper do 
				(
					custAttributes.add Current HelperAttributes
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




