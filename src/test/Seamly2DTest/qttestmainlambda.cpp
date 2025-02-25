/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2017  Seamly, LLC                                       *
 *                                                                         *
 *   https://github.com/fashionfreedom/seamly2d                            *
 *                                                                         *
 ***************************************************************************
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
 **************************************************************************

 ************************************************************************
 **
 **  @file   qttestmainlambda.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   31 3, 2015
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

#include <QtTest>

#include "tst_vposter.h"
#include "tst_vabstractpiece.h"
#include "tst_vspline.h"
#include "tst_nameregexp.h"
#include "tst_vlayoutdetail.h"
#include "tst_varc.h"
#include "tst_vellipticalarc.h"
#include "tst_qmutokenparser.h"
#include "tst_vmeasurements.h"
#include "tst_vlockguard.h"
#include "tst_misc.h"
#include "tst_vcommandline.h"
#include "tst_vpiece.h"
#include "tst_findpoint.h"
#include "tst_vabstractcurve.h"
#include "tst_vcubicbezierpath.h"
#include "tst_vgobject.h"
#include "tst_vsplinepath.h"
#include "tst_vpointf.h"
#include "tst_readval.h"
#include "tst_vtranslatevars.h"

#include "../vmisc/def.h"
#include "../qmuparser/qmudef.h"
#include "../vmisc/vabstractapplication.h"
#include "../vmisc/projectversion.h"

class TestVApplication : public VAbstractApplication
{
public:

                                  TestVApplication(int &argc, char ** argv);
    virtual                      ~TestVApplication() Q_DECL_EQ_DEFAULT;

    virtual const VTranslateVars *TrVars();
    virtual void                  OpenSettings();
    virtual bool                  IsAppInGUIMode() const;
    virtual void                  InitTrVars();
};

//---------------------------------------------------------------------------------------------------------------------
TestVApplication::TestVApplication(int &argc, char **argv)
    : VAbstractApplication(argc, argv)
{
    setApplicationName(VER_INTERNALNAME_2D_STR);
    setOrganizationName(VER_COMPANYNAME_STR);
    OpenSettings();
}

//---------------------------------------------------------------------------------------------------------------------
const VTranslateVars *TestVApplication::TrVars()
{
    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
void TestVApplication::OpenSettings()
{
    settings = new VSettings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(),
                             QCoreApplication::applicationName(), this);
}

//---------------------------------------------------------------------------------------------------------------------
bool TestVApplication::IsAppInGUIMode() const
{
    return false;
}

//---------------------------------------------------------------------------------------------------------------------
void TestVApplication::InitTrVars()
{
}

//---------------------------------------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
    Q_INIT_RESOURCE(schema);

    TestVApplication app( argc, argv );// For QPrinter

    int status = 0;
    auto ASSERT_TEST = [&status, argc, argv](QObject* obj)
    {
        status |= QTest::qExec(obj, argc, argv);
        delete obj;
    };

    ASSERT_TEST(new TST_FindPoint());
    ASSERT_TEST(new TST_VPiece());
    ASSERT_TEST(new TST_VPoster());
    ASSERT_TEST(new TST_VAbstractPiece());
    ASSERT_TEST(new TST_VSpline());
    ASSERT_TEST(new TST_VSplinePath());
    ASSERT_TEST(new TST_NameRegExp());
    ASSERT_TEST(new TST_VLayoutDetail());
    ASSERT_TEST(new TST_VArc());
    ASSERT_TEST(new TST_VEllipticalArc());
    ASSERT_TEST(new TST_QmuTokenParser());
    ASSERT_TEST(new TST_VMeasurements());
    ASSERT_TEST(new TST_VLockGuard());
    ASSERT_TEST(new TST_Misc());
    ASSERT_TEST(new TST_VCommandLine());
    ASSERT_TEST(new TST_VAbstractCurve());
    ASSERT_TEST(new TST_VCubicBezierPath());
    ASSERT_TEST(new TST_VGObject());
    ASSERT_TEST(new TST_VPointF());
    ASSERT_TEST(new TST_ReadVal());
    ASSERT_TEST(new TST_VTranslateVars());

    return status;
}
