#ifndef WINDOW_HPP_INCLUDED
#define WINDOW_HPP_INCLUDED

#include <QString>
#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QTableView>
#include <QSqlQueryModel>

class Window : public QMainWindow {
    Q_OBJECT

private:
    QLabel *javafaschifo_label;
    QPushButton *button;
    QTableView *tabview;
    QSqlQueryModel tabmodel;

public:
    explicit Window(QWidget *parent = nullptr);

public slots:
    void filltab(bool);
    void create_menu();
};

#endif
