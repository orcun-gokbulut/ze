
for obj in selection do
(
	if obj != undefined then
	(
		if isProperty obj "ZEBoneAttributes" == true then
		(
			for i = 1 to custAttributes.count obj  do
			(
				if (custAttributes.getdef obj i).name == #ZEBoneAttributes then custAttributes.delete obj i
			)
		)

		if isProperty obj "ZEMeshAttributes" == true then
		(
			for i = 1 to custAttributes.count obj  do
			(
				if (custAttributes.getdef obj i).name == #ZEMeshAttributes then custAttributes.delete obj i
			)
		)

		if isProperty obj "ZEPhysicalShapeAttributes" == true then
		(
			for i = 1 to custAttributes.count obj  do
			(
				if (custAttributes.getdef obj i).name == #ZEPhysicalShapeAttributes then custAttributes.delete obj i
			)
		)

		if isProperty obj "ZEBoundingBoxAttributes" == true then
		(
			for i = 1 to custAttributes.count obj  do
			(
				if (custAttributes.getdef obj i).name == #ZEBoundingBoxAttributes then 
					custAttributes.delete obj i
					obj.xray = false
			)
		)
		
		if isProperty obj "ZEInteriorDoorAttributes" == true then
		(
			for i = 1 to custAttributes.count obj  do
			(
				if (custAttributes.getdef obj i).name == #ZEInteriorDoorAttributes then custAttributes.delete obj i
			)
		)

		if isProperty obj "ZEInteriorRoomAttributes" == true then
		(
			for i = 1 to custAttributes.count obj  do
			(
				if (custAttributes.getdef obj i).name == #ZEInteriorRoomAttributes then custAttributes.delete obj i
			)
		)
	)
)

