sbin_PROGRAMS = kadpeer
kadpeer_SOURCES = kademlia/contact.cpp\
				  kademlia/kadroutingtable.cpp \
				  kademlia/kadid.cpp\
			      kademlia/datastore.cpp\
				  kademlia/kbucket.cpp\
				  kademlia/kadrpc.cpp\
				  kademlia/kadservice.cpp\
				  kademlia/knodeimpl.cpp\
				  common/sha1.cpp\
    			  common/TThreadPool.cpp\
 			      common/TTimer.cpp\
				  common/blah.cpp\
                  common/HashMap.cpp\
		  		  common/crypto.cpp\
  			      common/BigInt.cpp\
                  common/TThread.cpp\
				  thrift/gen-cpp/contact_info_constants.cpp\
			      thrift/gen-cpp/KademliaService.cpp\
			      thrift/gen-cpp/signed_kadvalue_types.cpp\
				  thrift/gen-cpp/contact_info_types.cpp\
				  thrift/gen-cpp/kademlia_service_messages_constants.cpp\
				  thrift/gen-cpp/kademlia_service_types.cpp\
				  thrift/gen-cpp/kademlia_service_constants.cpp\
			      thrift/gen-cpp/kademlia_service_messages_types.cpp\
			      thrift/gen-cpp/signed_kadvalue_constants.cpp\
				  oggplayer/Decoder.cpp\
				  oggplayer/oggutil.cpp\
				  oggplayer/SeekBar.cpp\
				  ClientSocket.cpp\
				  ServerSocket.cpp\
			      Socket.cpp\
				  UDTClientSocket.cpp\
				  UDTServerSocket.cpp\
				  UDTSocket.cpp\
				  main.cpp
kadpeer_CPPFLAGS = -I ../include \
		   -I../include/common\
		   -I../tinyxml\
		   -I../udt\
		   -I./kademlia\
		   -I./thrift/gen-cpp\
		   -I/usr/local/include/thrift\
		   -I./oggplayer
DEFS=-DTIXML_USE_STL
LDADD = ../udt/.libs/libudt.a ../tinyxml/.libs/libtinyxml.a ../librtmp/.libs/librtmp.a -lpthread -lthrift -lcryptopp -lboost_filesystem -lboost_system -lrt -lglib-2.0 -loggplay -lfishsound -loggz -lvorbis -lkate -logg -lsydneyaudio	-ltheora  -lSDLmain -lSDL -lasound
