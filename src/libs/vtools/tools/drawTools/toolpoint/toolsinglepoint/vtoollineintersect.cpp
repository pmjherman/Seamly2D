/***************************************************************************
 **  @file   vtoollineintersect.cpp
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
 **  @file   vtoollineintersect.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   November 15, 2013
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

#include "vtoollineintersect.h"

#include <QLine>
#include <QLineF>
#include <QPointF>
#include <QSharedPointer>
#include <QStaticStringData>
#include <QStringData>
#include <QStringDataPtr>
#include <new>

#include "../../../../dialogs/tools/dialoglineintersect.h"
#include "../../../../visualization/line/vistoollineintersect.h"
#include "../ifc/exception/vexception.h"
#include "../ifc/ifcdef.h"
#include "../vgeometry/vgobject.h"
#include "../vgeometry/vpointf.h"
#include "../vmisc/vabstractapplication.h"
#include "../vpatterndb/vcontainer.h"
#include "../vwidgets/vmaingraphicsscene.h"
#include "../../../../dialogs/tools/dialogtool.h"
#include "../../../../visualization/visualization.h"
#include "../../../vabstracttool.h"
#include "../../vdrawtool.h"
#include "vtoolsinglepoint.h"

template <class T> class QSharedPointer;

const QString VToolLineIntersect::ToolType = QStringLiteral("lineIntersect");

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief VToolLineIntersect constructor.
 * @param doc dom document container.
 * @param data container with variables.
 * @param id object id in container.
 * @param p1Line1 id first point first line.
 * @param p2Line1 id second point first line.
 * @param p1Line2 id first point second line.
 * @param p2Line2 id second point second line.
 * @param typeCreation way we create this tool.
 * @param parent parent object.
 */
VToolLineIntersect::VToolLineIntersect(VAbstractPattern *doc, VContainer *data, const quint32 &id,
                                       const quint32 &p1Line1, const quint32 &p2Line1, const quint32 &p1Line2,
                                       const quint32 &p2Line2, const Source &typeCreation,
                                       QGraphicsItem *parent)
    :VToolSinglePoint(doc, data, id, QColor(qApp->Settings()->getPointNameColor()), parent)
    , p1Line1(p1Line1)
    , p2Line1(p2Line1)
    , p1Line2(p1Line2)
    , p2Line2(p2Line2)
{
    ToolCreation(typeCreation);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief setDialog set dialog when user want change tool option.
 */
void VToolLineIntersect::setDialog()
{
    SCASSERT(not m_dialog.isNull())
    QSharedPointer<DialogLineIntersect> dialogTool = m_dialog.objectCast<DialogLineIntersect>();
    SCASSERT(not dialogTool.isNull())
    const QSharedPointer<VPointF> p = VAbstractTool::data.GeometricObject<VPointF>(m_id);
    dialogTool->SetP1Line1(p1Line1);
    dialogTool->SetP2Line1(p2Line1);
    dialogTool->SetP1Line2(p1Line2);
    dialogTool->SetP2Line2(p2Line2);
    dialogTool->SetPointName(p->name());
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Create help create tool from GUI.
 * @param dialog dialog.
 * @param scene pointer to scene.
 * @param doc dom document container.
 * @param data container with variables.
 * @return the created tool
 */
VToolLineIntersect* VToolLineIntersect::Create(QSharedPointer<DialogTool> dialog, VMainGraphicsScene *scene,
                                               VAbstractPattern *doc, VContainer *data)
{
    SCASSERT(not dialog.isNull())
    QSharedPointer<DialogLineIntersect> dialogTool = dialog.objectCast<DialogLineIntersect>();
    SCASSERT(not dialogTool.isNull())
    const quint32 p1Line1Id = dialogTool->GetP1Line1();
    const quint32 p2Line1Id = dialogTool->GetP2Line1();
    const quint32 p1Line2Id = dialogTool->GetP1Line2();
    const quint32 p2Line2Id = dialogTool->GetP2Line2();
    const QString pointName = dialogTool->getPointName();
    VToolLineIntersect* point = Create(0, p1Line1Id, p2Line1Id, p1Line2Id, p2Line2Id, pointName, 5, 10, true, scene, doc,
                                       data, Document::FullParse, Source::FromGui);
    if (point != nullptr)
    {
        point->m_dialog = dialogTool;
    }
    return point;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Create help create tool.
 * @param _id tool id, 0 if tool doesn't exist yet.
 * @param p1Line1Id id first point first line.
 * @param p2Line1Id id second point first line.
 * @param p1Line2Id id first point second line.
 * @param p2Line2Id id second point second line.
 * @param pointName point name.
 * @param mx label bias x axis.
 * @param my label bias y axis.
 * @param scene pointer to scene.
 * @param doc dom document container.
 * @param data container with variables.
 * @param parse parser file mode.
 * @param typeCreation way we create this tool.
 * @return the created tool
 */
VToolLineIntersect* VToolLineIntersect::Create(const quint32 _id, const quint32 &p1Line1Id, const quint32 &p2Line1Id,
                                               const quint32 &p1Line2Id, const quint32 &p2Line2Id,
                                               const QString &pointName, qreal mx, qreal my, bool showPointName,
                                               VMainGraphicsScene *scene, VAbstractPattern *doc, VContainer *data,
                                               const Document &parse, const Source &typeCreation)
{
    const QSharedPointer<VPointF> p1Line1 = data->GeometricObject<VPointF>(p1Line1Id);
    const QSharedPointer<VPointF> p2Line1 = data->GeometricObject<VPointF>(p2Line1Id);
    const QSharedPointer<VPointF> p1Line2 = data->GeometricObject<VPointF>(p1Line2Id);
    const QSharedPointer<VPointF> p2Line2 = data->GeometricObject<VPointF>(p2Line2Id);

    QLineF line1(static_cast<QPointF>(*p1Line1), static_cast<QPointF>(*p2Line1));
    QLineF line2(static_cast<QPointF>(*p1Line2), static_cast<QPointF>(*p2Line2));
    QPointF fPoint;
    QLineF::IntersectType intersect = line1.intersects(line2, &fPoint);
    if (intersect == QLineF::UnboundedIntersection || intersect == QLineF::BoundedIntersection)
    {
        quint32 id = _id;

        VPointF *p = new VPointF(fPoint, pointName, mx, my);
        p->setShowPointName(showPointName);

        if (typeCreation == Source::FromGui)
        {
            id = data->AddGObject(p);
            data->AddLine(p1Line1Id, id);
            data->AddLine(id, p2Line1Id);
            data->AddLine(p1Line2Id, id);
            data->AddLine(id, p2Line2Id);
        }
        else
        {
            data->UpdateGObject(id, p);
            data->AddLine(p1Line1Id, id);
            data->AddLine(id, p2Line1Id);
            data->AddLine(p1Line2Id, id);
            data->AddLine(id, p2Line2Id);
            if (parse != Document::FullParse)
            {
                doc->UpdateToolData(id, data);
            }
        }

        if (parse == Document::FullParse)
        {
            VDrawTool::AddRecord(id, Tool::LineIntersect, doc);
            VToolLineIntersect *point = new VToolLineIntersect(doc, data, id, p1Line1Id, p2Line1Id, p1Line2Id,
                                                               p2Line2Id, typeCreation);
            scene->addItem(point);
            InitToolConnections(scene, point);
            VAbstractPattern::AddTool(id, point);
            doc->IncrementReferens(p1Line1->getIdTool());
            doc->IncrementReferens(p2Line1->getIdTool());
            doc->IncrementReferens(p1Line2->getIdTool());
            doc->IncrementReferens(p2Line2->getIdTool());
            return point;
        }
    }
    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolLineIntersect::Line1P1Name() const
{
    return VAbstractTool::data.GetGObject(p1Line1)->name();
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolLineIntersect::Line1P2Name() const
{
    return VAbstractTool::data.GetGObject(p2Line1)->name();
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolLineIntersect::Line2P1Name() const
{
    return VAbstractTool::data.GetGObject(p1Line2)->name();
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolLineIntersect::Line2P2Name() const
{
    return VAbstractTool::data.GetGObject(p2Line2)->name();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief contextMenuEvent handle context menu events.
 * @param event context menu event.
 */
void VToolLineIntersect::showContextMenu(QGraphicsSceneContextMenuEvent *event, quint32 id)
{
    try
    {
        ContextMenu<DialogLineIntersect>(event, id);
    }
    catch(const VExceptionToolWasDeleted &error)
    {
        Q_UNUSED(error)
        return;//Leave this method immediately!!!
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief RemoveReferens decrement value of reference.
 */
void VToolLineIntersect::RemoveReferens()
{
    const auto p1L1 = VAbstractTool::data.GetGObject(p1Line1);
    const auto p2L1 = VAbstractTool::data.GetGObject(p2Line1);
    const auto p1L2 = VAbstractTool::data.GetGObject(p1Line2);
    const auto p2L2 = VAbstractTool::data.GetGObject(p2Line2);

    doc->DecrementReferens(p1L1->getIdTool());
    doc->DecrementReferens(p2L1->getIdTool());
    doc->DecrementReferens(p1L2->getIdTool());
    doc->DecrementReferens(p2L2->getIdTool());
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief SaveDialog save options into file after change in dialog.
 */
void VToolLineIntersect::SaveDialog(QDomElement &domElement)
{
    SCASSERT(not m_dialog.isNull())
    QSharedPointer<DialogLineIntersect> dialogTool = m_dialog.objectCast<DialogLineIntersect>();
    SCASSERT(not dialogTool.isNull())
    doc->SetAttribute(domElement, AttrName, dialogTool->getPointName());
    doc->SetAttribute(domElement, AttrP1Line1, QString().setNum(dialogTool->GetP1Line1()));
    doc->SetAttribute(domElement, AttrP2Line1, QString().setNum(dialogTool->GetP2Line1()));
    doc->SetAttribute(domElement, AttrP1Line2, QString().setNum(dialogTool->GetP1Line2()));
    doc->SetAttribute(domElement, AttrP2Line2, QString().setNum(dialogTool->GetP2Line2()));
}

//---------------------------------------------------------------------------------------------------------------------
void VToolLineIntersect::SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj)
{
    VToolSinglePoint::SaveOptions(tag, obj);

    doc->SetAttribute(tag, AttrType, ToolType);
    doc->SetAttribute(tag, AttrP1Line1, p1Line1);
    doc->SetAttribute(tag, AttrP2Line1, p2Line1);
    doc->SetAttribute(tag, AttrP1Line2, p1Line2);
    doc->SetAttribute(tag, AttrP2Line2, p2Line2);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolLineIntersect::ReadToolAttributes(const QDomElement &domElement)
{
    p1Line1 = doc->GetParametrUInt(domElement, AttrP1Line1, NULL_ID_STR);
    p2Line1 = doc->GetParametrUInt(domElement, AttrP2Line1, NULL_ID_STR);
    p1Line2 = doc->GetParametrUInt(domElement, AttrP1Line2, NULL_ID_STR);
    p2Line2 = doc->GetParametrUInt(domElement, AttrP2Line2, NULL_ID_STR);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolLineIntersect::SetVisualization()
{
    if (not vis.isNull())
    {
        VisToolLineIntersect *visual = qobject_cast<VisToolLineIntersect *>(vis);
        SCASSERT(visual != nullptr)

        visual->setObject1Id(p1Line1);
        visual->setLine1P2Id(p2Line1);
        visual->setLine2P1Id(p1Line2);
        visual->setLine2P2Id(p2Line2);
        visual->RefreshGeometry();
    }
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolLineIntersect::makeToolTip() const
{
    const QSharedPointer<VPointF> p1L1 = VAbstractTool::data.GeometricObject<VPointF>(p1Line1);
    const QSharedPointer<VPointF> p2L1 = VAbstractTool::data.GeometricObject<VPointF>(p2Line1);
    const QSharedPointer<VPointF> p1L2 = VAbstractTool::data.GeometricObject<VPointF>(p1Line2);
    const QSharedPointer<VPointF> p2L2 = VAbstractTool::data.GeometricObject<VPointF>(p2Line2);
    const QSharedPointer<VPointF> current = VAbstractTool::data.GeometricObject<VPointF>(m_id);

    const QLineF p1L1ToCur(static_cast<QPointF>(*p1L1), static_cast<QPointF>(*current));
    const QLineF curToP2L1(static_cast<QPointF>(*current), static_cast<QPointF>(*p2L1));
    const QLineF p1L2ToCur(static_cast<QPointF>(*p1L2), static_cast<QPointF>(*current));
    const QLineF curToP2L2(static_cast<QPointF>(*current), static_cast<QPointF>(*p2L2));

    const QString toolTip = QString("<table>"
                                    "<tr> <td><b>%10:</b> %11</td> </tr>"
                                    "<tr> <td><b>%1:</b> %2 %3</td> </tr>"
                                    "<tr> <td><b>%4:</b> %5 %3</td> </tr>"
                                    "<tr> <td><b>%6:</b> %7 %3</td> </tr>"
                                    "<tr> <td><b>%8:</b> %9 %3</td> </tr>"
                                    "</table>")
            .arg(QString("%1->%2").arg(p1L1->name(), current->name()))
            .arg(qApp->fromPixel(p1L1ToCur.length()))
            .arg(UnitsToStr(qApp->patternUnit(), true))
            .arg(QString("%1->%2").arg(current->name(), p2L1->name()))
            .arg(qApp->fromPixel(curToP2L1.length()))
            .arg(QString("%1->%2").arg(p1L2->name(), current->name()))
            .arg(qApp->fromPixel(p1L2ToCur.length()))
            .arg(QString("%1->%2").arg(current->name(), p2L2->name()))
            .arg(qApp->fromPixel(curToP2L2.length()))
            .arg(tr("Name"))
            .arg(current->name());

    return toolTip;
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolLineIntersect::GetP2Line2() const
{
    return p2Line2;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolLineIntersect::SetP2Line2(const quint32 &value)
{
    if (value != NULL_ID)
    {
        p2Line2 = value;

        QSharedPointer<VGObject> obj = VAbstractTool::data.GetGObject(m_id);
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolLineIntersect::ShowVisualization(bool show)
{
    ShowToolVisualization<VisToolLineIntersect>(show);
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolLineIntersect::GetP1Line2() const
{
    return p1Line2;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolLineIntersect::SetP1Line2(const quint32 &value)
{
    if (value != NULL_ID)
    {
        p1Line2 = value;

        QSharedPointer<VGObject> obj = VAbstractTool::data.GetGObject(m_id);
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolLineIntersect::GetP2Line1() const
{
    return p2Line1;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolLineIntersect::SetP2Line1(const quint32 &value)
{
    if (value != NULL_ID)
    {
        p2Line1 = value;

        QSharedPointer<VGObject> obj = VAbstractTool::data.GetGObject(m_id);
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolLineIntersect::GetP1Line1() const
{
    return p1Line1;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolLineIntersect::SetP1Line1(const quint32 &value)
{
    if (value != NULL_ID)
    {
        p1Line1 = value;

        QSharedPointer<VGObject> obj = VAbstractTool::data.GetGObject(m_id);
        SaveOption(obj);
    }
}
