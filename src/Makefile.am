AM_YFLAGS=			-d
AM_CXXFLAGS = 			-fPIC -I../contrib/sparsehash-2.0.3/src 
AM_CPPFLAGS = 			$(BOOST_CPPFLAGS)
AM_CXXFLAGS += 			-Wno-deprecated
#AM_CXXFLAGS +=    		-O0 -DDODEBUG -pg # debugging

AM_LDFLAGS =			$(BOOST_SYSTEM_LDFLAGS)
LDADD  = 			libsequoia.la \
				$(BOOST_SYSTEM_LIBS) \
				$(BOOST_GRAPH_LIBS) \
				$(BOOST_TIMER_LIBS) \
				$(BOOST_CHRONO_LIBS) \
				$(BOOST_FILESYSTEM_LIBS) \
				$(BOOST_THREAD_LIBS)

SEQUOIA_LIBRARY_VERSION =	1:0:0

lib_LTLIBRARIES =   		libsequoia.la

libsequoia_la_LDFLAGS =		-version-info $(SEQUOIA_LIBRARY_VERSION)
libsequoia_la_SOURCES =		common.cpp \
				structures/graphml_reader.cpp \
				structures/treedecomposition.cpp \
				structures/graph_structure.cpp \
				structures/graph_structure_factory.cpp \
				logic/vocabulary.cpp \
				ms2grammar.ypp \
				ms2scanner.lpp  \
				parseformula.cpp \
				game_determined.cpp \
				game_atomar.cpp \
				atomar_game_factory.cpp \
				leaf_game_factory.cpp \
				game_q_forget_iterator.cpp \
				game_q_introduce_iterator.cpp \
				dyn_prog_solver.cpp \
				sequoia_table.cpp \
				sequoia_solver.cpp \
				moves_pool.cpp \
				cache_introduce.cpp \
				cache_forget.cpp \
				cache_join.cpp \
				cache_control.cpp \
				temp_symbol_factory.cpp \
				sequoia_eval_factory.cpp \
				incidence_graph.cpp \
				sequoia_facade.cpp

bin_PROGRAMS =  		sequoia

sequoia_SOURCES =		main.cpp sequoia_app.cpp
