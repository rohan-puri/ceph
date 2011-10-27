=========================
 Planning a Ceph Cluster Configuration
=========================

.The following section contains guidelines for planning the deployment for a Ceph cluster configuration.  A Ceph cluster consists of the following core components:
•	Monitors – These must be an odd number, such as one, three, or five.  Three is the preferred configuration.
•	Object Storage Devices (OSD) – used as storage nodes
•	Metadata Servers (MDS)
For redundancy, you should employ several of these components.

Monitors
=========================
The monitors handle central cluster management, configuration, and state.  They are relatively lightweight daemons. Data is stored in a directory on a normal local file system, for example, ext3/4, btrfs or xfs.  The monitors also keep track of the list of Object Storage Devices (OSD). 
Hardware Requirements: 

•	A few gigs of local disk space 
•	A fixed network address 

.. image:: images/warning.png
 Warning: Never configure 2 monitors per cluster.  If you do, they will both have to be up all of the time, which will greatly degrade system performance. 


RADOS Object Storage Devices
===========
The OSDs store the actual data on the disks. A minimum of two is required. 
Hardware Requirements: 
•	As many disks as possible for faster performance and scalability
•	An SSD or NVRAM for a journal, or a RAID controller with a battery-backed NVRAM. 
•	Ample RAM for better file system caching 
•	Fast network 

Metadata Servers(MDS)
===
The metadata server daemon commands act as a distributed, coherent cache of file system metadata. They do not store data locally; all metadata is stored on disk via the storage nodes. 
Metadata servers can be added into the cluster on an as-needed basis.  The load is automatically balanced. The max_mds parameter controls how many cmds instances are active. Any additional running instances are put in standby mode and can be activated if one of the active daemons becomes unresponsive. 

Hardware Requirements:

•	Large amount of  RAM 
•	Fast CPU 
•	Fast (low latency) network 
•	At least two servers for redundancy and load balancing
TIPS: If you have just a few nodes, put cmon, cmds, and cosd on the same node.  For moderate node configurations, put cmon and cmds together, and cosd on the disk nodes.  For large node configurations, put cmon, cmds, and cosd each on their own dedicated machine. 


