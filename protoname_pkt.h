#ifndef __protname_pkt_h__
#define __protname_pkt_h__

#include <packet.h>

#define HDR_PROTONAME_PKT(p) hdr_protoname_pkt::access(p)

struct hdr_protoname_pkt {

	nsaddr_t pkt_src_;
	u_int16_t pkt_len_;
	u_int8_t pkt_seq_num_;

	inline nsaddr_t& pkt_src() { return pkt_src_; }
	inline u_int16_t& pkt_len() { return pkt_len_; }
	inline u_int8_t& pkt_seq_num() { return pkt_seq_num_; }

	//the following is defined to attach our packet defination to 
	//Packet class. offset is used as Packet class use an array of 
	//unsigned chars to store packet information. ** some confusions **
	static int offset_;
	inline static int& offset() { return offset_; }

	//this function is not completely understood
	inline static hdr_protoname_pkt* access(const Packet* p) {
		return (hdr_protoname_pkt*)p->access(offset_);
	}
};

#endif