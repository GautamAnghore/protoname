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