# PROJET IOT

## Commandes utiles
```bash
ln -s /usr/bin/gdb-multiarch /usr/bin/arm-none-eabi-gdb
```
Cette commande crée un lien symbolique (comme un raccourci) nommé arm-none-eabi-gdb qui pointe vers gdb-multiarch.
Cela permet d’utiliser gdb sous le nom attendu par certains outils (notamment pour le débogage des projets ARM embarqués) sans avoir à modifier leur configuration.

## Commande make disponible
```bash
make build
```
```bash
make run
```
```bash
make debug
```

## Etape 1 : Affichage de caractères via QEMU

### Compréhension
 
Chaque UART alloue un espace mémoire. Lui permettant notamment d'y placer différentes informations, les choses utiles dans notre cas : 
- DR (data register) qui correspond à un buffer dans lequel nos données de lecture et écriture vont transiter. 
- FR (flag register) qui correspond à un tableau de bit, chaque bit nous donnera une information. Par exemple isEmpty ? 0 ou 1 selon son état.
Chaque machine se servant de l'UART va posséder son propre fifo.

### Déduction
en utilisant les flag, nous pouvons déduire : 
* si le fifo du receiver est empty, qu'il n'y a pas de valeur, il n'est pas possible de lire des données. 
* A l'inverse si l'envoyeur est full, il n'est pas possible d'écrire.

### Code
Code : en se basant sur les explications précédentes et sur la documentation ARM926EJ et PL011. 

Définir l'adresse de l'UART, et ajouter les offsets pour associé à DR et FR.
Puis réaliser les méthodes uart_receive et uart_send. Ces deux fonctions permettent d’envoyer et recevoir des caractères via une interface UART.

Pour send : 
* uartno : numéro de l’UART (0, 1, 2...) à utiliser.
* *pt : pointeur où sera stocké le caractère reçu.
* UART_FR_RXFE : bit qui indique si le FIFO de réception est vide.
* La fonction bloque tant qu’aucun caractère n’est disponible.
* Dès qu’un caractère arrive, il est lu depuis le Data Register (UART_DR) et stocké dans *pt.

Pour receive :
* s : caractère à envoyer.
* UART_FR_TXFF : bit qui indique si le FIFO d’envoi est plein.
* Tant que le FIFO est plein, la fonction attend (bloque).
* Ensuite, elle écrit le caractère dans le Data Register (UART_DR), qui sera ensuite envoyé par l’UART.


# Etape 2 : ajouter les interruptions 

## Compréhension des notions

* VIC : Vecteur d'exception en mémoire (CH2- diapo 15) 
* FIQ (Fast Interrupt Requests)
* LR : link register --> l'endroit où l'interruption s'éxecute.
* SP : Stack register, sert à indiquer l'emplacement du dernier élément placé sur la pile
* PC : compteur de programme, contient l'adresse de la prochaine instruction à exécuter
* IR : registre d'instructions, contient l'instruction codée
* ^ : retour
* ! : valeur du registre SP, derniere valeur dans registre
* IRQ : Interrupt request 
* OFFSET : désigne une adresse de manière relative. C'est une valeur entière représentant le déplacement en mémoire nécessaire, par rapport à une adresse de référence, pour atteindre une autre adresse.


## Interruption 
    
* PIC/VIC (Programmable Interrupt Controller) : sonne chez le CPU, il a une mémoire, il faut dire au PIC d'arreter la sonnerie. Il gère les interruptions matérielles provenant des périphériques et les envoie au processeur de manière ordonnée. Il permet au CPU de gérer plusieurs sources d'interruptions en multiplexant et en priorisant les requêtes.
Le PIC agit donc comme un gestionnaire centralisé, qui :

    - Reçoit les requêtes d’interruptions des périphériques.
    - Mémorise ces requêtes dans un registre d’état.
    - Priorise les interruptions selon leur importance.
    - Envoie l’interruption la plus prioritaire au CPU.
    - Attend un acquittement du CPU avant d’envoyer la suivante.

### Fonctionnement PIC
Il est configuré et contrôlé via des registres mémoire (MMIO) ou des ports d’E/S (PIO).

### Structure PIC

Composé de lignes IRQ (Interrupt Request Lines) : chaque périphérique est connecté à une ligne IRQ différente. Il a un registre d’état pour stocker quelles interruptions sont actives. Un registre de masque permettant d’ignorer certaines interruptions. Un registre de priorité pour gérer l’ordre d’exécution.
Et un mécanisme d’acquittement pour informer le PIC que l’interruption a été traitée.

### Configuration via MMIO

Comme tout contrôleur, le PIC est programmable via des registres mémoire (MMIO). On peut :
* Activer/Désactiver certaines interruptions en configurant un registre de masque.
* Définir les priorités en réarrangeant l’ordre des IRQ.
* Envoyer une commande d’acquittement pour signaler qu’une interruption a été traitée.
     

Si je suis en train de faire un calcul et que j'utilise un registre, si je décide d'éxécuter une interruption, ce registre sera modifié à mon retour
si je ne l'ai pas store. 


## isr.h
Lors de cette étape il a fallut déterminer le nombre maximal d'interruptions
gérées par le VIC. Cela me permettant d'éviter d’en définir plus que ce que le VIC peut en gérer. Egalement de savoir combien peuvent être gérées en même temps.
Ensuite il fallait rechercher l'interruption relié aux UART 0, 1 et 2. Ainsi que les interruptions associés aux timers.
Et calculer pour chacun d'entre eux les masques associées. 

## isr.c

* VICIRQSTATUS/VICFIQSTATUS : Donne l’état des interruptions IRQ actives après filtrage (masquage). Chaque bit correspond à une interruption (bit 0 = interruption 0, bit 1 = interruption 1, etc). Si un bit est à 1, ça veut dire que cette interruption est active et envoyée comme IRQ au processeur.

* VICRAWSTATUS : Donne l’état brut des interruptions (sans masque). Utile pour diagnostiquer si une interruption est levée, même si elle est désactivée par les registres de masque.

* VICINTSELECT : Sert à choisir si une interruption est de type IRQ (0) ou FIQ (1).
    - Chaque bit = 1 ➜ FIQ
    - Chaque bit = 0 ➜ IRQ

* VICINTENABLE : Permet d’activer une interruption.En écrivant un 1 dans un bit, cela actives l’interruption correspondante.

* VICINTCLEAR : Permet de désactiver une interruption, 1 dans un bit pour désactiver l’interruption correspondante.


## Debug & erreurs
```
# affiche l'hexa pour uart_enable dont les bits 0 (uarts_init),4,5 doivent être à 1
x /w 0x101F1038 
# affiche l'hexa pour vic_enable_irq dont le 12 eme bits doit être à 1
x /w 0x10140010 --> 
```

Commande debug à connaitre :
```
# Première console
make debug 
```
```
# Seconde console dans le folder "build"
arm-none-eabi-gdb kernel.elf
target rem:1234
```

# Etape 3 : Lock-Free Ring


Lock Free Ring
- cohérence mémoire qui ordonne les write


## SOURCES

[text](https://www.gnu.org/software/make/manual/make.html)