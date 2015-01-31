
This is a small list of steps in order to build libbluray into a msvc DLL and lib file.

The project contains Release and Debug builds for static lib files (Debug/Release)
  as well as dynamic shared dll files (DebugDLL/ReleaseDLL).
Choose whichever project configuration meets your requirements.


*** Generating jar file ***

The build system requires the .jar file to be built automatically. This should have already been done and the resulting
source file being found in the SMC\src directory. If this file is missing then the project will try and automatically
generate a new one. If the file needs to be forced to update then it can be simply deleted which will result in new file
being generated the next time the project is built.

In order for a new file to be generated the project needs access to 'Java SDK' and 'Apache Ant'. Ensure that these are accessible by Visual Studio.

Java SDK can be downloaded from here:
http://www.oracle.com/technetwork/java/javase/downloads/index.html
Ant for Windows can be donwloaded from here:
http://ant.apache.org/bindownload.cgi

Pre-built windows binaries can also be found in the above link. To enable generation of new files ant should be extracted 
and added somewhere accessible to the Visual Studio path (e.g. extract it into JAVA_HOME) project folder.
The Java SDK should also bee installed and available through the 'JAVA_HOME' environment variable.