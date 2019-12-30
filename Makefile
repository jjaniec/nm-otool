# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jjaniec <jjaniec@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/10/26 17:45:35 by jjaniec           #+#    #+#              #
#    Updated: 2019/12/28 17:05:56 by jjaniec          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME_NM = ft_nm
NAME_OTOOL = ft_otool

UNAME_S := $(shell uname -s)

NM_SRC_NAME = 	ft_nm.c \
				ft_nm_process_file.c \
				is_magic_64.c \
				is_big_endian.c \
				is_magic_fat.c \
				swap_byte_range.c \
				slseek.c \
				sseek_read.c \
				init_header_info.c \
				goto_load_command.c \
				build_symbol_list.c \
				is_magic_mach.c \
				swap_32bit.c \
				check_hdr_overlap.c \
				read_byte_range_at_pos.c \
				check_load_commands.c \
				dump_symlist.c \
				read_file_content.c \
				goto_hdr_cpu_type.c

OTOOL_SRC_NAME =	ft_otool.c \
					ft_otool_process_file.c \
					is_magic_64.c \
					is_big_endian.c \
					is_magic_fat.c \
					swap_byte_range.c \
					slseek.c \
					sseek_read.c \
					init_header_info.c \
					goto_load_command.c \
					is_magic_mach.c \
					swap_32bit.c \
					check_hdr_overlap.c \
					read_byte_range_at_pos.c \
					check_load_commands.c \
					read_file_content.c \
					goto_hdr_cpu_type.c

HEADERS_NAME = ft_nm.h

SRC_DIR = ./srcs/
INCLUDES_DIR = ./includes/
OBJ_DIR = ./objs/

NM_OBJ = $(addprefix ${OBJ_DIR}, $(NM_SRC_NAME:.c=.o))
OTOOL_OBJ = $(addprefix ${OBJ_DIR}, $(OTOOL_SRC_NAME:.c=.o))

CC = gcc
CFLAGS = -Wall -Wextra # -Werror
DEV_FLAGS = -g -fsanitize=address -fno-omit-frame-pointer
IFLAGS = -I./ft_printf/includes -I./$(INCLUDES_DIR)
LFLAGS = -L./ft_printf -lftprintf

CFLAGS += $(DEV_FLAGS)
FT_PRINTF_DIR = ./ft_printf
LIBFTPRINTF = $(addprefix $(FT_PRINTF_DIR),"/libftprintf.a")

MAKEFILE_STATUS = $(addprefix $(addprefix $(FT_PRINTF_DIR),"/libft"),"/.makefile_status.sh")

UNAME_S := $(shell uname -s)

define ui_line
	$(MAKEFILE_STATUS) $(1) $(2) || true
endef

all : $(NAME_NM) $(NAME_OTOOL)

SET_NM_NAME :
	@CURRENT_NAME=$(NAME_OTOOL)

$(NAME_NM): $(LIBFTPRINTF) SET_NM_NAME $(NM_OBJ)
	@cp $(LIBFTPRINTF) ./libftprintf.a
ifeq ($(UNAME_S),Linux)
	@$(CC) $(CFLAGS) $(LFLAGS) $(NM_OBJ) $(LIBFTPRINTF) -o $(NAME_NM)
endif
ifeq ($(UNAME_S),Darwin)
	@$(CC) $(CFLAGS) $(LFLAGS) $(NM_OBJ) -o $(NAME_NM)
endif

SET_OTOOL_NAME :
	@CURRENT_NAME=$(NAME_OTOOL)

$(NAME_OTOOL) : $(LIBFTPRINTF) SET_OTOOL_NAME $(OTOOL_OBJ)
	@cp $(LIBFTPRINTF) ./libftprintf.a
ifeq ($(UNAME_S),Linux)
	@$(CC) $(CFLAGS) $(LFLAGS) $(OTOOL_OBJ) $(LIBFTPRINTF) -o $(NAME_OTOOL)
endif
ifeq ($(UNAME_S),Darwin)
	@$(CC) $(CFLAGS) $(LFLAGS) $(OTOOL_OBJ) -o $(NAME_OTOOL)
endif

$(OBJ_DIR)%.o : $(SRC_DIR)%.c $(INCLUDES_DIR)/$(HEADERS_NAME)
	@mkdir -p $(OBJ_DIR)
	@gcc $(CFLAGS) -c $(IFLAGS) $< -o $@ && $(call ui_line, $@, $(CURRENT_NAME))

$(FT_PRINTF_DIR):
	@git clone https://github.com/jjaniec/ft_printf 2> /dev/null || true

$(LIBFTPRINTF): $(FT_PRINTF_DIR)
	make -C ft_printf

clean:
	@rm -rf $(OBJ_DIR)
	@make clean -C ft_printf/

fclean: clean
	@make fclean -C ft_printf/
	@rm -f $(NAME_NM) $(NAME_OTOOL)

tests: all
	(cd tests && ./basic_tests.sh)

re: fclean all

.PHONY: clean fclean all re tests
