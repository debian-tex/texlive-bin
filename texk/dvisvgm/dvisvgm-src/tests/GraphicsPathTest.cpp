/*************************************************************************
** GraphicsPathTest.cpp                                                 **
**                                                                      **
** This file is part of dvisvgm -- a fast DVI to SVG converter          **
** Copyright (C) 2005-2017 Martin Gieseking <martin.gieseking@uos.de>   **
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
#include <sstream>
#include "GraphicsPath.hpp"

using std::ostringstream;

TEST(GraphicsPathTest, svg) {
	GraphicsPath<int> path;
	path.moveto(0,0);
	path.lineto(10,10);
	path.cubicto(20,20,30,30,40,40);
	path.closepath();
	EXPECT_FALSE(path.empty());
	EXPECT_EQ(path.size(), 4);
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M0 0L10 10C20 20 30 30 40 40Z");
	path.clear();
	EXPECT_TRUE(path.empty());
}


TEST(GraphicsPathTest, optimize) {
	GraphicsPath<int> path;
	path.moveto(0,0);
	path.lineto(10,0);
	path.lineto(10,20);
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M0 0H10V20");
}


TEST(GraphicsPathTest, transform) {
	GraphicsPath<double> path;
	path.moveto(0,0);
	path.lineto(1,0);
	path.lineto(1,1);
	path.lineto(0,1);
	path.closepath();
	Matrix m(1);
	m.scale(2,2);
	m.translate(10, 100);
	m.rotate(90);
	path.transform(m);
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M-100 10V12H-102V10Z");
}


TEST(GraphicsPathTest, closeOpenSubPaths) {
	GraphicsPath<double> path;
	path.moveto(0,0);
	path.lineto(1,0);
	path.lineto(1,1);
	path.lineto(0,1);
	path.moveto(10,10);
	path.lineto(11,10);
	path.lineto(11,11);
	path.lineto(10,11);
	path.closeOpenSubPaths();
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M0 0H1V1H0ZM10 10H11V11H10Z");
}


TEST(GraphicsPathTest, relative1) {
	GraphicsPath<int> path;
	path.moveto(0,0);
	path.lineto(10,10);
	path.lineto(10,20);
	path.cubicto(20,20,30,30,40,40);
	path.conicto(50, 50, 60, 60);
	path.lineto(100,60);
	path.closepath();
	ostringstream oss;
	path.writeSVG(oss, true);
	EXPECT_EQ(oss.str(), "m0 0l10 10v10c10 0 20 10 30 20q10 10 20 20h40z");
}


TEST(GraphicsPathTest, computeBBox) {
	GraphicsPath<int> path;
	path.moveto(10,10);
	path.lineto(100,10);
	path.conicto(10,100,40,80);
	path.cubicto(5,5,30,10,90,70);
	path.lineto(20,30);
	path.closepath();
	BoundingBox bbox;
	path.computeBBox(bbox);
	EXPECT_EQ(bbox, BoundingBox(5, 5, 100, 100));
}


TEST(GraphicsPathTest, removeRedundantCommands) {
	GraphicsPath<int> path;
	path.moveto(10,10);
	path.lineto(100,10);
	path.conicto(10,100,40,80);
	path.cubicto(5,5,30,10,90,70);
	path.moveto(10,10);
	path.moveto(15,10);
	path.moveto(20,20);
	path.lineto(20,30);
	path.moveto(10,10);
	path.moveto(20,20);
	path.removeRedundantCommands();
	ostringstream oss;
	path.writeSVG(oss, false);
	EXPECT_EQ(oss.str(), "M10 10H100Q10 100 40 80C5 5 30 10 90 70M20 20V30");
}
