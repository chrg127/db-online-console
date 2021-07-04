#include "gui.hpp"

#include <QComboBox>
#include <QDate>
#include <QDateTimeEdit>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QLocale>
#include <QPixmap>
#include <QPushButton>
#include <QRandomGenerator>
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

    setWindowTitle(QStringLiteral("Online console"));
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
    auto lt = make_layout<QVBoxLayout>(stack_widget);
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
    auto *image = make_image_label(":logo.png");
    auto *name_box = new QLineEdit(this);
    auto *surname_box = new QLineEdit(this);
    auto *pass_box = new QLineEdit(this);
    auto *admin_button = new QPushButton("Login admin", this);
    auto *user_button = new QPushButton("Login utente", this);
    auto *login_box = new QGroupBox("Login", this);

    image->setAlignment(Qt::AlignCenter);
    pass_box->setEchoMode(QLineEdit::EchoMode::Password);

#ifdef _CATPRISM
    auto *prism_button = new QPushButton("View the cat prism");
    connect(prism_button, &QPushButton::released, [mainwnd]() { mainwnd->show_screen(Window::Screen::CATPRISM); });
#endif

    login_box->setLayout(make_form_layout(
        std::tuple{ "Nome:",     name_box },
        std::tuple{ "Cognome:",  surname_box },
        std::tuple{ "Password:", pass_box }
    ));
    login_box->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    auto *mainlt = make_layout<QVBoxLayout>(
        image, login_box,
        make_layout<QHBoxLayout>(user_button, admin_button)
    );
    mainlt->setContentsMargins(100, 100, 100, 100);
    setLayout(mainlt);

    auto validate = [=](auto &&validate_fn, Window::Screen screen)
    {
        auto name = name_box->text(), surname = surname_box->text(), password = pass_box->text();
        if (int id = validate_fn(name, surname, password); id != -1) {
            mainwnd->show_screen(screen);
            emit logged(id);
        } else {
            msgbox(QString("Utente '%1 %2' non trovato. Hai digitato per bene la password?")
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
    svid = -1;
    user_profile = new UserProfile(this);
    plan_profile = new PlanProfile(this);
    favorites = new DBTable(this);
    create_plan_group = new QGroupBox("Crea nuovo piano", this);
    curr_plan_group = new QGroupBox("Piano corrente", this);
    session_users = new QListWidget(this);
    tabs = make_tabs(this,
        std::tuple{make_profile_tab(), "Il mio profilo"},
        std::tuple{make_game_tab(), "Videogiochi"},
        std::tuple{make_users_tab(), "Utenti"},
        std::tuple{make_session_tab(), "Sessioni"}
    );

    auto *exit_button = new QPushButton("Esci", this);

    auto *lt = make_layout<QVBoxLayout>(tabs, exit_button);
    lt->setAlignment(exit_button, Qt::AlignLeft);
    setLayout(lt);

    connect(exit_button, &QPushButton::released, [wnd]() { wnd->show_screen(Window::Screen::LOGIN); });
}

QWidget *UserScreen::make_profile_tab()
{
    auto *box = make_comboxbox(this,
        std::tuple{"Mensile", QVariant(1)},
        std::tuple{"Annuale", QVariant(2)}
    );
    auto *enddate = new QLabel(this);
    auto *createbt = new QPushButton("Crea piano", this);
    auto *cancelbt = new QPushButton("Cancella piano corrente", this);

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
    auto *lt = make_layout<QHBoxLayout>(
        user_profile,
        make_layout<QVBoxLayout>(create_plan_group, curr_plan_group)
    );

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
        if (db::cancel_plan(plan_profile->planid())) {
            msgbox("Piano cancellato con successo.");
            this->on_plan_changed(/* created = */ false);
        } else
            msgbox("Non hai nessuno piano in corso!");
    });

    return layout_widget(lt);
}

QWidget *UserScreen::make_game_tab()
{
    auto *fav_group = new QGroupBox("Giochi preferiti", this);
    auto *searcher = new Searcher("Cerca giochi", this);
    auto *category = make_comboxbox(this,
        std::tuple{"Titolo", QVariant("titolo")},
        std::tuple{"Genere", QVariant("genere")},
        std::tuple{"Azienda", QVariant("azienda")}
    );
    auto *bestbt = new QPushButton("Giochi più popolari", this);
    auto *most_player_bt = new QPushButton("Giochi più giocati", this);
    auto *profile = new VideogameProfile(this);
    auto *playbt = new QPushButton("Gioca", this);
    auto *buybt = new QPushButton("Compra copia fisica", this);
    auto *favbt = new QPushButton("Aggiungi ai preferiti", this);

    profile->hide();
    searcher->setMinimumWidth(400);
    searcher->insert(1, make_form_layout(std::tuple{"Filtra per:", category}));
    searcher->insert(2, make_layout<QHBoxLayout>(bestbt, most_player_bt));
    add_to_group(fav_group, favorites);
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
            profile->set_info(db::get_game_info(vid));
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

QWidget *UserScreen::make_users_tab()
{
    auto *searcher = new Searcher("Cerca utenti", this);
    auto *profile = new UserProfile(this);
    auto *fav_group = new QGroupBox("Giochi preferiti", this);
    auto *favtab = new DBTable(this);
    auto *session_group = new QGroupBox("Statistiche sessioni", this);
    auto *stattab = new DBTable(this);

    profile->hide();
    fav_group->hide();
    session_group->hide();
    searcher->setMinimumWidth(200);
    add_to_group(fav_group, favtab);
    add_to_group(session_group, stattab);
    auto *lt = make_layout<QHBoxLayout>(
        searcher,
        profile,
        make_layout<QVBoxLayout>(fav_group, session_group)
    );
    lt->setAlignment(searcher, Qt::AlignLeft);

    searcher->on_search([=]() { searcher->table->fill(db::search_users, searcher->bar->text()); });
    searcher->on_tab_click([=](const auto &i)
    {
        int id = i.siblingAtColumn(0).data().toInt();
        profile->set_info(db::get_user_info(id));
        favtab->fill(db::get_favorites, id);
        stattab->fill(db::get_session_statistics, id);
        profile->show();
        fav_group->show();
        session_group->show();
    });

    return layout_widget(lt);
}

QWidget *UserScreen::make_session_tab()
{
    auto *user_searcher = new Searcher("Cerca utenti", this);
    auto *game_searcher = new Searcher("Cerca videogiochi", this);
    auto *game_chosen = new QLabel(this);
    auto *session_group = new QGroupBox("Crea sessione", this);
    auto *create = new QPushButton("Crea", this);
    auto *remove = new QPushButton("Rimuovi utente", this);

    add_to_group(session_group,
        session_users,
        make_form_layout(std::tuple{"Gioco scelto:", game_chosen}),
        make_layout<QHBoxLayout>(create, remove)
    );
    auto *lt = make_layout<QHBoxLayout>(
        make_layout<QVBoxLayout>(
            user_searcher,
            game_searcher
        ),
        session_group
    );

    user_searcher->on_search([=]() { user_searcher->table->fill(db::search_users, user_searcher->bar->text()); });
    user_searcher->on_tab_click([=](const auto &i)
    {
        int id = i.siblingAtColumn(0).data().toInt();
        QString name = i.siblingAtColumn(1).data().toString();
        QString surname = i.siblingAtColumn(2).data().toString();
        if (std::find(session_users_ids.begin(), session_users_ids.end(), id) == session_users_ids.end()) {
            session_users_ids.push_back(id);
            session_users->addItem(name + ' ' + surname);
        }
    });
    game_searcher->on_search([=]() { game_searcher->table->fill(db::search_games, game_searcher->bar->text(), "titolo"); });
    game_searcher->on_tab_click([=](const auto &i)
    {
        svid = i.siblingAtColumn(0).data().toInt();
        game_chosen->setText(i.siblingAtColumn(1).data().toString());
    });
    connect(create, &QPushButton::released, [this]()
    {
        if (svid == -1) {
            msgbox("Devi scegliere un gioco!");
            return;
        }
        auto err = db::create_session(
            svid,
            session_users_ids[0],
            session_users_ids,
            QRandomGenerator::global()->bounded(1, 6)
        );
        if (err)
            msgbox("Errore: " + err.value());
        else
            msgbox("Sessione creata correttamente.");
    });
    connect(remove, &QPushButton::released, [=]()
    {
        int row = session_users->currentRow();
        if (row > 0) {
            session_users->takeItem(row);
            session_users_ids.erase(session_users_ids.begin() + row);
        } else
            msgbox("Non puoi rimuovere te stesso!");
    });

    return layout_widget(lt);
}

void UserScreen::on_plan_changed(bool created)
{
    if (created) {
        auto plan_info = db::get_curr_plan_info(uid);
        if (plan_info)
            plan_profile->set_info(plan_info.value());
        else
            created = false;
    } else
        plan_profile->set_info(std::nullopt);
    set_enabled(created, tabs->widget(1), tabs->widget(2), tabs->widget(3), curr_plan_group);
    create_plan_group->setEnabled(!created);
}

void UserScreen::on_login(int id)
{
    if (id == 0) // admin
        return;
    uid = id;
    auto user_info = db::get_user_info(uid);
    user_profile->set_info(user_info);

    on_plan_changed(true);

    favorites->fill(db::get_favorites, uid);

    session_users->clear();
    while (session_users->count() > 0)
        session_users->takeItem(0);
    session_users_ids.clear();
    session_users->addItem(user_info.name + ' ' + user_info.surname);
    session_users_ids.push_back(uid);
}

VideogameProfile::VideogameProfile(UserScreen *parent)
    : QGroupBox("Profilo gioco", parent)
{
    title = new QLabel(this);
    genre = new QLabel(this);
    year = new QLabel(this);
    company = new QLabel(this);
    director = new QLabel(this);
    price = new QLabel(this);
    ncopies = new QLabel(this);
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
    : QGroupBox("Profilo giocatore", parent)
{
    name = new QLabel(this);
    surname = new QLabel(this);
    daily_hours = new QLabel(this);
    total_hours = new QLabel(this);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    auto *lt = make_form_layout(
        std::tuple{ "Nome:",    name },
        std::tuple{ "Cognome:", surname },
        std::tuple{ "Ore giornaliere:", daily_hours },
        std::tuple{ "Ore totali:", total_hours }
    );
    lt->setContentsMargins(80, 0, 80, 0);
    setLayout(lt);
}

void UserProfile::set_info(const db::UserInfo &info)
{
    name->setText(info.name);
    surname->setText(info.surname);
    daily_hours->setText(QString::number(info.daily_hours));
    total_hours->setText(QString::number(info.total_hours));
}

Searcher::Searcher(const QString &title, QWidget *parent)
    : QGroupBox(title, parent)
{
    bar = new QLineEdit(this);
    bt = new QPushButton("Cerca", this);
    table = new DBTable(this);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
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
    type = new QLabel(this);
    start = new QLabel(this);
    end = new QLabel(this);
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

void PlanProfile::set_info(const std::optional<db::PlanInfo> &info)
{
    if (info) {
        id = info->id;
        type->setText(plan_to_string(info->type));
        start->setText(info->start.toString());
        end->setText(info->end.toString());
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
    auto *query_group  = new QGroupBox("Scrivi una query", this);
    auto *query_editor = new QTextEdit(this);
    auto *result_tab   = make_table(&result_model);
    auto *highlighter  = new SQLHighlighter(query_editor->document());
    auto *query_button = new QPushButton("Esegui", this);

    auto *profit_group = new QGroupBox("Profitto mensile", this);
    auto *month = new QComboBox(this);
    auto *year = new QDateTimeEdit(this);
    auto *profit = new QLabel(this);
    auto *getprofit = new QPushButton("Guarda profitto", this);

    auto *exit_button  = new QPushButton("Esci", this);

    for (int i = 1; i <= 12; i++)
        month->addItem(QLocale::system().monthName(i));
    query_editor->setFont(make_font("Monospace", QFont::TypeWriter));
    year->setDisplayFormat("yyyy");
    year->setDateRange(QDate(1753, 1, 1), QDate(8000, 1, 1));
    add_to_group(profit_group,
        make_form_layout(
            std::tuple{ "Mese:", month },
            std::tuple{ "Anno:", year },
            std::tuple{ "Profitto:", profit }
        ),
        getprofit
    );
    add_to_group(query_group, query_editor, result_tab, query_button);
    auto *lt = make_layout<QVBoxLayout>(
        make_layout<QHBoxLayout>(profit_group, query_group),
        exit_button
    );
    lt->setAlignment(exit_button, Qt::AlignLeft);
    setLayout(lt);

    connect(query_button, &QPushButton::released, this, [=]()
    {
        auto errmsg = db::run_query(result_model, query_editor->toPlainText());
        if (errmsg)
            msgbox(errmsg.value());
        result_tab->resizeColumnsToContents();
        result_tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    });
    connect(exit_button, &QPushButton::released, [wnd]() { wnd->show_screen(Window::Screen::LOGIN); });
    connect(getprofit, &QPushButton::released, [=]()
    {
        int yearnum  = year->date().year();
        int monthnum = month->currentIndex() + 1;
        int p = db::get_monthly_profit(yearnum, monthnum);
        profit->setText(QString::number(p));
    });
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

