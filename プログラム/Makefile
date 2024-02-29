OBJS1	=	server_main.o server_net.o
OBJS2	=	client_main.o client_net.o 
TARGET1	=	server
TARGET2	=	client
CFLAGS	=	-c -DNDEBUG

.c.o:
	gcc $(CFLAGS) $<

all: $(TARGET1) $(TARGET2)

$(TARGET1):	$(OBJS1)
	gcc -o $(TARGET1) $(OBJS1) -lm -lSDL2
$(TARGET2):	$(OBJS2)
	gcc -o $(TARGET2) $(OBJS2) -lm -lSDL2 -lSDL2_image -lSDL2_gfx -lSDL2_mixer $(LDFLAGS)

clean:
	rm  *.o $(TARGET1) $(TARGET2)
	find . -type f | xargs touch
	rm -rf $(OBJS)