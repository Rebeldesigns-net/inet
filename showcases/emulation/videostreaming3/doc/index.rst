Using Real Applications in a Simulated Network
==============================================

Goals
-----

In this showcase, we'll use real applications which communicate over a simulated network.
This feature is useful for testing how applications work over the network, without having to set up a physical network. The simulated network can be easily configured for various topologies and behaviors to test a variety of cases.

.. **TODO** reduce redundancy -> too many real applications

We'll use INET's emulation feature to interface the real world (the host OS environment) with the simulation.
INET has various modules which facilitate this interfacing, you can read about them in the :doc:`Emulation section </showcases/emulation/index>` of the showcases page.
In this scenario, we'll use a real video application to stream a video file to another real video application.

.. In the simulation scenario, a VLC instance streams video via the simulated network to another VLC instance.

Note that this showcase requires the ``Emulation`` feature of the INET Framework to be turned on (it's off by default), and it only runs on Linux.

| INET version: ``4.2``
| Source files location: `inet/showcases/emulation/videostreaming <https://github.com/inet-framework/inet-showcases/tree/master/emulation/TODO>`__

The Model
---------

The simulation scenario is illustrated with the following diagram:

.. **TODO** theoretical -> max utolag megmagyaraz -> ehhez kepest picit máshogy néz ki

.. **TODO** meaning that this is the logical overview of the setup; which parts are simulated which are real and how they are interfaced is another layer on top of this; there are multiple ways to do that, e.g. at the link layer, or transport layer, etc. Actually, this layer is another layer on this schematic as well (blue).

.. The simulation scenario is illustrated with the following topological/theoretical/? schematic:

.. so

  - first, about the overview
  - then, about where/how it is separated/interfaced
  - emphasize other possibilities of separation/interfacing (other ext interfaces)
  - then how that will work/the details of the scenario (?) -> the actual schematic
  - the scripts
  - the NED network
  - the config
  - maybe not in that order

.. The following schematic gives a logical overview of the simulation scenario:

.. figure:: media/setup.png
   :width: 50%
   :align: center

.. **V1** In the scenario, there are two hosts, a sender and a receiver. The hosts are realized in the real world from the link-layer up. Below that, the hosts and the network are simulated. The simulation and the real world is connected at the Ethernet interface. We'll use an :ned:`ExtUpperEthernet` interface. The upper part of this module is a TAP interface in the host OS, the lower part connects to the simulation. Packets received by the interface from the lower layers pass through the TAP interface to the host os. Inversely, packets received by the TAP interface are injected into the simulation.

In this scenario, a VLC instance in a sender host streams a video file to another VLC instance in a receiver host over the network. The hosts from the link-layer up are real; parts of the link-layer, as well as the physical layer and the network are simulated.

We'll use the :ned:`ExtUpperEthernet` interface to connect the real and simulated parts of the scenario.
The lower part of this interface is present in the simulation, and uses TAP interfaces in the host OS to send and receive packets to and from the upper layers of the host OS.

.. **V2.2** The lower part of this interface is present in the simulation, and uses TAP interfaces in the host os to connect the host os and the simulation.

.. **V2.3** The lower part of this interface is present in the simulation, and uses TAP interfaces in the host OS to connect the host OS and the simulation, i.e. send and receive packets to and from the upper layers of the host OS.

Note that the real and simulated parts can be separated at other levels of the protocol stack, using other, suitable EXT interface modules, such as at the transport layer (:ned:`ExtLowerUdp`), and the network layer (:ned:`ExtUpperIpv4`, :ned:`ExtLowerIpv4`).

.. **V1** Note that the in reality, the real parts of the sender and receiver hosts are the same machine, as both use the protocol stack of the host OS:

In fact, the real parts of the sender and receiver hosts are running on the same machine, as both use the protocol stack of the host OS (even though in this scenario logically they are different hosts):

.. **TODO** schematic

.. figure:: media/actualsetup.png
   :width: 50%
   :align: center

.. **TODO** even though logically they are different hosts (they can actaully be on different machines TODO) -> not needed

We'll use a VLC instance in the sender host to stream a video file. The packets created by VLC go down the host OS protocol stack and enter the simulation at the Ethernet interface. Then they traverse the simulated network, enter the receiver host's Ethernet interface, and are injected into the host OS protocol stack, and go up to another VLC instance which plays the video.

The network for the simulation is the following:

.. .. figure:: media/Network.png
   :width: 80%
   :align: center

.. figure:: media/Network2.png
   :width: 90%
   :align: center

.. It contains two :ned:`StandardHost`'s, which are connected by switches (:ned:`EtherSwitch`) to a :ned:`Router`.

It contains two :ned:`StandardHost`'s. Each host is connected by an :ned:`EtherSwitch` to a :ned:`Router`.

..  The hosts contain an :ned:`ExtUpperEthernetInterface`,
    they are also present in the host OS.

  - the vlc sender will send the video stream to the address out the router
  - the router will perform NAT to the address of the TAP interface of the receiver host
  - this is required so that the packets enter the simulation
  - otherwise they would use the loopback interface

  - the configuration
  - the scripts start the simulation and the streaming

The sender VLC application will stream the video to the address of the router's ``eth0`` in the simulation.
The router will perform network address translation to rewrite the destination address to the address of the receiver host's EXT/TAP interface.

This is required so that the video packets actually enter the simulated network; if they were sent to the receiver host's EXT/TAP interface, they would go through the loopback interface because the host OS optimizes traffic.

.. **V2** This is required so that the video packets enter the simulation, instead of going through the loopback interface.

Three shell scripts in the showcase's directory can be used to control the emulation scenario.
The ``setup.sh`` script creates the TAP interfaces, assigns IP addresses to them, and brings them up:

.. literalinclude:: ../setup.sh
   :language: bash

The ``teardown.sh`` script does the opposite; it destroys the TAP interfaces when they're no longer needed.

.. user is done playing with this whole thing TODO.

.. literalinclude:: ../teardown.sh
   :language: bash

The ``run.sh`` script starts the simulation, and both video applications:

.. literalinclude:: ../run.sh
   :language: bash

.. The hosts contain an extupperethernet interface...

.. **TODO** sim time limit

.. -----------------------------------

.. **V1** Here is the configuration in omnetpp.ini:

  .. literalinclude:: ../omnetpp.ini
     :language: ini

  In the configuration, the scheduler class is set to ``RealTimeScheduler`` so that the simulation can run in the real time of the host OS.
  The hosts are configured to have an :ned:`ExtUpperEthernetInterface`, and to use the TAP devices which were created by the setup script. The script assigned IP addresses to the TAP interfaces; the tap interfaces are set to copy the addresses from the tap interfaces.
  Finally, the addresses in the network are important; the configurator is set to assign the correct addresses so the simulation and the script can work together (the VLC sends the video packets to the router, so its address needs to match as the destination address in the script).
  The NAT table in the router's ivp4 module is configured to rewrite the source and destination address
  of packets which are addressed to it's eth0 interface before routing; the resulting packets have a source address of the router's eth1 interface and the destination address of the receiver host's ext interface. **TODO** rewrite
  Also, need to calculate CRC/FCS to properly serialize/deserialize packets

  -----------------------------------

In the configuration in omnetpp.ini, the scheduler class is set to ``RealTimeScheduler`` so that the simulation can run in the real time of the host OS:

.. literalinclude:: ../omnetpp.ini
   :language: ini
   :end-at: sim-time-limit

.. The ethernet interface type in both hosts is set to :ned:`ExtUpperEthernetInterface`:

The hosts are configured to have an :ned:`ExtUpperEthernetInterface`, and to use the TAP devices which were created by the setup script. The setup script assigned IP addresses to the TAP interfaces; the EXT interfaces are configured to copy the addresses from the TAP interfaces:

.. literalinclude:: ../omnetpp.ini
   :language: ini
   :start-at: ExtUpperEthernetInterface
   :end-at: host2.eth[0].copyConfiguration

The addresses in the network are important; the configurator is set to assign the correct addresses so the simulation and the script can work together (the VLC sends the video packets to the router, so its address needs to match as the destination address in the script):

.. literalinclude:: ../omnetpp.ini
   :language: ini
   :start-at: configurator
   :end-at: /config

Also, the CRC and FCS need to be set to ``computed`` to be able to properly serialize/deserialize packets.

.. Also, the CRC and FCS need to be set to ``computed`` to properly serialize/deserialize packets./in ethernet/udp/and other protocols like ipv4? **TODO**

.. need to calculate CRC/FCS to properly serialize/deserialize packets TODO:

.. literalinclude:: ../omnetpp.ini
   :language: ini
   :start-at: crcMode
   :end-at: fcsMode

.. -----------------------------------

  - the hosts use extupperethernetinterface
  - need the scheduler class
  - and the crc mode
  - need to specify the tap interface (created by scripts; later)
  - copy from the ext -> set the ip addresses from the script, and copy from there so they match
  - set up NAT
  - and ip addresses in the network
  - cos we'll use that to send the video file -> its needed so that the packets get into the simulation/simulated network

Running/Results
---------------

Before running the emulation scenario, run ``setenv`` in the `omnetpp` and `inet` directories,
and run the ``setup.sh`` script in the showcase's folder:

.. code-block:: bash

  $ cd ~/workspace/omnetpp
  $ . setenv
  $ cd ~/workspace/inet
  $ . setenv
  $ cd showcases/emulation/videostreaming
  $ ./setup.sh

To start the simulation and the VLC instances, run the ``run.sh`` script:

.. code-block:: bash

  $ ./run.sh

.. The script starts the simulation, and the video plays. Its not very high quality **TODO**

  so

  - the streaming VLC is run in command line mode...the script runs it in command line mode
  - the playing VLC is playing the video
  - note that its downscaled so that the playback is smooth
  - the simulation is cpu intensive
  - actually, it cant run in better quality in this machine
  - but on a more powerful machine it might
  - so i dont know
  - experiment with the settings

  The script starts the simulation in Cmdenv; the streaming VLC client is also started in command line mode.
  The playback VLC displays the received video stream. The received video is lower quality than the original video file,
  because it's downscaled, and the bitrate is reduced, so that the playback is smooth (emulating the network is CPU-intensive). **TODO** it is cpu intensive.
  The bitrate and the downscaling is controlled by the script; the user can experiment with other settings **TODO**

  emulating the network is CPU-intensive
  this is the quality that was still smooth on our test PC. on a faster computer it might be better -> experiment

The script starts the simulation in Cmdenv; the streaming VLC client is also started in command line mode. The received video stream is played by the other VLC instance. The received video is lower quality than the original video file,
because it's downscaled, and the bitrate is reduced, so that the playback is smooth.

.. note:: Emulating the network is CPU-intensive. The downscaling and bitrate settings were chosen to lead to smooth playback on the PC we tested the showcase on. However, it might be able to work in higher quality on a faster machine; the user can experiment with different encoding settings for the VLC streaming instance by editing them in the run script.

**TODO** should be shorter

.. TODO wireshark

Here are some of the packets captured in Wireshark:

.. figure:: media/wireshark.png
   :width: 100%
   :align: center

Note that there are packets sent from the ``tapa`` (192.168.2.20) interface to the router's ``eth0`` (192.168.2.99) interface,
and also packets sent from the router's ``eth1`` (192.168.3.99) interface to ``tapb`` (192.168.3.20).

.. **TODO** the video is downscaled/the quality is not the original because performance reasons

Sources: :download:`omnetpp.ini <../omnetpp.ini>`, :download:`VideostreamingShowcaseNetwork.ned <../VideostreamingShowcaseNetwork.ned>`,
:download:`run.sh <../run.sh>`, :download:`setup.sh <../setup.sh>`, :download:`teardown.sh <../teardown.sh>`

Discussion
----------

Use `this
page <https://github.com/inet-framework/inet-showcases/issues/TODO>`__ in
the GitHub issue tracker for commenting on this showcase.
