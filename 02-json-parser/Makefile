NAME	= jsonp
CC 		= gcc
CFLAGS 	= -Wall -Wextra -Iincludes -g -O0 -fno-omit-frame-pointer -fsanitize=address

SRC		= $(wildcard src/*.c)
OBJ 	= $(patsubst src/%.c, obj/%.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

obj/%.o: src/%.c
	@mkdir -p obj/
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf obj

fclean: clean
	@rm $(NAME)


run: $(NAME)
	@./$(NAME)
