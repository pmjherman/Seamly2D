/***************************************************************************
 **  @file   vtoolarcwithlength.cpp
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
 **  @file   vtoolarcwithlength.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   9 6, 2015
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

#include "vtoolarcwithlength.h"

#include <QPen>
#include <QSharedPointer>
#include <QStaticStringData>
#include <QStringData>
#include <QStringDataPtr>
#include <Qt>
#include <new>

#include "../../../dialogs/tools/dialogtool.h"
#include "../../../dialogs/tools/dialogarcwithlength.h"
#include "../../../visualization/visualization.h"
#include "../../../visualization/path/vistoolarcwithlength.h"
#include "../ifc/exception/vexception.h"
#include "../ifc/xml/vdomdocument.h"
#include "../ifc/ifcdef.h"
#include "../vgeometry/varc.h"
#include "../vgeometry/vgobject.h"
#include "../vgeometry/vpointf.h"
#include "../vmisc/vabstractapplication.h"
#include "../vmisc/vcommonsettings.h"
#include "../vpatterndb/vcontainer.h"
#include "../vpatterndb/vformula.h"
#include "../vpatterndb/vtranslatevars.h"
#include "../vwidgets/vmaingraphicsscene.h"
#include "../../vabstracttool.h"
#include "../vdrawtool.h"
#include "vabstractspline.h"

const QString VToolArcWithLength::ToolType = QStringLiteral("arcWithLength");

//---------------------------------------------------------------------------------------------------------------------
VToolArcWithLength::VToolArcWithLength(VAbstractPattern *doc, VContainer *data, quint32 id, const Source &typeCreation,
                                       QGraphicsItem *parent)
    :VAbstractSpline(doc, data, id, parent)
{
    sceneType = SceneObject::Arc;

    this->setFlag(QGraphicsItem::ItemIsFocusable, true);// For keyboard input focus

    ToolCreation(typeCreation);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolArcWithLength::setDialog()
{
    SCASSERT(not m_dialog.isNull())
    QSharedPointer<DialogArcWithLength> dialogTool = m_dialog.objectCast<DialogArcWithLength>();
    SCASSERT(not dialogTool.isNull())
    const QSharedPointer<VArc> arc = VAbstractTool::data.GeometricObject<VArc>(m_id);
    dialogTool->setArc(*arc);
    dialogTool->SetCenter(arc->GetCenter().id());
    dialogTool->SetF1(arc->GetFormulaF1());
    dialogTool->SetLength(arc->GetFormulaLength());
    dialogTool->SetRadius(arc->GetFormulaRadius());
    dialogTool->setLineColor(arc->getLineColor());
    dialogTool->setLineWeight(arc->getLineWeight());
    dialogTool->setPenStyle(arc->GetPenStyle());
}

//---------------------------------------------------------------------------------------------------------------------
VToolArcWithLength *VToolArcWithLength::Create(QSharedPointer<DialogTool> dialog, VMainGraphicsScene *scene,
                                               VAbstractPattern *doc, VContainer *data)
{
    SCASSERT(not dialog.isNull())
    QSharedPointer<DialogArcWithLength> dialogTool = dialog.objectCast<DialogArcWithLength>();
    SCASSERT(not dialogTool.isNull())
    const quint32 center     = dialogTool->GetCenter();
    QString radius           = dialogTool->GetRadius();
    QString f1               = dialogTool->GetF1();
    QString length           = dialogTool->GetLength();
    const QString color      = dialogTool->getLineColor();
    const QString penStyle   = dialogTool->getPenStyle();
    const QString lineWeight = dialogTool->getLineWeight();

    VToolArcWithLength* point = Create(0, center, radius, f1, length, color, penStyle, lineWeight, scene, doc, data,
                                       Document::FullParse, Source::FromGui);
    if (point != nullptr)
    {
        point->m_dialog = dialogTool;
    }
    return point;
}

//---------------------------------------------------------------------------------------------------------------------
VToolArcWithLength *VToolArcWithLength::Create(const quint32 _id, const quint32 &center, QString &radius, QString &f1,
                                               QString &length, const QString &color, const QString &penStyle,
                                               const QString &lineWeight, VMainGraphicsScene *scene, VAbstractPattern *doc, VContainer *data,
                                               const Document &parse, const Source &typeCreation)
{
    qreal calcRadius = 0, calcF1 = 0, calcLength = 0;

    calcRadius = qApp->toPixel(CheckFormula(_id, radius, data));
    calcLength = qApp->toPixel(CheckFormula(_id, length, data));
    calcF1 = CheckFormula(_id, f1, data);

    const VPointF c = *data->GeometricObject<VPointF>(center);
    VArc *arc = new VArc(calcLength, length, c, calcRadius, radius, calcF1, f1);
    arc->setLineColor(color);
    arc->SetPenStyle(penStyle);
    arc->setLineWeight(lineWeight);
    quint32 id = _id;
    if (typeCreation == Source::FromGui)
    {
        id = data->AddGObject(arc);
        data->AddArc(data->GeometricObject<VArc>(id), id);
    }
    else
    {
        data->UpdateGObject(id, arc);
        data->AddArc(data->GeometricObject<VArc>(id), id);
        if (parse != Document::FullParse)
        {
            doc->UpdateToolData(id, data);
        }
    }

    if (parse == Document::FullParse)
    {
        VDrawTool::AddRecord(id, Tool::ArcWithLength, doc);
        VToolArcWithLength *toolArc = new VToolArcWithLength(doc, data, id, typeCreation);
        scene->addItem(toolArc);
        InitArcToolConnections(scene, toolArc);
        VAbstractPattern::AddTool(id, toolArc);
        doc->IncrementReferens(c.getIdTool());
        return toolArc;
    }
    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolArcWithLength::getTagName() const
{
    return VAbstractPattern::TagArc;
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolArcWithLength::CenterPointName() const
{
    return VAbstractTool::data.GetGObject(getCenter())->name();
}

//---------------------------------------------------------------------------------------------------------------------
quint32 VToolArcWithLength::getCenter() const
{
    QSharedPointer<VArc> arc = VAbstractTool::data.GeometricObject<VArc>(m_id);
    SCASSERT(arc.isNull() == false)

    return arc->GetCenter().id();
}

//---------------------------------------------------------------------------------------------------------------------
void VToolArcWithLength::setCenter(const quint32 &value)
{
    if (value != NULL_ID)
    {
        QSharedPointer<VGObject> obj = VAbstractTool::data.GetGObject(m_id);
        QSharedPointer<VArc> arc = qSharedPointerDynamicCast<VArc>(obj);

        QSharedPointer<VPointF> point = VAbstractTool::data.GeometricObject<VPointF>(value);
        arc->SetCenter(*point.data());
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
VFormula VToolArcWithLength::GetFormulaRadius() const
{
    QSharedPointer<VArc> arc = VAbstractTool::data.GeometricObject<VArc>(m_id);
    SCASSERT(arc.isNull() == false)

    VFormula radius(arc->GetFormulaRadius(), getData());
    radius.setCheckZero(true);
    radius.setToolId(m_id);
    radius.setPostfix(UnitsToStr(qApp->patternUnit()));
    return radius;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolArcWithLength::SetFormulaRadius(const VFormula &value)
{
    if (value.error() == false)
    {
        if (value.getDoubleValue() > 0)// Formula don't check this, but radius can't be 0 or negative
        {
            QSharedPointer<VGObject> obj = VAbstractTool::data.GetGObject(m_id);
            QSharedPointer<VArc> arc = qSharedPointerDynamicCast<VArc>(obj);
            arc->SetFormulaRadius(value.GetFormula(FormulaType::FromUser), value.getDoubleValue());
            SaveOption(obj);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
VFormula VToolArcWithLength::GetFormulaF1() const
{
    QSharedPointer<VArc> arc = VAbstractTool::data.GeometricObject<VArc>(m_id);
    SCASSERT(arc.isNull() == false)

    VFormula f1(arc->GetFormulaF1(), getData());
    f1.setCheckZero(false);
    f1.setToolId(m_id);
    f1.setPostfix(degreeSymbol);
    return f1;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolArcWithLength::SetFormulaF1(const VFormula &value)
{
    if (value.error() == false)
    {
        QSharedPointer<VGObject> obj = VAbstractTool::data.GetGObject(m_id);
        QSharedPointer<VArc> arc = qSharedPointerDynamicCast<VArc>(obj);

        if (not VFuzzyComparePossibleNulls(value.getDoubleValue(), arc->GetEndAngle()))// Angles can't be equal
        {
            arc->SetFormulaF1(value.GetFormula(FormulaType::FromUser), value.getDoubleValue());
            SaveOption(obj);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
VFormula VToolArcWithLength::GetFormulaLength() const
{
    QSharedPointer<VArc> arc = VAbstractTool::data.GeometricObject<VArc>(m_id);
    SCASSERT(arc.isNull() == false)

    VFormula radius(arc->GetFormulaLength(), getData());
    radius.setCheckZero(true);
    radius.setToolId(m_id);
    radius.setPostfix(UnitsToStr(qApp->patternUnit()));
    return radius;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolArcWithLength::SetFormulaLength(const VFormula &value)
{
    if (value.error() == false)
    {
        QSharedPointer<VGObject> obj = VAbstractTool::data.GetGObject(m_id);
        QSharedPointer<VArc> arc = qSharedPointerDynamicCast<VArc>(obj);
        arc->SetFormulaLength(value.GetFormula(FormulaType::FromUser), value.getDoubleValue());
        SaveOption(obj);
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolArcWithLength::ShowVisualization(bool show)
{
    ShowToolVisualization<VisToolArcWithLength>(show);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolArcWithLength::showContextMenu(QGraphicsSceneContextMenuEvent *event, quint32 id)
{
    Q_UNUSED(id)

    try
    {
        ContextMenu<DialogArcWithLength>(event);
    }
    catch(const VExceptionToolWasDeleted &error)
    {
        Q_UNUSED(error)
        return;//Leave this method immediately!!!
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolArcWithLength::RemoveReferens()
{
    const auto arc = VAbstractTool::data.GeometricObject<VArc>(m_id);
    doc->DecrementReferens(arc->GetCenter().getIdTool());
}

//---------------------------------------------------------------------------------------------------------------------
void VToolArcWithLength::SaveDialog(QDomElement &domElement)
{
    SCASSERT(not m_dialog.isNull())
    QSharedPointer<DialogArcWithLength> dialogTool = m_dialog.objectCast<DialogArcWithLength>();
    SCASSERT(not dialogTool.isNull())
    doc->SetAttribute(domElement, AttrCenter,     QString().setNum(dialogTool->GetCenter()));
    doc->SetAttribute(domElement, AttrRadius,     dialogTool->GetRadius());
    doc->SetAttribute(domElement, AttrAngle1,     dialogTool->GetF1());
    doc->SetAttribute(domElement, AttrLength,     dialogTool->GetLength());
    doc->SetAttribute(domElement, AttrColor,      dialogTool->getLineColor());
    doc->SetAttribute(domElement, AttrLineWeight, dialogTool->getLineWeight());
    doc->SetAttribute(domElement, AttrPenStyle,   dialogTool->getPenStyle());
}

//---------------------------------------------------------------------------------------------------------------------
void VToolArcWithLength::SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj)
{
    VAbstractSpline::SaveOptions(tag, obj);

    QSharedPointer<VArc> arc = qSharedPointerDynamicCast<VArc>(obj);
    SCASSERT(arc.isNull() == false)

    doc->SetAttribute(tag, AttrType, ToolType);
    doc->SetAttribute(tag, AttrCenter, arc->GetCenter().id());
    doc->SetAttribute(tag, AttrRadius, arc->GetFormulaRadius());
    doc->SetAttribute(tag, AttrAngle1, arc->GetFormulaF1());
    doc->SetAttribute(tag, AttrLength, arc->GetFormulaLength());
}

//---------------------------------------------------------------------------------------------------------------------
void VToolArcWithLength::SetVisualization()
{
    if (not vis.isNull())
    {
        const QSharedPointer<VArc> arc = VAbstractTool::data.GeometricObject<VArc>(m_id);
        VisToolArcWithLength *visual = qobject_cast<VisToolArcWithLength *>(vis);
        SCASSERT(visual != nullptr)

        const VTranslateVars *trVars = qApp->TrVars();
        visual->setObject1Id(arc->GetCenter().id());
        visual->setRadius(trVars->FormulaToUser(arc->GetFormulaRadius(), qApp->Settings()->GetOsSeparator()));
        visual->setF1(trVars->FormulaToUser(arc->GetFormulaF1(), qApp->Settings()->GetOsSeparator()));
        visual->setLength(trVars->FormulaToUser(arc->GetFormulaLength(), qApp->Settings()->GetOsSeparator()));
        visual->setLineStyle(lineTypeToPenStyle(arc->GetPenStyle()));
        visual->setLineWeight(arc->getLineWeight());
        visual->RefreshGeometry();
    }
}

//---------------------------------------------------------------------------------------------------------------------
QString VToolArcWithLength::makeToolTip() const
{
    const QSharedPointer<VArc> arc = VAbstractTool::data.GeometricObject<VArc>(m_id);

    const QString toolTip = QString("<table>"
                                    "<tr>"
                                        "<td align ='right'><b>%12: </b></td>" // Tool name
                                        "<td align ='left'>%13</td>"
                                    "</tr>"
                                    "<tr>"
                                        "<td align ='right'><b>%10: </b></td>" // Point Name
                                        "<td align ='left'>%11</td>"
                                    "</tr>"
                                    "<tr>"
                                        "<td align ='right'><b>%1: </b></td>" // Length
                                        "<td align ='left'>%2 %3</td>"
                                    "</tr>"
                                    "<tr>"
                                        "<td align ='right'><b>%4: </b></td>" // Radius
                                        "<td align ='left'>%5 %3</td>"
                                    "</tr>"
                                    "<tr>"
                                        "<td align ='right'><b>%6: </b></td>" // Start angle
                                        "<td align ='left'>%7</td>"
                                    "</tr>"
                                    "<tr>"
                                        "<td align ='right'><b>%8: </b></td>" // End angle
                                        "<td align ='left'>%9</td>"
                                    "</tr>"
                                    "</table>")
            .arg(tr("     Length"))
            .arg(qApp->fromPixel(arc->GetLength()))
            .arg(UnitsToStr(qApp->patternUnit(), true))
            .arg(tr("     Radius"))
            .arg(qApp->fromPixel(arc->GetRadius()))
            .arg(tr("Start angle"))
            .arg(arc->GetStartAngle())
            .arg(tr("  End angle"))
            .arg(arc->GetEndAngle())
            .arg(tr("      Name"))
            .arg(arc->name())
            .arg(tr("      Tool"))
            .arg(tr("Arc - Radius and Length"));
    return toolTip;
}
