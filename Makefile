# --- NOMS ET OUTILS ---
NAME        = kernel.bin
ISO_NAME    = kfs.iso
CC          = gcc
ASM         = nasm
LD          = ld

# --- FLAGS DE COMPILATION ---
# -m32 : Force la compilation en 32 bits (architecture i386 obligatoire)
# Les autres flags sont ceux exigés par le sujet pour isoler le kernel du système hôte
CFLAGS      = -m32 -nostdlib -nodefaultlibs -fno-builtin -fno-stack-protector -Wall -Wextra -Werror
ASMFLAGS    = -f elf32
LDFLAGS     = -m elf_i386 -T linker.ld

# --- DOSSIERS ET FICHIERS ---
SRC_DIR     = src
OBJ_DIR     = obj
ISO_DIR     = iso

# Nos fichiers sources
SRC_C       = $(SRC_DIR)/kernel.c \
              $(SRC_DIR)/terminal/vga.c \
              $(SRC_DIR)/terminal/terminal.c \
              $(SRC_DIR)/terminal/print.c \
              $(SRC_DIR)/terminal/stack.c \
              $(SRC_DIR)/gdt/gdt.c
SRC_ASM     = $(SRC_DIR)/boot.s

# Nos fichiers objets (les briques compilées avant le linking)
OBJ_C       = $(SRC_C:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OBJ_ASM     = $(OBJ_DIR)/boot.o

# --- RÈGLES ---

all: $(ISO_NAME)

# Règle pour créer le dossier obj s'il n'existe pas
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Compilation de l'assembleur
$(OBJ_ASM): $(SRC_ASM) | $(OBJ_DIR)
	$(ASM) $(ASMFLAGS) $< -o $@

# Compilation du C
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Le Linking : on assemble le .o de l'ASM et le .o du C grâce à notre linker.ld
$(NAME): $(OBJ_ASM) $(OBJ_C)
	$(LD) $(LDFLAGS) $(OBJ_ASM) $(OBJ_C) -o $(NAME)

# Création de l'ISO bootable avec GRUB
$(ISO_NAME): $(NAME)
	# On place le kernel compilé dans le dossier de l'ISO
	cp $(NAME) $(ISO_DIR)/boot/$(NAME)
	# On force GRUB à n'utiliser que les modules i386 basiques
	grub-mkrescue -d /usr/lib/grub/i386-pc -o $(ISO_NAME) $(ISO_DIR)

# Règle magique pour tester directement avec l'émulateur QEMU
run: $(ISO_NAME)
	qemu-system-i386 -cdrom $(ISO_NAME) -monitor stdio

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME) $(ISO_NAME)
	rm -f $(ISO_DIR)/boot/$(NAME)

re: fclean all

.PHONY: all run clean fclean re