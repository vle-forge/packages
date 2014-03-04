/*
 * @file vle/gvle/RuleDialog.hpp
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

#ifndef VLE_GVLE_MODELING_DECISION_RULEDIALOG_HPP
#define VLE_GVLE_MODELING_DECISION_RULEDIALOG_HPP

#include <boost/algorithm/string/trim.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/gvle/Message.hpp>
#include <gtkmm.h>

#include <vle/gvle/modeling/decision/Utils.hpp>

namespace vle {
namespace gvle {
namespace modeling {
namespace decision {

typedef std::vector < std::string > strings_t;
typedef std::map < std::string, strings_t > hierarchicalPred;

/**
 * @class RuleDialog
 * @brief This class is used to manage the rules.
 */

class RuleDialog
{
public:
    RuleDialog(Glib::RefPtr < Gtk::Builder >& xml,
        std::map < std::string, strings_t > rule,
        strings_t predicateName,
        hierarchicalPred pPred);

    ~RuleDialog();

    int run();

    std::map < std::string, strings_t > getRule();

protected:
    class ModelColumnsRule : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumnsRule()
        {
            add(m_col_name);
            add(m_col_type);
        }
        Gtk::TreeModelColumn < std::string > m_col_name;
        Gtk::TreeModelColumn < std::string > m_col_type;
    };

    class ModelColumnsPred : public Gtk::TreeModel::ColumnRecord
    {
    public:
        ModelColumnsPred()
        {
            add(m_col_name);
        }
        Gtk::TreeModelColumn < std::string > m_col_name;
    };

private:
    Glib::RefPtr < Gtk::Builder > mXml;
    Gtk::Dialog* mDialog;

    /**
     * @brief This function check if a rule name is valid
     * @param the name of the rule
     */
    bool checkName(std::string name)
    {
        if (!Utils::isValidName(name)) {
            return false;
        }
        std::map < std::string, strings_t > ::const_iterator it_rule;
        for (it_rule = mRule.begin(); it_rule != mRule.end(); ++it_rule) {
            if (name == it_rule->first) {
                return false;
            }
        }
        return true;
    }

    //Data
    // Store the Rules
    std::map < std::string, strings_t > mRule;
    // Store the Predicates
    strings_t mPredicateName;
    hierarchicalPred mPred;

    // TreeView
    //Rules
    Gtk::TreeView* mTreeViewRules;
    Glib::RefPtr < Gtk::TreeStore > mRefTreeRules;
    ModelColumnsRule mColumnsRules;
    Gtk::Menu *mMenuPopup;
    Glib::RefPtr < Gtk::TreeSelection > mSrcSelect;

    //Predicates
    Gtk::TreeView* mTreeViewPred;
    Glib::RefPtr < Gtk::TreeStore > mRefTreePred;
    ModelColumnsPred mColumnsPred;
    Glib::RefPtr < Gtk::TreeSelection > mDstSelect;

    //Buttons
    Gtk::Button* mButtonAdd;
    Gtk::Button* mButtonDelete;

    void on_button_press(GdkEventButton* event);

    std::list < sigc::connection > mList;

    void makeRules();
    void makePredicates();

    void on_add_rule();
    void on_del_rule();
    void on_rename_rule();

    void on_add_pred();
    void on_del_pred();
};

}
}
}
}

// namespace vle gvle modeling decision

#endif
