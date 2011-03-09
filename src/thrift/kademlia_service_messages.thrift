include "contact_info.thrift"
include "signed_kadvalue.thrift"

namespace cpp kad

struct  PingRequest {
  1: string ping ,
  2: contact_info.ContactInfo sender_info ,
}

struct PingResponse {
  1: string result ,
  2: optional string  echo,
  3: optional string node_id,
}

struct FindRequest {
  1: string key ,
  2: contact_info.ContactInfo sender_info ,
  3: optional bool is_boostrap ,
  4: optional string sender_ext_ip ,
  5: optional i32 sender_ext_port ,
}

struct FindResponse {
  1: string result ,
  2: list<contact_info.ContactInfo> closest_nodes ,
  3: list<string> values ,
  4: list<signed_kadvalue.SignedValue> signed_values ,
  5: optional contact_info.ContactInfo alternative_value_holder ,
  6: optional string needs_cache_copy ,
  7: optional string requester_ext_addr ,
  8: optional string node_id ,
}

struct FindNodeResult {
  1: string result ,
  2: optional string contact ,
  3: optional string node_id ,
}


struct StoreRequest {
  1: string key ,
  2: optional string value ,
  3: optional signed_kadvalue.SignedValue sig_value ,
  4: i32 ttl ,
  5: contact_info.ContactInfo sender_info ,
  6: bool publish ,
  7: optional signed_kadvalue.SignedRequest signed_request ,
}

struct StoreResponse {
  1: string result ,
  2: optional string node_id ,
  4: optional signed_kadvalue.SignedRequest signed_request ,
}

struct DownlistRequest {
  1: list<string> downlist ,
  2:  contact_info.ContactInfo sender_info ,
}

struct DownlistResponse {
  1: string result ,
  2: optional string node_id ,
}

struct BootstrapRequest {
  1: string newcomer_id ,
  2: string newcomer_local_ip ,
  3: i32 newcomer_local_port ,
  5: i32 node_type ,
  6: optional string newcomer_ext_ip ,
  7: optional i32 newcomer_ext_port ,
}

struct BootstrapResponse {
  1: string result ,
  2: optional string bootstrap_id ,
  3: optional string newcomer_ext_ip ,
  4: optional i32 newcomer_ext_port ,
  5: optional i32 nat_type , // 1: directly connected or behind full-cone router or ...
                               // 2: need rendezvous server to punch the hole
                               // 3. no connection
}

# struct NatDetectionRequest {
#    string newcomer ,
#    string bootstrap_node ,
#    i32 type ,  //1: Node B asks C to try ping A, 2: Node B asks C to try a rendezvous to A with B as rendezvous
#    string sender_id ,
# }

# struct NatDetectionResponse {
#    string result ,
# }

# struct NatDetectionPingRequest {
#    string ping ,
# }

# struct NatDetectionPingResponse {
#    string result ,
#   optional string echo ,
#   optional string node_id ,
# }

struct DeleteRequest {
  1: string key ,
  2: signed_kadvalue.SignedValue value ,
  3: signed_kadvalue.SignedRequest signed_request ,
  4: contact_info.ContactInfo sender_info ,
}

struct DeleteResponse {
  1: string result ,
  2: optional string node_id ,
}

struct UpdateRequest {
  1: string key ,
  2: signed_kadvalue.SignedValue new_value ,
  3: signed_kadvalue.SignedValue old_value ,
  4: i32 ttl ,
  5: signed_kadvalue.SignedRequest request ,
  6: contact_info.ContactInfo sender_info ,
}

struct UpdateResponse {
  1: string result ,
  2: optional string node_id ,
}
