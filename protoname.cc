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