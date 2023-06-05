# Esercizio 1 Make Lab Slides 2
# Creare un makefile con una regola help di default che mostri una nota
# informativa, una regola backup che crei un backup di una cartella appendendo
# “.bak” al nome e una restore che ripristini il contenuto originale. Per
# definire la cartella sorgente passarne il nome come variabile, ad esempio:
# make -f mf-backup FOLDER=...
# (la variabile FOLDER è disponibile dentro il makefile)

SHELL:=/bin/bash

help:
	@echo "Please use 'make backup FOLDER=<folder>' to backup the folder"

check-env:
	@if [ -z "$(FOLDER)" ]; then \
		echo "FOLDER is undefined"; \
		exit 1; \
	fi

backup: check-env
	@echo "Backing up $(FOLDER)"
	@cp -r $(FOLDER) "$(FOLDER).bak"

.PHONY: help check-env backup
