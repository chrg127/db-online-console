#ifndef QTHELPERS_HPP_INCLUDED
#define QTHELPERS_HPP_INCLUDED

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QFont>
#include <QMessageBox>
#include <QTableView>
#include <QComboBox>
#include <QTabWidget>

template <typename TLayout> void add(TLayout *lt, QWidget *w) { lt->addWidget(w); }
template <typename TLayout> void add(TLayout *lt, QLayout *l) { lt->addLayout(l); }

template <typename TLayout>
TLayout *make_layout(auto... widgets)
{
    auto *lt = new TLayout;
    (add(lt, widgets), ...);
    return lt;
}

inline QGridLayout *make_grid_layout(auto... widget_tuples)
{
    auto *lt = new QGridLayout;
    (lt->addWidget(std::get<0>(widget_tuples),
                   std::get<1>(widget_tuples),
                   std::get<2>(widget_tuples)), ...);
    return lt;
}

inline QFormLayout *make_form_layout(auto... tuples)
{
    auto *lt = new QFormLayout;
    (lt->addRow(std::get<0>(tuples), std::get<1>(tuples)), ...);
    return lt;
}

inline QFont make_font(const QString &family, QFont::StyleHint hint)
{
    QFont font(family);
    font.setStyleHint(hint);
    return font;
}

inline void msgbox(const QString &msg)
{
    QMessageBox box;
    box.setText(msg);
    box.exec();
}

inline QWidget *layout_widget(QLayout *lt)
{
    auto *w = new QWidget;
    w->setLayout(lt);
    return w;
}

template <typename T>
inline QTableView *make_table(T *model)
{
    auto *tab = new QTableView;
    tab->setModel(model);
    tab->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    return tab;
}

QComboBox *make_comboxbox(auto... items)
{
    auto *box = new QComboBox;
    (box->addItem(std::get<0>(items), std::get<1>(items)), ...);
    return box;
}

QTabWidget *make_tabs(auto... items)
{
    auto *tabs = new QTabWidget;
    (tabs->addTab(std::get<0>(items), std::get<1>(items)), ...);
    return tabs;
}

#endif
