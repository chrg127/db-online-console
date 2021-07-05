#ifndef QTHELPERS_HPP_INCLUDED
#define QTHELPERS_HPP_INCLUDED

#include <utility>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
//#include <QFont>
#include <QMessageBox>
#include <QTableView>
#include <QComboBox>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>

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

/*
inline QFont make_font(const QString &family, QFont::StyleHint hint)
{
    QFont font(family);
    font.setStyleHint(hint);
    return font;
}
*/

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
    return tab;
}

QComboBox *make_comboxbox(QWidget *parent, auto... items)
{
    auto *box = new QComboBox(parent);
    (box->addItem(std::get<0>(items), std::get<1>(items)), ...);
    return box;
}

QTabWidget *make_tabs(QWidget *parent, auto... items)
{
    auto *tabs = new QTabWidget(parent);
    (tabs->addTab(std::get<0>(items), std::get<1>(items)), ...);
    return tabs;
}

template <typename TLayout = QVBoxLayout>
void add_to_group(QGroupBox *box, auto... widgets)
{
    box->setLayout(make_layout<TLayout>(widgets...));
}

void set_enabled(bool v, auto... widgets)
{
    (widgets->setEnabled(v), ...);
}

inline QLabel *make_image_label(const QString &path, QWidget *parent = nullptr)
{
    QLabel *im = new QLabel(parent);
    im->setPixmap(QPixmap(path));
    return im;
}

#endif
