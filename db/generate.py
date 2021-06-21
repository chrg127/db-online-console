#!/usr/bin/env python3

import random
import time
import datetime
import calendar

DATEFMT = "%Y-%m-%d"
HOURFMT = "%H:%M:%S"
ALLFMT = DATEFMT + ' ' + HOURFMT

def rand(mmin, mmax): return random.randrange(mmin, mmax)
def boolrand(): return rand(0, 2) == 0

def rand_datetime(start, end):
    return start + random.random() * (end - start)

def rand_datetime_def(): return rand_datetime(dt_fromstr("2021-01-01 0:0:0", ALLFMT), dt_fromstr("2021-06-21 23:59:59", ALLFMT))

def time_add_days(dt, ds):
    return dt + datetime.timedelta(days = ds)

def time_add_month(dt):
    month = dt.month - 1 + 1
    year = dt.year + month // 12
    month = month % 12 + 1
    day = min(dt.day, calendar.monthrange(year, month)[1])
    return datetime.datetime(year, month, day, dt.hour, dt.minute, dt.second)

def time_add_year(dt):
    return datetime.datetime(dt.year + 1, dt.month, dt.day, dt.hour, dt.minute, dt.second)

def dt_str(dt): return dt.strftime(DATEFMT + ' ' + HOURFMT)
def dt_date(dt): return dt.strftime(DATEFMT)
def dt_hour(dt): return dt.strftime(HOURFMT)
def dt_fromstr(datestr, fmt): return datetime.datetime.fromtimestamp(time.mktime(time.strptime(datestr, fmt)))

NUM_USR = 11
NUM_GAMES = 14

piano_query = "insert into Piano(id, id_usr, tipologia, data_acquisto, ora_acquisto) values"
cv_query = "insert into CopiaVideogioco(id, id_vg, id_usr, data_acquisto) values"
part_query = "insert into Partita values"
pref_query = "insert into Preferenza values"
canc_query = "insert into PianoCancellato values"
sess_query = "insert into Sessione(id, id_vg, id_creatore, data, tempo_trascorso) values"
partecip_query = "insert into Partecipazione(id_usr, id_session) values"
id_piano = 1

def add_piano(name, date):
    global id_piano
    if name != "Gratuito" and not boolrand():
        return date
    print("{}({}, {}, '{}', '{}', '{}');".format(piano_query, id_piano, u, name, dt_date(date), dt_hour(rand_datetime_def())))
    id_piano += 1
    if name == "Gratuito":
        return time_add_month(date)
    if boolrand():
        date = time_add_days(date, rand(1, 28))
        print("{}({}, '{}', '{}');".format(canc_query, id_piano-1, dt_date(date), dt_hour(rand_datetime_def())))
        return date
    elif name == "Annuale":
        return time_add_year(date)
    else:
        return time_add_month(date)

multiplayers = [1, 5, 6, 8, 11, 14]
mult_nums = {
    1: (1,2),
    5: (4,10),
    6: (1,3),
    8: (2,4),
    11: (3,10),
    14: (1,10)
}
sessions = []
session_id = 1
for vg in multiplayers:
    for i in range(1, rand(1, 25)):
        sessions.append([session_id, vg, rand_datetime_def(), rand(1, 5), []])
        session_id += 1

for u in range(1, NUM_USR+1):
    start_date = rand_datetime_def()
    end_date = add_piano("Gratuito", start_date)
    # print("{}({}, 'Gratuito', '{}', '{}');".format(piano_query, u, dt_date(start_date), dt_hour(start_date)))
    # end_date = time_add_month(start_date)
    end_date = add_piano("Mensile", end_date)
    end_date = add_piano("Annuale", end_date)
    end_date = add_piano("Mensile", end_date)
    for vg in range(1, NUM_GAMES+1):
        if vg not in multiplayers and boolrand():
            continue # user has never played this game and won't give any judgement
        # generate some matches
        for i in range(0, rand(1, 11)):
            print("{}({}, {}, '{}', {});".format(part_query, u, vg, dt_str(rand_datetime(start_date, end_date)), rand(1, 5)))
        # add preference
        if boolrand():
            print("{}({}, {});".format(pref_query, u, vg))
        # search for sessions for this game
        if vg not in multiplayers:
            continue
        good_sessions = list(filter(lambda s: s[1] == vg and start_date <= s[2] and end_date >= s[2], sessions))
        for session in good_sessions:
            session[4].append(u)

for s in sessions:
    l = len(s[4])
    num_players = mult_nums[s[1]]
    if l < 2 or l < num_players[0] or l > num_players[1]:
        continue
    creator = s[4][rand(0, len(s[4]))]
    print("{}({}, {}, {}, '{}', {});".format(sess_query, s[0], s[1], creator, dt_date(s[2]), s[3]))
    for usr in s[4]:
        print("{}({}, {});".format(partecip_query, usr, s[0]))
