# Luigi Miazzo 2023-06-06

SHELL:=/bin/bash

all: main.c 
	@if [ -z "$(NAME)" ]; then \
		gcc -o program main.c; \
	else \
		gcc -o "$(NAME)" main.c; \
	fi

