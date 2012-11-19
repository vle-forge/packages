/*
 * @file vle/gvle/modeling/decision/PrecedenceConstraintDialog.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <vle/gvle/modeling/decision/PrecedenceConstraintDialog.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

PrecedenceConstraintDialog::PrecedenceConstraintDialog(
        const Glib::RefPtr < Gtk::Builder >& xml,
        PrecedenceConstraintModel precedenceConstraint) :
    mXml(xml)
{
    mPrecedenceConstraint = precedenceConstraint;

    xml->get_widget("PrecedenceConstraintDialog", mDialog);
    mDialog->set_title(mPrecedenceConstraint.source() + " to " +
            mPrecedenceConstraint.destination());
    xml->get_widget("ActTlMin", mActTlMin);
    xml->get_widget("ActTlMax", mActTlMax);
    xml->get_widget("PreConsCombobox", mComboPCType);
    xml->get_widget("PreConsLabel0", mPreConsLabel0);
    xml->get_widget("PreConsLabel1", mPreConsLabel1);
    xml->get_widget("PreConsLabel2", mPreConsLabel2);
    xml->get_widget("PreConsLabel3", mPreConsLabel3);
    xml->get_widget("PreConsLabel4", mPreConsLabel4);

// Fill the ComboBox with the informations (FS, SS, FF)
    xml->get_widget("TreeViewPredicates", mTreeViewPred);
    mRefTreePred = Gtk::ListStore::create(mColumnsPredType);
    mRefTreePred->clear();
    mComboPCType->unset_model();
    mComboPCType->clear();
    mComboPCType->set_model(mRefTreePred);

    Gtk::TreeModel::Row row1 = *(mRefTreePred->append());
    row1[mColumnsPredType.m_col_id] = 1;
    row1[mColumnsPredType.m_col_name] = "FS";
    Gtk::TreeModel::Row row2 = *(mRefTreePred->append());
    row2[mColumnsPredType.m_col_id] = 2;
    row2[mColumnsPredType.m_col_name] = "SS";
    Gtk::TreeModel::Row row3 = *(mRefTreePred->append());
    row3[mColumnsPredType.m_col_id] = 3;
    row3[mColumnsPredType.m_col_name] = "FF";

    mComboPCType->pack_start(mColumnsPredType.m_col_id);
    mComboPCType->pack_start(mColumnsPredType.m_col_name);

// Select the good constraint type
    std::string constraintType = mPrecedenceConstraint.cType();
    if (constraintType == "FS") {
        mComboPCType->set_active(0);
        mPreConsLabel0->set_text("FS precedence constraint");
        mPreConsLabel1->set_text(mPrecedenceConstraint.destination()
                + " start between ");
        mPreConsLabel2->set_text(" and ");
        mPreConsLabel3->set_text("");
        mPreConsLabel4->set_text(" after " + mPrecedenceConstraint.source() +
                " end");
    }
    else if (constraintType == "SS") {
        mComboPCType->set_active(1);
        mPreConsLabel0->set_text("SS precedence constraint");
        mPreConsLabel1->set_text(mPrecedenceConstraint.destination()
                + " start between ");
        mPreConsLabel2->set_text(" and ");
        mPreConsLabel3->set_text("");
        mPreConsLabel4->set_text(" after " + mPrecedenceConstraint.source() +
                " start");
    }
    else if (constraintType == "FF") {
        mComboPCType->set_active(2);
        mPreConsLabel0->set_text("FF precedence constraint");
        mPreConsLabel1->set_text(mPrecedenceConstraint.destination()
                + " end between ");
        mPreConsLabel2->set_text(" and ");
        mPreConsLabel3->set_text("");
        mPreConsLabel4->set_text(" after " + mPrecedenceConstraint.source() +
                " end");
    }

    mComboPCType->signal_changed().connect(sigc::mem_fun(*this,
            &PrecedenceConstraintDialog::on_combo_changed));

    mActTlMin->set_text(mPrecedenceConstraint.actTlMin());
    mActTlMax->set_text(mPrecedenceConstraint.actTlMax());
}

int PrecedenceConstraintDialog::run()
{
    int response = mDialog->run();
    mDialog->hide();
    std::string tmp = mActTlMin->get_text();
    mPrecedenceConstraint.actTlMin(tmp);
    tmp = mActTlMax->get_text();
    mPrecedenceConstraint.actTlMax(tmp);
    mPrecedenceConstraint.cType(getCType());
    return response;
}

}
}
}
}    // namespace vle gvle modeling decision
