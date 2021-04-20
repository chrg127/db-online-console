#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QSqlQueryModel>
#include "sqlhighlighter.hpp"

class Window : public QMainWindow {
    Q_OBJECT

private:
    QLabel *java_label;
    QPushButton *query_button;
    QTableView *result_tab;
    QSqlQueryModel result_model;
    QTextEdit *query_editor;
    SQLHighlighter *highlighter;

public:
    explicit Window(QWidget *parent = nullptr);

public slots:
    void filltab(bool);

private:
    void create_widgets(QWidget *center);
    void create_editor(QWidget *parent);
    void create_menu();
    void create_statusbar();
};

#endif
