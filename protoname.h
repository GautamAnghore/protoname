#ifndef __protoname_h__
#define __protoname_h__

#include "protoname_pkt.h"
#include <agent.h>
#include <packet.h>
#include <trace.h>
#include <timer-handler.h>
#include <random.h>
#include <classifier-port.h>

#define CURRENT_TIME Scheduler::instance().clock()	// current time in the simulation
#define JITTER (Random::uniform()*0.5)	// to de sync with neighbours and avoid colloisions
	
class Protoname;	//forward declaration as used in Protoname_PktTimer class

// to schedule sending of control packets
class Protoname_PktTimer : public TimerHandler {
public:
	Protoname_PktTimer(Protoname* agent) : TimerHandler() {
		agent_ = agent;
	}
protected:
	Protoname* agent_;	// the agent which creates it
	virtual void expire(Event* e);	// will be overloaded later
};

// the agent declaration
class Protoname : public Agent {

	// to access the timer functions
	friend class Protoname_PktTimer;

	nsaddr_t		 ra_addr_;		// agent's address
	protoname_state  state_;		// agent's current state
	protoname_rtable rtable_;		// agent's routing table
	int 			 accesible_var_;// variable to be accessed by tcl script
	u_int8_t		 seq_num;		// sequence number to be attached with each packet

protected:

	PortClassifier*		dmux_;		// to pass the packet to upper layer agents
	Trace*				logtarget_; // to log the intermediate values
	Protoname_PktTimer	pkt_timer_; // to sent packets timely

	inline nsaddr_t& ra_addr() { return ra_addr_; }
	inline protoname_state& state() { return state_; }
	inline int& accesible_var() { return accesible_var_; }

	void forward_data(Packet*);			// for fwding packets
	void recv_protoname_pkt(Packet*);	// to be called when packet recieved
	void send_protoname_pkt();			// to be called to send a packet

	void reset_protoname_pkt_timer();

public:

	Protoname(nsaddr_t);					// constructor need a unique address
	int command(int, const char*const* );	// to be invoked by tcl script
	void recv(Packet*, Handler*);			// to be invoked when recieves a packet
};

#endif