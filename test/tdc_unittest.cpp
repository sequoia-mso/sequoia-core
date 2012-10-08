#include "structures/graph.h"
#include "structures/treedecomposition.h"
#include "gtest/gtest.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <vector>

#include <boost/graph/graph_utility.hpp>
#include <boost/graph/adjacency_list_io.hpp>

#include <boost/graph/adj_list_serialize.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <boost/serialization/string.hpp>

TEST(GraphsTests, FileIOTest) {
    sequoia::TreeDecomposition tdc;
    std::ifstream infile("tdc_unittest.leda");
    tdc.load_leda_graph(infile);
    //tdc.save(std::cout);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
