SHELL:=/bin/bash

all: main.c 
	@if [ -f "$(FILE)" ]; then \
		if [ ! "$(DIR)" == "" ]; then \
			if [ ! $(EXE) == "" ]; then \
				mkdir "$(DIR)"; \
				gcc -o "$(EXE)" -pthread main.c; \
				mv "$(EXE)" "$(DIR)"; \
			else \
				echo 'Errore, file eseguibile non specificato'; \
			fi \
		else \
			echo 'Errore, impossibile creare la cartella "$(DIR)"'; \
		fi \
	else \
		echo 'Errore, "$(FILE)" non esiste'; \
	fi