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
    auto *status_label = new QLabel(this);
    auto *center = new QWidget(this);
    auto *loginscreen = new LoginScreen(this, center);
    auto *userscreen = new UserScreen(this, center);

    setWindowTitle(QStringLiteral("Database project"));
    statusBar()->addPermanentWidget(status_label);
    statusBar()->setLayoutDirection(Qt::LayoutDirection::RightToLeft);
    center->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Maximum);
    setCentralWidget(center);

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

    connect(loginscreen, &LoginScreen::logged, userscreen, &UserScreen::on_login);
}

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
}

UserScreen::UserScreen(Window *wnd, QWidget *parent)
    : QWidget(parent)
{
    uid = 1;
    vid = 1;
    user_profile = new UserProfile;
    plan_profile = new PlanProfile;
    favorites = new DBTable;
    most_played = new DBTable;
    create_plan_group = new QGroupBox("Crea nuovo piano");
    curr_plan_group = new QGroupBox("Piano corrente");

    tabs = make_tabs(
        std::tuple{make_profile_tab(), "Il mio profilo"},
        std::tuple{make_game_tab(), "Videogiochi"},
        std::tuple{make_user_tab(), "Utenti"}
    );
    auto *exit_button = new QPushButton("Esci");

    auto *lt = make_layout<QVBoxLayout>(tabs, exit_button);
    lt->setAlignment(exit_button, Qt::AlignLeft);
    setLayout(lt);

    connect(exit_button, &QPushButton::released, [wnd]() { wnd->show_screen(Window::Screen::LOGIN); });
}

QWidget *UserScreen::make_profile_tab()
{
    create_plan_group = new QGroupBox("Crea nuovo piano");
    curr_plan_group   = new QGroupBox("Piano corrente");
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
        date = index == 0 ? date.addMonths(1) : date.addYears(1);
        enddate->setText(date.toString());
    };
    set_enddate(box->currentIndex());

    add_to_group(create_plan_group,
        box,
        make_layout<QHBoxLayout>(new QLabel("Data di fine:"), enddate),
        createbt
    );
    add_to_group(curr_plan_group, plan_profile, cancelbt);
    auto *lt = make_layout<QHBoxLayout>(user_profile, create_plan_group, curr_plan_group);

    connect(createbt, &QPushButton::released, [=]()
    {
        if (db::create_plan(uid, db::int_to_plan(box->currentData().toInt()))) {
            msgbox("Piano creato con successo.");
            this->on_plan_changed(/* created = */ true);
        } else
            msgbox("Hai già un piano in corso!");
    });
    connect(box, QOverload<int>::of(&QComboBox::currentIndexChanged), set_enddate);
    connect(cancelbt, &QPushButton::released, [=]()
    {
        if (db::cancel_plan(uid)) {
            msgbox("Piano cancellato con successo.");
            this->on_plan_changed(/* created = */ false);
        } else
            msgbox("Non hai nessuno piano in corso!");
    });

    return layout_widget(lt);
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
    auto *playbt = new QPushButton("Gioca");
    auto *buybt = new QPushButton("Compra copia fisica");
    auto *favbt = new QPushButton("Metti tra i preferiti");

    profile->hide();
    searcher->setMinimumWidth(400);
    searcher->insert(1, make_form_layout(std::tuple{"Filtra per:", category}));
    searcher->insert(2, make_layout<QHBoxLayout>(bestbt, most_player_bt));
    add_to_group(fav_group, favorites, most_played);
    playbt->setEnabled(false);
    profile->insert(1, make_layout<QHBoxLayout>(playbt, buybt));
    profile->insert(2, make_layout<QVBoxLayout>(favbt));

    auto *lt = make_layout<QHBoxLayout>(searcher, fav_group, profile);
    lt->setAlignment(searcher, Qt::AlignLeft);

    auto tabclick = [=](const auto &i)
    {
        vid = i.siblingAtColumn(0).data().toInt();
        profile->set_info(db::get_game_info(vid));
        profile->show();
    };

    searcher->on_search([=]()
    {
        searcher->table->fill(
            db::search_games,
            searcher->bar->text(),
            category->currentData().toString()
        );
    });
    searcher->on_tab_click(tabclick);
    connect(favorites,      &QTableView::clicked, tabclick);
    connect(bestbt,         &QPushButton::released, [=]() { searcher->table->fill(db::best_games); });
    connect(most_player_bt, &QPushButton::released, [=]() { searcher->table->fill(db::most_played_games); });
    connect(buybt, &QPushButton::released, [=]()
    {
        if (db::buy_game(vid, uid)) {
            msgbox("Il gioco è stato comprato.");
            profile->update_copies(vid);
        } else
            msgbox("Questo gioco non ha più copie fisiche disponibili.");
    });
    connect(favbt, &QPushButton::released, [this]()
    {
        if (db::add_favorite(uid, vid))
            favorites->fill(db::get_favorites, uid);
        else
            msgbox("Questo gioco è già tra i tuoi preferiti!");
    });

    return layout_widget(lt);
}

QWidget *UserScreen::make_user_tab()
{
    auto *searcher = new Searcher("Cerca utenti");
    auto *profile = new UserProfile;
    auto *fav_group = new QGroupBox("Giochi preferiti");
    auto *fav = new DBTable;
    auto *mostplayed = new DBTable;

    profile->hide();
    fav_group->hide();
    searcher->setMinimumWidth(200);
    add_to_group(fav_group, fav, mostplayed);
    auto *lt = make_layout<QHBoxLayout>(searcher, profile, fav_group);
    lt->setAlignment(searcher, Qt::AlignLeft);

    searcher->on_search([=]() { searcher->table->fill(db::search_users, searcher->bar->text()); });
    searcher->on_tab_click([=](const auto &i)
    {
        int id = i.siblingAtColumn(0).data().toInt();
        profile->set_info(db::get_user_info(id));
        profile->show();
        fav->fill(db::get_favorites, id);
        fav_group->show();
    });

    return layout_widget(lt);
}

void UserScreen::on_plan_changed(bool created)
{
    set_enabled(created, tabs->widget(1), tabs->widget(2), curr_plan_group);
    // tabs->widget(1)->setEnabled(created);
    // tabs->widget(2)->setEnabled(created);
    create_plan_group->setEnabled(!created);
    // curr_plan_group->setEnabled(created);
    favorites->fill(db::get_favorites, uid);
}

void UserScreen::on_login(int id)
{
    if (id == 0) // admin
        return;
    uid = id;
    user_profile->set_info(db::get_user_info(uid));
    auto p_info = db::get_curr_plan_info(uid);
    plan_profile->set_info(p_info);
    on_plan_changed(p_info.has_plan);
    favorites->fill(db::get_favorites, uid);
}

VideogameProfile::VideogameProfile(UserScreen *parent)
    : QWidget(parent)//QGroupBox("Profilo gioco", parent)
{
    title = new QLabel;
    genre = new QLabel;
    year = new QLabel;
    company = new QLabel;
    director = new QLabel;
    price = new QLabel;
    ncopies = new QLabel;
    lt = make_layout<QVBoxLayout>(
        make_form_layout(
            std::tuple{ "Titolo: ",                 title },
            std::tuple{ "Genere: ",                 genre },
            std::tuple{ "Anno: ",                   year },
            std::tuple{ "Azienda: ",                company },
            std::tuple{ "Direttore principale: ",   director },
            std::tuple{ "Prezzo copia fisica: ",    price },
            std::tuple{ "Numero copie disponibili:", ncopies }
        )
    );
    setLayout(lt);
}

void VideogameProfile::update_copies(int id)
{
    ncopies->setText(QString::number(db::get_copy_info(id).second));
}

void VideogameProfile::set_info(const db::GameInfo &info)
{
#define SET(name) name->setText(info.name)
    SET(title), SET(genre), SET(year), SET(company), SET(director);
#undef SET
    price->setText(QString::number(info.price));
    ncopies->setText(QString::number(info.ncopies));
}

UserProfile::UserProfile(QWidget *parent)
    : QWidget(parent)//QGroupBox("Profilo giocatore", parent)
{
    name = new QLabel;
    surname = new QLabel;
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
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
    table = new DBTable;
    table->verticalHeader()->hide();
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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


