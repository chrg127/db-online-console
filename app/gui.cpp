#include "gui.hpp"

#include <QDebug>
#include <QAction>
#include <QComboBox>
#include <QDate>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenuBar>
#include <QPixmap>
#include <QPushButton>
#include <QStackedWidget>
#include <QStatusBar>
#include <QSqlError>
#include <QSizePolicy>
#include <QTextEdit>
#include <QTableView>
#include "qthelpers.hpp"
#include "sqlhighlighter.hpp"
#include "database.hpp"
#ifdef _CATPRISM
#include "opengl.hpp"
#endif

Window::Window(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Database project"));
    auto *status_label = new QLabel(this);
    statusBar()->addPermanentWidget(status_label);
    statusBar()->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
    auto *center = new QWidget(this);
    center->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    setCentralWidget(center);

    auto *loginscreen = new LoginScreen(this, center);
    auto *userscreen = new UserScreen(this, center);
    connect(loginscreen, &LoginScreen::logged, userscreen, &UserScreen::on_login);

    stack_widget = make_layout<QStackedWidget>(
        loginscreen,
        new AdminScreen(this, center),
        userscreen
#ifdef _CATPRISM
        , new CatPrismScreen(center);
#endif
    );
    auto lt = make_layout<QVBoxLayout>(
        new QLabel("java fa schifo!", center),
        stack_widget
    );
    center->setLayout(lt);
}

// void Window::create_menu()
// {
    // enum MenuIndex { MENU_FILE, MENU_COUNT, };
    // QMenu *menus[MENU_COUNT];
    // const auto add_item = [&, this](MenuIndex idx, const QString &text, bool enable, auto &&func)
    // {
    //     auto *act = new QAction(text, this);
    //     connect(act, &QAction::triggered, this, func);
    //     menus[idx]->addAction(act);
    //     act->setEnabled(enable);
    // };
    // menus[MENU_FILE] = menuBar()->addMenu("&File");
    // add_item(MENU_FILE, "&Open", true, []() { fmt::print("dinosauri\n"); });
// }

void Window::show_screen(Screen screen)
{
    switch (screen) {
    case Screen::LOGIN: stack_widget->setCurrentIndex(0); break;
    case Screen::ADMIN: stack_widget->setCurrentIndex(1); break;
    case Screen::USER:  stack_widget->setCurrentIndex(2); break;
#ifdef _CATPRISM
    case Screen::CATPRISM: stack_widget->setCurrentIndex(3); break;
#endif
    }
}

LoginScreen::LoginScreen(Window *mainwnd, QWidget *parent)
    : QWidget(parent)
{
    QLabel *image = new QLabel;
    image->setPixmap(QPixmap("logo.png"));
    image->setAlignment(Qt::AlignCenter);

    auto *name_box = new QLineEdit;
    auto *surname_box = new QLineEdit;
    auto *pass_box = new QLineEdit;
    pass_box->setEchoMode(QLineEdit::EchoMode::Password);

    auto *admin_button = new QPushButton("Login as admin");
    auto *user_button = new QPushButton("Login as user");

    auto validate = [=](auto &&validate_fn, Window::Screen screen)
    {
        auto name = name_box->text(), surname = surname_box->text(), password = pass_box->text();
        if (int id = validate_fn(name, surname, password); id != -1) {
            // set_user_id(id);
            mainwnd->show_screen(screen);
            emit logged(id);
        } else {
            msgbox(QString("Couldn't validate user %1 %2. Make sure you've typed the right username and password.")
                   .arg(name).arg(surname));
        }
    };

    connect(admin_button, &QPushButton::released, [=](){ validate(db::validate_admin, Window::Screen::ADMIN); });
    connect(user_button,  &QPushButton::released, [=](){ validate(db::validate_user,  Window::Screen::USER); });

#ifdef _CATPRISM
    auto *prism_button = new QPushButton("View the cat prism");
    connect(prism_button, &QPushButton::released, [mainwnd]() { mainwnd->show_screen(Window::Screen::CATPRISM); });
#endif

    auto *login_box = new QGroupBox("Login");
    login_box->setLayout(make_form_layout(
        std::tuple{ "Nome:",     name_box },
        std::tuple{ "Cognome:",  surname_box },
        std::tuple{ "Password:", pass_box }
    ));
    login_box->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    auto *mainlt = make_layout<QVBoxLayout>(
        image, login_box,
        make_layout<QHBoxLayout>(admin_button, user_button)
    );
    mainlt->setContentsMargins(100, 10, 100, 10);
    this->setLayout(mainlt);
}

UserScreen::UserScreen(Window *wnd, QWidget *parent)
    : QWidget(parent)
{
    auto *exit_button = new QPushButton("Esci");
    auto *tabs = make_tabs(
        std::tuple{make_profile_tab(), "Il mio profilo"},
        std::tuple{make_game_tab(), "Videogiochi"},
        std::tuple{make_user_tab(), "Utenti"}
    );
    auto *lt = make_layout<QVBoxLayout>(tabs, exit_button);
    lt->setAlignment(exit_button, Qt::AlignLeft);
    setLayout(lt);
    connect(exit_button, &QPushButton::released, [wnd]() { wnd->show_screen(Window::Screen::LOGIN); });
}

QWidget *UserScreen::make_game_tab()
{
    auto *fav_group = new QGroupBox("Giochi preferiti");
    auto *searcher = new Searcher("Cerca giochi");
    auto *category = make_comboxbox(
        std::tuple{"Titolo", QVariant("titolo")},
        std::tuple{"Genere", QVariant("genere")},
        std::tuple{"Azienda", QVariant("azienda")}
    );
    auto *bestbt = new QPushButton("Giochi più popolari");
    auto *most_player_bt = new QPushButton("Giochi più giocati");
    auto *profile = new VideogameProfile(this);
    favorites = make_table(&fav_model);

    profile->hide();
    searcher->setMinimumWidth(450);
    searcher->insert(1, make_form_layout(std::tuple{"Filtra per:", category}));
    searcher->insert(2, make_layout<QHBoxLayout>(bestbt, most_player_bt));
    fav_group->setLayout(make_layout<QVBoxLayout>(favorites));
    auto *lt = make_layout<QHBoxLayout>(searcher, fav_group, profile);
    lt->setAlignment(searcher, Qt::AlignLeft);

    auto tabclick = [=](const auto &i)
    {
        int id = i.siblingAtColumn(0).data().toInt();
        profile->set_info(id, db::get_game_info(id));
        profile->show();
    };

    searcher->on_search([=]()
    {
        db::search_games(
            searcher->model,
            searcher->bar->text(),
            category->currentData().toString()
        );
        searcher->after_search();
    });
    searcher->on_tab_click(tabclick);
    connect(favorites, &QTableView::clicked, tabclick);
    connect(bestbt,         &QPushButton::released, [=]() { db::best_games(searcher->model); searcher->after_search(); });
    connect(most_player_bt, &QPushButton::released, [=]() { db::most_played_games(searcher->model); searcher->after_search(); });

    return layout_widget(lt);
}

QWidget *UserScreen::make_user_tab()
{
    auto *searcher = new Searcher("Cerca utenti");
    auto *profile = new UserProfile;
    profile->hide();
    searcher->setMinimumWidth(200);
    auto *lt = make_layout<QHBoxLayout>(searcher, profile);
    lt->setAlignment(searcher, Qt::AlignLeft);

    searcher->on_search([=]()
    {
        db::search_users(searcher->model, searcher->bar->text());
        searcher->after_search();
    });
    searcher->on_tab_click([=](const auto &i)
    {
        profile->set_info(db::get_user_info(i.siblingAtColumn(0).data().toInt()));
        profile->show();
    });

    return layout_widget(lt);
}

QWidget *UserScreen::make_profile_tab()
{
    auto *create_group = new QGroupBox("Crea nuovo piano");
    auto *curr_group   = new QGroupBox("Piano corrente");

    user_profile = new UserProfile;
    plan_profile = new PlanProfile;

    auto *box = make_comboxbox(
        std::tuple{"Mensile", QVariant(1)},
        std::tuple{"Annuale", QVariant(2)}
    );
    auto *enddate = new QLabel;
    auto *createbt = new QPushButton("Crea piano");
    auto *cancelbt = new QPushButton("Cancella piano corrente");

    auto set_enddate = [=](int index)
    {
        auto date = QDate::currentDate();
        if (index == 0)
            date = date.addMonths(1);
        else
            date = date.addYears(1);
        enddate->setText(date.toString());
    };
    set_enddate(box->currentIndex());

    create_group->setLayout(make_layout<QVBoxLayout>(
        box,
        make_layout<QHBoxLayout>(new QLabel("Data di fine:"), enddate),
        createbt
    ));
    curr_group->setLayout(make_layout<QVBoxLayout>(
        plan_profile,
        cancelbt
    ));
    auto *lt = make_layout<QHBoxLayout>(user_profile, create_group, curr_group);

    connect(createbt, &QPushButton::released, [=]()
    {
        if (db::create_plan(uid, db::int_to_plan(box->currentData().toInt())))
            msgbox("Piano creato con successo.");
        else
            msgbox("Hai già un piano in corso!");
    });
    connect(box, QOverload<int>::of(&QComboBox::currentIndexChanged), set_enddate);

    return layout_widget(lt);
}

void UserScreen::update_favorites()
{
    db::get_favorites(fav_model, uid);
    favorites->hideColumn(0);
    favorites->resizeColumnsToContents();
}

void UserScreen::on_login(int id)
{
    if (id == 0) // admin
        return;
    uid = id;
    user_profile->set_info(db::get_user_info(uid));
    plan_profile->set_info(db::get_curr_plan_info(uid));
    update_favorites();
}

VideogameProfile::VideogameProfile(UserScreen *parent)
    : QGroupBox("Profilo gioco", parent)
{
    title = new QLabel;
    genre = new QLabel;
    year = new QLabel;
    company = new QLabel;
    director = new QLabel;
    price = new QLabel;
    ncopies = new QLabel;
    auto *playbt = new QPushButton("Gioca");
    auto *buybt = new QPushButton("Compra copia fisica");
    auto *favbt = new QPushButton("Metti tra i preferiti");

    playbt->setEnabled(false);
    setLayout(make_layout<QVBoxLayout>(
        make_form_layout(
            std::tuple{ "Titolo: ",                 title },
            std::tuple{ "Genere: ",                 genre },
            std::tuple{ "Anno: ",                   year },
            std::tuple{ "Azienda: ",                company },
            std::tuple{ "Direttore principale: ",   director },
            std::tuple{ "Prezzo copia fisica: ",    price },
            std::tuple{ "Numero copie disponibili:", ncopies }
        ),
        make_layout<QHBoxLayout>(playbt, buybt),
        favbt
    ));

    connect(buybt, &QPushButton::released, [this, parent]()
    {
        if (db::buy_game(parent->getvid(), parent->getuid())) {
            msgbox("Il gioco è stato comprato.");
            ncopies->setText(QString::number(db::get_copy_info(id).second));
        } else
            msgbox("Questo gioco non ha più copie fisiche disponibili.");
    });
    connect(favbt, &QPushButton::released, [this, parent]()
    {
        qDebug() << "hello";
        if (db::add_favorite(parent->getuid(), parent->getvid()))
            parent->update_favorites();
        else
            msgbox("Questo gioco è già tra i tuoi preferiti!");
    });
}

void VideogameProfile::set_info(int id, const db::GameInfo &info)
{
    this->id = id;
#define SET(name) name->setText(info.name)
    SET(title), SET(genre), SET(year), SET(company), SET(director);
#undef SET
    price->setText(QString::number(info.price));
    ncopies->setText(QString::number(info.ncopies));
}

UserProfile::UserProfile(QWidget *parent)
    : QGroupBox("Profilo utente", parent)
{
    name = new QLabel;
    surname = new QLabel;
    setLayout(make_form_layout(
        std::tuple{ new QLabel("Nome: "),    name },
        std::tuple{ new QLabel("Cognome: "), surname }
    ));
}

void UserProfile::set_info(const db::UserInfo &info)
{
#define SET(name) name->setText(info.name)
    SET(name);
    SET(surname);
#undef SET
}

Searcher::Searcher(const QString &title, QWidget *parent)
    : QGroupBox(title, parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    bar = new QLineEdit;
    bt = new QPushButton("Cerca");
    table = make_table(&model);
    table->verticalHeader()->hide();
    lt = make_layout<QVBoxLayout>(
        make_layout<QHBoxLayout>(bar, bt),
        table
    );
    setLayout(lt);
}

PlanProfile::PlanProfile(QWidget *parent)
    : QWidget(parent)
{
    type = new QLabel;
    start = new QLabel;
    end = new QLabel;
    noplan = new QLabel("Non hai nessun piano attivo.");
    plan_form = layout_widget(make_form_layout(
        std::tuple{"Tipologia:", type},
        std::tuple{"Data inizio:", start},
        std::tuple{"Data fine:", end}
    ));
    setLayout(make_layout<QVBoxLayout>(
        noplan,
        plan_form
    ));
}

void PlanProfile::set_info(const db::PlanInfo &info)
{
    if (info.has_plan) {
        type->setText(plan_to_string(info.type));
        start->setText(info.start.toString());
        end->setText(info.end.toString());
        noplan->hide();
        plan_form->show();
    } else {
        noplan->show();
        plan_form->hide();
    }
}

AdminScreen::AdminScreen(Window *wnd, QWidget *parent)
    : QWidget(parent)
{
    auto *result_tab   = make_table(&result_model);
    auto *query_editor = new QTextEdit;
    auto *highlighter  = new SQLHighlighter(query_editor->document());
    auto *query_button = new QPushButton("Execute query");
    auto *exit_button  = new QPushButton("Exit");
    query_editor->setFont(make_font("Monospace", QFont::TypeWriter));
    setLayout(
        make_layout<QVBoxLayout>(
            make_layout<QHBoxLayout>(query_editor, result_tab),
            query_button,
            exit_button
        )
    );

    connect(query_button, &QPushButton::released, this, [=]()
    {
        auto errmsg = db::run_query(result_model, query_editor->toPlainText());
        if (errmsg)
            msgbox(errmsg.value());
        result_tab->resizeColumnsToContents();
    });
    connect(exit_button, &QPushButton::released, [wnd]() { wnd->show_screen(Window::Screen::LOGIN); });
}

#ifdef _CATPRISM
CatPrismScreen::CatPrismScreen(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *lt = new QVBoxLayout(this);
    glwidget = new GLWidget(this);
    lt->addWidget(glwidget);
}
#endif

