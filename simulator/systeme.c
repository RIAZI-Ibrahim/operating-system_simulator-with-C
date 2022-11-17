#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "asm.h"
#include "cpu.h"
#include "systeme.h"

int current_process = 0;

/**********************************************************
** Démarrage du système (création d'un programme)
***********************************************************/

PSW system_init(void)
{
    PSW cpu;
    printf("Booting\n");


/************************************************************
 **création d'un programme 
**************************************************************/

    /*begin(20);
        set(R2, 1000);
        label(10);
            add(R1, R2, 1000);
            nop();
            jump(10);
    end();*/

/************************************************************
 **création d'un programme  2
**************************************************************/

 /*begin(20);
       set(R1, 0);
       set(R2, 1000);
       set(R3, 5);
       label(10);
       	cmp(R1, R2);
        if_gt(20);
        nop();
        nop();
        add(R1,R3,0);
        //load(R4,R1,0);									//detecter un debordement de memoire 
        jump(10); 
       label(20); 
        sysc(R1,R2,SYSC_PUTI);
        sysc(R1,R2,SYSC_EXIT);
   end();*/

/************************************************************
 **exemple de creation de thread
**************************************************************/

    // begin(20);
    //  set(R5, 99); // R5 = 50

    /*** créer un thread ***/
    //  sysc(R1, R1, SYSC_THREAD); 							/* cr´eer un thread */
    //  if_gt(10);                 							/* le p`ere va en 10 */
    //  set(R4, 13);               							// R4 = 13
    //  store(R4, R5, 0); 								// on store la valeur de R4 dans mem[R5] ( case memoire R5) 

    /*** code du fils ***/
    // 	set(R3, 1000); 									/* R3 = 1000 */
    // 	sysc(R4, 0, SYSC_PUTI); 								/* afficher R4 */
    // 	nop();
    // 	nop();
    // 	sysc(0, 0, SYSC_EXIT);

    /*** code du p`ere ***/
    // label(10); 									/* set label 10 */
    //  nop();
    //  nop(); 										// temporiser
    //  nop();
    //  nop();
    //  load(R4, R5, 0); 								// charger la valeur de la case mémoire R5 dans le
    //  registre R4
    //  set(R3, 2000); 									/* R3 = 1000 */
    //  sysc(R4, 0, SYSC_PUTI); 								/* afficher R3 */
    //  sysc(0, 0, SYSC_EXIT);  								/* fin du thread */
    //end();
  
  
/************************************************************
 **test pour endormir le processus
**************************************************************/

    begin(0);
        label(10);
           set(R3, 1); 									/* R3 = 1 */
           sysc(R3, 0, SYSC_SLEEP); 							/* endormir le processus (4 secondes) */
           sysc(R3, 0, SYSC_GETCHAR); 							/* obtenir un caractère */
           sysc(R3, 0, SYSC_PUTI); 							/* afficher la valeur R3 */
           jump(10);
           //sysc(0, 0, SYSC_EXIT);
    end();
       


    /*** valeur initiale du PSW ***/
    
    memset(&cpu, 0, sizeof(cpu));
    cpu.PC = 0;
    cpu.SB = 0;
    cpu.SS = 100;


    /* Creation de processus*/
    process[0].state = IDLE;  								// intialiser l'etat du processus 0 en                         								// attente
    process[0].cpu   = cpu;   								// initialiser la CPU du processus 0
    process[1].state = READY; 								// intialiser l'etat du processus 1 a prêt
    process[1].cpu   = cpu;   								// initialiser la CPU du processus 1
    /********/
    

     return cpu;
}

/**********************************************************
** Fonction de création de thread fils
***********************************************************/

void set_return_value(PSW* cpu, int value)
{
    cpu->AC = value; 									// mettre le registre accumulateur à la valeur souhaité
    cpu->DR[cpu->RI.i] = value; 							// mettre le registre RI à la valeur souhaité                               				 
}


/******/
int new_thread(void)
{
    int new_process = -1;
    for (int i = 0; i < MAX_PROCESS; i++)
    {
        if (process[i].state == EMPTY)
        {
            new_process = i;
            break;
        }
    }
    if (new_process == -1)
    {
        printf("il n'y a plus de processus disponible \n");
        exit(4);
    }
    process[current_process].cpu.PC++; 							// pour pas que le nouveau thread
                                       							// se duplique à l'infinie
    process[new_process].cpu   = process[current_process].cpu;
    process[new_process].state = READY;
    return new_process;
}



/**********************************************************
** Traitement de l'ordonnanceur (mode système)
***********************************************************/

/* réveiller les processus en attendre de caractères */

void wakeup_getchar(int p, char c)
{
    /* remettre le processus en état d'éveil */
    process[p].state = READY;
    
    /* mettre comme valeur de retour le caractère demandé
     * dans le registre accumulateur et RI */
    process[p].cpu.AC                      = c;
    process[p].cpu.DR[process[p].cpu.RI.i] = c;
}

void keyboard_event(void)
{
    char caractere_du_clavier = get_keyboard_data();

    /* si il n'y a aucun caractère tappé par le clavier, on sort de la fonction keyboard_event */
     
    if (caractere_du_clavier == '\0')
    {
        return;
    }

    for (int i = 0; i < MAX_PROCESS; i++)
    {
        if (process[i].state == GETCHAR)
        {
            wakeup_getchar(i, caractere_du_clavier);
        }
    }
}


/* réveille les processus endormi */

void wakeup(void)
{
    /* prendre le temps courant */
    time_t current_time = time(NULL);

    for (int i = 0; i < MAX_PROCESS; i++)
    {
        /* faut-t-il réveiller le processus ? */
        if (process[i].state == SLEEP && process[i].time <= current_time)
        {
            /* on le met à l'état prêt; */
            process[i].state = READY;
        }
    }
}

PSW scheduler(PSW m)
{
    wakeup();

    /* sauvgarder l'etat de processus */
    process[current_process].cpu = m;
    /************************/

    int chosen_process_id = -1; 										// le processus choisi dans la boucle

     /*sauvegarder le processus courant si il existe*/ 

    for (int i = 0; i < MAX_PROCESS; i++)
    {
        int next_process = (current_process + 1 + i) % MAX_PROCESS;

         /* si un processus dans le tableau des processus est prêt, alors prendre ce processus*/ 

        if (process[next_process].state == READY)
        {
            chosen_process_id = next_process;
            break;
        }
    }


     /* si aucun processus n'est prêt alors on prends le procesus à l'état idle */
     
    if (chosen_process_id == -1)
    {
        current_process = 0;
    }
    else
    {
        /* sinon on affecte le processus courant au processus suivant
         * qui est prêt
         */

        current_process = chosen_process_id;
    }

    /* relancer ce processus */
    return process[current_process].cpu;
}

/**********************************************************
** Traitement des interruptions par le système (mode système)
***********************************************************/
PSW process_interrupt(PSW m)
{
    /*
    printf("le processus %i execute les interuptions \n",current_process); 					// afficher quel processus s'execute     											
    // dump_cpu(m);
    printf("le numero d'interruption reçu %d \n",
           m.IN); 												// afficher le numero de process
    */

    switch (m.IN)
    {
        case INT_SEGV:
            dump_cpu(m);
            exit(3); 												// quitter le programme lors d'un débordement
            break;

        case INT_TRACE:
            dump_cpu(m); 											// afficher les registres de la CPU
            break;

        case INT_INST:
            dump_cpu(m);
            exit(1); 												// provoque l'arret du cpu
            

        case INT_SYSC:
            if (m.RI.ARG == SYSC_EXIT)
            {
                printf("Quitter le processus %i \n", current_process);
                process[current_process].state = EMPTY; 							// le processus se quitte ( est à vide )                                    							
                int quitter = 1;
                for (int i = 0; i < MAX_PROCESS; i++)
                {
                    if (process[current_process].state != EMPTY)
                    { 												// verifier que tout les processus sont vides
                        quitter = 0;
                        break;
                    }
                }
                if (quitter == 1)
                {
                    printf("il y'a plus aucun processus \n");
                    exit(2);
                } 												// quitter le processus
            }
            else if (m.RI.ARG == SYSC_PUTI)
            {
                printf("affiche RI: premier regitre %i %c \n",
                       m.DR[m.RI.i], m.DR[m.RI.i]); 								// afficher le premier registre
            }
            else if (m.RI.ARG == SYSC_THREAD)
            {
                int thread = new_thread();
                set_return_value(&m, 0);
                set_return_value(&process[thread].cpu, thread);
            }
            else if (m.RI.ARG == SYSC_SLEEP)
            {
                printf("dormir le processus %i pour %i secondes\n",current_process, m.DR[m.RI.i]);
                process[current_process].state = SLEEP;
                time_t current_time            = time(NULL);

                /* on prends le temps courant + le temps qu'on veut dormir
                 */

                process[current_process].time = current_time
                                                + m.DR[m.RI.i];
            }
            else if (m.RI.ARG == SYSC_GETCHAR)
            {
                printf("le processus %i veut un caractère\n",
                       current_process);
                process[current_process].state = GETCHAR;
            }
            break;

        case INT_KEYBOARD:
            keyboard_event();
            break;

        default:
            break;
    }
    m = scheduler(m); 												// remmetre l'etat du processus suivant
    return m;
}

