=== Generazione del database ===

Usare lo script "generate.sql" nella cartella db/ per generare il database.
Volendo, si può anche generare un nuovo script di generare usando il Makefile trovato nella stessa cartella:
per usarlo, bisogna avere Python installato.

=== Uso dell'applicazione ===

L'applicazione, al momento dell'avvio, leggerà i dati di connessione al database da un file chiamato "DBINFO.txt",
che dovrà trovare nella directory di lavoro corrente (current working directory). Scrivere qui il nome e la password.
L'applicazione inoltre si aspetta che sia stato correttamente creato un database con nome "OnlineConsole", con tutte le
tabelle già pronte. Usare lo script descritto in precedenza ("generate.sql") per generare il database (l'applicazione
non lo genererà da solo).

