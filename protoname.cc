//binding our packet to TCL interface
//for that we use offset

//Q: don't know why this variable
int protoname_pkt::offset_;

//Our header class inherits Packet's header class
static class ProtonameHeaderClass : public PacketHeaderClass {

public:
	//constructor
	//we are passing values to the parent class constructor 
	ProtonameHeaderClass() : PacketHeaderClass("PacketHeader/Protoname", sizeof(hdr_protoname_pkt)) {
		//Q: don't know what this bind_offset does
		bind_offset(&hdr_protoname_pkt::offset_);
	}

}class_rtProtoProtoname_hdr;

static class ProtonameClass : public TclClass() {
public:
	ProtonameClass() : TclClass("Agent/Protoname") {} //constructor only passes value to parent class

	TclObject* create(int argc, const char*const* argv) {
		assert(argc == 5);	// I think this function checks true or false and handles display of error msg
		return (new Protoname((nsaddr_t)Address::instance().str2addr(argv[4])));
	}
}class_rtProtoProtoname;

/*
	defination of : protoname.h > Protoname_PktTimer > expire 
*/
void Protoname_PktTimer::expire(Event* e) {
	agent_->send_protoname_pkt();
	agent_->reset_protoname_pkt_timer();
}

/*
	defination of : protoname.h > Protoname > functions
*/

// constructor

Protoname::Protoname(nsaddr_t id) : Agent(PT_PROTONAME), pkt_timer_(this) {
	bind_bool("accessible_var_", &accessible_var_);
	ra_addr_ = id;
}

// command function

int Protoname::command(int argc, const char*const* argv) {
	
	if (argc==2) {

		// "start" mendatory operation
		if(strcasecmp(argv[1], "start") == 0) {
			pkt_timer_.resched(0.0);
			return TCL_OK;
		}
		// "print_rtable" operation
		else if(strcasecmp(argv[1], "print_rtable") == 0) {
			//check if logtarget_ i.e. the trace object already exists or not
			if(logtarget_ != 0) {
				sprintf(logtarget_->pt_->buffer(), "P %f _%d_ Routing Table", CURRENT_TIME, ra_addr());
				logtarget_->pt_->dump();
				rtable_.print(logtarget_);
			}
			else {
				fprintf(stdout, "%f _%d_ If you want to print this routing table you must create a trace file in your tcl script", CURRENT_TIME, ra_addr());
			}
			return TCL_OK;
		}
	}
	else if (argc==3) {
		// "port-dmux" mendatory operation
		if(strcmp(argv[1], "port-dmux")==0) {
			// lookup from the hashed data to find the reference of compiled c++ objects
			// ns property 
			dmux_ = (PortClassifier*)TclObject::lookup(argv[2]);
			
			if (dmux_ == 0) {
				fprintf(stderr, "%s: %s lookup of %s failed\n", __FILE__, argv[1], argv[2]);
				return TCL_ERROR;
			}
			return TCL_OK;
		}
		// "tracetarget" mendatory operation, also called "log-target"
		else if (strcmp(argv[1], "tracetarget") == 0 || strcmp(argv[1], "log-target") == 0) {
			logtarget_ = (Trace*)TclObject::lookup(argv[2]);
			if(logtarget_ == 0)
				return TCL_ERROR;
			return TCL_OK;
		}
	}

	// if command do not match any description,
	// hand it over to base class
	return Agent::command(argc, argv);
}

// recv function
void Protoname::recv(Packet* p, Handler* h) {
	// STEP 1 : get the headers
	// ch = common header
	// HDR_CMN(p) : macro defined in common/packet.h to get the common header
	struct hdr_cmn* ch = HDR_CMN(p);
	// ih = ip header
	// ip = internet protocol
	// HDR_IP(p) : macro defined in ip.h to get the ip header
	struct hdr_ip* ih = HDR_IP(p);

	// STEP 2 : check sender's address
	//if ip header -> sender's address is same as current nodes address
	//means either there is a loop in the network so drop
	//or the packet is generated at this node (by upper layers), so add ip header
	if(ih->saddr() == ra_addr()) {
		//network loop condition
		//if packet is already forwarded, means loop
		if(ch->num_forwards() > 0) {
			drop(p, DROP_RTR_ROUTE_LOOP);
			return;
		}
		//else packet is generated here, add IP header
		else if(ch->num_forwards() == 0) {
			ch->size() += IP_HDR_LEN;
		}
	}

	// STEP 3 : check packet type
	// if packet is protoname's packet, process it
	// else reduce ttl, check zero, forward it
	if(ch->ptype() == PT_PROTONAME) {
		//process the packet data
		recv_protoname_pkt(p);
	}
	else {
		ih->ttl_--;
		if(ih->ttl_ == 0) {
			drop(p, DROP_RTR_TTL);
			return;
		}
		forward_data(p);
	}

}