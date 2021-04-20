#include "window.hpp"

#include <QAction>
#include <QMenuBar>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QFontDatabase>
#include <fmt/core.h>

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumWidth(400);
    setMinimumHeight(400);
    setWindowTitle(QStringLiteral("Database project"));


    QWidget *center = new QWidget(this);
    center->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    setCentralWidget(center);

    create_widgets(center);
    create_menu();
    create_statusbar();

}

void Window::create_widgets(QWidget *center)
{
    QHBoxLayout *hlt = new QHBoxLayout;
    QVBoxLayout *vlt = new QVBoxLayout(center);
    query_button = new QPushButton("Execute a query", center);
    java_label = new QLabel("java fa schifo!", center);
    result_tab = new QTableView(center);

    create_editor(center);
    hlt->addWidget(query_editor);
    hlt->addWidget(result_tab);
    vlt->addWidget(java_label);
    vlt->addLayout(hlt);
    vlt->addWidget(query_button);

    // button->resize(200, 50);
    // tabview->move(25, 200);
    result_tab->setModel(&result_model);
    // tabview->resize(400, 150);

    connect(query_button, &QPushButton::clicked, this, &Window::filltab);
}

void Window::create_editor(QWidget *parent)
{
    query_editor = new QTextEdit(parent);
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);
    query_editor->setFont(font);
    highlighter = new SQLHighlighter(query_editor->document());
}

void Window::create_menu()
{
    enum MenuIndex {
        MENU_FILE,
        MENU_COUNT,
    };
    QMenu *menus[MENU_COUNT];

    const auto add_item = [&, this](MenuIndex idx, const QString &text, bool enable, auto &&func)
    {
        QAction *act = new QAction(text, this);
        connect(act, &QAction::triggered, this, func);
        menus[idx]->addAction(act);
        act->setEnabled(enable);
    };

    menus[MENU_FILE] = menuBar()->addMenu("&File");
    add_item(MENU_FILE, "&Open", true, []() { fmt::print("dinosauri\n"); });
}

void Window::create_statusbar()
{
    QLabel *status_label = new QLabel(this);
    statusBar()->addPermanentWidget(status_label);
    statusBar()->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
}

void Window::filltab(bool b)
{
    result_model.setQuery(query_editor->toPlainText());
}

