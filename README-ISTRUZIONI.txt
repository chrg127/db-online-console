=== Generazione del database ===

Usare lo script "generate.sql" nella cartella db/ per generare il database.
Volendo, si può anche generare un nuovo script di generare usando il Makefile trovato nella stessa cartella:
per usarlo, bisogna avere Python installato.

=== Uso dell'applicazione ===

L'applicazione, al momento dell'avvio, leggerà i dati di connessione al database da un file chiamato "DBINFO.txt",
che dovrà trovare nella directory di lavoro corrente (current working directory), che di solito dovrebbe essere la
stessa cartella dove si trova l'applicazione. Scrivere nel file il nome e la password: il nome va nella prima riga,
la password va nella seconda riga. Un esempio già pronto si può trovare insieme all'applicazione.

L'applicazione inoltre si aspetta che sia stato correttamente creato un database con nome "onlineconsole", con tutte le
tabelle già pronte. Usare lo script descritto in precedenza ("generate.sql") per generare il database (l'applicazione
non lo genererà da solo).

Per accedere come utente, si può usare il seguente nome, cognome e password: "Christian" "Ricci" "abc". Per altri nomi, basta
consultare il file di generazione del database.
