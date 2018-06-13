#  Makefile to prepare and compile the IBRDTN_Adapter project
#  targets:
#      all          build a version of IBRDTN_Adapter with symbolic links
#      clean        remove generated directories

VERSION=1.0

#====================================================================
#  Main targets
#====================================================================

#  Build a version of IBRDTN_Adapter
all: build

#  Remove the libs/ and target/ directories
clean:
	rm -rf libs/ target/

#====================================================================
#  Intermediate targets
#====================================================================

#  Build the maven project and create a link to the resulting JAR file
#  in libs
build: clean
	@mvn install
	@mkdir -p libs
	@ln -f -s ../target/adtnPlus-adapter-${VERSION}.jar libs/
