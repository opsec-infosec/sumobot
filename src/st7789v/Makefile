NAME	=	libst7789vw.a

SRCS	=	./render.c ./st7789vw.c

OBJS	=	${SRCS:.c=.o}

INC		=	./
CC		=	gcc
CFLAGS	=	-Wall -Wextra -O3
LDFLAGS = 	-lbcm2835
DEBUG	=	-g
LIBC	=	ar -rcs
RANLIB	=	ranlib
RM		=	rm -f

${NAME}:	${OBJS}
			${CC} ${CFLAGS} ${LDFLAGS} -c ${SRCS}
			${LIBC} ${NAME} ${OBJS}

all:		${NAME}

install:
			${RANLIB} ${NAME}
			cp ./libst7789vw.a /usr/local/lib/
			cp ./*.h /usr/local/include/
			ldconfig
			${RM} ${NAME}
			${RM} ${OBJS}

uninstall:
			rm /usr/local/lib/libst7789vw.a
			rm /usr/local/include/render.h
			rm /usr/local/include/st7789vw.h

clean:
			${RM} ${OBJS}

fclean:		clean
			${RM} ${NAME}

re:			fclean all

.PHONY : all clean install uninstall re fclean
