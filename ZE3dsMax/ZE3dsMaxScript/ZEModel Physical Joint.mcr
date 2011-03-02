macroScript ZEPhysicalJointAttributes_Add
	category:"ZEModel"
	toolTip:"Add Physical Joint Attributes"
	icon:#("ZEToolbarIcons", 2)
(
	PhysicalJointAttributes = attributes ZEPhysicalJointAttributes
	(
		parameters ZEPhysicalJointParameters rollout:ZEPhysicalJointRollout
		(
			Joint_Type									type:#integer					default:1					ui:uiJoint_Type									animatable:false
			Joint_UseGlobalAnchorAxis				type:#boolean					default:true				ui:uiJoint_UseGlobalAnchorAxis				animateble:false
			Joint_Enabled								type:#boolean					default:false				ui:uiJoint_Enabled								animatable:false
			Joint_Body1								type:#node 													ui:uiJoint_Body1									animatable:false
			Joint_Body1_Anchor						type:#node 													ui:uiJoint_Body1_Anchor						animatable:false
			Joint_Body2								type:#node 													ui:uiJoint_Body2									animatable:false
			Joint_Body2_Anchor						type:#node 													ui:uiJoint_Body2_Anchor						animatable:false
			Joint_CollideBodies						type:#boolean					default:false				ui:uiJoint_CollideBodies							animatable:false
			Joint_Breakable							type:#boolean					default:false				ui:uiJoint_Breakable								animatable:false
			Joint_Break_Force						type:#float						default:0.0					ui:uiJoint_Break_Force							animatable:false
			Joint_Break_Torque						type:#float						default:0.0					ui:uiJoint_Break_Torque							animatable:false

			Spherical_SwingLimit						type:#boolean					default:false				ui:uiSpherical_SwingLimit						animatable:false
			Spherical_SwingLimit_Value			type:#float						default:0.0					ui:uiSpherical_SwingLimit_Value				animatable:false
			Spherical_SwingLimit_Restitution		type:#float						default:0.0					ui:uiSpherical_SwingLimit_Restitution		animatable:false
			Spherical_TwistLimit						type:#boolean					default:false				ui:uiSpherical_TwistLimit						animatable:false
			Spherical_TwistLimit_LowValue		type:#float						default:0					ui:uiSpherical_TwistLimit_LowValue			animatable:false
			Spherical_TwistLimit_HighValue		type:#float						default:0					ui:uiSpherical_TwistLimit_HighValue			animatable:false
			Spherical_TwistLimit_Restitution		type:#float						default:0					ui:uiSpherical_TwistLimit_Restitution		animatable:false

			Revolute_HasLimit						type:#boolean					default:false				ui:uiRevolute_HasLimit							animatable:false
			Revolute_Limit_HighValue				type:#float						default:0.0					ui:uiRevolute_Limit_HighValue					animatable:false
			Revolute_Limit_LowValue				type:#float						default:0.0					ui:uiRevolute_Limit_LowValue					animatable:false
			Revolute_Limit_Restitution				type:#float						default:0.0					ui:uiRevolute_Limit_Restitution				animatable:false
			Revolute_HasSpring						type:#boolean					default:false				ui:uiRevolute_HasSpring						animatable:false
			Revolute_Spring_Value					type:#float						default:0.0					ui:uiRevolute_Spring_Value					animatable:false
			Revolute_Spring_Damper				type:#float						default:0.0					ui:uiRevolute_Spring_Damper					animatable:false
			Revolute_Spring_Target				type:#float						default:0.0					ui:uiRevolute_Spring_Target					animatable:false
			Revolute_HasMotor						type:#boolean					default:false				ui:uiRevolute_HasMotor							animatable:false
			Revolute_Motor_Force					type:#float						default:0.0					ui:uiRevolute_Motor_Force						animatable:false
			Revolute_Motor_Velocity				type:#float						default:0.0					ui:uiRevolute_Motor_Velocity					animatable:false

			Distance_HasMinLimit					type:#boolean					default:false				ui:uiDistance_HasMinLimit						animatable:false
			Distance_MinLimit_MinDistance		type:#float						default:0.0					ui:uiDistance_MinLimit_MinDistance			animatable:false
			Distance_HasMaxLimit					type:#boolean					default:false				ui:uiDistance_HasMaxLimit						animatable:false
			Distance_MaxLimit_MaxDistance		type:#float						default:0.0					ui:uiDistance_MaxLimit_MaxDistance		animatable:false
			Distance_HasSpring						type:#boolean					default:false				ui:uiDistance_HasSpring						animatable:false
			Distance_Spring_SpringValue			type:#float						default:0.0					ui:uiDistance_Spring_SpringValue			animatable:false
			Distance_Spring_SpringDamper		type:#float						default:0.0					ui:uiDistance_Spring_SpringDamper			animatable:false

			Pulley_Pulley1								type:#Node													ui:uiPulley_Pulley1								animatable:false
			Pulley_Pulley2								type:#Node													ui:uiPulley_Pulley2								animatable:false
			Pulley_Distance							type:#float						default:0.0					ui:uiPulley_Distance								animatable:false
			Pulley_Ratio								type:#float						default:1.0					ui:uiPulley_Ratio									animatable:false
			Pulley_Stiffness							type:#float						default:0.0					ui:uiPulley_Stiffness								animatable:false
			Pulley_IsRigid								type:#boolean					default:false				ui:uiPulley_IsRigid									animatable:false
			Pulley_HasMotor							type:#boolean					default:false				ui:uiPulley_HasMotor								animatable:false
			Pulley_Motor_MotorForce				type:#float						default:0.0					ui:uiPulley_Motor_MotorForce					animatable:false
			Pulley_Motor_MotorVelocity			type:#float						default:0.0					ui:uiPulley_Motor_MotorVelocity				animatable:false
			
			Free_XMotion								type:#integer					default:3					ui:uiFree_XMotion								animatable:false
			Free_YMotion 								type:#integer					default:3					ui:uiFree_YMotion								animatable:false
			Free_ZMotion 								type:#integer					default:3					ui:uiFree_ZMotion								animatable:false
			Free_LinearLimitValue					type:#float						default:0.0					ui:uiFree_LinearLimitValue						animatable:false
			Free_LinearLimitRestitution				type:#float						default:0.0					ui:uiFree_LinearLimitRestitution				animatable:false
			Free_LinearLimitSpring					type:#float						default:0.0					ui:uiFree_LinearLimitSpring						animatable:false
			Free_LinearLimitDamper					type:#float						default:0.0					ui:uiFree_LinearLimitDamper					animatable:false
			
			Free_TwistMotion 						type:#integer					default:3					ui:uiFree_TwistMotion							animatable:false
			Free_TwistLimitHighValue				type:#float						default:0.0					ui:uiFree_TwistLimitHighValue					animatable:false
			Free_TwistLimitLowValue				type:#float						default:0.0					ui:uiFree_TwistLimitLowValue					animatable:false
			Free_TwistLimitRestitution				type:#float						default:0.0					ui:uiFree_TwistLimitRestitution				animatable:false
			Free_TwistLimitSpring					type:#float						default:0.0					ui:uiFree_TwistLimitSpring						animatable:false
			Free_TwistLimitDamper					type:#float						default:0.0					ui:uiFree_TwistLimitDamper					animatable:false
			
			Free_Swing1Motion						type:#integer					default:3					ui:uiFree_Swing1Motion							animatable:false
			Free_Swing1LimitValue					type:#float						default:0.0					ui:uiFree_Swing1LimitValue					animatable:false
			Free_Swing1LimitRestitution			type:#float						default:0.0					ui:uiFree_Swing1LimitRestitution				animatable:false
			Free_Swing1LimitSpring					type:#float						default:0.0					ui:uiFree_Swing1LimitSpring					animatable:false
			Free_Swing1LimitDamper				type:#float						default:0.0					ui:uiFree_Swing1LimitDamper					animatable:false
			
			Free_Swing2Motion 						type:#integer					default:3					ui:uiFree_Swing2Motion							animatable:false
			Free_Swing2LimitValue					type:#float						default:0.0					ui:uiFree_Swing2LimitValue					animatable:false
			Free_Swing2LimitRestitution			type:#float						default:0.0					ui:uiFree_Swing2LimitRestitution				animatable:false
			Free_Swing2LimitSpring					type:#float						default:0.0					ui:uiFree_Swing2LimitSpring					animatable:false
			Free_Swing2LimitDamper				type:#float						default:0.0					ui:uiFree_Swing2LimitDamper					animatable:false
			
			Free_XLinearMotor						type:#integer					default:1					ui:uiFree_XLinearMotor							animatable:false
			Free_XLinearMotorForce				type:#float 					default:0.0					ui:uiFree_XLinearMotorForce					animatable:false
			Free_XLinearMotorSpring				type:#float 					default:0.0					ui:uiFree_XLinearMotorSpring					animatable:false
			Free_XLinearMotorDamper				type:#float 					default:0.0					ui:uiFree_XLinearMotorDamper				animatable:false
			
			Free_YLinearMotor						type:#integer					default:1					ui:uiFree_YLinearMotor							animatable:false
			Free_YLinearMotorForce				type:#float 					default:0.0					ui:uiFree_YLinearMotorForce					animatable:false
			Free_YLinearMotorSpring				type:#float 					default:0.0					ui:uiFree_YLinearMotorSpring					animatable:false
			Free_YLinearMotorDamper				type:#float 					default:0.0					ui:uiFree_YLinearMotorDamper				animatable:false
			
			Free_ZLinearMotor						type:#integer					default:1					ui:uiFree_ZLinearMotor							animatable:false
			Free_ZLinearMotorForce				type:#float						default:0.0					ui:uiFree_ZLinearMotorForce					animatable:false
			Free_ZLinearMotorSpring				type:#float						default:0.0					ui:uiFree_ZLinearMotorSpring					animatable:false
			Free_ZLinearMotorDamper				type:#float						default:0.0					ui:uiFree_ZLinearMotorDamper				animatable:false
			
			Free_AngularMotor						type:#integer					default:1					ui:uiFree_AngularMotor							animatable:false
			Free_AngularMotorForce				type:#float						default:0.0					ui:uiFree_AngularMotorForce					animatable:false
			Free_AngularMotorSpring				type:#float						default:0.0					ui:uiFree_AngularMotorSpring					animatable:false
			Free_AngularMotorDamper				type:#float						default:0.0					ui:uiFree_AngularMotorDamper				animatable:false

			Free_MotorPosition						type:#node													ui:uiFree_MotorTargetPosition				animatable:false
			Free_MotorTargetOrientation			type:#node													ui:uiFree_MotorTargetOrientation			animatable:false
			Free_MotorTargetLinearVelocity		type:#Point3					default:[0, 0, 0]																	animatable:false
			Free_MotorTargetAngularVelocity	type:#Point3					default:[0, 0, 0]																	animatable:false

		)
		
		rollout ZEPhysicalJointRollout "Physical Joint Parameters" rolledUp:false
		(
			/* PICK FILTERS */
			fn PhysicalBody_Filter obj = (isproperty obj "ZEMeshAttributes" == true and obj.PhysicalBody_Enabled == true)
			fn PointHelper_Filter obj = (classof obj) == Point and (superclassof obj) == helper
			fn DummyHelper_Filter obj = (classof obj) == Dummy and (superclassof obj) == helper
		
			
			/* LAYOUT DEFINITION */
			bitmap					uiLogo																				fileName:"zineklogo.bmp"
			dropdownlist			uiJoint_Type									"Joint Type:"					items:#("None", "Fixed", "Spherical", "Revolute", "Distance", "Prismatic", "Cylindrical", "PointOnLine", "PointInPlane", "Pulley", "Free")
			checkbox				uiJoint_Enabled								"Enabled"																																			enabled:(Joint_Type != 1)
			label						uiJoint_Body1_Label							"Body 1:"						Align:#left																										enabled:(Joint_Type != 1)																			
			pickbutton				uiJoint_Body1									"Pick"								autoDisplay:true		filter:PhysicalBody_Filter 				across:2								enabled:(Joint_Type != 1)	
			button					uiJoint_Body1_Clean							"Clean"																																				enabled:(Joint_Type != 1)
			label						uiJoint_Body2_Label							"Body 2:"						Align:#left																										enabled:(Joint_Type != 1)
			pickbutton				uiJoint_Body2									"Pick"								autoDisplay:true		filter:PhysicalBody_Filter				across:2								enabled:(Joint_Type != 1)
			button					uiJoint_Body2_Clean							"Clean"																																				enabled:(Joint_Type != 1)				
			
			checkbox				uiJoint_UseGlobalAnchorAxis				"Use Global Anchor/Axis"																														enabled:(Joint_Type != 1)	
			group "Local Achor Axis"
			(
				label					uiJoint_Body1_Anchor_Label				"Body 1 Anchor:"				Align:#left																	enabled:(Joint_Type != 1 and not Joint_UseGlobalAnchorAxis)
				pickbutton			uiJoint_Body1_Anchor						"Pick"								autoDisplay:true		filter:PointHelper_Filter		across:2		enabled:(Joint_Type != 1 and not Joint_UseGlobalAnchorAxis)
				button				uiJoint_Body1_Anchor_Clean				"Clean"																											enabled:(Joint_Type != 1 and not Joint_UseGlobalAnchorAxis)

				label					uiJoint_Body2_Anchor_Label				"Body 2 Anchor:"				Align:#left																	enabled:(Joint_Type != 1 and not Joint_UseGlobalAnchorAxis)
				pickbutton			uiJoint_Body2_Anchor						"Pick"								autoDisplay:true		filter:PointHelper_Filter		across:2		enabled:(Joint_Type != 1 and not Joint_UseGlobalAnchorAxis)
				button				uiJoint_Body2_Anchor_Clean				"Clean"																											enabled:(Joint_Type != 1 and not Joint_UseGlobalAnchorAxis)
			)
			
			checkbox 				uiJoint_CollideBodies							"Bodies Collide"																				enabled:(Joint_Type != 2 and Joint_Breakable)
			checkbox			 	uiJoint_Breakable								"Breakable"																						enabled:(Joint_Type != 2 and Joint_Breakable)
			spinner					uiJoint_Break_Force							"Break Force:"					type:#float													enabled:(Joint_Type != 2 and Joint_Breakable)
			spinner					uiJoint_Break_Torque						"Break Torque:"				type:#float													enabled:(Joint_Type != 2 and Joint_Breakable)

			group "Spherical Joint"
			(
				checkbox			uiSpherical_SwingLimit						"Enable SwingLimit"			type:#boolean												enabled:(Joint_Type == 3)
				spinner 			uiSpherical_SwingLimit_Value				"Limit Value:"					type:#float			range:[-180, 180, 1]			enabled:(Joint_Type == 3 and Spherical_SwingLimit)
				spinner				uiSpherical_SwingLimit_Restitution		"Limit Restution:"				type:#float			range:[0, 1, 0.01]				enabled:(Joint_Type == 3 and Spherical_SwingLimit)
				checkbox			uiSpherical_TwistLimit						"Enable Twist Limit"																			enabled:(Joint_Type == 3)
				spinner				uiSpherical_TwistLimit_LowValue			"Limit Low Value:"			type:#float			range:[-180, 180, 1]			enabled:(Joint_Type == 3 and Spherical_TwistLimit)
				spinner				uiSpherical_TwistLimit_HighValue			"Limit High Value:"			type:#float			range:[-180, 180, 1]			enabled:(Joint_Type == 3 and Spherical_TwistLimit)
				spinner				uiSpherical_TwistLimit_Restitution		"Limit Restitution:"			type:#float			range:[0, 1, 0.01]				enabled:(Joint_Type == 3 and Spherical_TwistLimit)
			)
			
			group "Revolute Joint"
			(
				checkbox 			uiRevolute_HasLimit							"Limited"																							enabled:(Joint_Type == 4 and Revolute_HasLimit)
				spinner 			uiRevolute_Limit_HighValue				"Limit High Value:"			type:#float			range:[-180, 180, 1]			enabled:(Joint_Type == 4 and Revolute_HasLimit)
				spinner 			uiRevolute_Limit_LowValue					"Limit Low Value:"			type:#float			range:[-180, 180, 1]			enabled:(Joint_Type == 4 and Revolute_HasLimit)
				spinner 			uiRevolute_Limit_Restitution				"Limit Restitution:"			type:#float			range:[0, 1, 0.01]				enabled:(Joint_Type == 4 and Revolute_HasLimit)
				checkbox 			uiRevolute_HasSpring						"Has Spring"																					enabled:(Joint_Type == 4)
				spinner 			uiRevolute_Spring_Value					"Spring Value:"				type:#float													enabled:(Joint_Type == 4 and Revolute_HasSpring)
				spinner 			uiRevolute_Spring_Damper					"Spring Damper:"				type:#float													enabled:(Joint_Type == 4 and Revolute_HasSpring)
				spinner 			uiRevolute_Spring_Target					"Spring Target:"				type:#float			range:[-180, 180, 1]			enabled:(Joint_Type == 4 and Revolute_HasSpring)
				checkbox 			uiRevolute_HasMotor						"Has Motor"																						enabled:(Joint_Type == 4)
				spinner 			uiRevolute_Motor_Force					"Motor Force:"					type:#float													enabled:(Joint_Type == 4 and Revolute_HasMotor)
				spinner 			uiRevolute_Motor_Velocity					"Motor Velocity:"				type:#float													enabled:(Joint_Type == 4 and Revolute_HasMotor)
			)	
			
			group "Distance Joint"
			(
				checkbox 			uiDistance_HasMinLimit						"Has Min. Limit"																				enabled:(Joint_Type == 5)
				spinner 			uiDistance_MinLimit_MinDistance			"Min. Distance:"				type:#float													enabled:(Joint_Type == 5 and uiDistance_HasMinLimit)
				checkbox 			uiDistance_HasMaxLimit						"Has Max. Limit:"																				enabled:(Joint_Type == 5)
				spinner 			uiDistance_MaxLimit_MaxDistance		"Max. Distance:"				type:#float													enabled:(Joint_Type == 5 and uiDistance_HasMaxLimit)
				checkbox 			uiDistance_HasSpring						"Has Spring"																					enabled:(Joint_Type == 5)
				spinner 			uiDistance_Spring_SpringValue			"Spiring Value:"				type:#float													enabled:(Joint_Type == 5 and uiDistance_HasSpring)
				spinner 			uiDistance_Spring_SpringDamper			"Spring Damper"				type:#float													enabled:(Joint_Type == 5 and uiDistance_HasSpring)
			)
			
			group "Pulley Joint"
			(
				label					uiPulley_Pulley1_Label						"Pulley 1"						across:3														enabled:(Joint_Type == 10)
				pickbutton 		uiPulley_Pulley1								"Pick"								autoDisplay:true		filter:PointHelper_Filter		enabled:(Joint_Type == 10)
				button				uiPulley_Pulley1_Clean						"Clean"																							enabled:(Joint_Type == 10)
				label					uiPulley_Pulley2_Label						"Pulley 2"						across:3														enabled:(Joint_Type == 10)
				pickbutton 		uiPulley_Pulley2								"Pick"								autoDisplay:true		filter:PointHelper_Filter		enabled:(Joint_Type == 10)
				button				uiPulley_Pulley2_Clean						"Clean"																							enabled:(Joint_Type == 10)
				spinner				uiPulley_Distance								"Distance:"						type:#float													enabled:(Joint_Type == 10)
				spinner				uiPulley_Ratio									"Ratio:"							type:#float													enabled:(Joint_Type == 10)
				spinner				uiPulley_Stiffness								"Stiffness:"						type:#float				range:[0, 1, 0.01]			enabled:(Joint_Type == 10)
				checkbox 			uiPulley_IsRigid								"Is Rigid"																							enabled:(Joint_Type == 10)
				checkbox			uiPulley_HasMotor							"Has Motor"																						enabled:(Joint_Type == 10)
				spinner				uiPulley_Motor_MotorForce				"Motor Force:"					type:#float													enabled:(Joint_Type == 10 and Pulley_HasMotor)
				spinner				uiPulley_Motor_MotorVelocity				"Motor Velocity"				type:#float													enabled:(Joint_Type == 10 and Pulley_HasMotor)
			)
			
			group "Free Joint"
			(
				dropdownlist 	uiFree_XMotion								"X Motion:"						items:#("Free", "Limited", "Locked")					enabled:(Joint_Type == 11)
				dropdownlist 	uiFree_YMotion								"Y Motion:"						items:#("Free", "Limited", "Locked")					enabled:(Joint_Type == 11)
				dropdownlist 	uiFree_ZMotion								"Z Motion:"						items:#("Free", "Limited", "Locked")					enabled:(Joint_Type == 11)
				spinner 			uiFree_LinearLimitValue						"Limit Value:"					type:#float													enabled:(Joint_Type == 11 and (Free_XMotion == 2 or Free_YMotion == 2 or Free_ZMotion == 2))
				spinner  			uiFree_LinearLimitRestitution				"Limit Restitution:"			type:#float				range:[0, 1, 0.01]			enabled:(Joint_Type == 11 and (Free_XMotion == 2 or Free_YMotion == 2 or Free_ZMotion == 2))
				spinner  			uiFree_LinearLimitSpring					"Limit Spring:"					type:#float													enabled:(Joint_Type == 11 and (Free_XMotion == 2 or Free_YMotion == 2 or Free_ZMotion == 2))
				spinner 			uiFree_LinearLimitDamper					"Limit Damper:"				type:#float													enabled:(Joint_Type == 11 and (Free_XMotion == 2 or Free_YMotion == 2 or Free_ZMotion == 2))
				
				dropdownlist 	uiFree_TwistMotion							"Twist Motion"					items:#("Free", "Limited", "Locked")					enabled:(Joint_Type == 11)
				spinner 			uiFree_TwistLimitLowValue					"Limit Low Value:" 			type:#float				range:[-180, 180, 1]		enabled:(Joint_Type == 11 and Free_TwistMotion == 2)
				spinner  			uiFree_TwistLimitHighValue				"Limit High Value:" 			type:#float				range:[-180, 180, 1]		enabled:(Joint_Type == 11 and Free_TwistMotion == 2)
				spinner 			uiFree_TwistLimitRestitution				"Limit Restitution:"			type:#float				range:[0, 1, 0.01]			enabled:(Joint_Type == 11 and Free_TwistMotion == 2)
				spinner 			uiFree_TwistLimitSpring						"Limit Spring:"					type:#float													enabled:(Joint_Type == 11 and Free_TwistMotion == 2)
				spinner 			uiFree_TwistLimitDamper					"Limit Damper:"				type:#float													enabled:(Joint_Type == 11 and Free_TwistMotion == 2)
				
				dropdownlist 	uiFree_Swing1Motion						"Swing 1 Motion"				items:#("Free", "Limited", "Locked")					enabled:(Joint_Type == 11)
				spinner  			uiFree_Swing1LimitValue					"Limit Value:"					type:#float				range:[-180, 180, 1]		enabled:(Joint_Type == 11 and Free_Swing1Motion == 2)
				spinner  			uiFree_Swing1LimitRestitution				"Limit Restitution:" 			type:#float				range:[0, 1, 0.01]		 	enabled:(Joint_Type == 11 and Free_Swing1Motion == 2)
				spinner  			uiFree_Swing1LimitSpring					"Limit Spring:"					type:#float													enabled:(Joint_Type == 11 and Free_Swing1Motion == 2)
				spinner  			uiFree_Swing1LimitDamper					"Limit Damper:"				type:#float													enabled:(Joint_Type == 11 and Free_Swing1Motion == 2)
				
				dropdownlist 	uiFree_Swing2Motion						"Swing 2 Motion"				items:#("Free", "Limited", "Locked")					enabled:(Joint_Type == 11)
				spinner  			uiFree_Swing2LimitValue					"Limit Value:"					type:#float				range:[-180, 180, 1]		enabled:(Joint_Type == 11 and Free_Swing2Motion == 2)
				spinner  			uiFree_Swing2LimitRestitution				"Limit Restitution:" 			type:#float				range:[0, 1, 0.01]			enabled:(Joint_Type == 11 and Free_Swing2Motion == 2)
				spinner  			uiFree_Swing2LimitSpring					"Limit Spring:"					type:#float													enabled:(Joint_Type == 11 and Free_Swing2Motion == 2)
				spinner  			uiFree_Swing2LimitDamper					"Limit Damper:"				type:#float													enabled:(Joint_Type == 11 and Free_Swing2Motion == 2)
				
				dropdownlist 	uiFree_XLinearMotor							"X Linear Motor"				items:#("Disabled", "Position", "Velocity")			enabled:(Joint_Type == 11)
				spinner  			uiFree_XLinearMotorForce					"Motor Force:"					type:#float													enabled:(Joint_Type == 11 and Free_XLinearMotor != 1)
				spinner  			uiFree_XLinearMotorSpring					"Motor Spring:"				type:#float													enabled:(Joint_Type == 11 and Free_XLinearMotor != 1)
				spinner  			uiFree_XLinearMotorDamper				"Motor Damper:" 				type:#float													enabled:(Joint_Type == 11 and Free_XLinearMotor != 1)
				
				dropdownlist 	uiFree_YLinearMotor							"Y Linear Motor"				items:#("Disabled", "Position", "Velocity")			enabled:(Joint_Type == 11)
				spinner  			uiFree_YLinearMotorForce					"Motor Force:"					type:#float													enabled:(Joint_Type == 11 and Free_YLinearMotor != 1)
				spinner 			uiFree_YLinearMotorSpring					"Motor Spring:"				type:#float													enabled:(Joint_Type == 11 and Free_YLinearMotor != 1)
				spinner  			uiFree_YLinearMotorDamper				"Motor Damper:" 				type:#float													enabled:(Joint_Type == 11 and Free_YLinearMotor != 1)
				
				dropdownlist 	uiFree_ZLinearMotor							"Z Linear Motor:"				items:#("Disabled", "Position", "Velocity")			enabled:(Joint_Type == 11)
				spinner  			uiFree_ZLinearMotorForce					"Motor Force:"					type:#float													enabled:(Joint_Type == 11 and Free_ZLinearMotor != 1)
				spinner  			uiFree_ZLinearMotorSpring					"Motor Spring:"				type:#float													enabled:(Joint_Type == 11 and Free_ZLinearMotor != 1)
				spinner  			uiFree_ZLinearMotorDamper				"Motor Damper:"				type:#float													enabled:(Joint_Type == 11 and Free_ZLinearMotor != 1)
				
				dropdownlist 	uiFree_AngularMotor							"Angular Motor:"				items:#("Disabled", "Orientation", "Velocity")		enabled:(Joint_Type == 11)
				spinner 			uiFree_AngularMotorForce					"Motor Force:"					type:#float													enabled:(Joint_Type == 11 and Free_AngularMotor != 1)
				spinner 			uiFree_AngularMotorSpring					"Motor Spring:"				type:#float													enabled:(Joint_Type == 11 and Free_AngularMotor != 1)
				spinner 			uiFree_AngularMotorDamper				"Motor Damper:"				type:#float													enabled:(Joint_Type == 11 and Free_AngularMotor != 1)
				
				label					uiFree_MotorTargetPosition_Label		"Target Position:"				align:#left													enabled:(Joint_Type == 11)	
				pickbutton			uiFree_MotorTargetPosition				"Pick"								filter:PointHelper_Filter		across:2					enabled:(Joint_Type == 11)
				button				uiFree_MotorTargetPosition_Clean		"Clean"																							enabled:(Joint_Type == 11)
				label					uiFree_MotorTargetOrientation_Label	"Target Orientation:"		align:#left													enabled:(Joint_Type == 11)	
				pickbutton			uiFree_MotorTargetOrientation			"Pick"								filter:PointHelper_Filter		across:2					enabled:(Joint_Type == 11)
				button				uiFree_MotorTargetOrientation_Clean	"Clean"																							enabled:(Joint_Type == 11)
				label					uiFree_MotorTargetVelocity_Label		"Target Velocity:"			align:#left													enabled:(Joint_Type == 11)
				spinner				uiFree_MotorTargetVelocity_X			"X:"								range:[-10000,10000, 0.1]							enabled:(Joint_Type == 11)
				spinner				uiFree_MotorTargetVelocity_Y			"Y:"								range:[-10000,10000, 0.1]							enabled:(Joint_Type == 11)
				spinner				uiFree_MotorTargetVelocity_Z			"Z:"								range:[-10000,10000, 0.1]							enabled:(Joint_Type == 11)				
				label			 		uiFree_MotorTargetAngularVelocity_Label	"Target Angl.Velocity:"		align:#left												enabled:(Joint_Type == 11)	
				spinner				uiFree_MotorTargetAngularVelocity_X	"X:"								range:[-10000,10000, 0.1]							enabled:(Joint_Type == 11)
				spinner				uiFree_MotorTargetAngularVelocity_Y	"Y:"								range:[-10000,10000, 0.1]							enabled:(Joint_Type == 11)
				spinner				uiFree_MotorTargetAngularVelocity_Z	"Z:"								range:[-10000,10000, 0.1]							enabled:(Joint_Type == 11)				
			)
			
			/* EVENTS */
			on uiJoint_Body1_Clean pressed do Joint_Body1 = undefined
			on uiJoint_Body2_Clean pressed do Joint_Body2 = undefined
			on uiJoint_Body1_Anchor_Clean pressed do Joint_Body1_Anchor = undefined
			on uiJoint_Body2_Anchor_Clean pressed do Joint_Body2_Anchor = undefined
			
			on uiPulley_Pulley1_Clean pressed do Pulley_Pulley1 = undefined
			on uiPulley_Pulley2_Clean pressed do Pulley_Pulley2 = undefined		
				
			on uiFree_MotorTargetPosition_Clean pressed do Free_MotorTargetPosition_Clean = undefined
			on uiFree_MotorTargetOrientation_Clean pressed do Free_MotorTargetOrientation_Clean = undefined		
				
			on uiFree_MotorTargetVelocity_X changed arg do Free_MotorTargetVeoloctiy_X = arg
			on uiFree_MotorTargetVelocity_Y changed arg do Free_MotorTargetVeoloctiy_Y = arg				
			on uiFree_MotorTargetVelocity_Z changed arg do Free_MotorTargetVeoloctiy_Z = arg	
				
			on uiFree_MotorTargetAngularVelocity_X changed arg do Free_MotorTargetAngularVeoloctiy_X = arg				
			on uiFree_MotorTargetAngularVelocity_X changed arg do Free_MotorTargetAngularVeoloctiy_Y = arg				
			on uiFree_MotorTargetAngularVelocity_X changed arg do Free_MotorTargetAngularVeoloctiy_Z = arg				

			/* LAYOUT MANAGEMENT */
			fn ManageLayout =
			(	
				uiJoint_Enabled.enabled 							= (Joint_Type != 1)
				uiJoint_Body1_Label.enabled 						= (Joint_Type != 1)																			
				uiJoint_Body1.enabled 								= (Joint_Type != 1)	
				uiJoint_Body1_Clean.enabled 						= (Joint_Type != 1)
				uiJoint_Body2_Label.enabled 						= (Joint_Type != 1)
				uiJoint_Body2.enabled 								= (Joint_Type != 1)
				uiJoint_Body2_Clean.enabled 						= (Joint_Type != 1)				
			
				uiJoint_UseGlobalAnchorAxis.enabled 			= (Joint_Type != 1)	
	
				uiJoint_Body1_Anchor_Label.enabled 			= (Joint_Type != 1 and not Joint_UseGlobalAnchorAxis)
				uiJoint_Body1_Anchor.enabled 					= (Joint_Type != 1 and not Joint_UseGlobalAnchorAxis)
				uiJoint_Body1_Anchor_Clean.enabled 			= (Joint_Type != 1 and not Joint_UseGlobalAnchorAxis)

				uiJoint_Body2_Anchor_Label.enabled 			= (Joint_Type != 1 and not Joint_UseGlobalAnchorAxis)
				uiJoint_Body2_Anchor.enabled 					= (Joint_Type != 1 and not Joint_UseGlobalAnchorAxis)
				uiJoint_Body2_Anchor_Clean.enabled 			= (Joint_Type != 1 and not Joint_UseGlobalAnchorAxis)

				uiJoint_CollideBodies	.enabled 					= (Joint_Type != 1 and Joint_Breakable)
				uiJoint_Breakable.enabled 							= (Joint_Type != 1 and Joint_Breakable)
				uiJoint_Break_Force.enabled 						= (Joint_Type != 1 and Joint_Breakable)
				uiJoint_Break_Torque.enabled 					= (Joint_Type != 1 and Joint_Breakable)
				
				uiSpherical_SwingLimit.enabled 					=	(Joint_Type == 3)
				uiSpherical_SwingLimit_Value.enabled 			=	(Joint_Type == 3 and Spherical_SwingLimit)
				uiSpherical_SwingLimit_Restitution.enabled 	=	(Joint_Type == 3 and Spherical_SwingLimit)
				uiSpherical_TwistLimit.enabled 					=	(Joint_Type == 3)
				uiSpherical_TwistLimit_LowValue.enabled 		=	(Joint_Type == 3 and Spherical_TwistLimit)
				uiSpherical_TwistLimit_HighValue.enabled 	=	(Joint_Type == 3 and Spherical_TwistLimit)
				uiSpherical_TwistLimit_Restitution.enabled 	=	(Joint_Type == 3 and Spherical_TwistLimit)
				
				uiRevolute_HasLimit.enabled  						=	(Joint_Type == 4)
				uiRevolute_Limit_HighValue.enabled 			=	(Joint_Type == 4 and Revolute_HasLimit)
				uiRevolute_Limit_LowValue.enabled 				=	(Joint_Type == 4 and Revolute_HasLimit)
				uiRevolute_Limit_Restitution.enabled 			=	(Joint_Type == 4 and Revolute_HasLimit)
				uiRevolute_HasSpring.enabled 					=	(Joint_Type == 4)
				uiRevolute_Spring_Value.enabled 				=	(Joint_Type == 4 and Revolute_HasSpring)
				uiRevolute_Spring_Damper.enabled 				=	(Joint_Type == 4 and Revolute_HasSpring)
				uiRevolute_Spring_Target.enabled 				=	(Joint_Type == 4 and Revolute_HasSpring)
				uiRevolute_HasMotor.enabled 					=	(Joint_Type == 4)
				uiRevolute_Motor_Force.enabled 				=	(Joint_Type == 4 and Revolute_HasMotor)
				uiRevolute_Motor_Velocity.enabled 				=	(Joint_Type == 4 and Revolute_HasMotor)

				uiDistance_HasMinLimit.enabled 					=	(Joint_Type == 5)
				uiDistance_MinLimit_MinDistance.enabled		=	(Joint_Type == 5 and Distance_HasMinLimit)
				uiDistance_HasMaxLimit.enabled 					=	(Joint_Type == 5)
				uiDistance_MaxLimit_MaxDistance.enabled 	=	(Joint_Type == 5 and Distance_HasMaxLimit)
				uiDistance_HasSpring.enabled 					=	(Joint_Type == 5)
				uiDistance_Spring_SpringValue.enabled 		=	(Joint_Type == 5 and Distance_HasSpring)
				uiDistance_Spring_SpringDamper.enabled 		=	(Joint_Type == 5 and Distance_HasSpring)

				uiPulley_Pulley1_Label.enabled 					=	(Joint_Type == 10)
				uiPulley_Pulley1.enabled 							=	(Joint_Type == 10)
				uiPulley_Pulley1_Clean.enabled 					=	(Joint_Type == 10)
				uiPulley_Pulley2_Label.enabled 					=	(Joint_Type == 10)
				uiPulley_Pulley2.enabled 							=	(Joint_Type == 10)
				uiPulley_Pulley2_Clean.enabled 					=	(Joint_Type == 10)
				uiPulley_Distance.enabled 							=	(Joint_Type == 10)
				uiPulley_Ratio.enabled 								=	(Joint_Type == 10)
				uiPulley_Stiffness.enabled 							=	(Joint_Type == 10)
				uiPulley_IsRigid.enabled 							=	(Joint_Type == 10)
				uiPulley_HasMotor.enabled 						=	(Joint_Type == 10)
				uiPulley_Motor_MotorForce.enabled 			=	(Joint_Type == 10 and Pulley_HasMotor)
				uiPulley_Motor_MotorVelocity.enabled 			=	(Joint_Type == 10 and Pulley_HasMotor)
				
				uiFree_XMotion.enabled 							= 	(Joint_Type == 11)
				uiFree_YMotion.enabled								= 	(Joint_Type == 11)
				uiFree_ZMotion.enabled								= 	(Joint_Type == 11)

				uiFree_LinearLimitValue.enabled					=	(Joint_Type == 11 and (Free_XMotion == 2 or Free_YMotion == 2 or Free_ZMotion == 2))
				uiFree_LinearLimitRestitution.enabled			=	(Joint_Type == 11 and (Free_XMotion == 2 or Free_YMotion == 2 or Free_ZMotion == 2))
				uiFree_LinearLimitSpring.enabled					=	(Joint_Type == 11 and (Free_XMotion == 2 or Free_YMotion == 2 or Free_ZMotion == 2))
				uiFree_LinearLimitDamper.enabled				=	(Joint_Type == 11 and (Free_XMotion == 2 or Free_YMotion == 2 or Free_ZMotion == 2))
				
				uiFree_TwistMotion.enabled						=	(Joint_Type == 11)
				uiFree_TwistLimitLowValue.enabled				=	(Joint_Type == 11 and Free_TwistMotion == 2)
				uiFree_TwistLimitHighValue.enabled				=	(Joint_Type == 11 and Free_TwistMotion == 2)
				uiFree_TwistLimitRestitution.enabled			=	(Joint_Type == 11 and Free_TwistMotion == 2)
				uiFree_TwistLimitSpring.enabled					=	(Joint_Type == 11 and Free_TwistMotion == 2)
				uiFree_TwistLimitDamper.enabled				=	(Joint_Type == 11 and Free_TwistMotion == 2)
				
				uiFree_Swing1Motion.enabled						=	(Joint_Type == 11)
				uiFree_Swing1LimitValue.enabled					=	(Joint_Type == 11 and Free_Swing1Motion == 2)
				uiFree_Swing1LimitRestitution.enabled			=	(Joint_Type == 11 and Free_Swing1Motion == 2)
				uiFree_Swing1LimitSpring.enabled				=	(Joint_Type == 11 and Free_Swing1Motion == 2)
				uiFree_Swing1LimitDamper.enabled				=	(Joint_Type == 11 and Free_Swing1Motion == 2)
				
				uiFree_Swing2Motion.enabled						=	(Joint_Type == 11)
				uiFree_Swing2LimitValue.enabled					=	(Joint_Type == 11 and Free_Swing2Motion == 2)
				uiFree_Swing2LimitRestitution.enabled			=	(Joint_Type == 11 and Free_Swing2Motion == 2)
				uiFree_Swing2LimitSpring.enabled				=	(Joint_Type == 11 and Free_Swing2Motion == 2)
				uiFree_Swing2LimitDamper.enabled				=	(Joint_Type == 11 and Free_Swing2Motion == 2)

				uiFree_XLinearMotor.enabled						=	(Joint_Type == 11)
				uiFree_XLinearMotorForce.enabled				=	(Joint_Type == 11 and Free_XLinearMotor != 1)
				uiFree_XLinearMotorSpring.enabled				=	(Joint_Type == 11 and Free_XLinearMotor != 1)
				uiFree_XLinearMotorDamper.enabled			=	(Joint_Type == 11 and Free_XLinearMotor != 1)
				
				uiFree_YLinearMotor.enabled						=	(Joint_Type == 11)
				uiFree_YLinearMotorForce.enabled				=	(Joint_Type == 11 and Free_YLinearMotor != 1)
				uiFree_YLinearMotorSpring.enabled				=	(Joint_Type == 11 and Free_YLinearMotor != 1)
				uiFree_YLinearMotorDamper.enabled			=	(Joint_Type == 11 and Free_YLinearMotor != 1)
						
				uiFree_ZLinearMotor.enabled						=	(Joint_Type == 11)
				uiFree_ZLinearMotorForce.enabled				=	(Joint_Type == 11 and Free_ZLinearMotor != 1)
				uiFree_ZLinearMotorSpring.enabled				=	(Joint_Type == 11 and Free_ZLinearMotor != 1)
				uiFree_ZLinearMotorDamper.enabled			=	(Joint_Type == 11 and Free_ZLinearMotor != 1)
				
				uiFree_AngularMotor.enabled						=	(Joint_Type == 11)
				uiFree_AngularMotorForce.enabled				=	(Joint_Type == 11 and Free_AngularMotor != 1)
				uiFree_AngularMotorSpring.enabled				=	(Joint_Type == 11 and Free_AngularMotor != 1) 
				uiFree_AngularMotorDamper.enabled			=	(Joint_Type == 11 and Free_AngularMotor != 1)
				
				uiFree_MotorTargetPosition_Label.enabled 				= 	(Joint_Type == 11)
				uiFree_MotorTargetPosition.enabled							=	(Joint_Type == 11)
				uiFree_MotorTargetPosition_Clean.enabled 				= 	(Joint_Type == 11)
				uiFree_MotorTargetOrientation_Label.enabled 			= 	(Joint_Type == 11)
				uiFree_MotorTargetOrientation.enabled					=	(Joint_Type == 11)
				uiFree_MotorTargetOrientation_Clean.enabled			= 	(Joint_Type == 11)		
				uiFree_MotorTargetVelocity_Label.enabled				=	(Joint_Type == 11)
				uiFree_MotorTargetVelocity_X.enabled						=	(Joint_Type == 11)
				uiFree_MotorTargetVelocity_Y.enabled						=	(Joint_Type == 11)
				uiFree_MotorTargetVelocity_Z.enabled						=	(Joint_Type == 11)				
				uiFree_MotorTargetAngularVelocity_Label.enabled		=	(Joint_Type == 11)
				uiFree_MotorTargetAngularVelocity_X.enabled			=	(Joint_Type == 11)
				uiFree_MotorTargetAngularVelocity_Y.enabled			=	(Joint_Type == 11)
				uiFree_MotorTargetAngularVelocity_Z.enabled			=	(Joint_Type == 11)		
			)
			
			on uiJoint_UseGlobalAnchorAxis changed arg do ManageLayout()
			on uiJoint_Breakable changed arg do ManageLayout()
			on uiJoint_Type selected arg do ManageLayout()
			on uiSpherical_SwingLimit changed arg do ManageLayout()
			on uiSpherical_TwistLimit changed arg do ManageLayout()
			on uiRevolute_HasLimit changed arg do ManageLayout()
			on uiRevolute_HasSpring changed arg do ManageLayout()
			on uiRevolute_HasMotor changed arg do ManageLayout()
			on uiDistance_HasMinLimit changed arg do ManageLayout()
			on uiDistance_HasMaxLimit changed arg do ManageLayout()
			on uiDistance_HasSpring changed arg do ManageLayout()
			on uiPulley_IsRigid changed arg do ManageLayout()
			on uiPulley_HasMotor changed arg do ManageLayout()
			on uiFree_XMotion selected arg do ManageLayout()
			on uiFree_YMotion selected arg do ManageLayout()
			on uiFree_ZMotion selected arg do ManageLayout()
			on uiFree_TwistMotion selected arg do ManageLayout()
			on uiFree_Swing1Motion selected arg do ManageLayout()
			on uiFree_Swing2Motion selected arg do ManageLayout()
			on uiFree_XLinearMotor selected arg do ManageLayout()
			on uiFree_YLinearMotor selected arg do ManageLayout()
			on uiFree_ZLinearMotor selected arg do ManageLayout()
			on uiFree_AngularMotor selected arg do ManageLayout()
		)
	)
	
	/* ADDING ATTRIBUTES */
	for Current in Selection do
	(
		if $ != undefined then
		(	
			if (isProperty $ "ZEPhysicalJointAttributes" == false and  classof Current == Point and superclassof Current == Helper)  then custAttributes.add $ PhysicalJointAttributes
		)
	)
)

macroScript ZEPhysicalJointAttributes_Remove
	category:"ZEModel"
	toolTip:"Remove Physical Joint Attributes"
	icon:#("ZEToolbarIcons", 5)
(	
	/* REMOVING ATTRIBUTES */
	for Current in Selection do
	(
		if Current != undefined then	
		(
			if isProperty Current "ZEPhysicalJointAttributes" == true then 
			(
				for i = 1 to custAttributes.count Current  do 
				(
					if (custAttributes.getdef Current i).name == #ZEPhysicalJointAttributes then custAttributes.delete Current i
				)
			)
		)
	)
)