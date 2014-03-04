/*
 * @file vle/gvle/modeling/difference-equation/NameValue.cpp
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


#include <vle/gvle/modeling/difference-equation/NameValue.hpp>
#include <vle/gvle/Message.hpp>
#include <vle/gvle/SimpleTypeBox.hpp>
#include <vle/value/Double.hpp>
#include <vle/utils/Tools.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

namespace vu = vle::utils;

namespace vle { namespace gvle { namespace modeling { namespace de {

NameValue::InitTreeView::InitTreeView(
    BaseObjectType* cobject,
    const Glib::RefPtr < Gtk::Builder >&):
    Gtk::TreeView(cobject)
{
    m_refTreeInit = Gtk::ListStore::create(m_columnsInit);
    set_model(m_refTreeInit);
    append_column(_("Value"), m_columnsInit.m_col_value);

    mPopupActionGroup = Gtk::ActionGroup::create("InitTreeView");
    mPopupActionGroup->add(Gtk::Action::create("ITV_ContextMenu", _("Context Menu")));

    mPopupActionGroup->add(Gtk::Action::create("ITV_ContextAdd", _("_Add")),
        sigc::mem_fun(*this, &NameValue::InitTreeView::onAdd));

    mPopupActionGroup->add(Gtk::Action::create("ITV_ContextRemove", _("_Remove")),
        sigc::mem_fun(*this, &NameValue::InitTreeView::onRemove));

    mPopupActionGroup->add(Gtk::Action::create("ITV_ContextUp", _("_Up")),
        sigc::mem_fun(*this, &NameValue::InitTreeView::onUp));

    mPopupActionGroup->add(Gtk::Action::create("ITV_ContextDown", _("_Down")),
        sigc::mem_fun(*this, &NameValue::InitTreeView::onDown));

    mPopupUIManager = Gtk::UIManager::create();
    mPopupUIManager->insert_action_group(mPopupActionGroup);

    Glib::ustring ui_info =
        "<ui>"
        "  <popup name='ITView_Popup'>"
        "      <menuitem action='ITV_ContextAdd'/>"
        "      <menuitem action='ITV_ContextRemove'/>"
        "      <menuitem action='ITV_ContextUp'/>"
        "      <menuitem action='ITV_ContextDown'/>"
        "  </popup>"
        "</ui>";

    try {
        mPopupUIManager->add_ui_from_string(ui_info);
        mMenuPopup = (Gtk::Menu *) (
            mPopupUIManager->get_widget("/ITView_Popup"));
    } catch(const Glib::Error& ex) {
        std::cerr << "building menus failed: ITView_Popup " <<  ex.what();
    }

    if (!mMenuPopup)
        std::cerr << "menu not found : ITView_Popup\n";

}

NameValue::InitTreeView::~InitTreeView()
{ }

void NameValue::InitTreeView::init(const vpz::Condition& condition)
{
    value::VectorValue vector = condition.firstValue("init").toSet().value();
    value::VectorValue::iterator iter = vector.begin();
    while (iter != vector.end()) {
	value::Value* val = *iter;
	if (val->isDouble()) {
	    Gtk::TreeModel::Row row = *(m_refTreeInit->append());
	    row[m_columnsInit.m_col_value] =
                vu::toScientificString(val->toDouble().value(),
                                       true);
	}
	++iter;
    }
}

bool NameValue::InitTreeView::on_button_press_event(GdkEventButton* event)
{
    bool return_value = Gtk::TreeView::on_button_press_event(event);
    if ( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) ) {
        mMenuPopup->popup(event->button, event->time);
    }

    return return_value;
}

void NameValue::InitTreeView::onAdd()
{
    SimpleTypeBox box(_("Value of the init"), "");
    try {
	double value = vu::convert < double >(box.run(),true);

	if (box.valid()) {
	    Gtk::TreeModel::Row row = *(m_refTreeInit->append());
	    row[m_columnsInit.m_col_value] =
                vu::toScientificString(value, true);
	}
    } catch(const boost::bad_lexical_cast& e ) {}
}

void NameValue::InitTreeView::onRemove()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();

	if (iter) {
            m_refTreeInit->erase(iter);
	}
    }
}

void NameValue::InitTreeView::onUp()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Glib::RefPtr<Gtk::TreeModel> refModel = get_model();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();
	Gtk::TreeModel::iterator iterBefore = iter;
	if (iter and iter != refModel->children().begin()) {
	    --iterBefore;
	    m_refTreeInit->iter_swap(iter, iterBefore);
	}
    }
}

void NameValue::InitTreeView::onDown()
{
    Glib::RefPtr<Gtk::TreeView::Selection> refSelection = get_selection();
    Glib::RefPtr<Gtk::TreeModel> refModel = get_model();

    if (refSelection) {
	Gtk::TreeModel::iterator iter = refSelection->get_selected();
	Gtk::TreeModel::iterator iterBefore = iter;
	if (iter and (bool)(++iterBefore)) {
	    m_refTreeInit->iter_swap(iter, iterBefore);
	}
    }
}

/*  - - - - - - - - - - - - - --ooOoo-- - - - - - - - - - - -  */

void NameValue::assign(vpz::Condition& condition)
{
    if (not m_entryName->get_text().empty())
	condition.addValueToPort("name",
				 value::String::create(
				     m_entryName->get_text()));
    try {
	if (not m_entryValue->get_text().empty()) {
            double value = vu::convert < double >( m_entryValue->get_text(),
                                                   true);

	    condition.addValueToPort("value",
				     value::Double::create(value));
        }
    } catch(const boost::bad_lexical_cast& e ) {}

    createSetInit(condition);
}

Gtk::Widget& NameValue::build(Glib::RefPtr < Gtk::Builder >& ref)
{
    ref->get_widget("NameValueHBox", m_hbox);
    ref->get_widget("NameEntry", m_entryName);
    ref->get_widget("ValueEntry", m_entryValue);
    ref->get_widget_derived("InitTreeView", m_initTreeView);
    return *m_hbox;
}

void NameValue::createSetInit(vpz::Condition& condition)
{
    value::Set* set = 0;
    Glib::RefPtr<Gtk::TreeModel> refModel = m_initTreeView->get_model();
    Gtk::TreeRow row;

    if (refModel->children().size() != 0) {
	set = new value::Set();
	Gtk::TreeModel::iterator iter = refModel->children().begin();
	double initValue;

	while (iter != refModel->children().end()) {
	    row = *iter;
	    initValue = vu::convert < double >(
                row.get_value(
                    m_initTreeView->getColumns().m_col_value)
                , true);
	    set->addDouble(initValue);
	    ++iter;
	}
    }
    if (set != 0) {
	condition.addValueToPort("init",set);
    }
}

void NameValue::deletePorts(vpz::Condition& condition)
{
    std::list < std::string> list;

    condition.portnames(list);
    if (std::find(list.begin(), list.end(), "name") != list.end()) {
	condition.del("name");
    }
    if (std::find(list.begin(), list.end(), "value") != list.end()) {
	condition.del("value");
    }
    if (std::find(list.begin(), list.end(), "init") != list.end()) {
	condition.del("init");
    }
}

void NameValue::fillFields(const vpz::Condition& condition)
{
    if (validPort(condition, "name")) {
	m_entryName->set_text(condition.firstValue("name").
			      toString().value());
    }
    if (validPort(condition, "value")) {
	m_entryValue->set_text(vu::toScientificString(
                                   condition.firstValue("value").
                                   toDouble().value(), true));
    }
    if (validPort(condition, "init")) {
	m_initTreeView->init(condition);
    }
}

bool NameValue::validPort(const vpz::Condition& condition,
                              const std::string& name)
{
    std::list < std::string> list;

    condition.portnames(list);
    if (std::find(list.begin(), list.end(), name) != list.end()) {
	if (name == "name") {
	    if (condition.getSetValues(name).size() != 0
		and condition.firstValue(name).isString())
		return true;
	}
	if (name == "value") {
	    if (condition.getSetValues(name).size() != 0
		and condition.firstValue(name).isDouble())
		return true;
	}
	if (name == "init") {
	    if (condition.getSetValues(name).size() != 0
		and condition.firstValue(name).isSet())
		return true;
	}
    }
    return false;
}

}}}} // namespace vle gvle modeling de
