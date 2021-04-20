#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QSqlQueryModel>

class Window : public QMainWindow {
    Q_OBJECT

private:
    QLabel *java_label;
    QPushButton *query_button;
    QTableView *result_tab;
    QSqlQueryModel result_model;
    QTextEdit *query_editor;

public:
    explicit Window(QWidget *parent = nullptr);

public slots:
    void filltab(bool);

private:
    void create_widgets(QWidget *center);
    void create_menu();
    void create_statusbar();
};

#endif
