-- guarda le preferenze di un utente
select u.nome, vg.titolo
from Utente u, Preferenza p, Videogioco vg
where u.id = p.id_usr and vg.id = p.id_vg and u.nome = nome_utente;

-- profitto dall'acquisto di copie fisiche
select sum(prezzo)
from CopiaVideogioco cv, Videogioco vg
where cv.id_vg = vg.id and id_usr is not NULL;
