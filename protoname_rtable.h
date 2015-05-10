// definition of routing table class
// we are using hash table to store the routing table
#ifndef __protoname_rtable_h__
#define __protoname_rtable_h__

#include <trace.h>
#include <map>

typedef std::map<nsaddr_t, nsaddr_t> rtable_t;

class Protoname_rtable {
	rtable_t rt_;

public:
	Protoname_rtable();
	void print(Trace*);
	void clear();
	void rm_entry(nsaddr_t);
	void add_entry(nsaddr_t, nsaddr_t);
	nsaddr_t lookup(nsaddr_t);
	u_int32_t size();
};

#endif