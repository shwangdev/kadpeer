sbin_PROGRAMS = kadpeer
kadpeer_SOURCES = madmedia/contact.cpp\
				  madmedia/kadroutingtable.cpp \
				  madmedia/kadid.cpp\
			      madmedia/datastore.cpp\
				  madmedia/kbucket.cpp\
		          common/sha1.cpp\
    			  common/TThreadPool.cpp\
 			      common/TTimer.cpp\
				  common/blah.cpp\
                  common/HashMap.cpp\
  			      common/BigInt.cpp\
                  common/TThread.cpp\
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
		   -I./madmedia
DEFS=-DTIXML_USE_STL
LDADD = ../udt/.libs/libudt.a ../tinyxml/.libs/libtinyxml.a ../librtmp/.libs/librtmp.a -lpthread
