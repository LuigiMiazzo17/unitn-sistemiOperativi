# Luigi Miazzo 2023-06-06

SHELL:=/bin/bash

all: main.c 
	@if [ ! "$(FILE)" = "" ]; then \
		if [ ! -f "$(FILE)" ]; then \
			echo "start" > "$(FILE)"; \
		fi \
	fi

	@gcc -o SignalProxy main.c

clean:
	@if [ ! "$(FILE)" = "" ]; then \
		if [ -f "$(FILE)" ]; then \
			rm "$(FILE)"; \
		fi \
	fi
	@if [ -f "./SignalProxy" ]; then \
		rm "./SignalProxy"; \
	fi

.PHONY: clean
