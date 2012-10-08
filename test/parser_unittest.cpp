#include "parseformula.h"
#include "logic/vocabulary.h"
#include "gtest/gtest.h"

#include <iostream>

using namespace sequoia;

TEST(ParserTests, ParserTest) {
    Vocabulary *voc = new Vocabulary();
    voc->add_symbol(new Symbol("adj", 2, false, 0));
    Formula* res = NULL;
    res = parse_file(voc, "parser_unittest.mso");
    ASSERT_TRUE(res != NULL);
    std::cout << res->toString() << std::endl;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
