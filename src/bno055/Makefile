NAME	=	libbno055.a

SRCS	=	./bno055.c

OBJS	=	${SRCS:.c=.o}

INC		=	./
CC		=	gcc
CFLAGS	=	-Wall -Wextra -O3 -g3
LDFLAGS = 	-lrt -pthread
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
			cp ./libbno055.a /usr/local/lib/
			cp ./*.h /usr/local/include/
			ldconfig
			${RM} ${NAME}
			${RM} ${OBJS}

uninstall:
			rm /usr/local/lib/libbno055.a
			rm /usr/local/include/bno055.h

clean:
			${RM} ${OBJS}

fclean:		clean
			${RM} ${NAME}

re:			fclean all

.PHONY : all clean install uninstall re fclean
