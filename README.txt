----How to Compile
	make combied
----How to RUN
	bin/combied path_to_trace_file type_of_scenario selection path_to_lpn_file
	path_to_trace_file----> can use trace1.txt
	type_of_scenario-------> type of execution scenario
	selection----> signal selection options
				0--full observation
				1--s4 all datafile
				2--s2 all datafild
				3--s3 all datafiled
				4--s2 id
				5--s3 id
				6--s2 id+tag
				7--s3 id+tag
				8--s2 cmd+id
				9--s3 cmd+id
				10-s2 cmd+tag+id
				11-s3 cmd+tag+id
				12-s2 cmd+tag
				13-s3 cmd+tag
				15-s4 cmd+tag
				16-s4 cmd
				17-s4 cmd+id
				18-s4 cmd+id+tag
				19-s4 add+tag
				20-s4 uniq: cmd+id shared: tag+id
	path_to_lpn_file---> inside the folder called lpn_file
		mapping table between selection of lpn file
				0--full observation					lpn_file.txt
				1--s4 all data field					lpn_file4.txt
				2--s2 all data field					lpn_file2.txt
				3--s3 all data filed					lpn_file3.txt
				4--s2 uniq: cmd+id shared: tag+id			lpn_file2_3.txt
				5--s3 uniq: cmd+id shared: tag+id			lpn_file3_3.txt
				6--s2 id+tag						lpn_file2_2.txt
				7--s3 id+tag						lpn_file2_2.txt
				8--s2 cmd+id						lpn_file2.txt
				9--s3 cmd+id						lpn_file3.txt
				10-s2 cmd+tag+id					lpn_file2.txt
				11-s3 cmd+tag+id					lpn_file3.txt
				12-s2 cmd+tag						lpn_file2.txt
				13-s3 cmd+tag						lpn_file3.txt
				15-s4 cmd+tag						lpn_file4.txt
				16-s4 cmd						lpn_file4.txt
				17-s4 cmd+id						lpn_file4.txt
				18-s4 cmd+id+tag					lpn_file4.txt
				19-s4 id+tag						lpn_file4_2.txt
				20-s4 uniq: cmd+id shared: tag+id			lpn_file4_3.txt
----Sample Command:
bin/combied trace1.txt 3 0 examples/lpn_files/lpn_file.txt
bin/combied trace1.txt 3 1 examples/lpn_files/lpn_file4.txt
bin/combied trace1.txt 3 2 examples/lpn_files/lpn_file2.txt
bin/combied trace1.txt 3 3 examples/lpn_files/lpn_file3.txt
bin/combied trace1.txt 3 4 examples/lpn_files/lpn_file2_3.txt
bin/combied trace1.txt 3 5 examples/lpn_files/lpn_file3_3.txt
bin/combied trace1.txt 3 6 examples/lpn_files/lpn_file2_2.txt
bin/combied trace1.txt 3 7 examples/lpn_files/lpn_file3_2.txt
bin/combied trace1.txt 3 8 examples/lpn_files/lpn_file2.txt
bin/combied trace1.txt 3 9 examples/lpn_files/lpn_file3.txt
bin/combied trace1.txt 3 10 examples/lpn_files/lpn_file2.txt
bin/combied trace1.txt 3 11 examples/lpn_files/lpn_file3.txt
bin/combied trace1.txt 3 12 examples/lpn_files/lpn_file2.txt
bin/combied trace1.txt 3 13 examples/lpn_files/lpn_file3.txt
bin/combied trace1.txt 3 15 examples/lpn_files/lpn_file4.txt
bin/combied trace1.txt 3 16 examples/lpn_files/lpn_file4.txt
bin/combied trace1.txt 3 17 examples/lpn_files/lpn_file4.txt
bin/combied trace1.txt 3 18 examples/lpn_files/lpn_file4.txt
bin/combied trace1.txt 3 19 examples/lpn_files/lpn_file4_2.txt
bin/combied trace1.txt 3 20 examples/lpn_files/lpn_file4_3.txt
