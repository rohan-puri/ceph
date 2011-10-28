
======================================
Planning a Ceph Cluster Configuration
======================================

The following section contains guidelines for planning the deployment for a Ceph cluster configuration.  A Ceph cluster consists of the following core components:

* Monitors. These must be an odd number, such as one, three, or five.  Three is the preferred configuration.
* Object Storage Devices (OSD).  These are used as storage nodes
* Metadata Servers (MDS).  These are optional depending on the configuration.
For redundancy, you should employ several of these components.

--------------------
Monitors
--------------------
The monitors handle central cluster management, configuration, and state.  They are relatively lightweight daemons. Data is stored in a directory on a normal local file system, for example, 
ext3/4, btrfs or xfs.  The monitors also keep track of the list of Object Storage Devices (OSD). 

--------------------
Hardware Requirements:
--------------------

#. At least 10 gigs of local disk space 
#. A fixed network address 

.. image:: images/warning.jpg
.. Warning::

Never configure 2 monitors per cluster.  If you do, they will both have to be up all of the time, which will greatly degrade system performance. 


--------------------
RADOS Object Storage Devices
--------------------
The RADOS OSDs store the actual data on the disks. A minimum of two is required. 

--------------------
Hardware Requirements
--------------------
#.	As many disks as possible for faster performance and scalability
#.	At least one SSD or NVRAM for a journal, or a RAID controller with a battery-backed NVRAM. 
#.	Ample RAM for better file system caching 
#.	Fast network 

--------------------
Metadata Servers (MDS)
--------------------
The metadata server daemon commands act as a distributed, coherent cache of file system metadata. They do not store data locally; all metadata is stored on disk via the storage nodes. 
Metadata servers can be added into the cluster on an as-needed basis.  The load is automatically balanced. The max_mds parameter controls how many cmds instances are active. Any additional running instances are put in standby mode and can be activated if one of the active daemons becomes unresponsive. 

--------------------
Hardware Requirements
--------------------

#.	Large amount of  RAM 
#.	Fast CPU 
#.	Fast, low latency network 
#.	At least two servers for redundancy and load balancing

.. TIPS:: 
If you have just a few nodes, put cmon, cmds, and cosd on the same node.  For moderate node configurations, put cmon and cmds together, and cosd on the disk nodes.  
For large node configurations, put cmon, cmds, and cosd each on their own dedicated machine. 

=====================
Installing Ceph 
=====================

Ceph Installation consists of the following procedures:

#.	Installing and Compiling Ceph using mkcephfs
#.	Obtaining the tar file.
#.	Retrieving the source code from Git for the Linux Kernel for the server
#.	Configuring the Ceph Cluster
#.	Creating the file system
#.	Starting Ceph
#.	Mounting the file system 

Before installing Ceph, check to make sure the dependencies have been installed.  If they have not, execute the procedure shown in the next section.

After you have finished installing Ceph, you must configure it using the Ceph Chef Cookbooks.  See the Configuring Ceph with the Chef Cookbooks for more details. 

Ceph Dependencies
=====================
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
 

