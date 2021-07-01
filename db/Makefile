generate.sql: template.sql tmp.sql
	cat template.sql tmp.sql > generate.sql

tmp.sql: generate.py
	python3 generate.py > tmp.sql
