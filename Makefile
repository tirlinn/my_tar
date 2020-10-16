CC = gcc
CFLAGS = -g3 -fsanitize=address -Wall -Werror -Wextra
INCLUDES = -I./
SRCS = my_tar.c my_tar_c.c write_file.c functions.c my_tar_t.c my_tar_r.c my_tar_u.c my_tar_x.c
OBJS = $(SRCS:.c=.o)
MAIN = my_tar

.PHONY: depend clean

all:    $(MAIN) clean_o
	@echo  Simple compiler named $(MAIN) has been compiled

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) #$(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean_o:
	$(RM) *.o

clean:
	$(RM) *.o $(MAIN)
