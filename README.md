# KFS-2 : Kernel From Scratch

Deuxième volet de la série **Kernel From Scratch** de l'école 42, dans la continuité de [kfs-1](https://github.com/SamirNaggara/kfs-1). Le noyau du premier projet savait démarrer et écrire à l'écran ; celui-ci apprend à décrire sa propre mémoire au processeur.

## Ce que ce volet ajoute

- **GDT (Global Descriptor Table)** posée à l'adresse `0x00000800`, avec les segments de code et de données pour le noyau et pour l'espace utilisateur (anneaux 0 et 3).
- Chargement de la GDT en assembleur (`lgdt`) et rechargement des registres de segment.
- Un outil de dump de la mémoire de la GDT, pour vérifier visuellement que les descripteurs sont corrects.
- La base terminal de kfs-1 conservée : affichage VGA, défilement, curseur matériel, `printk`.

## Architecture

```text
src/
├── boot.s              amorçage multiboot
├── kernel.c            kmain : init GDT puis terminal
├── gdt/gdt.c           construction et chargement de la GDT
└── terminal/           VGA, print, pile, io
include/                les en-têtes correspondants
```

## Compiler et lancer

```bash
make        # produit l'ISO bootable
make run    # lance le noyau dans QEMU
```

Nécessite un cross-compiler i386 et `grub-mkrescue`.
