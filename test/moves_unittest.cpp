/*
 * Author: Alexander Langer
 */
#include "logic/moves.h"
#include "logic/assignment.h"
#include "gtest/gtest.h"

using namespace sequoia;

TEST(PointMoveTests, SimpleFunctions) {
  ConstantSymbol c8("c8", 8, false);

  ConstantSymbol c5("c5", 5, false);
  PointMove pmove(&c5);
  ASSERT_EQ(5, pmove.nesting_depth());

  /* add_label, remove_label and test_label*/
  ASSERT_FALSE(pmove.test_label(1));
  pmove.add_label(1);
  ASSERT_TRUE(pmove.test_label(1));
  pmove.remove_label(1);
  ASSERT_FALSE(pmove.test_label(1));

  /* add_edge, remove_edge and test_edge*/
  ASSERT_FALSE(pmove.test_edge(2));
  pmove.add_edge(2);
  ASSERT_TRUE(pmove.test_edge(2));
  pmove.remove_edge(2);
  ASSERT_FALSE(pmove.test_edge(2));

  /* Test operator== */
  PointMove pmove2(pmove);
  ASSERT_TRUE(pmove == pmove2);
  ASSERT_TRUE(pmove == PointMove(&c5));

  ASSERT_FALSE(pmove == PointMove(&c8));
  pmove.add_edge(1);
  ASSERT_FALSE(pmove == pmove2);
  pmove2.add_edge(1);
  ASSERT_TRUE(pmove == pmove2);
  pmove.add_label((unsigned int)0);
  ASSERT_FALSE(pmove == pmove2);
  pmove2.add_label((unsigned int)0);
  ASSERT_TRUE(pmove == pmove2);

  /* Test copy constructor */
  PointMove pmove3(pmove);
  ASSERT_TRUE(pmove == pmove3);
  ASSERT_TRUE(pmove.hash() == pmove3.hash());
}

TEST(PointMoveTests, Renaming) {
  ConstantSymbol c10("c10", 10, false);
  /* create new PointMove with global nesting depth 10, obj nesting depth 8 */
  PointMove* pmove3 = new PointMove(&c10);
  pmove3->add_edge((unsigned int)0);
  pmove3->add_edge(3);
  pmove3->add_edge(7);
  /* edges now at 0, 3, 7 */
  ASSERT_TRUE(pmove3->test_edge((unsigned int) 0));
  ASSERT_FALSE(pmove3->test_edge(1));
  ASSERT_FALSE(pmove3->test_edge(2));
  ASSERT_TRUE(pmove3->test_edge(3));
  ASSERT_FALSE(pmove3->test_edge(4));
  ASSERT_FALSE(pmove3->test_edge(5));
  ASSERT_FALSE(pmove3->test_edge(6));
  ASSERT_TRUE(pmove3->test_edge(7));

  pmove3->rename_obj_up(3, 3);
  /* edges now at 0, 4, 7 */
  ASSERT_FALSE(pmove3->test_edge(2));
  ASSERT_FALSE(pmove3->test_edge(3));
  ASSERT_TRUE(pmove3->test_edge(4));
  ASSERT_FALSE(pmove3->test_edge(5));
  ASSERT_FALSE(pmove3->test_edge(6));
  ASSERT_TRUE(pmove3->test_edge(7));

  pmove3->rename_obj_up(0, 4);
  /* edges now at 1, 5, 7 */
  ASSERT_FALSE(pmove3->test_edge((unsigned int)0));
  ASSERT_TRUE(pmove3->test_edge(1));
  ASSERT_FALSE(pmove3->test_edge(2));
  ASSERT_FALSE(pmove3->test_edge(3));
  ASSERT_FALSE(pmove3->test_edge(4));
  ASSERT_TRUE(pmove3->test_edge(5));
  ASSERT_FALSE(pmove3->test_edge(6));
  ASSERT_TRUE(pmove3->test_edge(7));

  pmove3->rename_obj_down(3, 7);
  /* edges now at 1, 4, 6 */
  ASSERT_FALSE(pmove3->test_edge((unsigned int)0));
  ASSERT_TRUE(pmove3->test_edge(1));
  ASSERT_FALSE(pmove3->test_edge(2));
  ASSERT_FALSE(pmove3->test_edge(3));
  ASSERT_TRUE(pmove3->test_edge(4));
  ASSERT_FALSE(pmove3->test_edge(5));
  ASSERT_TRUE(pmove3->test_edge(6));
  ASSERT_FALSE(pmove3->test_edge(7));

  pmove3->rename_obj_down(1, 7);
  /* edges now at 0, 3, 5 */
  ASSERT_TRUE(pmove3->test_edge((unsigned int)0));
  ASSERT_FALSE(pmove3->test_edge(1));
  ASSERT_FALSE(pmove3->test_edge(2));
  ASSERT_TRUE(pmove3->test_edge(3));
  ASSERT_FALSE(pmove3->test_edge(4));
  ASSERT_TRUE(pmove3->test_edge(5));
  ASSERT_FALSE(pmove3->test_edge(6));
  ASSERT_FALSE(pmove3->test_edge(7));

  /* hash test */
  PointMove pmove4(*pmove3);
  ASSERT_TRUE(*pmove3 == pmove4);
  pmove4.rename_obj_up(0, 6);
  /* edges in pmove4 now at 1, 4, 6*/
  PointMove pmove8(&c10);
  pmove8.add_edge(1);
  pmove8.add_edge(4);
  pmove8.add_edge(6);
  ASSERT_TRUE(pmove4 == pmove8);
  ASSERT_TRUE(pmove4.hash() == pmove8.hash());
  pmove4.rename_obj_down(1, 7);
  ASSERT_TRUE(*pmove3 == pmove4);
  ASSERT_TRUE(pmove3->hash() == pmove4.hash());

  PointMove pmove5(&c10);
  PointMove pmove6(&c10);
  pmove5.add_edge(2);
  pmove5.add_edge(4);
  pmove5.add_edge(7);
  pmove6.add_edge(3);
  pmove6.add_edge(5);
  pmove6.add_edge(8);
  pmove5.rename_obj_up(0, 8);
  ASSERT_TRUE(pmove5 == pmove6);
  ASSERT_TRUE(pmove5.hash() == pmove6.hash());
}

/*
 * Unit Tests for the SetMove class.
 */
TEST(SetMoveTests, SimpleFunctions) {
    UnarySymbol u8("U8", 8, false);
    UnarySymbol u9("U9", 9, false);
    /* Test whether the constructor sets all fields as expected */
    SetMove smove(&u8);
    ASSERT_EQ(8, smove.nesting_depth());

    /* Test operator== */
    SetMove smove2(&u8);
    ASSERT_TRUE(smove == smove2);
    ASSERT_FALSE(smove == SetMove(&u9));

    /* test add(), remove(), test(), and size() */
    ASSERT_FALSE(smove.test(2));
    ASSERT_FALSE(smove.test(3));
    smove.add(2);
    smove.add(3);
    ASSERT_TRUE(smove.test(2));
    ASSERT_TRUE(smove.test(3));
    ASSERT_EQ(2, smove.size());
    smove.remove(3);
    ASSERT_TRUE(smove.test(2));
    ASSERT_FALSE(smove.test(3));
    ASSERT_EQ(1, smove.size());

    /* Test copy constructor */
    SetMove smove3(smove);
    ASSERT_TRUE(smove.hash() == smove3.hash());

    /* Test hash */
    SetMove smove4(&u8);
    smove4.add(2);
    ASSERT_TRUE(smove.hash() == smove4.hash());
}

TEST(SetMoveTests, Renaming) {
    UnarySymbol u8("U8", 8, false);
    UnarySymbol u9("U9", 9, false);
  SetMove smove1(&u8);
  std::cout << smove1.toString() << std::endl;
  ASSERT_FALSE(smove1.test((unsigned int)0));
  ASSERT_FALSE(smove1.test(1));
  ASSERT_FALSE(smove1.test(2));
  ASSERT_FALSE(smove1.test(3));
  ASSERT_FALSE(smove1.test(4));

  std::cout << "Add 2,4" << std::endl;
  smove1.add(2);
  smove1.add(4);
  std::cout << smove1.toString() << std::endl;
  ASSERT_FALSE(smove1.test((unsigned int)0));
  ASSERT_FALSE(smove1.test(1));
  ASSERT_TRUE(smove1.test(2));
  ASSERT_FALSE(smove1.test(3));
  ASSERT_TRUE(smove1.test(4));

  std::cout << "rename_obj_down(2, 5)" << std::endl;
  smove1.rename_obj_down(2, 5);
  std::cout << smove1.toString() << std::endl;
  ASSERT_FALSE(smove1.test((unsigned int)0));
  ASSERT_TRUE(smove1.test(1));
  ASSERT_FALSE(smove1.test(2));
  ASSERT_TRUE(smove1.test(3));
  ASSERT_FALSE(smove1.test(4));

  std::cout << "rename_obj_down(1, 5)" << std::endl;
  smove1.rename_obj_down(1, 5);
  std::cout << smove1.toString() << std::endl;
  ASSERT_TRUE(smove1.test((unsigned int)0));
  ASSERT_FALSE(smove1.test(1));
  ASSERT_TRUE(smove1.test(2));
  ASSERT_FALSE(smove1.test(3));
  ASSERT_FALSE(smove1.test(4));

  std::cout << "Remove 0, add 4" << std::endl;
  smove1.remove((unsigned int)0);
  smove1.add(4);
  std::cout << smove1.toString() << std::endl;
  ASSERT_FALSE(smove1.test((unsigned int)0));
  ASSERT_FALSE(smove1.test(1));
  ASSERT_TRUE(smove1.test(2));
  ASSERT_FALSE(smove1.test(3));
  ASSERT_TRUE(smove1.test(4));

  std::cout << "rename_obj_down(1, 5)" << std::endl;
  smove1.rename_obj_down(1, 5);
  std::cout << smove1.toString() << std::endl;
  ASSERT_FALSE(smove1.test((unsigned int)0));
  ASSERT_TRUE(smove1.test(1));
  ASSERT_FALSE(smove1.test(2));
  ASSERT_TRUE(smove1.test(3));
  ASSERT_FALSE(smove1.test(4));

  std::cout << "rename_obj_down(1, 5)" << std::endl;
  smove1.rename_obj_down(1, 5);
  std::cout << smove1.toString() << std::endl;
  ASSERT_TRUE(smove1.test((unsigned int)0));
  ASSERT_FALSE(smove1.test(1));
  ASSERT_TRUE(smove1.test(2));
  ASSERT_FALSE(smove1.test(3));
  ASSERT_FALSE(smove1.test(4));

  SetMove smove2(&u8);
  smove2.add((unsigned int)0);
  smove2.add(2);
  ASSERT_TRUE(smove1.hash() == smove2.hash());
  ASSERT_TRUE(smove1 == smove2);

  std::cout << "rename_obj_down(2, 2)" << std::endl;
  smove1.rename_obj_down(2, 2);
  std::cout << smove1.toString() << std::endl;
  ASSERT_TRUE(smove1.test((unsigned int)0));
  ASSERT_TRUE(smove1.test(1));
  ASSERT_FALSE(smove1.test(2));
  ASSERT_FALSE(smove1.test(3));
  ASSERT_FALSE(smove1.test(4));

  std::cout << "rename_obj_up(2, 2) // no effect" << std::endl;
  smove1.rename_obj_up(2, 2);
  std::cout << smove1.toString() << std::endl;
  ASSERT_TRUE(smove1.test((unsigned int)0));
  ASSERT_TRUE(smove1.test(1));
  ASSERT_FALSE(smove1.test(2));
  ASSERT_FALSE(smove1.test(3));

  std::cout << "rename_obj_up(1, 1)" << std::endl;
  smove1.rename_obj_up(1, 1);
  std::cout << smove1.toString() << std::endl;
  ASSERT_TRUE(smove1 == smove2);

  std::cout << "rename_obj_up(3, 3) // no effect" << std::endl;
  smove1.rename_obj_up(3, 3);
  std::cout << smove1.toString() << std::endl;
  ASSERT_TRUE(smove1 == smove2);

  std::cout << "rename_obj_up(0, 3)" << std::endl;
  smove1.rename_obj_up(0, 3);
  std::cout << smove1.toString() << std::endl;
  ASSERT_FALSE(smove1.test((unsigned int)0));
  ASSERT_TRUE(smove1.test(1));
  ASSERT_FALSE(smove1.test(2));
  ASSERT_TRUE(smove1.test(3));
  ASSERT_FALSE(smove1.test(4));

  std::cout << "rename_obj_up(0, 3)" << std::endl;
  smove1.rename_obj_up(0, 3);
  std::cout << smove1.toString() << std::endl;
  ASSERT_FALSE(smove1.test((unsigned int)0));
  ASSERT_FALSE(smove1.test(1));
  ASSERT_TRUE(smove1.test(2));
  ASSERT_FALSE(smove1.test(3));
  ASSERT_TRUE(smove1.test(4));

  std::cout << "rename_obj_up(2, 2)" << std::endl;
  smove1.rename_obj_up(2,2);
  std::cout << smove1.toString() << std::endl;
  ASSERT_FALSE(smove1.test((unsigned int)0));
  ASSERT_FALSE(smove1.test(1));
  ASSERT_FALSE(smove1.test(2));
  ASSERT_TRUE(smove1.test(3));
  ASSERT_TRUE(smove1.test(4));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

