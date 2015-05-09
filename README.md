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

### `ifdef` and `ifndef`

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

### Function on left hand side of assignment operator
`setref() = 10`
	
  + Assignment operator need _lvalue_ on left hand side of assignment operator
  + In C++, function call is an _lvalue_ if the function returns a variable by reference or by pointer

```c
int ref=0;
int& setRef() {
    return ref;
}

int main() {
   setRef() = 11;
   cout<<ref;
}
```


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

####TCL Hook
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

####TCL Hook
`protoname/protoname.cc`
 + Bind our agent to Tcl so that it can be instantiated from Tcl
 + inherits `TclClass`
 + constructor simply calls `TclClass`'s constructor with arguement "Agent/Protoname"
 + function `create()`, returns a `TclObject*`, takes arguements argc and argv, argc should be 5.
 + argv of the form `<object’s name> <$self> <$class> <$proc> <user argument>` [ for more information chapter 3[2]]. In this case, `<object’s name> <$self> Agent/Protoname create-shadow <id>`. `<id>` is converted to nsaddr_t and then passed in the constructor of `Protoname` agent class's new object which is returned also.

###Timer
`protoname/protoname.cc`
 + code the `expire()` method
 + two tasks:
   1. send a new control packet
   2. reschedule the timer itself

 + more on timers [ chapter 11 [2]]

##The Routing Agent function definations

###Constructor
`protoname/protoname.cc`

 + call constructor for `Agent` super class. `Agent(PT_PROTONAME)` where `PT_PROTONAME` will be defined later.(where will it be defined?). `PT_PROTONAME` is used to identify the control packets sent and recieved by this agent.
 + call the constructor for `Protoname_PktTimer` object in our class(`pkt_timer_`).
 + `bind()` the `accessible_var_` as a boolean attribute to be accessed by tcl interface. For boolean binding `bind_bool()` and for integer binding `bind()`.
 + save the address of agent

 __Using the `accessible_var_` from Tcl interface__
 <br/>
 `Agent/Protoname set accessible_var_ true`

###command() method defination
`protoname/protoname.cc`

 + `command()` function is inherited from `Agent` class.
 + this function is invoked when we pass the command like
   <br/>`$ns_ at 15.0 "[$node_ agent 255] print_rtable"`
   <br/>passing 255 as an arguement because this is the port to which routing agent is attached to.
 + [ chapter 3[2] ] specification of "cmd"
  <br/>as per the specifications 
   - `argv[0]` name of method being invoked
   - `argv[1]` requested operation
   - `argv[2..argc-1]` additional arguements
 + code the operations which are mendatory plus some methods which you want to be accessible from TCL interface.
 + we will have different cases for each operations
 + each case must end with `TCL_OK` (everything is ok) or `TCL_ERROR` (error occured)
 + `start` : mendatory operation, configure the agent to start its execution, define all actions that are required by the routing agent to perform in order to begin its operation.
 + `port-dmux` : mendatory operation, 
 	<br/>[ chapter 3 [2] ] NS stores a reference of every compiled object (c++ object) in hash table for fast access to each of them given its name. Example usage -
 	<br/>`dmux_ = (PortClassifier*)TclObject::lookup(argv[2]);`
 	<br/>`lookup` is the function of `TclObject` class to find the reference and as we are looking for `Portclassifier` class object, we need type casting.
 + `tracetarget` ( can also be called as `log-target`) : mendatory operation, obtains a `Trace` object given its name. `logtarget_ = (Trace*)TclObject::lookup(argv[2]);`
  	
 + some other operations, for example
   - `print_rtable` : prints the routing table to trace file. To understand `Trace` class, view `trace/trace.h` header file. `buffer()` and `dump()` functions are implemented in `BaseTrace` class.<br/>
    `buffer()` - to get the variable where the output is buffered<br/>
    `dump()` - to flush the buffer to the output file<br/>
    calls the `print()` function of the routing table.

 + every operation returns either `TCL_OK` or `TCL_ERROR`
 + finally if the command did not match any description, we pass it to base class
   <br/>`return Agent::command(argc, argv);`

###recv() method definition
`protoname/protoname.cc`

 + Invoked whenever routing agent recieves a packet (from other node or upper layer agent). Even when the packet is generated at this node, recv() is called as the packet is recieved by the agent from the upper layer agent.
 
 + Every `Packet` has a common header `hdr_cmn` defined in `common/packet.h`. To access this header there is a macro defined.<br/>
 `struct hdr_cmn* ch = HDR_CMN(p);` where p is `Packet*`.
 + Same is with the IP header. To access the IP header `HDR_IP` macro defined in `ip.h` might be used.
 <br/> `struct hdr_ip* ih = HDR_IP(p);`
 
 + first of all, check for a loop. If the packet recieved by agent is sent by itself, drop the packet as there is a loop in the network.
 + then, if the packet is generated within the node (by upper layer agents), add the overhead that routing protocol is adding(in bytes). In this case, it is IP, so add the IP's header size to packet size.<br/>
 `ch->size() += IP_HDR_LEN;`
 
 + Data packet can be of various types. It can be data packet or control packet. Control packet is used by the routing protocol to figure out routing meta data. Control packet can be of `PT_PROTONAME` type which corrosponds to our protocol or any other protocol's control packet. Data Packet contains the actual data.
 
 + Check the type of packet. 
   <br/>__If__ it is `PT_PROTONAME`, it is protoname packet and we need to process it by calling the function
   <br/>`recv_protoname_pkt(p);`
   <br/>__Else__ decrease the ttl by one, check if it is zero. _If_ yes drop the packet with 
   `drop(p, DROP_RTR_TTL);`, _else_, forward the packet with `forward_data()` function.
 
 + `recv_protoname_pkt(p)` is called when the packet is protoname's packet. This function should perform the actual task of protocol, like updating the routing table or so.
 
 + `forward_data(p)` is called if the packet is not of protoname type. This function should check if the packet is to be forwarded to other node, or the packet is destined to this node. If packet is destined to this node, deliver the packet to upper layer agents.

 + __`drop(p, const)`__ is used to drop the packet. `drop(p, const)`, p is Packet* and const is a constant giving reason to discard the packet. `trace/cmu-trace.h` have the defination of constants.

###recv\_protoname\_pkt() method definition
`protoname/protoname.cc`

 + invoked by `recv()` when a protoname packet is recieved
 + get the ip header and the Protoname's header as defined in protoname_pkt.h
   <br/>`struct hdr_protoname_pkt* ph = HDR_PROTONAME_PKT(p);`
 + __note :__ All routing messages are sent to and from port number 255. This port is reserved to attach the routing agent.
 + `RT_PORT` as defined in `common/packet.h` is the routing port with value 255.
 + check the source and destination port to be `RT_PORT`
 + process the packet according to protocol specifications
 + release the resources `Packet::free(p)`

###send\_protoname\_pkt() method definition
`protoname/protoname.c`

 + called by custom timer function ( Protoname_PktTimer > expire() ) when it expires
 + allocate the packet using `allocpkt()`, this function is defined for all agents
 + get the common, ip and protoname's packet headers
 + then fill these headers with values we want to.
 + according to packet structure of protocol, fill the required values
 + common header
   - set the packet type to protoname
   - set the direction `hdr_cmn::DOWN` for sending
   - size of packet in bytes (nothing to do with the size of `hdr_protoname_pkt` structure, used by ns2 to calculate ns2 propagation delay)
   - error tolarable or not
   - next hop address
   - address type (can be `NS_AF_NONE`, `NS_AF_ILINK` or `NS_AF_INET` (see common/packet.h), `NS_AF_INET` -> for internet protocol).
 + ip header
   - time to live -> `IP_DEF_TTL` (default defined in common/ip.h)
 + Packets are events(chapter 12[2]) so they needs to be scheduled
 + _In fact, sending a packet is equivalent to schedule it at a certain time._
 + Packet class inherits from `Connector` class, which has a member of `TclObject` class called `target_`. Event will be handled by this object. It is passed to the `schedule()` function.
    