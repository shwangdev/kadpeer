sbin_PROGRAMS = kadpeer
kadpeer_SOURCES = ClientSocket.cpp main.cpp Makefile.am ServerSocket.cpp Socket.cpp UDTClientSocket.cpp UDTServerSocket.cpp UDTSocket.cpp
kadpeer_CPPFLAGS = -I ../include \
		   -I../include/common\
		   -I../tinyxml\
		   -I../udt\
		   -I../madmedia
DEFS=-DTIXML_USE_STL
LDADD = ../madmedia/.libs/libmadmedia.a ../udt/.libs/libudt.a ../tinyxml/.libs/libtinyxml.a ../librtmp/.libs/librtmp.a -lpthread
