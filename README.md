sequoia-core
============

Sequoia MSO Core Library and Programs

Building
========

Required 3rd party tools and libraries:
boost including boost-graph, boost-thread, boost-regex (and, for static linking, the boost static libraries)

        ./configure && make

Important:  Requires support for GraphML compiled into boost-graph.
For some reason, RHEL 6.x does ship a boost-graph library w/o GraphML
support compiled in although the headers are present.  On RHEL 6.x (and
Centos, Scientific Linux, ...), please download a recent copy of boost
from http://www.boost.org and build a staged boost library (see 
boost's "Getting Started" guide for detailed info):

	cd /path/to/your/boost-src-directory
	sh bootstrap.sh && ./b2
	
Then run:

        ./configure --with-boost=/path/to/your/boost-src-directory


Multi-Threading Support (Beta)
==============================

Multi-threading support based on Intel's Thread Building Blocks library
is available.  To compile with multi-threading enabled, install Intel TBB 4.0
or higher and run

        ./configure --with-tbb


Acknowledgements
================
This project is supported by the DFG (German Research Foundation) under
grant RO 927/8.