/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2017-2017 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <QColorDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include "svgconfig.h"
#include <QDebug>

svgConfig::svgConfig(QWidget *parent) : QWidget(parent)
{
    mPlan         = 0;
    mUiColorTable = 0;
    mViewWidget   = 0;

    mDefaultColor = "#000000";

    setupUi();

    connect(mUiColorTable, SIGNAL(itemSelectionChanged()),
            this, SLOT(colorSelectionChange()));
    connect(mUiColorTable, SIGNAL(cellDoubleClicked(int,int)),
            this, SLOT(colorSelectionEdit(int,int)));
}

void svgConfig::clear(void)
{
    mUiColorTable->clear();

    // Insert default headers
    QTableWidgetItem *h0 = new QTableWidgetItem();
    h0->setText("Name");
    mUiColorTable->setHorizontalHeaderItem(0, h0);

    QTableWidgetItem *h1 = new QTableWidgetItem();
    h1->setText("Color");
    mUiColorTable->setHorizontalHeaderItem(1, h1);

    QTableWidgetItem *h2 = new QTableWidgetItem();
    h2->setText("Preview");
    mUiColorTable->setHorizontalHeaderItem(2, h2);
}

void svgConfig::colorSelectionChange(void)
{
    // Defined for future use ...
    // ... see mUiColorTable setSelectionMode
}

void svgConfig::colorSelectionEdit(int row, int col)
{
    (void)col;

    QTableWidgetItem *nameItem    = mUiColorTable->item(row, 0);
    QTableWidgetItem *colorItem   = mUiColorTable->item(row, 1);
    QTableWidgetItem *previewItem = mUiColorTable->item(row, 2);

    // Convert color name to QColor
    QColor currentColor;
    currentColor.setNamedColor( colorItem->text() );

    // Open the color picker
    QString title;
    title += nameItem->text() + " " + tr("color");
    QColor color = QColorDialog::getColor(currentColor, this, title);
    if (color.isValid())
    {
        colorItem->setText( color.name() );

        // Update preview cell into table
        previewItem->setBackground(QBrush(color));

        // Update Plan viewer (if view available)
        if (mViewWidget)
        {
            mViewWidget->setConfig("color", nameItem->text(), colorItem->text());
            mViewWidget->reload();
        }
    }
}

void svgConfig::setDefaultColor(QString name)
{
    mDefaultColor = name;
}

void svgConfig::setPlan(vlePlan *plan)
{
    mPlan = plan;

    clear();

    QList<QString> knownClasses;

    // Search activities into all plan groups
    for (int i=0; i < mPlan->countGroups(); i++)
    {
        vlePlanGroup *planGroup = mPlan->getGroup(i);
        for (int j = 0; j < planGroup->count(); j++)
        {
            vlePlanActivity *planActivity = planGroup->getActivity(j);
            // Get the activity class
            QString className = planActivity->getClass();
            // If this class has already been seen, nothing to do
            if (knownClasses.contains(className))
                continue;
            knownClasses.append(className);

            qDebug() << "setPlan add class " << className;
            // Insert a new line into the color table
            int count = mUiColorTable->rowCount();
            mUiColorTable->insertRow(count);
            // Set name for this new class
            QTableWidgetItem *nameItem  = new QTableWidgetItem(className);
            nameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);
            mUiColorTable->setItem(count, 0, nameItem);
            // Set color name for this new class
            QTableWidgetItem *colorItem = new QTableWidgetItem(mDefaultColor);
            colorItem->setFlags(colorItem->flags() ^ Qt::ItemIsEditable);
            mUiColorTable->setItem(count, 1, colorItem);
            // Set color preview
            QColor previewColor;
            previewColor.setNamedColor( mDefaultColor );
            QTableWidgetItem *previewItem = new QTableWidgetItem("");
            previewItem->setFlags(previewItem->flags() ^ Qt::ItemIsEditable);
            previewItem->setBackground(QBrush(previewColor));
            mUiColorTable->setItem(count, 2, previewItem);

            // Update Plan viewer (if view available)
            if (mViewWidget)
                mViewWidget->setConfig("color", className, mDefaultColor);
        }
    }
}

void svgConfig::setView(SvgView *view)
{
    mViewWidget = view;
}

void svgConfig::setupUi(void)
{
    QVBoxLayout *vLayoutMain;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Create a Vertical layout for all controls
    vLayoutMain = new QVBoxLayout(this);
    vLayoutMain->setSpacing(6);
    vLayoutMain->setContentsMargins(11, 11, 11, 11);
    vLayoutMain->setObjectName(QStringLiteral("vLayout_main"));

    // Create a table for the color list
    mUiColorTable = new QTableWidget(this);
    mUiColorTable->setObjectName(QStringLiteral("tableWidget"));
    mUiColorTable->setColumnCount(3);
    mUiColorTable->setSelectionMode(QAbstractItemView::NoSelection);
    mUiColorTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mUiColorTable->horizontalHeader()->setStretchLastSection(true);
    mUiColorTable->verticalHeader()->setVisible(false);
    clear();
    vLayoutMain->addWidget(mUiColorTable);

#ifdef UI_EXTEND
    QHBoxLayout *hLayoutButtons;
    // Create an horizontal layout for additional controls
    hLayoutButtons = new QHBoxLayout();
    hLayoutButtons->setSpacing(6);
    hLayoutButtons->setObjectName(QStringLiteral("hLayoutExtend"));
#endif
}
