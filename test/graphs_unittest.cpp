#include "structures/graph.h"
#include "gtest/gtest.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adjacency_list_io.hpp>
#include <boost/graph/graphviz.hpp>


#include <boost/graph/adj_list_serialize.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <boost/serialization/string.hpp>

struct MyProperties {
    std::string name;

    template<class archive> void load(archive& ar, const unsigned int version) {
        ar & boost::serialization::make_nvp("Name", name);
    }
    template<class archive> void save(archive& ar, const unsigned int version) const {
        ar & boost::serialization::make_nvp("Name", name);
    }
    // define serialize() using save() and load()
    BOOST_SERIALIZATION_SPLIT_MEMBER();
};

#if 0 // unused
std::ostream& operator<<(std::ostream& out, const MyProperties& prop) {
    out << "[name=" << prop.name << "]";
    return out;
}
#endif

typedef sequoia::VertexPropertyGraph<MyProperties> MyGraph;

TEST(GraphsTests, FileIOTest) {
    MyGraph *G = new MyGraph(10);
    std::vector<MyGraph::vertex_descriptor> vertices(10);

    G->property(0).name = "A test name";
    G->add_edge(1, 9);
    G->add_edge(1, 6);
    MyGraph::edge_descriptor e = G->add_edge(2, 4);

#if 0
    // serialize graph to xml stream
    std::stringstream sstream;
    G->output_xml(sstream);

    // unserialize this graph back to this stream
    MyGraph *G3 = new MyGraph();
    G3->load_xml(sstream);

    // check whether these graphs do match
    ASSERT_EQ(10, G3->num_vertices());
    ASSERT_STREQ("A test name", G3->property(0).name.c_str());
    ASSERT_TRUE(G->is_out_neighbor(2, 4));
    ASSERT_TRUE(G->is_out_neighbor(2, 4));

    // re-serialize the new graph again
    std::stringstream sstream3;
    G->output_xml(sstream3);
    // check whether the two serialized stirngs match
    ASSERT_STREQ(sstream.str().c_str(), sstream3.str().c_str());

    //std::cout << sstream3.str() << std::endl;
#endif

#if 0
    using namespace boost;
    typedef boost::property_map<MyGraph::GraphImpl, boost::vertex_degree_t>::type VertexDegreeMap;
    VertexDegreeMap g = boost::get(degree, G->_graph);
#endif


}

TEST(GraphsTests, LEDAFileIOTest) {
    sequoia::Graph ledaG;
    std::ifstream ifile("graphs_unittest.leda");
    sequoia::GraphFactory<sequoia::Graph> fac;
    fac.load_leda_graph(ledaG, ifile);
    ifile.close();

#if 0 // disabled with the hack
    ASSERT_EQ(200, ledaG.num_vertices());
    ASSERT_STREQ("Arbitrary File Labels", ledaG.property(27).c_str());
#endif

#if 0
    ASSERT_TRUE(G->test_node_label(0, 0));
    ASSERT_FALSE(G->test_node_label(1, 0));
    ASSERT_TRUE(G->test_node_label(126, 0));
    ASSERT_FALSE(G->test_node_label(125, 0));
#endif
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
