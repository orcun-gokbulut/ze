
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
					if isProperty Current "ZEInteriorDoorAttributes" do
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
		if selection.count == 1 AND isProperty $ "ZEInteriorDoorAttributes" do
			UpdateZECoordinates $
	)
	
	/* ADDING ATTRIBUTES */

	ArgFilterValue = 4
	
	CheckResult = CheckZEAttributes selection ArgFilterValue

	if CheckResult == false then
	(
		for Current in selection do
		(
			if Current != undefined then
			(
				if isProperty Current "ZEInteriorDoorAttributes" == false and classof Current == plane do
				(
					custAttributes.add Current DoorAttributes
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





