# iot

# Commandes apprises
ln -s /usr/bin/gdb-multiarch /usr/bin/arm-none-eabi-gdb
but : lien symbolique entre deux binaires.



# STEP1
But : Voir les caractères affichés sur notre console virtuel émulé par QEMU.

Compréhension : 
chaque UART alloue un espace de la mémoire. Lui permettant notamment d'y placer différentes informations parmi-elle : 
- DR (data register) qui correspond à un buffer dans lequel nos données de lecture et écriture vont transiter. 
- FR (flag register) qui correspond à un tableau de bit, chaque bit nous donnera une information. Par exemple isEmpty ? 0 ou 1 selon son état.
Chaque machine se servant de l'UART va posséder son propre fifo.

Deduction :
en utilisant les flag, nous pouvons déduire que si le fifo du receiver est empty, qu'il n'y a pas de valeur, il n'est pas possible de lire des données. A l'inverse si l'envoyeur est full, il n'est pas possible d'écrire.

Code : en se basant sur les explications précédentes et sur la documentation ARM926EJ et PL011. J'ai pu débuter la rédaction de mon code. Il a fallut d'abord définir l'adresse de l'UART, et ensuite ajouter les offsets pour utiliser le DR et les FR.
Appliquer ce que j'ai déduit précédemment sur les méthodes uart_send et uart_receive.

# UART
correspond à récepteur/émetteur asynchrone universel et définit un protocole, ou un ensemble de règles, 
dédié à l'échange de données série entre deux appareils.


# kernel.ld 

## startup.s
## exception.s

# Makefile
addprexix : 2 paramètres, 
- folder (string)
- serie de nom séparé par des espaces (nom1 nomX)
Pour chaque : folder/nom1 

etapes :
1. configurer les variables
2. non configurable, les options QEMU
Versatile boards : 
3. 


[text](https://www.gnu.org/software/make/manual/make.html)







# QEMU
Démarre son execution après avoir charger le fichier ELF. 
QEMU fournit l'état 1 (state1).





# STEP2 


Interruption : 
Les trois à configurer
    Device : 
    PIC/VIC (Programmable Interrupt Controller) : sonne chez le CPU, il a une mémoire, il faut dire au PIC d'arreter la sonnerie
    CPU : s'il est endormi, réveil et appel la fonction demandé
    instruction halt : permet d'endormir le processeur

PIC definition : un chipset qui gère les interruptions matérielles provenant des périphériques et les envoie au processeur de manière ordonnée. Il permet au CPU de gérer plusieurs sources d'interruptions en multiplexant et en priorisant les requêtes.
Le PIC agit donc comme un gestionnaire centralisé, qui :

    - Reçoit les requêtes d’interruptions des périphériques.
    - Mémorise ces requêtes dans un registre d’état.
    - Priorise les interruptions selon leur importance.
    - Envoie l’interruption la plus prioritaire au CPU.
    - Attend un acquittement du CPU avant d’envoyer la suivante.


Fonctionnement d’un PIC

Un PIC classique (comme le 8259A, utilisé historiquement dans les PC) est configuré et contrôlé via des registres mémoire (MMIO) ou des ports d’E/S (PIO).

Structure d’un PIC

Un PIC dispose généralement :

    De lignes IRQ (Interrupt Request Lines) : chaque périphérique est connecté à une ligne IRQ différente.

    D’un registre d’état pour stocker quelles interruptions sont actives.

    D’un registre de masque permettant d’ignorer certaines interruptions.

    D’un registre de priorité pour gérer l’ordre d’exécution.

    D’un mécanisme d’acquittement pour informer le PIC que l’interruption a été traitée.

2. Configuration via MMIO

Comme tout contrôleur, le PIC est programmable via des registres mémoire (MMIO). On peut :

    Activer/Désactiver certaines interruptions en configurant un registre de masque.

    Définir les priorités en réarrangeant l’ordre des IRQ.

    Envoyer une commande d’acquittement pour signaler qu’une interruption a été traitée.
     

Si je suis en train de faire un calcul et que j'utilise un registre, si je décide d'éxécuter une interruption, ce registre sera modifié à mon retour
si je ne l'ai pas store. 




VIC : Vecteur d'exception en mémoire (CH2- diapo 15) 
LR : link register --> l'endroit où l'interruption s'éxecute.
SP : Stack register, sert à indiquer l'emplacement du dernier élément placé sur la pile
PC : compteur de programme, contient l'adresse de la prochaine instruction à exécuter
IR : registre d'instructions, contient l'instruction codée
^ : retour
! : valeur du registre SP, derniere valeur dans registre
IRQ : Interrupt request 
OFFSET : désigne une adresse de manière relative. C'est une valeur entière représentant le déplacement en mémoire nécessaire, par rapport à une adresse de référence, pour atteindre une autre adresse.





- soft 



CODE : 
uart-init(UART0)
for(;;)
    code = uart-receive(UART0)
    uart-send(uart0, code)