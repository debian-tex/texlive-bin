/*************************************************************************
** CalculatorTest.cpp                                                   **
**                                                                      **
** This file is part of dvisvgm -- the DVI to SVG converter             **
** Copyright (C) 2005-2013 Martin Gieseking <martin.gieseking@uos.de>   **
**                                                                      **
** This program is free software; you can redistribute it and/or        **
** modify it under the terms of the GNU General Public License as       **
** published by the Free Software Foundation; either version 3 of       **
** the License, or (at your option) any later version.                  **
**                                                                      **
** This program is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         **
** GNU General Public License for more details.                         **
**                                                                      **
** You should have received a copy of the GNU General Public License    **
** along with this program; if not, see <http://www.gnu.org/licenses/>. **
*************************************************************************/

#include <gtest/gtest.h>
#include "Calculator.h"

TEST(CalculatorTest, eval) {
	Calculator calc;
	EXPECT_EQ(calc.eval("2+3+4"),    9);
	EXPECT_EQ(calc.eval("2*3+4"),   10);
	EXPECT_EQ(calc.eval("2+3*4"),   14);
	EXPECT_EQ(calc.eval("(2+3)*4"), 20);
	EXPECT_EQ(calc.eval("2*(3+4)"), 14);
	EXPECT_EQ(calc.eval("-2+3+4"),   5);
	EXPECT_EQ(calc.eval("3/2"),    1.5);
	EXPECT_EQ(calc.eval("3%2"),      1);
}


TEST(CalculatorTest, variables) {
	Calculator calc;
	calc.setVariable("a", 1);
	EXPECT_EQ(calc.getVariable("a"), 1);
	
	calc.setVariable("a", 2);
	EXPECT_EQ(calc.getVariable("a"), 2);
	
	calc.setVariable("b", 3);
	EXPECT_EQ(calc.eval("a+b"), 5);
	EXPECT_EQ(calc.eval("2a+2b"), 10);
}


TEST(CalculatorTest, exceptions) {
	Calculator calc;
	ASSERT_THROW(calc.eval("2++3"), CalculatorException);
	ASSERT_THROW(calc.eval("c"), CalculatorException);
	ASSERT_THROW(calc.eval("1/0"), CalculatorException);
	ASSERT_THROW(calc.eval("1%0"), CalculatorException);
	ASSERT_THROW(calc.eval("2*(3+4"), CalculatorException);
	ASSERT_THROW(calc.eval("2*(3+4))"), CalculatorException);
}

