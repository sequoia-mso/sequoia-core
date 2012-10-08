
#include "mindegree_heuristic.h"
#include "treedecomposition_check.h"
#include "structures/treedecomposition.h"
#include "structures/labeled_graph.h"
#include "gtest/gtest.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <vector>

TEST(MinDegreeHeuristicTests, Grid5x10Test) {
    sequoia::LabeledGraph g;
    std::ifstream ifile("grid-5x10.leda");
    g.load_leda_graph(ifile);
    ifile.close();

    sequoia::MinDegreeHeuristic<sequoia::LabeledGraph> a(g);
    a.compute();
    sequoia::TreeDecomposition *tdc = a.get();
    if (tdc == NULL)
        return;

    // Warning!!  Tdc must not be changed after we initialized the checker!
    typedef sequoia::TreeDecompositionCheck<sequoia::LabeledGraph> Checker;

    std::cout << "First check in progres..." << std::endl;
    Checker* check = new Checker(g, *tdc);
    // TODO:  Add check for cycles as follows:
    //tdc->add_edge(3, 20); // TODO add back check
    // TODO add non-connectivity check failure
    //tdc->bag(18).add(1);
    EXPECT_TRUE(check->valid());
    delete check;

    std::cout << "First check done, now make_nice" << std::endl;

    tdc->make_nice();

    std::cout << "Second test in progress..." << std::endl;

    check = new Checker(g, *tdc);
    EXPECT_TRUE(check->valid());
    EXPECT_TRUE(check->is_nice());
    //tdc.save_leda_graph(std::cout);
    
    delete check;
    delete tdc;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
