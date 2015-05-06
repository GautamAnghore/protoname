Implementing a new Manet Unicast Routing Protocol in NS2
========================================================

# Index

 - Terminologies
 - c++ reference
   + ifdef
 - protname how-to
   + 

# Objective

 To learn the implementation of a new Mobile Networking Routing protocol in NS2. 

 NS2 is Network Simulation v2. It is used to simulate a Real life network scenerio and traffic conditions to test new theories about various levels of networking.

 Here we are going to implement a new protocol for mobile networking with a name `protname`.

# Terminologies

# c++ reference

## `ifdef` and `ifndef`

 + __ifdef__

  ```c
	#ifdef MACRO
     //controlled text
    #else
     //else controlled text
    #endif /* MACRO */
  ```

   If macro is defined, then only the controlled text is going to be included by the preprocessor. This block is called conditional group.
   It is tokanized even if macro is not defined so lexically valid c code.

 + __ifndef__

  ```c
    #ifndef MACRO
    #else
    #endif
  ```

  Same as `ifdef`. The content is included only if MACRO is not defined. Used to include the content only once.


# protname how-to

## File Structure
 
 To allocate our code we will firstly create a new directory called protoname inside your NS2 base directory.
 `ns2-allinone-2.35/ns-2.35/protoname`. You can organize your code as you want to. That is, you can create more or less files, with those names or with others; this is only a hint.

 + __protoname.h__
  <br/>This is the header file where will be defined all necessary timers (if any) and routing agent which performs protocol’s functionality.

 + __protoname.cc__
 <br/>In this file are actually implemented all timers, routing agent and Tcl hooks.
 
 + __protoname_pkt.h__
 <br/>Here are declared all packets protoname protocol needs to exchange among nodes in the manet.

 + __protoname_rtable.h__
 <br/>Header file where our own routing table is declared. 

 + __protoname_rtable.cc__
 <br/>Routing table implementation.

## Class Structure

 + __Agent__ Class
 <br/>_Agents represent endpoints where network-layer packets are constructed or consumed, and are used in the implementation of protocols at various layers._

   - This is the main class we will have to code in order to implement our routing protocol. In addition, this class offers a linkage with Tcl interface, so we will be able to control our routing protocol through simulation scripts written in Tcl.

   - To implement a routing protocol in NS2 you must create an agent by inheriting from Agent class of ns2.

 + __protoname_rtable__ Class
 <br/>Routing Agent will maintain an internal state and a routing table. We will treat routing table as a new class, protoname rtable.

 + __Timer__ Class
 <br/>Everything realted to timing like some periodic updating of routing table or discarding any routes after some time can be done using timer class.

 + __Trace__ Class
 <br/>To write to a trace file about what happened in the simulation.

 + __debug() function__
 <br/>To print debug message in your code.


## Packet Types
  
`protoname/protoname_pkt.h`

 + define a structure which represents the pkt structure.
 + raw_attributes are used according to ns2 specification
 + `config.h` header file in ns2 defines these raw_attributes
 + example of raw_attributes:
   - `nsaddr_t` : everytime we need address in ns2, use this
   - `u_int16_t`: unsigned 16 bit int
   - `u_int8_t`: unsigned 8 bit int
 + As specification by ns2, it is good to name the variables using raw_attributes such that it ends with '\_' to distinguish them from normal variables like `pkt_id_`
 + To inherit __Packet__ class, include `common/packet.h` file as `#include <packet.h>` where actual Packet class is defined. We need to add our defined structure(`hdr_protoname_pkt`) to the Packets so that our control packet can be used in the simulation. These packets are used to exchange information between objects in simulation.
 + __Packet__ class stores all the defined packets as an array of unsigned chars where packets' fields are saved. To access our defined packet, we need to define in our header, an offset property which will find where our packet is.
 + __question__ : there is a member function `access` which is defined so that it can return a `hdr_protoname_pkt` when passed a `Packet`. _HOW? the function is calling itself with passing offset_.

 + Now bind our packet header to Tcl interface.
 
 `protoname/protoname.cc`

 + Define the static `ProtonameHeaderClass` class which inherits the `PacketHeaderClass`.
 + Define constructor passing values to `PacketHeaderClass` constructor and binding the offset of our packet.

## The Routing Agent

`protoname/protoname.h`

 + Define a new `Protoname` class having attributes and functions which are needed for protocol to do its job.

 + Header files included :
   - `protoname/protoname_pkt.h` : Defines our packet header.
   - `common/agent.h` : Defines Agent base class.
   - `common/packet.h` : Defines Packet class.
   - `common/timer-handler.h` : Defines TimerHandler base class. We will use it to create our custom timers.
   - `trace/trace.h` : Defines Trace class, used for writing simulation results out to a trace file.
   - `tools/random.h` : Defines Random class, useful for generating pseudo-random numbers. We will use it soon.
   - `classifier/classifier-port.h` : Defines PortClassifier class, used for passing packets up to upper layers.

 + Macro for current time using `Schedule` class
 + Macro for random numbers b/w 0-0.5 for non synchronization of neighbour nodes
 + Forward declaration of `Protoname` agent class
 + A `Protoname_PktTimer` class defination inherits `TimerHandler` class, reference to routing agent who creats it. Used 'as a callback' to tell agent when to send the control packet and to schedule next one. `Protoname` class have this class as a friend to access the callback.
 + `Protoname` Agent class defination
   - inherits `Agent` class
   - its own address
   - internal state
   - routing table
   - accessible variable from TCL
   - counter for assigning sequence number
   - `PortClassifier` object - [ nodes structure - chapter5[2]] node consists of address classifier and port classifier. Address classifier to guide the incoming packets to suitable link, pass them to port classifier. Port classifier passes these to appropriate upper layer agent. [ detailed architecture of mobile node - chapter16[2] ]
   - `Trace` class object : we use it to write the content of routing table to logs whenever user asks for it using Tcl
   - a custom timer declaration
   - function to forward data
   - function to be called on recieving a packet
   - function for sending packet
   - function to schedule our custom timer expiration
   - constructor needs identifier used as routing agent's address
   - `recv()` to call whenever agent recieves a packet either from upper layer agent like UDP or TCP or from some other node
   - `command()` invoked from Tcl [ chapter 3[2] ]

