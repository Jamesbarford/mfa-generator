OBJ_DIR	= ./build
SRC = .
TARGET = generate_mfa
CFLAGS = -Wall -Werror -Wextra -Wpedantic -Wno-shadow -O2
CC = cc

$(OBJ_DIR)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(OBJ_DIR)/%.o: ./includes/%.c
	$(CC) -c $(CFLAGS) -o $@ $<

all: $(TARGET)

clean:
	rm $(OBJ_DIR)/*.o
	rm $(TARGET)

OBJLIST = $(OBJ_DIR)/main.o \
          $(OBJ_DIR)/readinfile.o \
          $(OBJ_DIR)/base64tohex.o \
          $(OBJ_DIR)/bytestobase64.o \
          $(OBJ_DIR)/decrypt.o \
          $(OBJ_DIR)/encrypt.o

$(TARGET): $(OBJLIST)
	$(CC) -o $(TARGET) $(OBJLIST) -loath -lssl -lcrypto

$(OBJ_DIR)/main.o: $(SRC)/main.c ./decrypt.h ./includes/base64tohex.h ./includes/bytestobase64.h
$(OBJ_DIR)/base64tohex.o: ./includes/base64tohex.c ./includes/base64tohex.h ./includes/readinfile.h
$(OBJ_DIR)/readinfile.o: ./includes/readinfile.c ./includes/readinfile.h
$(OBJ_DIR)/bytestobase64.o: ./includes/bytestobase64.c ./includes/bytestobase64.h
$(OBJ_DIR)/decrypt.o: ./decrypt.c ./decrypt.h
$(OBJ_DIR)/encrypt.o: ./encrypt.c ./encrypt.h
