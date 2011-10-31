.. index:: RBD

=====================
RADOS Block Device
=====================
This document describes the features and benefits provided by the RADOS Block Device feature of Ceph.  The audience for this document includes sales and marketing personnel, new customers, 
and all persons who need to get a basic overview of the features and functionality of the system


The RADOS Block Device (RBD) provides a block device interface to a Linux machine.  
To the user, RDB is transparent, which means that the entire Ceph system looks like a single, limitless hard drive that is always up and has no size limitations.  .


Installation
============

To use `RBD`, you need to install a Ceph cluster. Follow the
instructions in :doc:`/ops/install/index`. Continue with these
instructions once you have a healthy cluster running.


Setup
=====

The default `pool` used by `RBD` is called ``rbd``. It is created for
you as part of the installation. If you wish to use multiple pools,
for example for access control, see :ref:`create-new-pool`.

First, we need a ``client`` key that is authorized to access the right
pool. Follow the instructions in :ref:`add-new-key`. Let's set the
``id`` of the key to be ``bar``. You could set up one key per machine
using `RBD`, or let them share a single key; your call. Make sure the
keyring containing the new key is available on the machine.

Then, authorize the key to access the new pool. Follow the
instructions in :ref:`auth-pool`.


RBD Snapshots
=============
RBD provides the ability to create snapshots of any image for backup purposes.  These images can then be exported to any file on the cluster.  
The following drawing shows how this feature can be used to create clones of an image called the Golden Master Snapshot.  
These clones can then be ready to be used as backups any time an image goes down or needs to be duplicated for a new configuration

`RBD` can be accessed in two ways:

- as a block device on a Linux machine
- via the ``rbd`` network storage driver in Qemu/KVM


.. rubric:: Example: As a block device

Using the ``client.bar`` key you set up earlier, we can create an RBD
image called ``tengigs``::

	rbd --name=client.bar create --size=10240 tengigs

And then make that visible as a block device::

	touch secretfile
	chmod go= secretfile
	ceph-authtool --name=bar --print-key /etc/ceph/client.bar.keyring >secretfile
	rbd map tengigs --user bar --secret secretfile

.. todo:: the secretfile part is really clumsy

For more information, see :doc:`rbd </man/8/rbd>`\(8).


.. rubric:: Example: As a Qemu/KVM storage driver via Libvirt

You'll need ``kvm`` v0.15, and ``libvirt`` v0.8.7 or newer.

Create the RBD image as above, and then refer to it in the ``libvirt``
virtual machine configuration::

    <disk type='network' device='disk'>
      <source protocol='rbd' name='rbd/tengigs'>
          <host name='10.0.0.101' port='6789'/>
          <host name='10.0.0.102' port='6789'/>
          <host name='10.0.0.103' port='6789'/>
      </source>
      <target dev='vda' bus='virtio'/>
    </disk

.. todo:: use secret keys

.. todo:: ceph.conf usage for mon addresses

.. todo:: pending libvirt xml schema changes
