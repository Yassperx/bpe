CC=cc
CFLAGS=-Wall -Werror
DEBUGFLAGS=-fsanitize=address,undefined -ggdb
RELEASEFLAGS=-O2

PYTHON=~/penv/bin/python

SRC_C_DIR=src_c
SRC_PY_DIR=src_py

SOURCE=$(SRC_C_DIR)/main.c $(SRC_C_DIR)/bpe.c $(SRC_C_DIR)/core.c
BIN=bin/main 
DATA_SIZE=medium

all: debug
	./bin/main

release: src_c/main.c
	$(CC) $(SOURCE) -o $(BIN) $(CLFAGS) $(RELEASEFLAGS)

debug: src_c/main.c
	$(CC) $(SOURCE) -o $(BIN) $(CLFAGS) $(DEBUGFLAGS)

assets/small_saved.txt: $(SRC_PY_DIR)/bpe_core.py assets/$(DATA_SIZE)_data.txt
	$(PYTHON) $(SRC_PY_DIR)/bpe_core.py save assets/$(DATA_SIZE)_data.txt assets/$(DATA_SIZE)_saved.txt

assets/small_data.txt: $(SRC_PY_DIR)/csv_to_txt.py assets/$(DATA_SIZE)data.csv
	$(PYTHON) $(SRC_PY_DIR)/csv_to_txt.py assets/$(DATA_SIZE)_data.csv assets/$(DATA_SIZE)_data.txt
