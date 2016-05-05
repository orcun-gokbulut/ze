utility ZEMeshBatchUtility "ZEModel Mesh Batch Ops"
(
		/* LAYOUT DEFINITION */
		bitmap					uiLogo								fileName:"zineklogo.bmp"
		spinner 				uiMesh_LOD_Batch					"LOD Level:" 					type:#integer												toolTip:"Indicates the Level of Detail order."
		spinner 				uiMesh_LOD_Start_Distance_Batch		"LOD Start Distance:" 			type:#integer				range:[0,100000,0]				toolTip:"Indicates starting drawing distance for the LOD Level."
		spinner 				uiMesh_LOD_End_Distance_Batch		"LOD End Distance:" 			type:#integer				range:[0,100000,0]				toolTip:"Indicates ending drawing distance for the LOD Level."
		checkbox 				uiMesh_Visibility_Batch 			"Is Visible"					checked:true												toolTip:"Indicates whether the mesh is visible or not."
	
	
	on uiMesh_LOD_Batch changed NewValue do
	(
		for Current in selection do
		(
			if Current != undefined then
			(	
				if isProperty Current "ZEMeshAttributes" == true and superclassof Current == GeometryClass do 
				(
					Current.ZEMeshAttributes.Mesh_LOD = NewValue
				)
			)
		)
	)
	
	on uiMesh_LOD_Start_Distance_Batch changed NewValue do
	(
		for Current in selection do
		(
			if Current != undefined then
			(	
				if isProperty Current "ZEMeshAttributes" == true and superclassof Current == GeometryClass do 
				(
					Current.ZEMeshAttributes.Mesh_LOD_Start_Distance = NewValue
				)
			)
		)
	)
	
	on uiMesh_LOD_End_Distance_Batch changed NewValue do
	(
		for Current in selection do
		(
			if Current != undefined then
			(	
				if isProperty Current "ZEMeshAttributes" == true and superclassof Current == GeometryClass do 
				(
					Current.ZEMeshAttributes.Mesh_LOD_End_Distance = NewValue
				)
			)
		)
	)
	
	on uiMesh_Visibility_Batch changed NewValue do
	(
		for Current in selection do
		(
			if Current != undefined then
			(	
				if isProperty Current "ZEMeshAttributes" == true and superclassof Current == GeometryClass do 
				(
					Current.ZEMeshAttributes.Mesh_Visibility = NewValue
				)
			)
		)
	)
) -- end of utility definition 