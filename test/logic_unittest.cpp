/*
 * Author: Alexander Langer
 */
#include "logic/structure.h"
#include "gtest/gtest.h"

using namespace sequoia;

TEST(SymbolsTests, Symbol) {
    const char* ident = "ident";
    Symbol sym(ident, 2, false, 1);

    ASSERT_TRUE(sym.identifier() == ident);
    ASSERT_TRUE(sym.arity() == 2);
    ASSERT_TRUE(sym.nesting_depth() == 1);

    ASSERT_TRUE(sym == Symbol(ident, 2, false, 1));
    ASSERT_FALSE(sym == Symbol("ident2", 2, false, 1));
    ASSERT_FALSE(sym == Symbol(ident, 3, false, 1));
    ASSERT_FALSE(sym == Symbol(ident, 2, false, 2));
}

TEST(SymbolsTests, UnarySymbol) {
    const char* ident = "Ident";
    UnarySymbol sym(ident, 1, false);

    ASSERT_TRUE(sym.identifier() == ident);
    ASSERT_TRUE(sym.arity() == 1);
    ASSERT_TRUE(sym.nesting_depth() == 1);
    ASSERT_TRUE(sym == UnarySymbol(ident, 1, false));
    ASSERT_FALSE(sym == UnarySymbol("Ident2", 1, false));
    ASSERT_FALSE(sym == UnarySymbol(ident, 2, false));
}

TEST(SymbolsTests, ConstantSymbol) {
    const char* ident = "ident";
    ConstantSymbol sym(ident, 2, false);

    ASSERT_TRUE(sym.identifier() == ident);
    ASSERT_TRUE(sym.arity() == 0);
    ASSERT_TRUE(sym.nesting_depth() == 2);

    ASSERT_TRUE(sym == ConstantSymbol(ident, 2, false));
    ASSERT_FALSE(sym == ConstantSymbol("ident2", 2, false));
    ASSERT_FALSE(sym == ConstantSymbol(ident, 3, false));
}

TEST(VocabularyTests, Simple) {
    Symbol S("Unspecified", 2, false, 0);
    ConstantSymbol u1("u1", 1, false), u2("u2", 3, false);
    UnarySymbol U("U", 2, false);

    Vocabulary voc;
    ASSERT_EQ(0, voc.size());
    ASSERT_EQ(0, voc.number_of_unary_symbols());
    ASSERT_EQ(0, voc.number_of_constant_symbols());
    ASSERT_FALSE(voc.contains(&S));
    ASSERT_FALSE(voc.contains(&u1));
    ASSERT_FALSE(voc.contains(&U));
    ASSERT_FALSE(voc.contains(&u2));

    voc.add_symbol(&S);
    ASSERT_EQ(1, voc.size());
    ASSERT_EQ(0, voc.number_of_unary_symbols());
    ASSERT_EQ(0, voc.number_of_constant_symbols());
    ASSERT_TRUE(voc.contains(&S));
    ASSERT_FALSE(voc.contains(&u1));
    ASSERT_FALSE(voc.contains(&U));
    ASSERT_FALSE(voc.contains(&u2));

    Vocabulary* voc2 = voc.extend_by(&u1);
    ASSERT_EQ(2, voc2->size());
    ASSERT_EQ(0, voc2->number_of_unary_symbols());
    ASSERT_EQ(1, voc2->number_of_constant_symbols());
    ASSERT_TRUE(voc2->contains(&S));
    ASSERT_TRUE(voc2->contains(&u1));
    ASSERT_FALSE(voc2->contains(&U));
    ASSERT_FALSE(voc2->contains(&u2));

    Vocabulary* voc3 = voc2->extend_by(&U);
    ASSERT_EQ(3, voc3->size());
    ASSERT_EQ(1, voc3->number_of_unary_symbols());
    ASSERT_EQ(1, voc3->number_of_constant_symbols());
    ASSERT_TRUE(voc3->contains(&S));
    ASSERT_TRUE(voc3->contains(&u1));
    ASSERT_TRUE(voc3->contains(&U));
    ASSERT_FALSE(voc3->contains(&u2));

    Vocabulary* voc4 = voc3->extend_by(&u2);
    ASSERT_EQ(4, voc4->size());
    ASSERT_EQ(1, voc4->number_of_unary_symbols());
    ASSERT_EQ(2, voc4->number_of_constant_symbols());
    ASSERT_TRUE(voc4->contains(&S));
    ASSERT_TRUE(voc4->contains(&u1));
    ASSERT_TRUE(voc4->contains(&U));
    ASSERT_TRUE(voc4->contains(&u2));

    /* reduce and extend tests */
    ASSERT_TRUE(voc.reduces(voc2));
    ASSERT_TRUE(voc.reduces(voc3));
    ASSERT_TRUE(voc.reduces(voc4));
    ASSERT_FALSE(voc.extends(voc2));
    ASSERT_FALSE(voc.extends(voc3));
    ASSERT_FALSE(voc.extends(voc4));
    ASSERT_FALSE(voc2->reduces(&voc));
    ASSERT_TRUE(voc2->reduces(voc3));
    ASSERT_TRUE(voc2->reduces(voc4));
    ASSERT_TRUE(voc2->extends(&voc));
    ASSERT_FALSE(voc2->extends(voc3));
    ASSERT_FALSE(voc2->extends(voc4));
    ASSERT_FALSE(voc4->reduces(&voc));
    ASSERT_FALSE(voc4->reduces(voc2));
    ASSERT_FALSE(voc4->reduces(voc3));
    ASSERT_TRUE(voc4->extends(&voc));
    ASSERT_TRUE(voc4->extends(voc2));
    ASSERT_TRUE(voc4->extends(voc3));

    /* more adding etc and test operator== */
    ASSERT_TRUE(voc == Vocabulary(&voc));
    voc.add_constant_symbol(&u1);
    ASSERT_TRUE(voc == *voc2);
    voc.add_unary_symbol(&U);
    ASSERT_TRUE(voc == *voc3);
    voc.add_constant_symbol(&u2);
    ASSERT_TRUE(voc == *voc4);


    ASSERT_TRUE(voc4->symbol(0) == &S);
    ASSERT_TRUE(voc4->symbol(1) == (Symbol *) &u1);
    ASSERT_TRUE(voc4->symbol(2) == (Symbol *) &U);
    ASSERT_TRUE(voc4->symbol(3) == (Symbol *) &u2);
    ASSERT_TRUE(voc4->constant_symbol(0) == &u1);
    ASSERT_TRUE(voc4->constant_symbol(1) == &u2);
    ASSERT_TRUE(voc4->unary_symbol(0) == &U);

    /* test copy constructor */
    ASSERT_TRUE(voc == Vocabulary(&voc));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

