create database if not exists onlineconsole;
use onlineconsole;

drop table if exists Acquisto;
drop table if exists CopiaVideogioco;
drop table if exists Partita;
drop table if exists Preferenza;
drop table if exists PianoCancellato;
drop table if exists Piano;
drop table if exists TipologiaPiano;
drop table if exists Partecipazione;
drop table if exists Sessione;
drop table if exists VideogiocoMultiplayer;
drop table if exists Utente;
drop table if exists Videogioco;

create table Utente (
    id              int                 primary key auto_increment,
    nome            varchar(255)        not null,
    cognome         varchar(255)        not null,
    password        varchar(255)        not null,
    email           varchar(255)        not null,
    telefono        int
);

create table Videogioco (
    id              int                 primary key auto_increment,
    titolo          varchar(255)        not null,
    genere          varchar(255),
    anno            year,
    azienda         varchar(255),
    produttore      varchar(255),
    prezzo          int
);

create table CopiaVideogioco (
    id              int                 primary key auto_increment,
    id_vg           int                 not null references Videogioco(id)
);

create table Acquisto (
    id_copia        int                 not null references CopiaVideogioco(id),
    id_usr          int                 not null references Utente(id),
    data_acquisto   date,
    ora_acquisto    time,
    unique (id_copia, id_usr)
);

create table Partita (
    id_usr          int                 not null references Utente(id),
    id_vg           int                 not null references Videogioco(id),
    data            date                not null,
    ora             time                not null,
    ore_gioco       int,
    unique (id_usr, id_vg, data, ora)
);

create table Preferenza (
    id_usr          int                 not null references Utente(id),
    id_vg           int                 not null references Videogioco(id),
    unique (id_usr, id_vg)
);

create table VideogiocoMultiplayer (
    id_vg           int                 primary key references Videogioco(id),
    min_giocatori   int                 default 1,
    max_giocatori   int
);

create table TipologiaPiano (
    tipologia       enum('Gratuito',
                         'Mensile',
                         'Annuale')     primary key,
    costo           int                 not null
);

create table Piano (
    id              int                 primary key auto_increment,
    id_usr          int                 not null references Utente(id),
    tipologia       enum('Gratuito',
                         'Mensile',
                         'Annuale')     not null references TipologiaPiano(tipologia),
    data_acquisto   date                not null,
    ora_acquisto    time                not null,
    data_fine       date                not null,
    ora_fine        time                not null
);

create table PianoCancellato (
    id              int                 primary key references Piano(id),
    data_canc       date                not null,
    ora_canc        time                not null
);

create table Sessione (
    id              int                 primary key auto_increment,
    id_vg           int                 not null references VideogiocoMultiplayer(id_vg),
    id_creatore     int                 not null references Utente(id),
    data            date,
    ora             time,
    tempo_trascorso int
);

create table Partecipazione (
    id_usr          int                 not null references Utente(id),
    id_session      int                 not null references Sessione(id),
    unique(id_usr, id_session)
);

create index ind_utente_id                  on Utente(id);
create index ind_vg_id                      on Videogioco(id);
create index ind_cv_id                      on CopiaVideogioco(id);
create index ind_vgm_id_vg                  on VideogiocoMultiplayer(id_vg);
create index ind_tipo_tipo                  on TipologiaPiano(tipologia);
create index ind_piano_id                   on Piano(id);
create index ind_piano_canc_id              on PianoCancellato(id);

create index indFK_cv_id_vg                 on CopiaVideogioco(id_vg);
create index indFK_acq_id_copia             on Acquisto(id_copia);
create index indFK_acq_id_usr               on Acquisto(id_usr);
create index indFK_partita_id_usr           on Partita(id_usr);
create index indFK_partita_id_vg            on Partita(id_vg);
create index indFK_preferenza_id_usr        on Preferenza(id_usr);
create index indFK_preferenza_id_vg         on Preferenza(id_vg);
create index indFK_piano_id_usr             on Piano(id_usr);
create index indFK_piano_tipologia          on Piano(tipologia);
create index indFK_sessione_id_vg           on Sessione(id_vg);
create index indFK_sessione_id_creatore     on Sessione(id_creatore);
create index indFK_part_id_usr              on Partecipazione(id_usr);
create index indFK_part_id_session          on Partecipazione(id_session);

insert into Utente(nome, cognome, password, email) values('Christian', 'Ricci', 'abc', 'chri@mail.com');
insert into Utente(nome, cognome, password, email) values('Roberto', 'Montalti', 'def', 'rob@mail.com');
insert into Utente(nome, cognome, password, email) values('Juri', 'Simoncini', 'ghi', 'juri@mail.com');
insert into Utente(nome, cognome, password, email) values('Edoardo', 'Savini', 'jkl', 'sedoardo98@mail.com');
insert into Utente(nome, cognome, password, email) values('Giacomo', 'Cavalieri', 'mno', 'giaccava@mail.com');
insert into Utente(nome, cognome, password, email) values('Chad', 'Smith', 'pqr', 'chad@mail.com');
insert into Utente(nome, cognome, password, email) values('Federico', 'Orazio', 'stu', 'orazio@mail.com');
insert into Utente(nome, cognome, password, email) values('Dario', 'Verdi', 'vwx', 'dario@mail.com');
insert into Utente(nome, cognome, password, email) values('Gianluca', 'Migliarini', 'yz', 'gianlu@mail.com');
insert into Utente(nome, cognome, password, email) values('Simone', 'Rega', 'abcdef', 'rega@mail.com');
insert into Utente(nome, cognome, password, email) values('Francesco', 'Tappi', 'ghijk', 'tappi@mail.com');

insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('Super Mario World', 'Platformer', 1990, 'Nintendo', 'Shigeru Miyamoto', 30);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('Touhou 6', 'Danmaku', 2002, 'Team Alice', 'ZUN', 20);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('DOOM', 'FPS', 1993, 'id Software', 'John Carmack', 20);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('EarthBound', 'JRPG', 1995, 'Nintendo', 'Shigesato Itoi', 30);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('Overwatch', 'FPS', 2016, 'Blizzard', 'Jeff Kaplam', 60);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('Genshin Impact', 'ARPG', 2020, 'miHoYo', NULL, 20);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('Celeste', 'Platformer', 2018, 'Extremely OK Games', 'Maddy Thorson', 20);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('Rocket League', 'Sport', 2015, 'Psyonix Studios', NULL, 30);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('DOOM Eternal', 'FPS', 2020, 'id Software', 'Hugo Martin', 60);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('Super Mario 64', 'Platformer', 1997, 'Nintendo', 'Shigeru Myamoto', 20);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('Rainbow Six Siege', 'FPS', 2015, 'Ubisoft', 'Xavier Marquis', 60);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('Final Fantasy VII', 'JRPG', 1997, 'Square', 'Yoshinori Kitase', 30);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('Cave Story', 'Platformer', 2004, 'Nicalis', 'Pixel', 10);
insert into Videogioco(titolo, genere, anno, azienda, produttore, prezzo) values('KrunkerIO', 'FPS', 2015, NULL, NULL, NULL);

insert into VideogiocoMultiplayer values(1, 1, 2);
insert into VideogiocoMultiplayer values(5, 4, 10);
insert into VideogiocoMultiplayer values(6, 1, 3);
insert into VideogiocoMultiplayer values(8, 2, 4);
insert into VideogiocoMultiplayer values(11, 3, 10);
insert into VideogiocoMultiplayer values(14, 1, 10);

insert into TipologiaPiano values('Gratuito', 0);
insert into TipologiaPiano values('Mensile', 10);
insert into TipologiaPiano values('Annuale', 100);

