# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dfurneau <dfurneau@student.abudhabi.ae>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/02/08 15:58:46 by dfurneau          #+#    #+#              #
#    Updated: 2022/03/21 23:12:30 by dfurneau         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		=	sumo.out

SSRCS		=	./sumo.c ./sumoInit.c ./motors.c ./navigation.c ./sensors.c \
				./sequence.c ./piPinSetup.c ./display.c ./watchdog.c \
				./gyro.c

SOBJS		=	${SSRCS:.c=.o}

CC			=	gcc
CFLAGS		=	-Wall -Wextra -O3 -std=c99 ${DEBUG}
LIBS		=	-lrt -pthread -lpigpio -lst7789vw -lbcm2835 -lbno055 -lm
INC			= 	-I ./includes
DEBUG		=	-g3
RM			=	rm -f


all:		${NAME}

${NAME}:	${SOBJS}
			${CC} ${CFLAGS} ${INC} ${SOBJS} ${LIBS} -o ${NAME}

lib:
			${MAKE} all  -C ./pigpio
			cd ./bcm2835-1.71 && ./configure && cd ..
			${MAKE} all -C ./bcm2835-1.71
			${MAKE} all -C ./st7789v
			${MAKE} all -c ./bno055

libinstall:
			${MAKE} install -C ./pigpio
			${MAKE} install -C ./bcm2835-1.71
			${MAKE} install -C ./st7789v
			${MAKE} install -C ./bno055

uninstall:	clean
			${MAKE} uninstall -C ./pigpio
			${MAKE} uninstall -C ./bcm2835-1.71
			${MAKE} uninstall -C ./st7789v
			${MAKE} uninstall -C ./bno055
clean:
			${RM} ${NAME}
			${RM} ${SOBJS}

fclean:		clean
			${MAKE} clean -C ./pigpio
			${MAKE} clean -C ./bcm2835-1.71
			${MAKE} fclean -C ./st7789v
			${MAKE} fclean -C ./bno055

re:			clean all

.PHONY : all lib libinstall clean re fclean
