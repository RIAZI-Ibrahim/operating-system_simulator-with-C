
#ifndef __SYSTEM_H
#define __SYSTEM_H

#define MAX_PROCESS (20) /* nb maximum de processus */
#define EMPTY       (0)  /* processus non-prêt*/
#define READY       (1)  /* processus prêt        */
#define SLEEP	    (2)  /* processus dort */
#define IDLE	    (3)  /* processus stagne */
#define GETCHAR     (4)  /* le processus attends un caractère */

/**********************************************************
** création des processus
***********************************************************/

struct{
	PSW cpu;
	int state;				// l'etat 
	time_t time;				//temps d'attente 
      }
process[MAX_PROCESS];

extern int current_process;

/**********************************************************
** enumération des appels système
***********************************************************/
enum{ SYSC_EXIT,
      SYSC_PUTI,
      SYSC_THREAD,
      SYSC_SLEEP,
      SYSC_GETCHAR
     };

/**********************************************************
** initialisation du système
***********************************************************/

PSW system_init(void);

/**********************************************************
** appel du système (traitement des interruptions)
***********************************************************/

PSW process_interrupt(PSW m);
#endif

