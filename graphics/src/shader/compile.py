import sys
import os

if len(sys.argv) == 1:
	print("Error: at least the first argument should be a path to shader files")
	exit()
	
# go through all directories provided as arguments
for i in range(1, len(sys.argv)):
	
	base_dir = sys.argv[i]
	print("building shader source directory: " + base_dir)
	
	# create a directory for the binaries
	bin_dir = base_dir + "/bin"
	if not os.path.exists(bin_dir):
		os.makedirs(bin_dir)

	# find all glsl source files in the base_dir
	glsl_source_files=[]
	for file in os.listdir(base_dir):
		if file.endswith(".vert") or file.endswith(".frag"):
		    glsl_source_files.append(file) # contains only the file name, not the path

	# compile all the source files
	for file in glsl_source_files:
		input_file = base_dir + "/" + file
		bin_file = bin_dir + "/" + file + ".spv"
		print("  Building Shader: " + bin_file)
		cmd = "glslc -c " + input_file + " -o " + bin_file # build the command that compiles the glsl source files
		print("    running command: " + cmd)
		os.system(cmd) # execute the command
