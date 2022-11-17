
#ifndef __CPU_H
#define __CPU_H




/**********************************************************
** Codes associés aux interruptions
***********************************************************/

enum {
    INT_NONE = 0,   	// pas d'interruption
    INT_SEGV,       	// violation mémoire
    INT_INST,       	// instruction inconnue
    INT_TRACE,    	// trace entre chaque instruction
    INT_SYSC,      	// appel au système
    INT_KEYBOARD,   	// événement clavier (simulé)
};


/**********************************************************
** Codes associés aux instructions
***********************************************************/

enum {
    INST_ADD,
    INST_CMP,
    INST_HALT,
    INST_IFGT,
    INST_JUMP,
    INST_LOAD,
    INST_NOP,
    INST_SET,
    INST_STORE,
    INST_SUB,
    INST_SYSC,
};


/**********************************************************
** définition d'un mot mémoire
***********************************************************/

typedef int WORD;         	/* un mot est un entier 32 bits  */


/* fonctions de lecture 	/ écriture */
WORD read_mem(int physical_address);
void write_mem(int physical_address, WORD value);


/**********************************************************
** Codage d'une instruction (32 bits)
***********************************************************/

typedef struct {
    unsigned OP: 10;  /* code operation (10 bits)  */
    unsigned i:   3;  /* nu 1er registre (3 bits)  */
    unsigned j:   3;  /* nu 2eme registre (3 bits) */
    short    ARG;     /* argument (16 bits)        */
} INST;


/**********************************************************
** Le mot d'état du processeur (PSW)
***********************************************************/

typedef struct PSW { /* Processor Status Word */
    WORD PC;         /* Program Counter */
    WORD SB;         /* Segment Base */
    WORD SS;         /* Segment Size */
    WORD IN;         /* Interrupt number */
    WORD DR[8];      /* Data Registers */
    WORD AC;         /* Accumulateur */
    INST RI;         /* Registre instruction */
} PSW;


/**********************************************************
** encoder une instruction
***********************************************************/

WORD encode_instruction(unsigned code, unsigned i, unsigned j, short arg);


/**********************************************************
** initialiser la machine (notamment la mémoire)
***********************************************************/

void init_cpu();


/**********************************************************
** afficher les registres de la CPU
***********************************************************/

void dump_cpu(PSW regs);


/**********************************************************
** exécuter un code en mode utilisateur
***********************************************************/

PSW cpu(PSW);


/**********************************************************
** récupérer le caractère arrivé depuis le clavier
***********************************************************/

char get_keyboard_data();


#endif
