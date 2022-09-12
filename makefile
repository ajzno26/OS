CC = gcc
FLAGS = -Wall -Werror -std=c99
APP = shell

all: $(APP) $(APP)_d

$(APP): $(APP).c 
	$(CC) $(FLAGS) $^ -o $@

$(APP)_d: $(APP).s shell.c
	$(CC) $(FLAGS) -D DOUBLE $^ -o $@

clean:
	rm -rf $(APP) $(APP)_d


