/************************************************************************
 **
 **  @file   tst_vellipticalarc.h
 **  @author Valentina Zhuravska <zhuravska19(at)gmail.com>
 **  @date   12 2, 2016
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentine project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2015 Seamly2D project
 **  <https://github.com/fashionfreedom/seamly2d> All Rights Reserved.
 **
 **  Seamly2D is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Seamly2D is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Seamly2D.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#ifndef TST_VELLIPTICALARC_H
#define TST_VELLIPTICALARC_H

#include "../vtest/abstracttest.h"

class TST_VEllipticalArc : public AbstractTest
{
    Q_OBJECT
public:
    explicit TST_VEllipticalArc(QObject *parent = nullptr);

private slots:
    void CompareTwoWays_data();
    void CompareTwoWays();
    void NegativeArc();
    void TestGetPoints1_data();
    void TestGetPoints2_data();
    void TestGetPoints3_data();
    void TestGetPoints4_data();
    void TestGetPoints1();
    void TestGetPoints2();
    void TestGetPoints3();
    void TestGetPoints4();
    void TestRotation_data();
    void TestRotation();
    void TestFlip_data();
    void TestFlip();

private:
    Q_DISABLE_COPY(TST_VEllipticalArc)
    void TestData();
};

#endif // TST_VELLIPTICALARC_H
