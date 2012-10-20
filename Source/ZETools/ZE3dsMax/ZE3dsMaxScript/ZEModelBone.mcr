macroScript ZEBoneAttributes_Add
	category:"ZEModel"
	toolTip:"Add Bone Attributes"
	icon:#("ZEToolbarIcons", 2)
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
	
	
	BoneAttributes = attributes ZEBoneAttributes
	(	
		parameters ZEBone rollout:ZEBoneRollout
		(
			ZEType 					type:#string 				default:"Bone" 												animatable:false									 
		)
		
		rollout ZEBoneRollout "ZE Bone Parameters" rolledUp:false
		(
			/* LAYOUT DEFINITION */
			bitmap						uiLogo																				fileName:"zineklogo.bmp"
			label						uiNoParams		"No Parameters"				enabled:false
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
		
		rollout ZEPhysicalBodyRollout "ZE Physical Body Parameters" rolledUp:true
		(
			/* PICK FILTERS */
			fn Shape_Filter obj =  isProperty obj "ZEPhysicalShapeAttributes" == true and obj != $
			fn PointHelper_Filter obj = (classof obj) == Point and (superclassof obj) == helper
			
			/* LAYOUT DEFINITION */
			dropdownlist 			uiPhysicalBody_Type 						"Type:" 						items:#("None", "Rigid Body", "Deformable", "Cloth")
			checkbox				uiPhysicalBody_Enabled						"Enabled" 																										enabled:(PhysicalBody_Type != 1)
			checkbox 				uiPhysicalBody_Kinematic 					"Kinematic Body:"																								enabled:(PhysicalBody_Type != 1)
			spinner 				uiPhysicalBody_Mass 						"Mass:"							type:#float																		enabled:(PhysicalBody_Type != 1)
			label					uiPhysicalBody_CenterOfMass_Label			"Center Of Mass:"				align:#left																		enabled:(PhysicalBody_Type != 1)
			pickbutton				uiPhysicalBody_CenterOfMass					"Pick"							type:#node across:2 filter:PointHelper_Filter autoDisplay:true					enabled:(PhysicalBody_Type != 1)
			button					uiPhysicalBody_CenterOfMass_Clear			"Clean"																											enabled:(PhysicalBody_Type != 1)
			spinner 				uiPhysicalBody_LinearDamping				"Linear Damp.:"					type:#float																		enabled:(PhysicalBody_Type != 1)
			spinner 				uiPhysicalBody_AngularDamping				"Angular Damp.:"				type:#float																		enabled:(PhysicalBody_Type != 1)
			group "Body Shapes"
			(
				listbox 			uiPhysicalBody_Shapes 						"Shapes:" 						items:(for o in PhysicalBody_Shapes collect o.name)								enabled:(PhysicalBody_Type != 1)
				pickbutton 			uiPhysicalBody_Shape_Add					"Add" 							type:#node filter:Shape_Filter across:2											enabled:(PhysicalBody_Type != 1)								
				button 				uiPhysicalBody_Shape_Remove 				"Remove" 																										enabled:(PhysicalBody_Type != 1)
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
				uiPhysicalBody_Enabled.enabled  					= PhysicalBody_Type != 1	
				uiPhysicalBody_Kinematic.enabled  			 		= PhysicalBody_Type != 1
				uiPhysicalBody_Mass.enabled  			 			= PhysicalBody_Type != 1
				uiPhysicalBody_CenterOfMass_Label.enabled 			= PhysicalBody_Type != 1
				uiPhysicalBody_CenterOfMass.enabled  				= PhysicalBody_Type != 1 
				uiPhysicalBody_CenterOfMass_Clear.enabled 			= PhysicalBody_Type != 1
				uiPhysicalBody_LinearDamping.enabled  				= PhysicalBody_Type != 1
				uiPhysicalBody_AngularDamping.enabled 				= PhysicalBody_Type != 1	
				uiPhysicalBody_Shapes.enabled  						= PhysicalBody_Type != 1
				uiPhysicalBody_Shape_Add.enabled 					= PhysicalBody_Type != 1								
				uiPhysicalBody_Shape_Remove.enabled 				= PhysicalBody_Type != 1	
			)	
			
			on uiPhysicalBody_Type selected arg do ManageLayout()

			on ZEPhysicalBodyRollout open do
			(
				ZEPhysicalBodyRollout.open = PhysicalBody_Type != 1
			)
		)

		parameters ZEPhysicalJointParameters rollout:ZEPhysicalJointRollout
		(
			Joint_Type								type:#integer					default:1					ui:uiJoint_Type									animatable:false
			Joint_Enabled							type:#boolean					default:false				ui:uiJoint_Enabled								animatable:false
			Joint_UseGlobalAnchorAxis				type:#boolean					default:true																animateble:false
			Joint_Body1								type:#node 																									animatable:false
			Joint_Body1_Anchor						type:#node 																									animatable:false
			Joint_Body2								type:#node 																									animatable:false
			Joint_Body2_Anchor						type:#node 																									animatable:false
			Joint_CollideBodies						type:#boolean					default:false				ui:uiJoint_CollideBodies						animatable:false
			Joint_Breakable							type:#boolean					default:false				ui:uiJoint_Breakable							animatable:false
			Joint_Break_Force						type:#float						default:0.0					ui:uiJoint_Break_Force							animatable:false
			Joint_Break_Torque						type:#float						default:0.0					ui:uiJoint_Break_Torque							animatable:false
			Joint_XMotion							type:#integer					default:3					ui:uiJoint_XMotion								animatable:false
			Joint_YMotion 							type:#integer					default:3					ui:uiJoint_YMotion								animatable:false
			Joint_ZMotion 							type:#integer					default:3					ui:uiJoint_ZMotion								animatable:false
			Joint_LinearLimitValue					type:#float						default:0.0					ui:uiJoint_LinearLimitValue						animatable:false
			Joint_LinearLimitRestitution			type:#float						default:0.0					ui:uiJoint_LinearLimitRestitution				animatable:false
			Joint_LinearLimitSpring					type:#float						default:0.0					ui:uiJoint_LinearLimitSpring					animatable:false
			Joint_LinearLimitDamping				type:#float						default:0.0					ui:uiJoint_LinearLimitDamping					animatable:false
			
			Joint_TwistMotion 						type:#integer					default:3					ui:uiJoint_TwistMotion							animatable:false
			Joint_TwistLowLimitValue				type:#float						default:0.0					ui:uiJoint_TwistLowLimitValue					animatable:false
			Joint_TwistLowLimitRestitution			type:#float						default:0.0					ui:uiJoint_TwistLowLimitRestitution				animatable:false
			Joint_TwistLowLimitSpring				type:#float						default:0.0					ui:uiJoint_TwistLowLimitSpring					animatable:false
			Joint_TwistLowLimitDamping				type:#float						default:0.0					ui:uiJoint_TwistLowLimitDamping					animatable:false
			Joint_TwistHighLimitValue				type:#float						default:0.0					ui:uiJoint_TwistHighLimitValue					animatable:false
			Joint_TwistHighLimitRestitution			type:#float						default:0.0					ui:uiJoint_TwistHighLimitRestitution			animatable:false
			Joint_TwistHighLimitSpring				type:#float						default:0.0					ui:uiJoint_TwistHighLimitSpring					animatable:false
			Joint_TwistHighLimitDamping				type:#float						default:0.0					ui:uiJoint_TwistHighLimitDamping				animatable:false
			
			
			Joint_Swing1Motion						type:#integer					default:3					ui:uiJoint_Swing1Motion							animatable:false
			Joint_Swing1LimitValue					type:#float						default:0.0					ui:uiJoint_Swing1LimitValue						animatable:false
			Joint_Swing1LimitRestitution			type:#float						default:0.0					ui:uiJoint_Swing1LimitRestitution				animatable:false
			Joint_Swing1LimitSpring					type:#float						default:0.0					ui:uiJoint_Swing1LimitSpring					animatable:false
			Joint_Swing1LimitDamping				type:#float						default:0.0					ui:uiJoint_Swing1LimitDamping					animatable:false
			
			Joint_Swing2Motion 						type:#integer					default:3					ui:uiJoint_Swing2Motion							animatable:false
			Joint_Swing2LimitValue					type:#float						default:0.0					ui:uiJoint_Swing2LimitValue						animatable:false
			Joint_Swing2LimitRestitution			type:#float						default:0.0					ui:uiJoint_Swing2LimitRestitution				animatable:false
			Joint_Swing2LimitSpring					type:#float						default:0.0					ui:uiJoint_Swing2LimitSpring					animatable:false
			Joint_Swing2LimitDamping				type:#float						default:0.0					ui:uiJoint_Swing2LimitDamping					animatable:false
			
			Joint_MotorTargetPosition				type:#node													ui:uiJoint_MotorTargetPosition					animatable:false
			Joint_MotorTargetOrientation			type:#node													ui:uiJoint_MotorTargetOrientation				animatable:false
			Joint_MotorTargetVelocity				type:#Point3					default:[0, 0, 0]															animatable:false
			Joint_MotorTargetAngularVelocity		type:#Point3					default:[0, 0, 0]															animatable:false
			
			Joint_XLinearMotor						type:#integer					default:1					ui:uiJoint_XLinearMotor							animatable:false
			Joint_XLinearMotorForce					type:#float 					default:0.0					ui:uiJoint_XLinearMotorForce					animatable:false
			Joint_XLinearMotorSpring				type:#float 					default:0.0					ui:uiJoint_XLinearMotorSpring					animatable:false
			Joint_XLinearMotorDamper				type:#float 					default:0.0					ui:uiJoint_XLinearMotorDamper					animatable:false
			
			Joint_YLinearMotor						type:#integer					default:1					ui:uiJoint_YLinearMotor							animatable:false
			Joint_YLinearMotorForce					type:#float 					default:0.0					ui:uiJoint_YLinearMotorForce					animatable:false
			Joint_YLinearMotorSpring				type:#float 					default:0.0					ui:uiJoint_YLinearMotorSpring					animatable:false
			Joint_YLinearMotorDamper				type:#float 					default:0.0					ui:uiJoint_YLinearMotorDamper					animatable:false
			
			Joint_ZLinearMotor						type:#integer					default:1					ui:uiJoint_ZLinearMotor							animatable:false
			Joint_ZLinearMotorForce					type:#float						default:0.0					ui:uiJoint_ZLinearMotorForce					animatable:false
			Joint_ZLinearMotorSpring				type:#float						default:0.0					ui:uiJoint_ZLinearMotorSpring					animatable:false
			Joint_ZLinearMotorDamper				type:#float						default:0.0					ui:uiJoint_ZLinearMotorDamper					animatable:false
			
			Joint_AngularSwingMotor					type:#integer					default:1					ui:uiJoint_AngularSwingMotor					animatable:false
			Joint_AngularSwingMotorForce			type:#float						default:0.0					ui:uiJoint_AngularSwingMotorForce				animatable:false
			Joint_AngularSwingMotorSpring			type:#float						default:0.0					ui:uiJoint_AngularSwingMotorSpring				animatable:false
			Joint_AngularSwingMotorDamper			type:#float						default:0.0					ui:uiJoint_AngularSwingMotorDamper				animatable:false
			
			Joint_AngularTwistMotor					type:#integer					default:1					ui:uiJoint_AngularTwistMotor					animatable:false
			Joint_AngularTwistMotorForce			type:#float						default:0.0					ui:uiJoint_AngularTwistMotorForce				animatable:false
			Joint_AngularTwistMotorSpring			type:#float						default:0.0					ui:uiJoint_AngularTwistMotorSpring				animatable:false
			Joint_AngularTwistMotorDamper			type:#float						default:0.0					ui:uiJoint_AngularTwistMotorDamper				animatable:false
			
			Joint_AngularSlerpMotor					type:#integer					default:1					ui:uiJoint_AngularSlerpMotor					animatable:false
			Joint_AngularSlerpMotorForce			type:#float						default:0.0					ui:uiJoint_AngularSlerpMotorForce				animatable:false
			Joint_AngularSlerpMotorSpring			type:#float						default:0.0					ui:uiJoint_AngularSlerpMotorSpring				animatable:false
			Joint_AngularSlerpMotorDamper			type:#float						default:0.0					ui:uiJoint_AngularSlerpMotorDamper				animatable:false
		)
		
		rollout ZEPhysicalJointRollout "ZE Physical Joint Parameters" rolledUp:true
		(
			/* PICK FILTERS */
			fn PhysicalBody_Filter obj = (isproperty obj "ZEMeshAttributes" == true and obj.PhysicalBody_Type != 1 == true)
			fn PointHelper_Filter obj = (classof obj) == Point and (superclassof obj) == helper
			fn DummyHelper_Filter obj = (classof obj) == Point and (superclassof obj) == helper
		
			
			/* LAYOUT DEFINITION */
			bitmap					uiLogo																		fileName:"zineklogo.bmp"
			dropdownlist			uiJoint_Type								"Joint Type:"					items:#("None", "D6")
			checkbox				uiJoint_Enabled								"Enabled"																						enabled:(Joint_Type != 1)
			checkbox 				uiJoint_CollideBodies						"Bodies Collide"																				enabled:(Joint_Type != 1)
			checkbox			 	uiJoint_Breakable							"Breakable"																						enabled:(Joint_Type != 1)
			spinner					uiJoint_Break_Force							"Break Force:"					type:#float														enabled:(Joint_Type != 1 and Joint_Breakable)
			spinner					uiJoint_Break_Torque						"Break Torque:"					type:#float														enabled:(Joint_Type != 1 and Joint_Breakable)

			group "Properties"
			(
				dropdownlist 		uiJoint_XMotion								"X Motion:"						items:#("Free", "Limited", "Locked")							enabled:(Joint_Type == 2)
				dropdownlist 		uiJoint_YMotion								"Y Motion:"						items:#("Free", "Limited", "Locked")							enabled:(Joint_Type == 2)
				dropdownlist 		uiJoint_ZMotion								"Z Motion:"						items:#("Free", "Limited", "Locked")							enabled:(Joint_Type == 2)
				spinner 			uiJoint_LinearLimitValue					"Limit Value:"					type:#float														enabled:(Joint_Type == 2 and (Joint_XMotion == 2 or Joint_YMotion == 2 or Joint_ZMotion == 2))
				spinner  			uiJoint_LinearLimitRestitution				"Limit Restitution:"			type:#float				range:[0, 1, 0.01]						enabled:(Joint_Type == 2 and (Joint_XMotion == 2 or Joint_YMotion == 2 or Joint_ZMotion == 2))
				spinner  			uiJoint_LinearLimitSpring					"Limit Spring:"					type:#float														enabled:(Joint_Type == 2 and (Joint_XMotion == 2 or Joint_YMotion == 2 or Joint_ZMotion == 2))
				spinner 			uiJoint_LinearLimitDamping					"Limit Damping:"				type:#float														enabled:(Joint_Type == 2 and (Joint_XMotion == 2 or Joint_YMotion == 2 or Joint_ZMotion == 2))
				
				dropdownlist 		uiJoint_TwistMotion							"Twist Motion"					items:#("Free", "Limited", "Locked")							enabled:(Joint_Type == 2)
				spinner 			uiJoint_TwistLowLimitValue					"Low Limit Value:" 				type:#float				range:[-180, 180, 1]					enabled:(Joint_Type == 2 and Joint_TwistMotion == 2)
				spinner 			uiJoint_TwistLowLimitRestitution			"Low Limit Restitution:"		type:#float				range:[0, 1, 0.01]						enabled:(Joint_Type == 2 and Joint_TwistMotion == 2)
				spinner 			uiJoint_TwistLowLimitSpring					"Low Limit Spring:"				type:#float														enabled:(Joint_Type == 2 and Joint_TwistMotion == 2)
				spinner 			uiJoint_TwistLowLimitDamping				"Low Limit Damping:"			type:#float														enabled:(Joint_Type == 2 and Joint_TwistMotion == 2)
				spinner  			uiJoint_TwistHighLimitValue					"High Limit Value:" 			type:#float				range:[-180, 180, 1]					enabled:(Joint_Type == 2 and Joint_TwistMotion == 2)
				spinner 			uiJoint_TwistHighLimitRestitution			"High Limit Restitution:"		type:#float				range:[0, 1, 0.01]						enabled:(Joint_Type == 2 and Joint_TwistMotion == 2)
				spinner 			uiJoint_TwistHighLimitSpring				"High Limit Spring:"			type:#float														enabled:(Joint_Type == 2 and Joint_TwistMotion == 2)
				spinner 			uiJoint_TwistHighLimitDamping				"High Limit Damping:"			type:#float														enabled:(Joint_Type == 2 and Joint_TwistMotion == 2)
				
				dropdownlist 		uiJoint_Swing1Motion						"Swing 1 Motion"				items:#("Free", "Limited", "Locked")							enabled:(Joint_Type == 2)
				spinner  			uiJoint_Swing1LimitValue					"Limit Value:"					type:#float				range:[-180, 180, 1]					enabled:(Joint_Type == 2 and Joint_Swing1Motion == 2)
				spinner  			uiJoint_Swing1LimitRestitution				"Limit Restitution:" 			type:#float				range:[0, 1, 0.01]		 				enabled:(Joint_Type == 2 and Joint_Swing1Motion == 2)
				spinner  			uiJoint_Swing1LimitSpring					"Limit Spring:"					type:#float														enabled:(Joint_Type == 2 and Joint_Swing1Motion == 2)
				spinner  			uiJoint_Swing1LimitDamping					"Limit Damping:"				type:#float														enabled:(Joint_Type == 2 and Joint_Swing1Motion == 2)
				
				dropdownlist 		uiJoint_Swing2Motion						"Swing 2 Motion"				items:#("Free", "Limited", "Locked")							enabled:(Joint_Type == 2)
				spinner  			uiJoint_Swing2LimitValue					"Limit Value:"					type:#float				range:[-180, 180, 1]					enabled:(Joint_Type == 2 and Joint_Swing2Motion == 2)
				spinner  			uiJoint_Swing2LimitRestitution				"Limit Restitution:" 			type:#float				range:[0, 1, 0.01]						enabled:(Joint_Type == 2 and Joint_Swing2Motion == 2)
				spinner  			uiJoint_Swing2LimitSpring					"Limit Spring:"					type:#float														enabled:(Joint_Type == 2 and Joint_Swing2Motion == 2)
				spinner  			uiJoint_Swing2LimitDamping					"Limit Damping:"				type:#float														enabled:(Joint_Type == 2 and Joint_Swing2Motion == 2)
				
				label				uiJoint_MotorTargetPosition_Label			"Target Position:"				align:#left														enabled:(Joint_Type == 2)	
				pickbutton			uiJoint_MotorTargetPosition					"Pick"							filter:PointHelper_Filter		across:2						enabled:(Joint_Type == 2)
				button				uiJoint_MotorTargetPosition_Clean			"Clean"																							enabled:(Joint_Type == 2)
				label				uiJoint_MotorTargetOrientation_Label		"Target Orientation:"			align:#left														enabled:(Joint_Type == 2)	
				pickbutton			uiJoint_MotorTargetOrientation				"Pick"							filter:PointHelper_Filter		across:2						enabled:(Joint_Type == 2)
				button				uiJoint_MotorTargetOrientation_Clean		"Clean"																							enabled:(Joint_Type == 2)
				label				uiJoint_MotorTargetVelocity_Label			"Target Velocity:"				align:#left														enabled:(Joint_Type == 2)
				spinner				uiJoint_MotorTargetVelocity_X				"X:"							range:[-10000,10000, 0.1]										enabled:(Joint_Type == 2)
				spinner				uiJoint_MotorTargetVelocity_Y				"Y:"							range:[-10000,10000, 0.1]										enabled:(Joint_Type == 2)
				spinner				uiJoint_MotorTargetVelocity_Z				"Z:"							range:[-10000,10000, 0.1]										enabled:(Joint_Type == 2)				
				label			 	uiJoint_MotorTargetAngularVelocity_Label	"Target Angl.Velocity:"			align:#left														enabled:(Joint_Type == 2)	
				spinner				uiJoint_MotorTargetAngularVelocity_X		"X:"							range:[-10000,10000, 0.1]										enabled:(Joint_Type == 2)
				spinner				uiJoint_MotorTargetAngularVelocity_Y		"Y:"							range:[-10000,10000, 0.1]										enabled:(Joint_Type == 2)
				spinner				uiJoint_MotorTargetAngularVelocity_Z		"Z:"							range:[-10000,10000, 0.1]										enabled:(Joint_Type == 2)				
				
				dropdownlist 		uiJoint_XLinearMotor						"X Linear Motor"				items:#("Disabled", "Position", "Velocity")						enabled:(Joint_Type == 2)
				spinner  			uiJoint_XLinearMotorForce					"Motor Force:"					type:#float														enabled:(Joint_Type == 2 and Joint_XLinearMotor != 1)
				spinner  			uiJoint_XLinearMotorSpring					"Motor Spring:"					type:#float														enabled:(Joint_Type == 2 and Joint_XLinearMotor != 1)
				spinner  			uiJoint_XLinearMotorDamper					"Motor Damper:" 				type:#float														enabled:(Joint_Type == 2 and Joint_XLinearMotor != 1)
				
				dropdownlist 		uiJoint_YLinearMotor						"Y Linear Motor"				items:#("Disabled", "Position", "Velocity")						enabled:(Joint_Type == 2)
				spinner  			uiJoint_YLinearMotorForce					"Motor Force:"					type:#float														enabled:(Joint_Type == 2 and Joint_YLinearMotor != 1)
				spinner 			uiJoint_YLinearMotorSpring					"Motor Spring:"					type:#float														enabled:(Joint_Type == 2 and Joint_YLinearMotor != 1)
				spinner  			uiJoint_YLinearMotorDamper					"Motor Damper:" 				type:#float														enabled:(Joint_Type == 2 and Joint_YLinearMotor != 1)
				
				dropdownlist 		uiJoint_ZLinearMotor						"Z Linear Motor:"				items:#("Disabled", "Position", "Velocity")						enabled:(Joint_Type == 2)
				spinner  			uiJoint_ZLinearMotorForce					"Motor Force:"					type:#float														enabled:(Joint_Type == 2 and Joint_ZLinearMotor != 1)
				spinner  			uiJoint_ZLinearMotorSpring					"Motor Spring:"					type:#float														enabled:(Joint_Type == 2 and Joint_ZLinearMotor != 1)
				spinner  			uiJoint_ZLinearMotorDamper					"Motor Damper:"					type:#float														enabled:(Joint_Type == 2 and Joint_ZLinearMotor != 1)
				
				dropdownlist 		uiJoint_AngularSwingMotor					"Angular Swing Motor:"			items:#("Disabled", "Position", "Velocity")						enabled:(Joint_Type == 2)
				spinner 			uiJoint_AngularSwingMotorForce				"Motor Force:"					type:#float														enabled:(Joint_Type == 2 and Joint_AngularSwingMotor != 1)
				spinner 			uiJoint_AngularSwingMotorSpring				"Motor Spring:"					type:#float														enabled:(Joint_Type == 2 and Joint_AngularSwingMotor != 1)
				spinner 			uiJoint_AngularSwingMotorDamper				"Motor Damper:"					type:#float														enabled:(Joint_Type == 2 and Joint_AngularSwingMotor != 1)
				
				dropdownlist 		uiJoint_AngularTwistMotor					"Angular Twist Motor:"			items:#("Disabled", "Position", "Velocity")						enabled:(Joint_Type == 2)
				spinner 			uiJoint_AngularTwistMotorForce				"Motor Force:"					type:#float														enabled:(Joint_Type == 2 and Joint_AngularTwistMotor != 1)
				spinner 			uiJoint_AngularTwistMotorSpring				"Motor Spring:"					type:#float														enabled:(Joint_Type == 2 and Joint_AngularTwistMotor != 1)
				spinner 			uiJoint_AngularTwistMotorDamper				"Motor Damper:"					type:#float														enabled:(Joint_Type == 2 and Joint_AngularTwistMotor != 1)
				
				dropdownlist 		uiJoint_AngularSlerpMotor					"Angular Slerp Motor:"			items:#("Disabled", "Position", "Velocity")						enabled:(Joint_Type == 2)
				spinner 			uiJoint_AngularSlerpMotorForce				"Motor Force:"					type:#float														enabled:(Joint_Type == 2 and Joint_AngularSlerpMotor != 1)
				spinner 			uiJoint_AngularSlerpMotorSpring				"Motor Spring:"					type:#float														enabled:(Joint_Type == 2 and Joint_AngularSlerpMotor != 1)
				spinner 			uiJoint_AngularSlerpMotorDamper				"Motor Damper:"					type:#float														enabled:(Joint_Type == 2 and Joint_AngularSlerpMotor != 1)
				
			)
			
			/* EVENTS */	
				
			on uiJoint_MotorTargetPosition_Clean pressed do Joint_MotorTargetPosition_Clean = undefined
			on uiJoint_MotorTargetOrientation_Clean pressed do Joint_MotorTargetOrientation_Clean = undefined		
				
			on uiJoint_MotorTargetVelocity_X changed arg do Joint_MotorTargetVelocity.x = arg
			on uiJoint_MotorTargetVelocity_Y changed arg do Joint_MotorTargetVelocity.y = arg				
			on uiJoint_MotorTargetVelocity_Z changed arg do Joint_MotorTargetVelocity.z = arg	
				
			on uiJoint_MotorTargetAngularVelocity_X changed arg do Joint_MotorTargetAngularVelocity.x = arg				
			on uiJoint_MotorTargetAngularVelocity_X changed arg do Joint_MotorTargetAngularVelocity.y = arg				
			on uiJoint_MotorTargetAngularVelocity_X changed arg do Joint_MotorTargetAngularVelocity.z = arg				

			/* LAYOUT MANAGEMENT */
			fn ManageLayout =
			(	
				uiJoint_Enabled.enabled								= (Joint_Type != 1)
				uiJoint_CollideBodies.enabled						= (Joint_Type != 1)
				uiJoint_Breakable.enabled							= (Joint_Type != 1)
				uiJoint_Break_Force.enabled							= (Joint_Type != 1 and Joint_Breakable)
				uiJoint_Break_Torque.enabled						= (Joint_Type != 1 and Joint_Breakable)
				uiJoint_Break_Force.enabled							= (Joint_Breakable)
				uiJoint_Break_Torque.enabled						= (Joint_Breakable)
				
				uiJoint_XMotion.enabled 							= (Joint_Type == 2)
				uiJoint_YMotion.enabled								= (Joint_Type == 2)
				uiJoint_ZMotion.enabled								= (Joint_Type == 2)

				uiJoint_LinearLimitValue.enabled					= (Joint_Type == 2 and (Joint_XMotion == 2 or Joint_YMotion == 2 or Joint_ZMotion == 2))
				uiJoint_LinearLimitRestitution.enabled				= (Joint_Type == 2 and (Joint_XMotion == 2 or Joint_YMotion == 2 or Joint_ZMotion == 2))
				uiJoint_LinearLimitSpring.enabled					= (Joint_Type == 2 and (Joint_XMotion == 2 or Joint_YMotion == 2 or Joint_ZMotion == 2))
				uiJoint_LinearLimitDamping.enabled					= (Joint_Type == 2 and (Joint_XMotion == 2 or Joint_YMotion == 2 or Joint_ZMotion == 2))
				
				uiJoint_TwistMotion.enabled							= (Joint_Type == 2)
				uiJoint_TwistLowLimitValue.enabled					= (Joint_Type == 2 and Joint_TwistMotion == 2)
				uiJoint_TwistLowLimitRestitution.enabled			= (Joint_Type == 2 and Joint_TwistMotion == 2)
				uiJoint_TwistLowLimitSpring.enabled					= (Joint_Type == 2 and Joint_TwistMotion == 2)
				uiJoint_TwistLowLimitDamping.enabled				= (Joint_Type == 2 and Joint_TwistMotion == 2)
				uiJoint_TwistHighLimitValue.enabled					= (Joint_Type == 2 and Joint_TwistMotion == 2)
				uiJoint_TwistHighLimitRestitution.enabled			= (Joint_Type == 2 and Joint_TwistMotion == 2)
				uiJoint_TwistHighLimitSpring.enabled				= (Joint_Type == 2 and Joint_TwistMotion == 2)
				uiJoint_TwistHighLimitDamping.enabled				= (Joint_Type == 2 and Joint_TwistMotion == 2)
				
				uiJoint_Swing1Motion.enabled						= (Joint_Type == 2)
				uiJoint_Swing1LimitValue.enabled					= (Joint_Type == 2 and Joint_Swing1Motion == 2)
				uiJoint_Swing1LimitRestitution.enabled				= (Joint_Type == 2 and Joint_Swing1Motion == 2)
				uiJoint_Swing1LimitSpring.enabled					= (Joint_Type == 2 and Joint_Swing1Motion == 2)
				uiJoint_Swing1LimitDamping.enabled					= (Joint_Type == 2 and Joint_Swing1Motion == 2)
				
				uiJoint_Swing2Motion.enabled						= (Joint_Type == 2)
				uiJoint_Swing2LimitValue.enabled					= (Joint_Type == 2 and Joint_Swing2Motion == 2)
				uiJoint_Swing2LimitRestitution.enabled				= (Joint_Type == 2 and Joint_Swing2Motion == 2)
				uiJoint_Swing2LimitSpring.enabled					= (Joint_Type == 2 and Joint_Swing2Motion == 2)
				uiJoint_Swing2LimitDamping.enabled					= (Joint_Type == 2 and Joint_Swing2Motion == 2)
			
				uiJoint_MotorTargetPosition_Label.enabled 			= (Joint_Type == 2)
				uiJoint_MotorTargetPosition.enabled					= (Joint_Type == 2)
				uiJoint_MotorTargetPosition_Clean.enabled 			= (Joint_Type == 2)
				uiJoint_MotorTargetOrientation_Label.enabled 		= (Joint_Type == 2)
				uiJoint_MotorTargetOrientation.enabled				= (Joint_Type == 2)
				uiJoint_MotorTargetOrientation_Clean.enabled		= (Joint_Type == 2)		
				uiJoint_MotorTargetVelocity_Label.enabled			= (Joint_Type == 2)
				uiJoint_MotorTargetVelocity_X.enabled				= (Joint_Type == 2)
				uiJoint_MotorTargetVelocity_Y.enabled				= (Joint_Type == 2)
				uiJoint_MotorTargetVelocity_Z.enabled				= (Joint_Type == 2)				
				uiJoint_MotorTargetAngularVelocity_Label.enabled	= (Joint_Type == 2)
				uiJoint_MotorTargetAngularVelocity_X.enabled		= (Joint_Type == 2)
				uiJoint_MotorTargetAngularVelocity_Y.enabled		= (Joint_Type == 2)
				uiJoint_MotorTargetAngularVelocity_Z.enabled		= (Joint_Type == 2)	

				uiJoint_XLinearMotor.enabled						= (Joint_Type == 2)
				uiJoint_XLinearMotorForce.enabled					= (Joint_Type == 2 and Joint_XLinearMotor != 1)
				uiJoint_XLinearMotorSpring.enabled					= (Joint_Type == 2 and Joint_XLinearMotor != 1)
				uiJoint_XLinearMotorDamper.enabled					= (Joint_Type == 2 and Joint_XLinearMotor != 1)
				
				uiJoint_YLinearMotor.enabled						= (Joint_Type == 2)
				uiJoint_YLinearMotorForce.enabled					= (Joint_Type == 2 and Joint_YLinearMotor != 1)
				uiJoint_YLinearMotorSpring.enabled					= (Joint_Type == 2 and Joint_YLinearMotor != 1)
				uiJoint_YLinearMotorDamper.enabled					= (Joint_Type == 2 and Joint_YLinearMotor != 1)
						
				uiJoint_ZLinearMotor.enabled						= (Joint_Type == 2)
				uiJoint_ZLinearMotorForce.enabled					= (Joint_Type == 2 and Joint_ZLinearMotor != 1)
				uiJoint_ZLinearMotorSpring.enabled					= (Joint_Type == 2 and Joint_ZLinearMotor != 1)
				uiJoint_ZLinearMotorDamper.enabled					= (Joint_Type == 2 and Joint_ZLinearMotor != 1)
				
				uiJoint_AngularSwingMotor.enabled					= (Joint_Type == 2)
				uiJoint_AngularSwingMotorForce.enabled				= (Joint_Type == 2 and Joint_AngularSwingMotor != 1)
				uiJoint_AngularSwingMotorSpring.enabled				= (Joint_Type == 2 and Joint_AngularSwingMotor != 1) 
				uiJoint_AngularSwingMotorDamper.enabled				= (Joint_Type == 2 and Joint_AngularSwingMotor != 1)
				
				uiJoint_AngularTwistMotor.enabled					= (Joint_Type == 2)
				uiJoint_AngularTwistMotorForce.enabled				= (Joint_Type == 2 and Joint_AngularTwistMotor != 1)
				uiJoint_AngularTwistMotorSpring.enabled				= (Joint_Type == 2 and Joint_AngularTwistMotor != 1) 
				uiJoint_AngularTwistMotorDamper.enabled				= (Joint_Type == 2 and Joint_AngularTwistMotor != 1)
				
				uiJoint_AngularSlerpMotor.enabled					= (Joint_Type == 2)
				uiJoint_AngularSlerpMotorForce.enabled				= (Joint_Type == 2 and Joint_AngularSlerpMotor != 1)
				uiJoint_AngularSlerpMotorSpring.enabled				= (Joint_Type == 2 and Joint_AngularSlerpMotor != 1) 
				uiJoint_AngularSlerpMotorDamper.enabled				= (Joint_Type == 2 and Joint_AngularSlerpMotor != 1)
				
			)
			
			on uiJoint_UseGlobalAnchorAxis changed arg do ManageLayout()
			on uiJoint_Breakable changed arg do ManageLayout()
			on uiJoint_Type selected arg do ManageLayout()
			on uiJoint_XMotion selected arg do ManageLayout()
			on uiJoint_YMotion selected arg do ManageLayout()
			on uiJoint_ZMotion selected arg do ManageLayout()
			on uiJoint_TwistMotion selected arg do ManageLayout()
			on uiJoint_Swing1Motion selected arg do ManageLayout()
			on uiJoint_Swing2Motion selected arg do ManageLayout()
			on uiJoint_XLinearMotor selected arg do ManageLayout()
			on uiJoint_YLinearMotor selected arg do ManageLayout()
			on uiJoint_ZLinearMotor selected arg do ManageLayout()
			on uiJoint_AngularSwingMotor selected arg do ManageLayout()
			on uiJoint_AngularTwistMotor selected arg do ManageLayout()
			on uiJoint_AngularSlerpMotor selected arg do ManageLayout()
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
						if isProperty Current "ZEBoneAttributes" do
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
		if selection.count == 1 AND isProperty $ "ZEBoneAttributes" do
			UpdateZECoordinates $
	)
	
	/* ADDING ATTRIBUTES */
	for Current in Selection do
	(
		if $ != undefined then
		(	
			if (isProperty $ "ZEBoneAttributes" == false and classof Current == BoneGeometry)  do 
			(
				custAttributes.add $ BoneAttributes
				UpdateZECoordinates Current
			)
			
		)
	)
)

macroScript ZEBoneAttributes_Remove
	category:"ZEModel"
	toolTip:"Remove Bone Attributes"
	icon:#("ZEToolbarIcons", 5)
(	
	/* REMOVING ATTRIBUTES */
	for Current in Selection do
	(
		if Current != undefined then	
		(
			if isProperty Current "ZEBoneAttributes" == true then 
			(
				for i = 1 to custAttributes.count Current  do 
				(
					if (custAttributes.getdef Current i).name == #ZEBoneAttributes then custAttributes.delete Current i
				)
			)
		)
	)
)