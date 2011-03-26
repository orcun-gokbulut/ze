Filename = getSaveFileName caption:"zeCanvas File" types:"zeCanvas Files (*.zeCanvas)|*.zeCanvas|All Files (*.*)|*.*|"
File = openFile Filename mode:"w"
if File != undefined then
(
	VertexCount = 0;

	for Cur in geometry do VertexCount += (getNumFaces Cur.mesh) * 3
	format "%\n" (VertexCount as integer) to:File

	for Cur in geometry do
	(
		obj = snapshotAsMesh Cur
		for i = 1 to getNumFaces obj do
		(
			Face = getFace obj i

			v1 = (meshop.getVert obj Face.z)
			v2 = (meshop.getVert obj Face.y) 
			v3 = (meshop.getVert obj Face.x) 
			
			Normals = meshop.getFaceRNormals  obj i node:Cur

			n1 = Normals[3]
			n2 = Normals[2]
			n3 = Normals[1]
			
			TFace = getTVFace obj i
			t1 = (getTVert  obj TFace.z)
			t2 = (getTVert  obj TFace.y) 
			t3 = (getTVert  obj TFace.x) 

			format "% % % " (formattedPrint v1.x format:".6f") (formattedPrint v1.z format:".6f") (formattedPrint v1.y format:".6f") to:File
			format "% % % " (formattedPrint n1.x format:".6f") (formattedPrint n1.z format:".6f") (formattedPrint n1.y format:".6f") to:File
			format "% %\n" (formattedPrint t1.x format:".6f") (formattedPrint -t1.y format:".6f") to:File
			format "% % % " (formattedPrint v2.x format:".6f") (formattedPrint v2.z format:".6f") (formattedPrint v2.y format:".6f") to:File
			format "% % % " (formattedPrint n2.x format:".6f") (formattedPrint n2.z format:".6f") (formattedPrint n2.y format:".6f") to:File
			format "% %\n" (formattedPrint t2.x format:".6f") (formattedPrint -t2.y format:".6f") to:File
			format "% % % " (formattedPrint v3.x format:".6f") (formattedPrint v3.z format:".6f") (formattedPrint v3.y format:".6f") to:File
			format "% % % " (formattedPrint n3.x format:".6f") (formattedPrint n3.z format:".6f") (formattedPrint n3.y format:".6f") to:File
			format "% %\n" (formattedPrint t3.x format:".6f") (formattedPrint -t3.y format:".6f") to:File
		)
	)
	close File
)
else
	print "Error"