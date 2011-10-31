.. index:: RBD

============================
Ceph Installation Procedures
============================

Ceph Installation consists of the following procedures:

#. Installing and Compiling Ceph using ``mkcephfs``
#.	Obtaining the tar file.
#.	Retrieving the source code from Git for the Linux Kernel for the server
#.	Configuring the Ceph Cluster
#.	Creating the file system
#.	Starting Ceph
#.	Mounting the file system 

Before installing Ceph, check to make sure the dependencies have been installed.  If they have not, execute the procedure shown in the next section.

After you have finished installing Ceph, you must configure it using the Ceph Chef Cookbooks.  See the Configuring Ceph with the Chef Cookbooks for more details. 

--------------------
Ceph Dependencies
--------------------
To build Ceph from the source code, you must have the following packages installed on your Linux machine:
 
*	autotools-dev 
* autoconf 
*	automake 
*	cdbs 
*	g++ 
*	gcc 
*	git 
*	libatomic-ops-dev 
*	libboost-dev 
*	libcrypto++-dev 
*	libcrypto++ 
*	libedit-dev 
*	libexpat1-dev 
*	libfcgi-dev 
*	libfuse-dev 
*	libgoogle-perftools-dev 
*	libgtkmm-2.4-dev 
*	libtool 
*	pkg-config 

==================
Installing Ceph  
=================

Ceph uses Git for version control, so you will need a git login to proceed.

#.	Login to your Linux machine.
#.	Retrieve the latest version of Ceph using the following command:

``Git clone fit://ceph.newdream.net/ceph.git``
The screen displays the following:

.. image:: images/CephInstall1.jpg

#.	Change directories into Ceph using ``cd ceph``.
#.	Next, build the server daemons and the FUSE client. Compile Ceph using the following command:
``./autogen.sh``

#.	The compiler will install the files as shown in the following screen:
 
.. image:: images/CephInstall2.jpg

#.	Configure the files using the following command: ./configure
#.	The screen displays the configuration status:
 
#.	Make the libraries.  Insert the number of core files after the ``–j`` as shown in the following command:

$ ``make –jn``

#.	It should take several minutes to complete compiling.  When the compile has completed, the screen will return the shell prompt:

.. image:: images/CephInstall3.jpg
 

..NOTE::

The FUSE client will only be built if libfuse is present.
The following binaries files are built into the /src directory:

---------------------
Installing Tar Files
---------------------

Download the latest version of the tar file from the http://ceph.newdream.net/download/ directory.
Unzip the tar file using the following command:
``tar -xvfz file.tar.gz``

-----------------------------------------------
Retrieving the Source Code for the Linux Kernel
-----------------------------------------------
Ceph uses Git for version control of the Linux Kernel files. The Linux Kernel version 2.6.34 already includes the Ceph kernel client.   The kernel must be upgraded to version 2.6.27 or later. 
There are two Git trees: 

*	ceph-client.git contains the full kernel and the latest code, but will generally only build on very recent (or the latest) kernel. 
*	ceph-client-standalone.git contains the Ceph module source only, and includes branches with backport #ifdefs back through 2.6.27. It mirrors the code in the main ceph-client.git, but may lag behind by a few days. 

Use the ``ceph-client-standalone.git`` source code for updates. 

#.	Login to https://git.newdream.net/
#.	Retrieve the code using the following commands: 

``git clone git://ceph.newdream.net/git/ceph-client-standalone.git
git branch master-backport origin/master-backport``

#.	After retrieving the code, build the kernel using the following commands: 

``git checkout master-backport``

..NOTE::
If you are not using the latest mainline kernel, execute the following: 
``make -C libceph``                

#. Next run the build against the running kernel.            
``cp libceph/Module.symvers ceph/``
``make -C ceph``
``make KERNELDIR=/path/to/linux  -C libceph`` 

* You can also run it against another kernel as shown in the following command:
``$ cp libceph/Module.symvers ceph/ 
$ make KERNELDIR=/path/to/linux -C ceph``
#.	Load the module using the following commands: 
``insmod ./libceph/libceph.ko``
``insmod ./ceph/ceph.ko``
* You can also load the module using the following commands: 
``make modules_install
$ depmod
$ modprobe libceph
$ modprobe ceph``
#.	Determine what IP address the monitor is running on.
#.	Enter the following command to mount Ceph: 
``$ mount -t ceph 1.2.3.4:/ /mnt/ceph``
If ``mount.ceph`` is installed in /sbin, you can use a host name in place of an IP address. 
 	TIP: The mount command supports a Round Robin DNS record. If you are installing a large cluster, you can create multiple DNS A records and the mount command will use all the IP addresses that it returns as monitor addresses. 


