#include "../gtest/gtest.h"
#include <cmath>
#include "../src/Translator.h"

double Evaluate(std::string expr) {
    auto tokens = parse_into_tokens(expr);
    return solve(tokens);
}

const double EPS = 1e-9;

TEST(ArithmeticTest, BasicOps) {
    EXPECT_DOUBLE_EQ(Evaluate("1+1"), 2.0);
    EXPECT_DOUBLE_EQ(Evaluate("10-5"), 5.0);
    EXPECT_DOUBLE_EQ(Evaluate("3*4"), 12.0);
    EXPECT_DOUBLE_EQ(Evaluate("20/5"), 4.0);
    EXPECT_DOUBLE_EQ(Evaluate("1.5+2.5"), 4.0);
    EXPECT_DOUBLE_EQ(Evaluate("0+0"), 0.0);
    EXPECT_DOUBLE_EQ(Evaluate("100-0.5"), 99.5);
    EXPECT_DOUBLE_EQ(Evaluate("10/4"), 2.5);
    EXPECT_DOUBLE_EQ(Evaluate("1*1*1*1"), 1.0);
    EXPECT_DOUBLE_EQ(Evaluate("1.005+0.005"), 1.01);
}

TEST(PrecedenceTest, OrderOfOps) {
    EXPECT_DOUBLE_EQ(Evaluate("2+2*2"), 6.0);
    EXPECT_DOUBLE_EQ(Evaluate("10-4/2"), 8.0);
    EXPECT_DOUBLE_EQ(Evaluate("2*3+4*5"), 26.0);
    EXPECT_DOUBLE_EQ(Evaluate("1+2+3*4*5"), 63.0);
    EXPECT_DOUBLE_EQ(Evaluate("10/2+3"), 8.0);
    EXPECT_DOUBLE_EQ(Evaluate("3+10/2"), 8.0);
    EXPECT_DOUBLE_EQ(Evaluate("2*2*2+1"), 9.0);
    EXPECT_DOUBLE_EQ(Evaluate("5+5-5+5"), 10.0);
    EXPECT_DOUBLE_EQ(Evaluate("10/5*2"), 4.0);
    EXPECT_DOUBLE_EQ(Evaluate("2+3*4-5"), 9.0);
}

TEST(BracketTest, NestedBrackets) {
    EXPECT_DOUBLE_EQ(Evaluate("(2+2)*2"), 8.0);
    EXPECT_DOUBLE_EQ(Evaluate("((1+1))"), 2.0);
    EXPECT_DOUBLE_EQ(Evaluate("10/(2+3)"), 2.0);
    EXPECT_DOUBLE_EQ(Evaluate("2*(3+(4-2))"), 10.0);
    EXPECT_DOUBLE_EQ(Evaluate("(1+2)*(3+4)"), 21.0);
    EXPECT_DOUBLE_EQ(Evaluate("((((1+1)*2)*2)*2)"), 16.0);
    EXPECT_DOUBLE_EQ(Evaluate("10-(2+2+2)"), 4.0);
    EXPECT_DOUBLE_EQ(Evaluate("((10))"), 10.0);
    EXPECT_DOUBLE_EQ(Evaluate("2*(1+(1+(1+1)))"), 8.0);
    EXPECT_DOUBLE_EQ(Evaluate("((1+2)/3)"), 1.0);
}

TEST(UnaryTest, Signs) {
    EXPECT_DOUBLE_EQ(Evaluate("+5"), 5.0);
    EXPECT_DOUBLE_EQ(Evaluate("-2*(-2)"), 4.0);
}

TEST(PowerTest, Exponent) {
    EXPECT_DOUBLE_EQ(Evaluate("2^3"), 8.0);
    EXPECT_DOUBLE_EQ(Evaluate("-2^2"), -4.0);
    EXPECT_DOUBLE_EQ(Evaluate("(-2)^2"), 4.0);
}

TEST(FunctionTest, SinCos) {
    // EXPECT_NEAR(Evaluate("sin(sin(cos--1)))"), 0.0, EPS);
    EXPECT_NEAR(Evaluate("sin(0)"), 0.0, EPS);
    EXPECT_NEAR(Evaluate("cos(0)"), 1.0, EPS);
    EXPECT_NEAR(Evaluate("sin(3.1415926535/2)"), 1.0, EPS);
    EXPECT_NEAR(Evaluate("cos(3.1415926535)"), -1.0, EPS);
    EXPECT_NEAR(Evaluate("sin(0)^2 + cos(0)^2"), 1.0, EPS);
    EXPECT_NEAR(Evaluate("sin(cos(0)-1)"), 0.0, EPS);
    EXPECT_NEAR(Evaluate("-sin(0)"), 0.0, EPS);
    EXPECT_NEAR(Evaluate("cos(2*0)"), 1.0, EPS);
    EXPECT_NEAR(Evaluate("sin(1+1-2)"), 0.0, EPS);
    EXPECT_NEAR(Evaluate("cos(0)^2"), 1.0, EPS);
}

TEST(ComplexTest, Mixed) {
    EXPECT_DOUBLE_EQ(Evaluate("2^3*2 + (10/2)"), 21.0);
    EXPECT_NEAR(Evaluate("sin(0) + cos(0) * 10 - 2^2"), 6.0, EPS);
    EXPECT_DOUBLE_EQ(Evaluate("1.25 * 4 + 5.5 * 2"), 16.0);
    EXPECT_DOUBLE_EQ(Evaluate("((2+2)*2)^2"), 64.0);
    EXPECT_NEAR(Evaluate("2^sin(0)"), 1.0, EPS);
    EXPECT_DOUBLE_EQ(Evaluate("100/10/2"), 5.0);
    EXPECT_DOUBLE_EQ(Evaluate("1+2+3+4+5+6+7+8+9+10"), 55.0);
    EXPECT_DOUBLE_EQ(Evaluate("2^3^1"), 8.0);
    EXPECT_NEAR(Evaluate("cos(3.1415926535/2 * 2)"), -1.0, EPS);
}

TEST(EdgeCaseTest, Errors) {
    EXPECT_THROW(Evaluate("1++1"), std::invalid_argument);
    EXPECT_THROW(Evaluate("2+"), std::invalid_argument);
    EXPECT_THROW(Evaluate(""), std::invalid_argument);
}