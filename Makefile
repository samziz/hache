BINARY_NAME?=hache
BIN_DIR?=/usr/local/bin
COMPILER?=gcc
FILESET?=*.c

build:
	$(COMPILER) $(FILESET) -Wall -lreadline -o $(BINARY_NAME)
	mv $(BINARY_NAME) $(BIN_DIR)

pedantic:
	$(COMPILER) $(FILESET) -Wpedantic -lreadline -o $(BINARY_NAME)
	mv $(BINARY_NAME) $(BIN_DIR)
	
local:
	$(COMPILER) $(FILESET) -Wall -lreadline -o $(BINARY_NAME)