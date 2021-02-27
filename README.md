## Open ITRON Specification based Kernel

   This is a real-time kernel I developed while I was a researcher
for the Industrial Informatics Group of the Politechnical University of 
Valencia. This kernel implements the Standard Profile of the ITRON specification.
At this point, the only architectura supported is the Blackfin, more specifically
the BF548 processor. However, with minimal modifications on the code you can port 
it to any single core Blackfin model.
   
   The blackfin toolchain is needed to compile and run applications. For the
compilation, the bfin-elf bare metal toolchain provided by blackfin.uclinux.com
has been used.

   Developing applications with this kernel implies some knowledge about the
architecture. Three steps are needed for application developement:


###   Step 1:  Configure the kernel

   The code comes with the menuconfig application, which is widely used for Linux
kernel configuration. Configuration options reflect the kernel features, so just
check them out to have an overview of the capabilities of the software.

###   Step 2:  Compile the kernel

   Compile the kernel with your toolchain. Makefiles are provided to simplify the task.
The result of the compilation will be an object file "itron.o" on the directory $ITRON/bin

###   Step 3:  Link the kernel with the application

   To link the kernel with the applications, two default Linker Description Scripts have been 
provided on directory $ITRON/bin. One of them also links the lwIP TCP/IP stack should the
application need make use of it.



