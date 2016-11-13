DEPS=		$(top_builddir)/src/libsequoia.la
AM_CXXFLAGS = 	-I../src -I../contrib/sparsehash-2.0.3/src 
AM_CPPFLAGS = 	$(BOOST_CPPFLAGS)
AM_LDFLAGS =	-lfl -ldl \
		$(BOOST_GRAPH_LDFLAGS) \
		$(BOOST_TIMER_LDFLAGS) \
		$(BOOST_THREADS_LDFLAGS)
LDADD =		$(top_builddir)/src/libsequoia.la \
		$(BOOST_GRAPH_LIBS) \
		$(BOOST_TIMER_LIBS) \
		$(BOOST_THREAD_LIBS) \
		-lgtest

bin_PROGRAMS = moves_unittest parser_unittest \
    graphs_unittest tdc_unittest labeled_graph_unittest \
    mindegree_heuristic_unittest \
    logic_unittest

moves_unittest_SOURCES = moves_unittest.cpp
parser_unittest_SOURCES = parser_unittest.cpp
graphs_unittest_SOURCES = graphs_unittest.cpp
tdc_unittest_SOURCES = tdc_unittest.cpp
labeled_graph_unittest_SOURCES = labeled_graph_unittest.cpp
mindegree_heuristic_unittest_SOURCES = mindegree_heuristic_unittest.cpp
logic_unittest_SOURCES = logic_unittest.cpp

TESTS =	$(bin_PROGRAMS)

