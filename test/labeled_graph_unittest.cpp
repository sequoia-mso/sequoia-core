#include "structures/labeled_graph.h"
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

#include <tr1/functional>

TEST(GraphsTests, LEDAFileIOTest) {
    sequoia::LabeledGraph g;
    std::ifstream ifile("labeled_graph_unittest.leda");
    g.load_leda_graph(ifile);
    ifile.close();

    //g.save(std::cout);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
