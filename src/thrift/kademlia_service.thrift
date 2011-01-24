include "kademlia_service_messages.thrift"

namespace cpp kad

service KademliaService {
  kademlia_service_messages.PingResponse Ping (1: kademlia_service_messages.PingRequest request),
  kademlia_service_messages.FindResponse FindValue (1: kademlia_service_messages.FindRequest request),
  kademlia_service_messages.FindResponse FindNode (1: kademlia_service_messages.FindRequest request) ,
  kademlia_service_messages.StoreResponse Store (1: kademlia_service_messages.StoreRequest request),
  kademlia_service_messages.DownlistResponse Downlist (1: kademlia_service_messages.DownlistRequest request) ,
  kademlia_service_messages.BootstrapResponse Bootstrap (1: kademlia_service_messages.BootstrapRequest request) ,
  kademlia_service_messages.DeleteResponse Delete (1: kademlia_service_messages.DeleteRequest request) ,
  kademlia_service_messages.UpdateResponse Update (1: kademlia_service_messages.UpdateRequest request) ,
}

