from BGPdump import addr_is_IPv4, addr_is_IPswen, addr_is_IPv6, ip2string
import struct
from cffi import FFI
from graphviz import Digraph
import argparse
import json
import time

class MRTDB:
    __slots__ = ['ffi', 'libc', 'libcmrtdb', 'CTypes', 'mrtdb']
    def __init__(self, dir_name = './data'):
        self.CTypes = '''
        struct list_head
        {
	        struct list_head *next, *prev;
        };
        struct rb_node
        {
	        unsigned long  rb_parent_color;
        #define	RB_RED		0
        #define	RB_BLACK	1
	        struct rb_node *rb_right;
	        struct rb_node *rb_left;
        };
        //} __attribute__((aligned(sizeof(long))));
        /* The alignment might seem pointless, but allegedly CRIS needs it */

        struct rb_root
        {
	        struct rb_node *rb_node;
        };

        struct mrtdb
        {
	        char dir_name[32];

            int prefix_ipv4_cnt;
            int prefix_ipv6_cnt;
            int prefix_ipswen_cnt;
            int route_ipv4_cnt;
            int route_ipv6_cnt;
            int route_ipswen_cnt;

	        //uint32_t key_list_len[KEY_MAX + 1];
	        //struct list_head key_list[KEY_MAX + 1];
	        //struct rb_root key_rbtree[KEY_MAX + 1];

            uint32_t key_list_len[4];
            struct list_head key_list[4];
            struct rb_root key_rbtree[4];

        };

        typedef union
        {
	        uint8_t bytes[16];
	        int8_t sbytes[16];
	        uint32_t data[4];

        } mrtdb_addr_t;

        typedef struct
        {
	        uint32_t plen;
	        mrtdb_addr_t addr;
	        // here 20 bytes

        } mrtdb_prefix_t;

        typedef struct
        {
	        uint32_t len;
	        uint32_t *asn;

        } mrtdb_as_path_t;

        typedef struct
        {
	        union
	        {
		        mrtdb_prefix_t prefix;
		        uint32_t asn;
		        mrtdb_as_path_t as_path;
		        mrtdb_addr_t next_hop;
	        };

	        union
	        {
		        uint8_t tier;
		        uint32_t as_path_cnt;
                uint32_t origin_asn_len;
	        };

            uint32_t origin_asn[32];
	        int8_t key_type;
	        uint32_t index;
	        long int offset;
	        //uint32_t value_list_len[VALUE_MAX + 1];
	        //struct list_head value_list[VALUE_MAX + 1];
            uint32_t value_list_len[5];
            struct list_head value_list[5];
	        struct list_head list;

        } mrtdb_list_t;

        typedef struct
        {
	        int8_t value_type;
	        uint32_t index;
	        mrtdb_list_t *ptr;
	        struct list_head list;

        } mrtdb_value_t;

        mrtdb_value_t* list_entry_value_func(struct list_head *ptr);
        mrtdb_list_t* list_entry_key_func(struct list_head *ptr);

        int mrtdb_new_index(char *dir_name);
        struct mrtdb* mrtdb_init(char* dir_name);
        int mrtdb_exit(struct mrtdb* mrtdb);
        mrtdb_list_t* mrtdb_query_prefix(struct mrtdb* mrtdb, char *q);
        mrtdb_list_t* mrtdb_query_as(struct mrtdb* mrtdb, uint32_t q);
        int mrtdb_key_load_value(struct mrtdb* mrtdb, mrtdb_list_t *x);
        int mrtdb_key_unload_value(mrtdb_list_t *x);

        /*
        #define list_for_each_entry(pos, head, member)    \
         for (pos = list_entry((head)->next, typeof(*pos), member); \
              &pos->member != (head);  \
              pos = list_entry(pos->member.next, typeof(*pos), member))
        */
        '''
        self.ffi = FFI()
        self.libc = self.ffi.dlopen(None)
        self.libcmrtdb = self.ffi.dlopen('./cmrtdb/libcmrtdb.so')
        self.ffi.cdef(self.CTypes)
        #self.libcmrtdb.mrtdb_new_index(dir_name.encode())
        self.mrtdb = self.libcmrtdb.mrtdb_init(dir_name.encode())

    def __del__(self):
        self.libcmrtdb.mrtdb_exit(self.mrtdb)
        del self.libcmrtdb
        self.libcmrtdb = None
        del self.libc
        self.libc = None
        del self.ffi
        self.ffi = None

    def stat(self):
        '''
        #Traverse the list of prefix
        cnt = [0] * 35
        head = self.ffi.addressof(self.mrtdb.key_list[0])
        pos = self.libcmrtdb.list_entry_key_func(head.next)
        while self.ffi.addressof(pos.list) != head:
            cnt[pos.origin_asn_len] += 1
            pos = self.libcmrtdb.list_entry_key_func(pos.list.next)
        for i in range(35):
            print('cnt[%d] = %d' % (i, cnt[i]))
        '''
        return [self.mrtdb.prefix_ipv4_cnt, self.mrtdb.prefix_ipv6_cnt, self.mrtdb.prefix_ipswen_cnt, self.mrtdb.route_ipv4_cnt, self.mrtdb.route_ipv6_cnt, self.mrtdb.route_ipswen_cnt]

    def get_total_ip(self):
        d = {}
        #Traverse the list of as
        head = self.ffi.addressof(self.mrtdb.key_list[1])
        pos = self.libcmrtdb.list_entry_key_func(head.next)
        while self.ffi.addressof(pos.list) != head:
            self.libcmrtdb.mrtdb_key_load_value(self.mrtdb, pos)

            #as_ptr.value_list[0] --> none
            #as_ptr.value_list[1] --> as_paths starting with the as
            #as_ptr.value_list[2] --> prefixes originating from the as
            #as_ptr.value_list[3] --> upstreams
            #as_ptr.value_list[4] --> downstreams

            head1 = self.ffi.addressof(pos.value_list[2])
            pos1 = self.libcmrtdb.list_entry_value_func(head1.next)
            while self.ffi.addressof(pos1.list) != head1:
                if pos.asn not in d:
                    d[pos.asn]  = set()
                d[pos.asn].add(ip2string(self.ffi.buffer(self.ffi.addressof(pos1.ptr.prefix.addr))) + '/%u' % (pos1.ptr.prefix.plen))
                pos1 = self.libcmrtdb.list_entry_value_func(pos1.list.next)
            self.libcmrtdb.mrtdb_key_unload_value(pos)

            pos = self.libcmrtdb.list_entry_key_func(pos.list.next)

        with open('total_ip.jsonl', 'w') as f:
            for i in d:
                f.write(json.dumps([i, list(d[i])]) + '\n')


    def ip2asn(self, ip):
        prefix = self.libcmrtdb.mrtdb_query_prefix(self.mrtdb, ip.encode())
        asn = [0]
        if prefix != self.ffi.NULL:
            #print(ip2string(self.ffi.buffer(self.ffi.addressof(prefix.prefix.addr))) + '/%d' % (prefix.prefix.plen))
            asn = [prefix.origin_asn[i] for i in range(prefix.origin_asn_len)]
            #prefix_ptr.value_list[0] --> next_hops
            #prefix_ptr.value_list[1] --> as_paths
            #prefix_ptr.value_list[2] --> none
            #prefix_ptr.value_list[3] --> none
            #prefix_ptr.value_list[4] --> none
            '''
            head = self.ffi.addressof(prefix.value_list[1])
            pos = self.libcmrtdb.list_entry_value_func(head.next)
            while self.ffi.addressof(pos.list) != head:
                asn = pos.ptr.as_path.asn[pos.ptr.as_path.len - 1]
                break
                pos = self.libcmrtdb.list_entry_value_func(pos.list.next)
            '''

        return asn

    def get_as_path(self):
        f = open('as_path.jsonl', 'w')
        #Traverse the list of as_path
        head = self.ffi.addressof(self.mrtdb.key_list[2])
        pos = self.libcmrtdb.list_entry_key_func(head.next)
        while self.ffi.addressof(pos.list) != head:
            f.write(json.dumps([[pos.as_path.asn[i] for i in range(pos.as_path.len)], pos.as_path_cnt]) + '\n')
            pos = self.libcmrtdb.list_entry_key_func(pos.list.next)
        f.close()

if __name__ == "__main__":
    p = argparse.ArgumentParser(description='This script loads MRTDB from files')
    p.add_argument('--dir', dest='dir_name', default='./data')
    args = p.parse_args()
    m = MRTDB(args.dir_name)
    print(m.stat())


'''
mrtdb = MRTDB()
st = time.time()
for i in range(10000000):
    mrtdb.ip2asn('120.53.103.17')
et = time.time()
print(et - st)

mrtdb_ptr.key_list[0] --> prefix
mrtdb_ptr.key_list[1] --> as
mrtdb_ptr.key_list[2] --> as_path
mrtdb_ptr.key_list[3] --> next_hop
'''
'''
#Traverse the list of prefix
head = self.ffi.addressof(self.mrtdb.key_list[0])
pos = self.libcmrtdb.list_entry_key_func(head.next)
while self.ffi.addressof(pos.list) != head:
    self.libcmrtdb.mrtdb_key_load_value(self.mrtdb, pos)
    pos = self.libcmrtdb.list_entry_key_func(pos.list.next)
'''



'''
#Traverse the list of next_hop
head = ffi.addressof(mrtdb_ptr.key_list[3])
pos = libcmrtdb.list_entry_key_func(head.next)
while ffi.addressof(pos.list) != head:
    print('next_hop[%d]: ' % (pos.index), ip2string(ffi.buffer(ffi.addressof(pos.next_hop))))
    pos = libcmrtdb.list_entry_key_func(pos.list.next)
'''
'''

'''
'''
as_ptr = libcmrtdb.mrtdb_query_as(mrtdb_ptr, 141748)
if as_ptr != ffi.NULL:
    print(as_ptr.asn)
    libcmrtdb.mrtdb_key_load_value(mrtdb_ptr, as_ptr)


    #as_ptr.value_list[0] --> none
    #as_ptr.value_list[1] --> as_paths starting with the as
    #as_ptr.value_list[2] --> prefixes originating from the as
    #as_ptr.value_list[3] --> upstreams
    #as_ptr.value_list[4] --> downstreams

    if as_ptr.value_list_len[1] > 0:
        head = ffi.addressof(as_ptr.value_list[1])
        pos = libcmrtdb.list_entry_value_func(head.next)
        ls = []
        while ffi.addressof(pos.list) != head:
            print('as_path[%d]: ' % (pos.index), ' '.join([str(pos.ptr.as_path.asn[i]) for i in range(pos.ptr.as_path.len)]))
            ls.append([pos.ptr.as_path.asn[i] for i in range(pos.ptr.as_path.len)][::-1])
            pos = libcmrtdb.list_entry_value_func(pos.list.next)


        dot = Digraph(name="MyPicture", comment="the test", format="png", strict=True)
        for as_path in ls:
            for i in range(len(as_path)):
                q = libcmrtdb.mrtdb_query_as(mrtdb_ptr, as_path[i])
                if i == 0:
                    dot.node(name=str(as_path[i]), fillcolor='blue', style='filled')
                else:
                    if q.tier == 1:
                        dot.node(name=str(as_path[i]), fillcolor='green', style='filled')
                    else:
                        dot.node(name=str(as_path[i]))
                if i > 0 and as_path[i - 1] != as_path[i]:
                    dot.edge(str(as_path[i - 1]), str(as_path[i]))

    libcmrtdb.mrtdb_key_unload_value(as_ptr)
'''
