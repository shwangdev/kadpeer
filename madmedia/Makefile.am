lib_LTLIBRARIES = libmadmedia.la
libmadmedia_la_SOURCES = datastore.cpp contact.cpp contact.h kademlia_config.h kadid.cpp kadid.h kadroutingtable.cpp kadroutingtable.h kbucket.cpp kbucket.h ../common/BigInt.cpp ../common/blah.cpp ../common/HashMap.cpp ../common/Makefile ../common/Makefile.am ../common/Makefile.in ../common/sha1.cpp ../common/TThread.cpp ../common/TThreadPool.cpp ../common/TTimer.cpp
#libmadmediainclude_HEADERS = contact.h kademlia_config.h kadid.h kadroutingtable.h kbucket.h
#libmadmediainclude_HEADERS = $(wildcard *.h )
libmadmediaincludedir = ${includedir}/madmedia
INCLUDES= -I ../include -I../include/common -I../tinyxml -I../librtmp/ -I./
LDADD = ../common/.libs/libcommon.a ../tinyxml/.libs/libtinyxml.a ../udt/.libs/libudt.a -lphread
DEFS=-DTIXML_USE_STL
