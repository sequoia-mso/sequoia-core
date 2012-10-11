DEPS=				$(top_builddir)/src/libsequoia.la
AM_CXXFLAGS = 			-fPIC -I../src -I../contrib/sparsehash-2.0.2/src
AM_CFLAGS = 			-I../src
AM_CPPFLAGS = 			$(BOOST_CPPFLAGS)
AM_CXXFLAGS += 			-Wno-deprecated
AM_LDFLAGS =			$(BOOST_SYSTEM_LDFLAGS)
LDADD  = 			$(top_builddir)/src/libsequoia.la \
				$(BOOST_SYSTEM_LIBS) \
				$(BOOST_GRAPH_LIBS) \
				$(BOOST_TIMER_LIBS) \
				$(BOOST_CHRONO_LIBS) \
				$(BOOST_FILESYSTEM_LIBS) \
				$(BOOST_THREAD_LIBS) 

bin_PROGRAMS =  		cnf2graph \
				compute_tw_bound \
				connected_components \
				file_format_converter \
				gen_grid_tdc \
				gen_rand_graph \
				make_incidence_graph \
				make_nice_tdc

cnf2graph_SOURCES =		cnf2graph.cpp
compute_tw_bound_SOURCES = 	compute_tw_bound.cpp
connected_components_SOURCES =	connected_components.cpp
gen_grid_tdc_SOURCES =		gen_grid_tdc.cpp
gen_rand_graph_SOURCES =  	gen_rand_graph.cpp
make_incidence_graph_SOURCES = 	make_incidence_graph.cpp
make_nice_tdc_SOURCES =		make_nice_tdc.cpp
file_format_converter_SOURCES =	file_format_converter.cpp
