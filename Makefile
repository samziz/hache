BINARY_NAME?=hache
BIN_DIR?=/usr/local/bin
COMPILER?=gcc
DIR=src
FILESET?=*.c

build:
	$(COMPILER) $(DIR)/$(FILESET) -Wall -lreadline -o $(BINARY_NAME)
	mv $(BINARY_NAME) $(BIN_DIR)

pedantic:
	$(COMPILER) $(DIR)/$(FILESET) -Wpedantic -lreadline -o $(BINARY_NAME)
	mv $(BINARY_NAME) $(BIN_DIR)
	
local:
	$(COMPILER) $(DIR)/$(FILESET) -Wall -lreadline -o $(BINARY_NAME)