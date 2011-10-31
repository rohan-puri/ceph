=======================
 Using RADOS
=======================

.. highlight:: python

.. index:: RADOS, object

Introduction
============

The Reliable Autonomic Distributed Object Store (RADOS) provides a scalable object storage management platform.  RADOS allows the Object Storage Devices (OSD) to operate autonomously when 
recovering from failures or migrating data to expand clusters.   
RADOS has a gateway that is used to facilitate using the Representational State Transfer (REST) interface to the Ceph RADOS cluster.  The RADOS Gateway uses fastcgi.
.

The OSDs store the actual data on the disks. A minimum of two is required.  The OSDs must contain the following:

- a `name`: a sequence of bytes, unique within its container, that is
  used to locate and access the object
- `content`: sequence of bytes
- `metadata`: a mapping from keys to values, for example ``color:
  blue, importance: low``

None of these have any prescribed meaning to Ceph, and can be freely
chosen by the user.

RADOS exemplifies the latest and greatest technology for petabyte storage devices.  It  effectively maintains proper levels of node replication while balancing the distribution of data, 
totally transparent to the user.   Metadata bottlenecks are effectively eliminated with the RADOS cluster mapping feature, which uses cutting edge Placement Group (PG) layouts. 
The following table outlines these features and corresponding benefits


=======================    =====  
Feature                    Benefit
=======================    =====  
Performance                Devices fail and recover continuously with no effect on the user experience
Balanced---Distribution    Data and workload is distributed evenly across the nodes
Scalability                Storage systems can grow and contract seamlessly over many thousands of devices
=======================    ===== 

RADOS Placement Groups
============

The RADOS Placement Groups (PG) use the CRUSH algorithm to distribute data and load throughout the nodes. The following figure shows how the RADOS Placement Groups interact with OSDs.

.. image:: ../images/RADOSPlacementGroups.jpg
Setup
=====

First, we need to create a `pool` that will hold our assets. Follow
the instructions in :ref:`create-new-pool`. Let's name the pool
``assets``.

Then, we need a ``client`` key that is authorized to access that
pool. Follow the instructions in :ref:`add-new-key`. Let's set the
``id`` of the key to be ``webapp``. You could set up one key per
machine running the web service, or let them share a single key; your
call. Make sure the keyring containing the new key is available on the
machine running the asset management system.

Then, authorize the key to access the new pool. Follow the
instructions in :ref:`auth-pool`.


Usage
=====

`RADOS` is accessed via a network protocol, implemented in the
:doc:`/api/librados` and :doc:`/api/libradospp` libraries. There are
also wrappers for other languages.

.. todo:: link to python, phprados here

Instead of a low-level programming library, you can also use a
higher-level service, with user accounts, access control and such
features, via the :ref:`radosgw` HTTP service. See :doc:`/ops/radosgw`
for more.


.. rubric:: Example: Asset management

Let's say we write our asset management system in Python. We'll use
the ``rados`` Python module for accessing `RADOS`.

.. todo:: link to rados.py, where ever it'll be documented

With the key we created in Setup_, we'll be able to open a RADOS
connection::

	import rados

	r=rados.Rados('webapp')
	r.conf_read_file()
	r.connect()

	ioctx = r.open_ioctx('assets')

and then write an object::

	# holding content fully in memory to make the example simpler;
	# see API docs for how to do this better
	ioctx.write_full('1.jpg', 'jpeg-content-goes-here')

and read it back::

	# holding content fully in memory to make the example simpler;
	# see API docs for how to do this better
	content = ioctx.write_full('1.jpg')


We can also manipulate the metadata related to the object::

	ioctx.set_xattr('1.jpg', 'content-type', 'image/jpeg')


Now you can use these as fits the web server framework of your choice,
passing the ``ioctx`` variable from initialization to the request
serving function.
