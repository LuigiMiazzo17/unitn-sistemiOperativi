# Luigi Miazzo 2023-06-05

all: main.c check-env
	@gcc -o coda main.c #-lpthread

check-env:
	@if [ -z "$(DEST)" ]; then \
		echo "?ERROR" 1>&2; \
		exit 1; \
	fi
	@if [ ! -d "$(DEST)" ]; then \
		echo "?ERROR" 1>&2; \
		exit 1; \
	fi

.PHONY: check-env
