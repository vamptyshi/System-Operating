#include <iostream>
#include <thread> //permite crear y manejar hilos de ejecucion
#include <atomic> //variables atomicas, garantizan que las operaciones sobre ellas sean invisibles para otros hilos, evitando condiciones de carrera
#include <chrono> //funciones como sleep_for, pausa la ejecucion de un hilo por un tiempo determinado


/*El uso de atomic garantiza que los cambios en estas variables
sean visibles inmediatatamente para todos los hilo sy evitar que el compilador
optimice el buble while de forma incorrecta*/
std::atomic<bool> flag[2]; //los dos procesos pueden ver el valor de estas variables y modificarlas
std::atomic<int> turn(0); //turn indica a que proceso le toca entrar a la seccion critica, 0 o 1


void process_0(){
    for (int i=0; i<2; ++i){ 
        flag[0]=true; //indica que el proceso 0 quiere entrar a la seccion critica
        turn=1; //indica que el proceso 1 tiene prioridad para entrar a la seccion critica

        //SALA DE ESPERA
        while (flag[1]&&turn==1){ //Si el proceso 1 quiere entrar y es su turno, el proceso 0 espera
            std::this_thread::yield(); //Pone en pausa el hilo actual, permitiendo que otros hilos se ejecuten. Esto evita que el proceso 0 consuma CPU mientras espera.
        }
        //Critical Section
        std::cout<<"Process 0 is in CRITICAL SECTION"<<std::endl; 
        std::this_thread::sleep_for(std::chrono::seconds(1)); //Simula el tiempo que el proceso 0 pasa en la seccion critica
        std::cout<<"Process 0 is leaving CRITICAL SECTION"<<std::endl;
        flag[0]=false; //indica que ya no quiere entrar a la seccion critica
    
    }
}
void process_1(){
    for(int i=0; i <2; ++i){
        flag[1]=true;
        turn=0;
        while(flag[0]&&turn==0){
            std::this_thread::yield();
        }
        std::cout<<"Process 1 is in CRITICAL SECTION"<<std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout<<"Process 1 is leaving CRITICAL SECTION"<<std::endl;
        flag[1]=false;
    }
}
int main (){
    flag[0]=false; //indica que el proceso 0 no quiere entrar a la seccion critica
    flag[1]=false; //indica que el proceso 1 no quiere entrar a la seccion critica

    std:: thread th1(process_0); //creamos un hilo para ejecutar la funcion process_0
    std:: thread th2(process_1);

    th1.join(); //espera a que el hilo th1 termine su ejecucion antes de continuar con el hilo principal
    th2.join();
    std::cout<<"Finished Execution"<<std::endl;
    return 0;
}