#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/mman.h>


void destornillador();

int nHijos = 3;
int num_hijo = 0; // Para identificar cada proceso: maestro 1, maestro2 , etc en ves del pid.
int contador = 0; // vuetas de tornillos por realizar
sem_t* mutex;

int main(void){

    mutex = mmap(NULL, sizeof(mutex), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(mutex,1,1);

    //Crearemos n - 1 procesos ya que el padre tambien trabajara.
    for(int i = 0; i < nHijos - 1; i++){
        int pid = fork();
        if(pid == 0){
            num_hijo = i + 1;
            while(contador <10)
                destornillador();
            printf("Maestro %d termino.\n",num_hijo);
            exit(0);
        }
    }

    // El padre igual sera maestro.
    num_hijo = nHijos;
    while(contador < 10)
        destornillador();
    printf("Maestro(PADRE) %d termino.\n",num_hijo);

    //Esperamos a los nHijos - 1(El padre tambien es maestro)
    for(int i=0; i < nHijos - 1; i++) {
        wait(NULL);
    }
    printf("Todos los maestros han terminado.\n");
}

void destornillador(){
    sem_wait(mutex);

    int random = rand() % (3 + 1 - 1) + 1; // Pensar 1 - 3 segundos
    printf("Maestro %d piensa por %d segundos.\n",num_hijo,random);
    sleep(random);

    random = rand() % (5 + 1 - 2) + 2; // atornillar 2 - 5
    for(int i = 0; i < random; i++){
        if(contador == 10)
            break;
        contador++;
        sleep(1);
        printf("Maestro %d lleva %d vueltas.\n",num_hijo,contador);
    }

    sem_post(mutex);
}
