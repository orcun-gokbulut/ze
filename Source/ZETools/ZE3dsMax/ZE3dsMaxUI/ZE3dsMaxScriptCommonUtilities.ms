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
	
	Result.PositionValue.x = if abs(obj.transform.position.x) < Threshold then 0.0 else obj.transform.position.x
	Result.PositionValue.y = if abs(obj.transform.position.z) < Threshold then 0.0 else obj.transform.position.z
	Result.PositionValue.z = if abs(obj.transform.position.y) < Threshold then 0.0 else obj.transform.position.y

	TempValues = (obj.transform.rotation) as eulerAngles
	
	Result.RotationValue.x = if abs(TempValues.x) < Threshold then 0.0 else TempValues.x
	Result.RotationValue.y = if abs(TempValues.z) < Threshold then 0.0 else TempValues.z
	Result.RotationValue.z = if abs(TempValues.y) < Threshold then 0.0 else TempValues.y
	
	Result.ScaleValue.x = if abs(obj.transform.scale.x) < Threshold then 0.0 else obj.transform.scale.x
	Result.ScaleValue.y = if abs(obj.transform.scale.z) < Threshold then 0.0 else obj.transform.scale.z
	Result.ScaleValue.z = if abs(obj.transform.scale.y) < Threshold then 0.0 else obj.transform.scale.y
	
	return Result		
)

/*ZE COORDINATES UPDATE FUNCTION*/

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

/*ZE ATTRIBUTES CHECK FUNCTION*/

fn CheckZEAttributes CurrentSelection AttributeFilterValue =
(	
	for CurrentObject in CurrentSelection do
	(
		if isProperty CurrentObject "ZEBoneAttributes" == true then
		(
			if AttributeFilterValue != 1 do
				return true
		)
		else if isProperty CurrentObject "ZEMeshAttributes" == true then
		(
			if AttributeFilterValue != 2 do
				return true
		)
		else if isProperty CurrentObject "ZEPhysicalShapeAttributes" == true then
		(
			if AttributeFilterValue != 3 do
				return true
		)
		else if isProperty CurrentObject "ZEInteriorDoorAttributes" == true then
		(
			if AttributeFilterValue != 4 do
				return true
		)
		else if isProperty CurrentObject "ZEInteriorRoomAttributes" == true then
		(
			if AttributeFilterValue != 5 do
				return true
		)
		else if isProperty CurrentObject "ZEHelperAttributes" == true then
		(
			if AttributeFilterValue != 6 do
				return true
		)
		else if isProperty CurrentObject "ZEBoundingBoxAttributes" == true then
		(
			if AttributeFilterValue != 7 do
				return true
		)
	)
	
	return false
)

fn ActivateZECallbacks =
(
	for CurrentObject in Geometry where CheckZEAttributes CurrentObject 0 == true do
	(
		when transform CurrentObject changes do
		(
				if selection.count == 1 AND CheckZEAttributes selection 0 == true do
				UpdateZECoordinates $
		)
	)
)

/* ZECoordinates transform update callback initialization for all scene nodes.*/

callbacks.addScript #filePostOpenProcess "ActivateZECallbacks()" id:#ZECoordinatesCallback 


