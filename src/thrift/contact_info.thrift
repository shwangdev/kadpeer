#/**
#* @file   contack_info.thritf
#* @author Devil Wang <wxjeacen@gmail.com>
#* @date   Mon Jan 24 13:13:41 2011
#* 
#* @brief  
#* 
#* 
#*/

namespace cpp kad

struct ContactInfo {
  1: string node_id, 
  2: string ip ,
  3: i32 port ,
  4: optional string local_ip ,
  5: optional i32 local_port,
  6: optional string rendezvous_ip ,
  7: optional i32 rendezvous_port,
} 


