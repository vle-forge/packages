/*
 * @file vle/gvle/modeling/decision/PrecedenceConstraintDialog.hpp
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


#ifndef VLE_GVLE_MODELING_DECISION_PRECE_CONS_DIALOG_HPP
#define VLE_GVLE_MODELING_DECISION_PRECE_CONS_DIALOG_HPP

#include <vle/gvle/modeling/decision/Decision.hpp>
#include <vle/gvle/CalendarBox.hpp>
#include <vle/utils/DateTime.hpp>
#include <vle/utils/Tools.hpp>
#include <boost/algorithm/string.hpp>
#include <gtkmm.h>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

typedef std::vector < std::string > strings_t;

/**
 * @class PrecedenceConstraintDialog
 * @brief This class is used to manage the precedence constraints.
 */

class PrecedenceConstraintDialog
{
public:
/**
 * @brief Constructor of the PrecedenceConstraintDialog
 * @param reference to the xml builder
 * @param reference to the selected ActivityModel
 * @param reference to a rule vector
 */
    PrecedenceConstraintDialog(const Glib::RefPtr < Gtk::Builder >& xml,
            PrecedenceConstraintModel precedenceConstraint);

/**
 * @brief Destructor of the precedence constraint dialog
 */
    ~PrecedenceConstraintDialog()
    {
        mRefTreePred->clear();
        mComboPCType->unset_model();
    }

/**
 * @brief Launch the window
 */
    int run();

/**
 * @brief Get the precedence constraint stored in the window
 * @return the precedence constraint stored in the window
 */
    PrecedenceConstraintModel getPrecedenceConstraint() {
        return mPrecedenceConstraint;
    }

/**
 * @brief Get the min time unit in the window
 * @return the min time in the window
 */
    std::string getActTlMin()
    {
        return mActTlMin->get_text();
    }

/**
 * @brief Get the max time unit in the window
 * @return the max time in the window
 */
    std::string getActTlMax()
    {
        return mActTlMax->get_text();
    }

/**
 * @brief Get the type of the constraint
 * @return the precedence constraint type
 */
    std::string getCType()
    {
        Gtk::TreeModel::iterator iter = mComboPCType->get_active();
        if(iter) {
            Gtk::TreeModel::Row row = *iter;
            if(row) {
                std::string name = row[mColumnsPredType.m_col_name];
                return name;
            }
        }
        return "FS";
    }

    class ModelColumnsPredType : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumnsPredType()
        {
            add(m_col_name);
            add(m_col_id);
        }
        Gtk::TreeModelColumn < std::string > m_col_name;
        Gtk::TreeModelColumn<int> m_col_id;
    };

/**
 * @brief This function is called when the selected item in the ComboBox box is modified.
 * It updates the informations displayed in the labels.
 */
    void on_combo_changed()
    {
        Gtk::TreeModel::iterator iter = mComboPCType->get_active();
        if(iter) {
            Gtk::TreeModel::Row row = *iter;
            if(row) {
                std::string constraintType = row[mColumnsPredType.m_col_name];
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
            }
        }
    }

private:
    Glib::RefPtr < Gtk::Builder > mXml;
    PrecedenceConstraintModel mPrecedenceConstraint;

    Gtk::Dialog* mDialog;
    Gtk::Label* mPreConsLabel0;
    Gtk::Label* mPreConsLabel1;
    Gtk::Label* mPreConsLabel2;
    Gtk::Label* mPreConsLabel3;
    Gtk::Label* mPreConsLabel4;
    Gtk::Entry* mActTlMin;
    Gtk::Entry* mActTlMax;
    Gtk::ComboBox* mComboPCType;

// Precedence Constraint ComboBox
    Gtk::TreeView* mTreeViewPred;
    Glib::RefPtr < Gtk::ListStore > mRefTreePred;
    ModelColumnsPredType mColumnsPredType;
};

}
}
}
}    // namespace vle gvle modeling decision

#endif
