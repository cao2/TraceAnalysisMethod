----How to Compile
	make combied
----How to RUN
	./combied path_to_trace_file x selection path_to_lpn_file
	path_to_trace_file----> can use trace1.txt
	x-------> file that will keep track of the memory usage
	selection----> signal selection options
				0--full observation
				1--s4 all datafile
				2--s2 all datafild
				3--s3 all datafiled
				4--s2 addr
				5--s3 addr
				6--s2 addr+tag
				7--s3 addr+tag
				8--s2 cmd+addr
				9--s3 cmd+addr
				10-s2 cmd+tag+addr
				11-s3 cmd+tag+addr
				12-s2 cmd+tag
				13-s3 cmd+tag
				15-s4 cmd+tag
				16-s4 cmd
				17-s4 cmd+addr
				18-s4 cmd+addr+tag
	path_to_lpn_file---> inside the folder called lpn_file
		mapping table between selection of lpn file
				0--full observation					lpn_file.txt
				1--s4 all data field				lpn_file4.txt
				2--s2 all data field				lpn_file2.txt
				3--s3 all data filed				lpn_file3.txt
				4--s2 addr							lpn_file2_2.txt
				5--s3 addr							lpn_file3_2.txt
				6--s2 addr+tag						lpn_file2_2.txt
				7--s3 addr+tag						lpn_file2_2.txt
				8--s2 cmd+addr						lpn_file2.txt
				9--s3 cmd+addr						lpn_file3.txt
				10-s2 cmd+tag+addr					lpn_file2.txt
				11-s3 cmd+tag+addr					lpn_file3.txt
				12-s2 cmd+tag						lpn_file2.txt
				13-s3 cmd+tag						lpn_file3.txt
				15-s4 cmd+tag						lpn_file4.txt
				16-s4 cmd							lpn_file4.txt
				17-s4 cmd+addr						lpn_file4.txt
				18-s4 cmd+addr+tag					lpn_file4.txt
----Sample Command:
./combied trace1.txt x 0 lpn_files/lpn_file.txt
./combied trace1.txt x 1 lpn_files/lpn_file4.txt
./combied trace1.txt x 2 lpn_files/lpn_file2.txt
./combied trace1.txt x 3 lpn_files/lpn_file3.txt
./combied trace1.txt x 4 lpn_files/lpn_file2_2.txt
./combied trace1.txt x 5 lpn_files/lpn_file3_2.txt
./combied trace1.txt x 6 lpn_files/lpn_file2_2.txt
./combied trace1.txt x 7 lpn_files/lpn_file3_2.txt
./combied trace1.txt x 8 lpn_files/lpn_file2.txt
./combied trace1.txt x 9 lpn_files/lpn_file3.txt
./combied trace1.txt x 10 lpn_files/lpn_file2.txt
./combied trace1.txt x 11 lpn_files/lpn_file3.txt
./combied trace1.txt x 12 lpn_files/lpn_file2.txt
./combied trace1.txt x 13 lpn_files/lpn_file3.txt
./combied trace1.txt x 15 lpn_files/lpn_file4.txt
./combied trace1.txt x 16 lpn_files/lpn_file4.txt
./combied trace1.txt x 17 lpn_files/lpn_file4.txt
./combied trace1.txt x 18 lpn_files/lpn_file4.txt
