/***************************************************************************
 **  @file   vtoolcubicbezierpath.cpp
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
 **  @file   vtoolcubicbezierpath.cpp
 **  @author Roman Telezhynskyi <dismine(at)gmail.com>
 **  @date   18 3, 2016
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

#include "vtoolcubicbezierpath.h"

#include <QDomElement>
#include <QPen>
#include <QSharedPointer>
#include <QStaticStringData>
#include <QStringData>
#include <QStringDataPtr>
#include <Qt>
#include <new>

#include "../../../dialogs/tools/dialogtool.h"
#include "../../../dialogs/tools/dialogcubicbezierpath.h"
#include "../../../visualization/visualization.h"
#include "../../../visualization/path/vistoolcubicbezierpath.h"
#include "../ifc/exception/vexception.h"
#include "../ifc/xml/vdomdocument.h"
#include "../ifc/ifcdef.h"
#include "../vgeometry/vabstractcubicbezierpath.h"
#include "../vgeometry/vabstractcurve.h"
#include "../vgeometry/vcubicbezierpath.h"
#include "../vgeometry/vgobject.h"
#include "../vgeometry/vpointf.h"
#include "../vmisc/vabstractapplication.h"
#include "../vpatterndb/vcontainer.h"
#include "../vwidgets/vmaingraphicsscene.h"
#include "../../vabstracttool.h"
#include "../vdrawtool.h"
#include "vabstractspline.h"

const QString VToolCubicBezierPath::ToolType = QStringLiteral("cubicBezierPath");

//---------------------------------------------------------------------------------------------------------------------
VToolCubicBezierPath::VToolCubicBezierPath(VAbstractPattern *doc, VContainer *data, quint32 id,
                                           const Source &typeCreation, QGraphicsItem *parent)
    : VAbstractSpline(doc, data, id, parent)
{
    sceneType = SceneObject::SplinePath;

    this->setFlag(QGraphicsItem::ItemIsFocusable, true);// For keyboard input focus

    ToolCreation(typeCreation);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolCubicBezierPath::setDialog()
{
    SCASSERT(not m_dialog.isNull())
    auto dialogTool = qobject_cast<DialogCubicBezierPath*>(m_dialog);
    SCASSERT(dialogTool != nullptr)
    const QSharedPointer<VCubicBezierPath> splPath = VAbstractTool::data.GeometricObject<VCubicBezierPath>(m_id);
    dialogTool->SetPath(*splPath);
    dialogTool->setLineColor(splPath->getLineColor());
    dialogTool->setPenStyle(splPath->GetPenStyle());
    dialogTool->setLineWeight(splPath->getLineWeight());
}

//---------------------------------------------------------------------------------------------------------------------
VToolCubicBezierPath *VToolCubicBezierPath::Create(QSharedPointer<DialogTool> dialog, VMainGraphicsScene *scene,
                                                   VAbstractPattern *doc, VContainer *data)
{
    SCASSERT(not dialog.isNull())
    QSharedPointer<DialogCubicBezierPath> dialogTool = dialog.objectCast<DialogCubicBezierPath>();
    SCASSERT(not dialogTool.isNull())
    auto path = new VCubicBezierPath(dialogTool->GetPath());
    const QString color      = dialogTool->getLineColor();
    const QString penStyle   = dialogTool->getPenStyle();
    const QString lineWeight = dialogTool->getLineWeight();
    for (qint32 i = 0; i < path->CountPoints(); ++i)
    {
        doc->IncrementReferens((*path)[i].getIdTool());
    }
    path->setLineColor(color);
    path->SetPenStyle(penStyle);
    path->setLineWeight(lineWeight);

    VToolCubicBezierPath* spl = Create(0, path, scene, doc, data, Document::FullParse, Source::FromGui);
    if (spl != nullptr)
    {
        spl->m_dialog = dialogTool;
    }
    return spl;
}

//---------------------------------------------------------------------------------------------------------------------
VToolCubicBezierPath *VToolCubicBezierPath::Create(const quint32 _id, VCubicBezierPath *path, VMainGraphicsScene *scene,
                                                   VAbstractPattern *doc, VContainer *data, const Document &parse,
                                                   const Source &typeCreation)
{
    quint32 id = _id;
    if (typeCreation == Source::FromGui)
    {
        id = data->AddGObject(path);
        data->AddCurveWithSegments(data->GeometricObject<VAbstractCubicBezierPath>(id), id);
    }
    else
    {
        data->UpdateGObject(id, path);
        data->AddCurveWithSegments(data->GeometricObject<VAbstractCubicBezierPath>(id), id);
        if (parse != Document::FullParse)
        {
            doc->UpdateToolData(id, data);
        }
    }

    if (parse == Document::FullParse)
    {
        VDrawTool::AddRecord(id, Tool::CubicBezierPath, doc);
        VToolCubicBezierPath *spl = new VToolCubicBezierPath(doc, data, id, typeCreation);
        scene->addItem(spl);
        InitSplinePathToolConnections(scene, spl);
        VAbstractPattern::AddTool(id, spl);
        return spl;
    }
    return nullptr;
}

//---------------------------------------------------------------------------------------------------------------------
void VToolCubicBezierPath::UpdatePathPoints(VAbstractPattern *doc, QDomElement &element, const VCubicBezierPath &path)
{
    VDomDocument::RemoveAllChildren(element);
    for (qint32 i = 0; i < path.CountPoints(); ++i)
    {
        AddPathPoint(doc, element, path.at(i));
    }
}

//---------------------------------------------------------------------------------------------------------------------
VCubicBezierPath VToolCubicBezierPath::getSplinePath() const
{
    QSharedPointer<VCubicBezierPath> splPath = VAbstractTool::data.GeometricObject<VCubicBezierPath>(m_id);
    return *splPath.data();
}

//---------------------------------------------------------------------------------------------------------------------
void VToolCubicBezierPath::setSplinePath(const VCubicBezierPath &splPath)
{
    QSharedPointer<VGObject> obj = VAbstractTool::data.GetGObject(m_id);
    QSharedPointer<VCubicBezierPath> splinePath = qSharedPointerDynamicCast<VCubicBezierPath>(obj);
    *splinePath.data() = splPath;
    SaveOption(obj);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolCubicBezierPath::ShowVisualization(bool show)
{
    ShowToolVisualization<VisToolCubicBezierPath>(show);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolCubicBezierPath::showContextMenu(QGraphicsSceneContextMenuEvent *event, quint32 id)
{
    Q_UNUSED(id)

    try
    {
        ContextMenu<DialogCubicBezierPath>(event);
    }
    catch(const VExceptionToolWasDeleted &error)
    {
        Q_UNUSED(error)
        return;//Leave this method immediately!!!
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolCubicBezierPath::RemoveReferens()
{
    const QSharedPointer<VCubicBezierPath> splPath = VAbstractTool::data.GeometricObject<VCubicBezierPath>(m_id);
    for (qint32 i = 0; i < splPath->CountSubSpl(); ++i)
    {
        doc->DecrementReferens(splPath->at(i).getIdTool());
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolCubicBezierPath::SaveDialog(QDomElement &domElement)
{
    SCASSERT(not m_dialog.isNull())
    const auto dialogTool = qobject_cast<DialogCubicBezierPath*>(m_dialog);
    SCASSERT(dialogTool != nullptr)

    doc->SetAttribute(domElement, AttrColor,       dialogTool->getLineColor());
    doc->SetAttribute(domElement, AttrPenStyle,    dialogTool->getPenStyle());
    doc->SetAttribute(domElement, AttrLineWeight,  dialogTool->getLineWeight());
    SetSplinePathAttributes(domElement, dialogTool->GetPath());
}

//---------------------------------------------------------------------------------------------------------------------
void VToolCubicBezierPath::SaveOptions(QDomElement &tag, QSharedPointer<VGObject> &obj)
{
    VAbstractSpline::SaveOptions(tag, obj);

    QSharedPointer<VCubicBezierPath> splPath = qSharedPointerDynamicCast<VCubicBezierPath>(obj);
    SCASSERT(splPath.isNull() == false)

    SetSplinePathAttributes(tag, *splPath);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolCubicBezierPath::SetVisualization()
{
    if (not vis.isNull())
    {
        auto visual = qobject_cast<VisToolCubicBezierPath *>(vis);
        SCASSERT(visual != nullptr)

        QSharedPointer<VCubicBezierPath> splPath = VAbstractTool::data.GeometricObject<VCubicBezierPath>(m_id);
        visual->setPath(*splPath.data());
        visual->setLineStyle(lineTypeToPenStyle(splPath->GetPenStyle()));
        visual->SetMode(Mode::Show);
        visual->RefreshGeometry();
    }
}

//---------------------------------------------------------------------------------------------------------------------
void VToolCubicBezierPath::RefreshGeometry()
{
    QSharedPointer<VCubicBezierPath> splPath = VAbstractTool::data.GeometricObject<VCubicBezierPath>(m_id);
    setPath(splPath->GetPath());

    SetVisualization();
}

//---------------------------------------------------------------------------------------------------------------------
void VToolCubicBezierPath::AddPathPoint(VAbstractPattern *doc, QDomElement &domElement, const VPointF &splPoint)
{
    SCASSERT(doc != nullptr)
    QDomElement pathPoint = doc->createElement(AttrPathPoint);
    doc->SetAttribute(pathPoint, AttrPSpline, splPoint.id());
    domElement.appendChild(pathPoint);
}

//---------------------------------------------------------------------------------------------------------------------
void VToolCubicBezierPath::SetSplinePathAttributes(QDomElement &domElement, const VCubicBezierPath &path)
{
    doc->SetAttribute(domElement, AttrType, ToolType);

    if (path.GetDuplicate() > 0)
    {
        doc->SetAttribute(domElement, AttrDuplicate, path.GetDuplicate());
    }
    else
    {
        if (domElement.hasAttribute(AttrDuplicate))
        {
            domElement.removeAttribute(AttrDuplicate);
        }
    }

    UpdatePathPoints(doc, domElement, path);
}
