# Generating multiple debs with Docker

In this folder exist a docker-compose to generate the debian package for multiple distributions and arch.

Each folder contains a dockerfile with the image generator, and the docker-compose has a run command to copy the generated deb to the **/pkg** folder.

To run this deb generators you must [install Docker](https://docs.docker.com/engine/installation/).

After installing docker you must install docker-compose as well.

`sudo apt-get install docker-compose`

To generate **arm** versions **qemu** must be installed also.

`sudo apt-get install qemu qemu-user-static qemu-user binfmt-support`

With all this installed calling

`sudo docker-compose build`

Will generate all the images, and to retrieve one of the generated debs

`sudo docker-compose arm6_stretch run`

All the same arch version generate the same name of the package, so multiple calls of **run** for different distributions will cause overwrite.