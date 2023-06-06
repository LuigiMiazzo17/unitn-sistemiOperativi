# Luigi Miazzo 2023-06-06

SHELL:=/bin/bash

all: main.c 
	@if [ -z "$(NAME)" ]; then \
		gcc -o app main.c; \
	else \
		gcc -o "$(NAME)" main.c; \
	fi

