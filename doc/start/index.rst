****************
Getting Started
****************


This document contains procedures for a new customer to configure a Ceph System.  
=================
Before You Begin
=================
Before you begin configuring your system for Ceph, use the following checklist to decide what type of system you need.

#. Identify the amount of storage that you need based on your current data, network traffic, workload, and other parameters
#. Identify the growth potential for your business so that you can project ahead for future storage needs.
#. Plan ahead for redundancy and replacement options.
#. Study market forecasts and how they affect your business. 

------------------------
Preparing a Ceph Cluster
-----------------------
A Ceph cluster consists of the following core components:

1.	Monitors. These must be an odd number, such as one, three, or five.  Three is the preferred configuration.
2.	Object Storage Devices (OSD) which are used as storage nodes
3.	Metadata Servers (MDS).  These devices are optional. 

Although Ceph is extremely scalable, and nodes can be added any time on an as-needed basis, it is important to first determine the base needs of your configuration prior to setting up your system.  This will save time and money in the long run. 


.. image:: ../images/warning.jpg
..Warning::
  If you are using a low bandwidth system, and are connecting to the cluster over the internet, you must use the ``librados`` object level interface, and your OSDs must be located in 
  the same data center.
