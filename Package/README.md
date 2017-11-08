# Package Information

This folder contains the files to generate a debian package of this repository.

In the **InstallScripts** folder the *postinst* and *prerm* scripts can be found.

In the **DockerPackageGeneration** a readme with the instructions fo generating the debian package with docker can be found.

## Generate debian package without Docker

To generate the debian packages without docker you need **cmake** and **build-essential** packages.

`sudo apt-get install build-essential cmake`

After that create a temporally folder

`mkdir build && cd build`

In this folder call the cmake command

`cmake ..`

An when cmake has exited call the make to generate the package.

`make package`

This will result in a file with name aDTNPlus-version-revision_processor.deb