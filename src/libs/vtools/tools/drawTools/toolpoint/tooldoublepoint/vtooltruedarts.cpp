/***************************************************************************
 **  @file   vtooltruedarts.cpp
 **  @author Douglas S Caskey
 **  @date   17 Sep, 2023
 **
 **  @copyright
 **  Copyright (C) 2017 - 2023 Seamly, LLC
 **  https://github.com/fashionfreedom/seamly2d
 **
 **  @brief
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
 **  along with Seamly2D. If not, see <http://www.gnu.org/licenses/>.
 **************************************************************************/

/************************************************************************
 **  @file   vtooltruedarts.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   23 6, 2015
 **
 **  @brief
 **  @copyright
 **  This source code is part of the Valentina project, a pattern making
 **  program, whose allow create and modeling patterns of clothing.
 **  Copyright (C) 2013-2015 Valentina project
 **  <https://bitbucket.org/dismine/valentina> All Rights Reserved.
 **
 **  Valentina is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Valentina is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Seamly2D.  If not, see <http://www.gnu.org/licenses/>.
 **
 *************************************************************************/

#include "vtooltruedarts.h"

#include <QLine>
#include <QLineF>
#include <QSharedPointer>
#include <QStaticStringData>
#include <QStringData>
#include <QStringDataPtr>
#include <new>

#include "../../../../dialogs/tools/dialogtruedarts.h"
#include "../../../../dialogs/tools/dialogtool.h"
#include "../../../../visualization/visualization.h"
#include "../../../../visualization/line/vistooltruedarts.h"
#include "../ifc/exception/vexception.h"
#include "../ifc/ifcdef.h"
#include "../vgeometry/vgobject.h"
#include "../vgeometry/vpointf.h"
#include "../vpatterndb/vcontainer.h"
#include "../vwidgets/vmaingraphicsscene.h"
#include "../../../vabstracttool.h"
#include "../../vdrawtool.h"
#include "vtooldoublepoint.h"

template <class T> class QSharedPointer;

const QString VToolTrueDarts::ToolType = QStringLiteral("trueDarts");

//---------------------------------------------------------------------------------------------------------------------
VToolTrueDarts::VToolTrueDarts(VAbstractPattern *doc,
                               VContainer *data,
                               const quint32 &id,
                               const quint32 &p1id,
                               const quint32 &p2id,
                               const quint32 &baseLineP1Id,
                               const quint32 &baseLineP2Id,
                               const quint32 &dartP1Id,
                               const quint32 &dartP2Id,
                               const quint32 &dartP3Id,
                               const Source &typeCreation,
                               QGraphicsItem *parent)
    : VToolDoublePoint(doc, data, id, p1id, p2id, parent)
    , baseLineP1Id (baseLineP1Id)
    , baseLineP2Id(baseLineP2Id)
    , dartP1Id(dartP1Id)
    , dartP2Id(dartP2Id)
    , dartP3Id(dartP3Id)
{
    ToolCreation(typeCreation);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::FindPoint(const QPointF &baseLineP1, const QPointF &baseLineP2, const QPointF &dartP1,
                               const QPointF &dartP2, const QPointF &dartP3, QPointF &p1, QPointF &p2)
{
    const QLineF d2d1(dartP2, dartP1);
    const QLineF d2d3(dartP2, dartP3);

    const qreal degrees = d2d3.angleTo(d2d1);

    QLineF d2blP2(dartP2, baseLineP2);
    d2blP2.setAngle(d2blP2.angle()+degrees);

    if (QLineF(baseLineP1, d2blP2.p2()).intersects(d2d1, &p1) == QLineF::NoIntersection)
    {
        p1 = QPointF(0, 0);
        p2 = QPointF(0, 0);
        return;
    }

    QLineF d2p1(dartP2, p1);
    d2p1.setAngle(d2p1.angle()-degrees);
    p2 = d2p1.p2();
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::setDialog()
{
    SCASSERT(not m_dialog.isNull())
    QSharedPointer<DialogTrueDarts> dialogTool = m_dialog.objectCast<DialogTrueDarts>();
    SCASSERT(not dialogTool.isNull())

    const QSharedPointer<VPointF> p1 = VAbstractTool::data.GeometricObject<VPointF>(p1id);
    const QSharedPointer<VPointF> p2 = VAbstractTool::data.GeometricObject<VPointF>(p2id);

    dialogTool->SetChildrenId(p1id, p2id);
    dialogTool->SetNewDartPointNames(p1->name(), p2->name());
    dialogTool->SetFirstBasePointId(baseLineP1Id);
    dialogTool->SetSecondBasePointId(baseLineP2Id);
    dialogTool->SetFirstDartPointId(dartP1Id);
    dialogTool->SetSecondDartPointId(dartP2Id);
    dialogTool->SetThirdDartPointId(dartP3Id);
}

//---------------------------------------------------------------------------------------------------------------------
VToolTrueDarts *VToolTrueDarts::Create(QSharedPointer<DialogTool> dialog, VMainGraphicsScene *scene,
                                       VAbstractPattern *doc, VContainer *data)
{
    SCASSERT(not dialog.isNull())
    QSharedPointer<DialogTrueDarts> dialogTool = dialog.objectCast<DialogTrueDarts>();
    SCASSERT(not dialogTool.isNull())

    const QString point1Name = dialogTool->GetFirstNewDartPointName();
    const QString point2Name = dialogTool->GetSecondNewDartPointName();
    const quint32 baseLineP1Id = dialogTool->GetFirstBasePointId();
    const quint32 baseLineP2Id = dialogTool->GetSecondBasePointId();
    const quint32 dartP1Id = dialogTool->GetFirstDartPointId();
    const quint32 dartP2Id = dialogTool->GetSecondDartPointId();
    const quint32 dartP3Id = dialogTool->GetThirdDartPointId();

    VToolTrueDarts *point = Create(0, 0, 0, baseLineP1Id, baseLineP2Id, dartP1Id, dartP2Id, dartP3Id,
                                   point1Name, 5, 10, true, point2Name, 5, 10, true, scene, doc, data, Document::FullParse,
                                   Source::FromGui);
    if (point != nullptr)
    {
        point->m_dialog = dialogTool;
    }
    return point;
}

//---------------------------------------------------------------------------------------------------------------------
VToolTrueDarts *VToolTrueDarts::Create(quint32 _id,
                                       const quint32 &_p1id, const quint32 &_p2id,
                                       const quint32 &baseLineP1Id,
                                       const quint32 &baseLineP2Id,
                                       const quint32 &dartP1Id,
                                       const quint32 &dartP2Id,
                                       const quint32 &dartP3Id,
                                       const QString &point1Name,
                                       qreal mx1, qreal my1, bool showPointName1,
                                       const QString &point2Name,
                                       qreal mx2, qreal my2, bool showPointName2,
                                       VMainGraphicsScene *scene, VAbstractPattern *doc, VContainer *data,
                                       const Document &parse, const Source &typeCreation)
{
    const QSharedPointer<VPointF> baseLineP1 = data->GeometricObject<VPointF>(baseLineP1Id);
    const QSharedPointer<VPointF> baseLineP2 = data->GeometricObject<VPointF>(baseLineP2Id);
    const QSharedPointer<VPointF> dartP1 = data->GeometricObject<VPointF>(dartP1Id);
    const QSharedPointer<VPointF> dartP2 = data->GeometricObject<VPointF>(dartP2Id);
    const QSharedPointer<VPointF> dartP3 = data->GeometricObject<VPointF>(dartP3Id);

    QPointF fPoint1;
    QPointF fPoint2;
    VToolTrueDarts::FindPoint(static_cast<QPointF>(*baseLineP1), static_cast<QPointF>(*baseLineP2),
                              static_cast<QPointF>(*dartP1), static_cast<QPointF>(*dartP2),
                              static_cast<QPointF>(*dartP3), fPoint1, fPoint2);

    quint32 id = _id;
    quint32 p1id = _p1id;
    quint32 p2id = _p2id;

    VPointF *p1 = new VPointF(fPoint1, point1Name, mx1, my1, id);
    p1->setShowPointName(showPointName1);

    VPointF *p2 = new VPointF(fPoint2, point2Name, mx2, my2, id);
    p2->setShowPointName(showPointName2);


    if (typeCreation == Source::FromGui)
    {
        id = VContainer::getNextId();  //Just reserve id for tool
        p1id = data->AddGObject(p1);
        p2id = data->AddGObject(p2);
    }
    else
    {
        data->UpdateGObject(p1id, p1);
        data->UpdateGObject(p2id, p2);
        if (parse != Document::FullParse)
        {
            doc->UpdateToolData(id, data);
        }
    }

    if (parse == Document::FullParse)
    {
        VDrawTool::AddRecord(id, Tool::TrueDarts, doc);
        VToolTrueDarts *points = new VToolTrueDarts(doc, data, id, p1id, p2id, baseLineP1Id, baseLineP2Id,
                                                    dartP1Id, dartP2Id, dartP3Id, typeCreation);
        scene->addItem(points);
        InitToolConnections(scene, points);
        VAbstractPattern::AddTool(id, points);
        doc->IncrementReferens(baseLineP1->getIdTool());
        doc->IncrementReferens(baseLineP2->getIdTool());
        doc->IncrementReferens(dartP1->getIdTool());
        doc->IncrementReferens(dartP2->getIdTool());
        doc->IncrementReferens(dartP3->getIdTool());
        return points;
    }
    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::ShowVisualization(bool show)
{
    ShowToolVisualization<VisToolTrueDarts>(show);
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolTrueDarts::BaseLineP1Name() const
{
    return VAbstractTool::data.GetGObject(baseLineP1Id)->name();
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolTrueDarts::BaseLineP2Name() const
{
    return VAbstractTool::data.GetGObject(baseLineP2Id)->name();
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolTrueDarts::DartP1Name() const
{
    return VAbstractTool::data.GetGObject(dartP1Id)->name();
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolTrueDarts::DartP2Name() const
{
    return VAbstractTool::data.GetGObject(dartP2Id)->name();
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolTrueDarts::DartP3Name() const
{
    return VAbstractTool::data.GetGObject(dartP3Id)->name();
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolTrueDarts::GetBaseLineP1Id() const
{
    return baseLineP1Id;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::SetBaseLineP1Id(const quint32 &value)
{
    if (value != NULL_ID)
    {
        baseLineP1Id = value;

        QSharedPointer<VGObject> obj = VContainer::GetFakeGObject(m_id);
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolTrueDarts::GetBaseLineP2Id() const
{
    return baseLineP2Id;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::SetBaseLineP2Id(const quint32 &value)
{
    if (value != NULL_ID)
    {
        baseLineP2Id = value;

        QSharedPointer<VGObject> obj = VContainer::GetFakeGObject(m_id);
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolTrueDarts::GetDartP1Id() const
{
    return dartP1Id;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::SetDartP1Id(const quint32 &value)
{
    if (value != NULL_ID)
    {
        dartP1Id = value;

        QSharedPointer<VGObject> obj = VContainer::GetFakeGObject(m_id);
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolTrueDarts::GetDartP2Id() const
{
    return dartP2Id;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::SetDartP2Id(const quint32 &value)
{
    if (value != NULL_ID)
    {
        dartP2Id = value;

        QSharedPointer<VGObject> obj = VContainer::GetFakeGObject(m_id);
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolTrueDarts::GetDartP3Id() const
{
    return dartP3Id;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::SetDartP3Id(const quint32 &value)
{
    if (value != NULL_ID)
    {
        dartP3Id = value;

        QSharedPointer<VGObject> obj = VContainer::GetFakeGObject(m_id);
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::showContextMenu(QGraphicsSceneContextMenuEvent *event, quint32 id)
{
    try
    {
        ContextMenu<DialogTrueDarts>(event, id);
    }
    catch(const VExceptionToolWasDeleted &error)
    {
        Q_UNUSED(error)
        return;//Leave this method immediately!!!
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::RemoveReferens()
{
    const auto baseLineP1 = VAbstractTool::data.GetGObject(baseLineP1Id);
    const auto baseLineP2 = VAbstractTool::data.GetGObject(baseLineP2Id);
    const auto dartP1 = VAbstractTool::data.GetGObject(dartP1Id);
    const auto dartP2 = VAbstractTool::data.GetGObject(dartP2Id);
    const auto dartP3 = VAbstractTool::data.GetGObject(dartP3Id);

    doc->DecrementReferens(baseLineP1->getIdTool());
    doc->DecrementReferens(baseLineP2->getIdTool());
    doc->DecrementReferens(dartP1->getIdTool());
    doc->DecrementReferens(dartP2->getIdTool());
    doc->DecrementReferens(dartP3->getIdTool());
    VToolDoublePoint::RemoveReferens();
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::SaveDialog(QDomElement &domElement)
{
    SCASSERT(not m_dialog.isNull())
    QSharedPointer<DialogTrueDarts> dialogTool = m_dialog.objectCast<DialogTrueDarts>();
    SCASSERT(not dialogTool.isNull())

    doc->SetAttribute(domElement, AttrName1, dialogTool->GetFirstNewDartPointName());
    doc->SetAttribute(domElement, AttrName2, dialogTool->GetSecondNewDartPointName());
    doc->SetAttribute(domElement, AttrBaseLineP1, QString().setNum(dialogTool->GetFirstBasePointId()));
    doc->SetAttribute(domElement, AttrBaseLineP2, QString().setNum(dialogTool->GetSecondBasePointId()));
    doc->SetAttribute(domElement, AttrDartP1, QString().setNum(dialogTool->GetFirstDartPointId()));
    doc->SetAttribute(domElement, AttrDartP2, QString().setNum(dialogTool->GetSecondDartPointId()));
    doc->SetAttribute(domElement, AttrDartP3, QString().setNum(dialogTool->GetThirdDartPointId()));
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj)
{
    VToolDoublePoint::SaveOptions(tag, obj);

    doc->SetAttribute(tag, AttrType, ToolType);
    doc->SetAttribute(tag, AttrBaseLineP1, baseLineP1Id);
    doc->SetAttribute(tag, AttrBaseLineP2, baseLineP2Id);
    doc->SetAttribute(tag, AttrDartP1, dartP1Id);
    doc->SetAttribute(tag, AttrDartP2, dartP2Id);
    doc->SetAttribute(tag, AttrDartP3, dartP3Id);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::ReadToolAttributes(const QDomElement &domElement)
{
    baseLineP1Id = doc->GetParametrUInt(domElement, AttrBaseLineP1, NULL_ID_STR);
    baseLineP2Id = doc->GetParametrUInt(domElement, AttrBaseLineP2, NULL_ID_STR);
    dartP1Id = doc->GetParametrUInt(domElement, AttrDartP1, NULL_ID_STR);
    dartP2Id = doc->GetParametrUInt(domElement, AttrDartP2, NULL_ID_STR);
    dartP3Id = doc->GetParametrUInt(domElement, AttrDartP3, NULL_ID_STR);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolTrueDarts::SetVisualization()
{
    if (not vis.isNull())
    {
        VisToolTrueDarts *visual = qobject_cast<VisToolTrueDarts *>(vis);
        SCASSERT(visual != nullptr)

        visual->setObject1Id(baseLineP1Id);
        visual->setObject2Id(baseLineP2Id);
        visual->setD1PointId(dartP1Id);
        visual->setD2PointId(dartP2Id);
        visual->setD3PointId(dartP3Id);
        visual->RefreshGeometry();
    }
}
