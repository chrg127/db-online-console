#ifndef QTHELPERS_HPP_INCLUDED
#define QTHELPERS_HPP_INCLUDED

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <type_traits>

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

inline QFont make_font(const QString &family, QFont::StyleHint hint)
{
    QFont font(family);
    font.setStyleHint(hint);
    return font;
}

#endif
