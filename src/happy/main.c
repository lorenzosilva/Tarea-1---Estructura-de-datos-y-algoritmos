#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* Retorna true si ambos strings son iguales */
static bool string_equals(char *string1, char *string2) {
    return !strcmp(string1, string2);
}

static bool check_arguments(int argc, char **argv) {
    if (argc != 3) {
        printf("Modo de uso: %s INPUT OUTPUT\n", argv[0]);
        printf("Donde:\n");
        printf("\tINPUT es la ruta del archivo de input\n");
        printf("\tOUTPUT es la ruta del archivo de output\n");
        exit(1);
    }
    return true;
}






#define MAXIMO 100000

typedef struct Penguin {
    int id;
    int felicidad;
} Penguin;


typedef struct Heap {
    Penguin* data[MAXIMO];
    int size;
    bool esmin;  /*si es un minheap o false si es un maxheap*/
} Heap;



typedef struct Sector {
    int capacidad;
    int cantidad_actual;
    Heap* max_heap; /*los menores a la mediana*/
    Heap* min_heap; /*los mayores a la mediana*/
    Penguin* mas_feliz;
    Penguin* mas_triste;
} Sector;





int mejor_sector = -1;
int peor_sector = -1;
int mejor_mediana = INT_MIN;
int peor_mediana = INT_MAX;





void actualizarMedianas(Sector* sectors[], int S);
void removeHeap(Heap* heap, int index);
void medians(Sector* sectors[], int S, FILE* output_file);
void happiness_buff(Sector* sector, int penguin_id, int increase, FILE* output_file);










Heap* crearHeap(bool esmin) {
    Heap* heap = (Heap*)malloc(sizeof(Heap));
    heap->size = 0;
    heap->esmin = esmin;

    return heap;
}





Sector* crearSector(int capacidad) {
    Sector* sector = (Sector*)malloc(sizeof(Sector));
    sector->capacidad = capacidad;
    sector->cantidad_actual = 0;
    sector->max_heap = crearHeap(false);
    sector->min_heap = crearHeap(true);
    sector->mas_feliz = NULL;
    sector->mas_triste = NULL;

    return sector;
}




void intercambo(Penguin** a, Penguin** b) {

    Penguin* temp = *a;
    *a = *b;
    *b = temp;

}





void heapInsert(Heap* heap, Penguin* penguin) {

    heap->data[heap->size]= penguin;
    int i = heap->size;
    heap->size++;


    if (heap->esmin) {
        while (i > 0 && (heap->data[i]->felicidad < heap->data[(i - 1) / 2]->felicidad || (heap->data[i]->felicidad == heap->data[(i - 1) / 2]->felicidad && heap->data[i]->id < heap->data[(i - 1) / 2]->id))) {

            intercambo(&heap->data[i], &heap->data[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    } 


    else {
        while (i > 0 && (heap->data[i]->felicidad > heap->data[(i - 1) / 2]->felicidad || (heap->data[i]->felicidad == heap->data[(i - 1) / 2]->felicidad && heap->data[i]->id < heap->data[(i - 1) / 2]->id))){

            intercambo(&heap->data[i], &heap->data[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    }
}




Penguin* rootRemove(Heap* heap) {

    if (heap->size == 0)return NULL;
    Penguin* root = heap->data[0];
    heap->data[0] = heap->data[--heap->size];


    int i = 0;
    while (i * 2 + 1 < heap->size) {

        int child = i * 2 + 1;
        if (child + 1 < heap->size && ((heap->esmin && (heap->data[child + 1]->felicidad < heap->data[child]->felicidad || (heap->data[child + 1]->felicidad == heap->data[child]->felicidad && heap->data[child + 1]->id < heap->data[child]->id))) || (!heap->esmin && (heap->data[child + 1]->felicidad > heap->data[child]->felicidad || (heap->data[child + 1]->felicidad == heap->data[child]->felicidad && heap->data[child + 1]->id < heap->data[child]->id))))) {

            child++;
        }



        if ((heap->esmin && (heap->data[i]->felicidad < heap->data[child]->felicidad || (heap->data[i]->felicidad == heap->data[child]->felicidad && heap->data[i]->id < heap->data[child]->id))) || (!heap->esmin && (heap->data[i]->felicidad > heap->data[child]->felicidad || (heap->data[i]->felicidad == heap->data[child]->felicidad && heap->data[i]->id < heap->data[child]->id)))) {
            break;
        }



        intercambo(&heap->data[i], &heap->data[child]);
        i = child;
    }

    return root;
}








void removeHeap(Heap* heap, int index) {

    if (index < 0 || index >= heap->size) return;
    heap->data[index] = heap->data[--heap->size];


    int i = index;
    while (i * 2 + 1 < heap->size) {


        int child = i * 2 + 1;
        if (child + 1 < heap->size && ((heap->esmin && (heap->data[child + 1]->felicidad < heap->data[child]->felicidad || (heap->data[child + 1]->felicidad == heap->data[child]->felicidad && heap->data[child + 1]->id < heap->data[child]->id))) || (!heap->esmin && (heap->data[child + 1]->felicidad > heap->data[child]->felicidad || (heap->data[child + 1]->felicidad == heap->data[child]->felicidad && heap->data[child + 1]->id < heap->data[child]->id))))) {

            child++;
        }




        if ((heap->esmin && (heap->data[i]->felicidad < heap->data[child]->felicidad || (heap->data[i]->felicidad == heap->data[child]->felicidad && heap->data[i]->id < heap->data[child]->id))) || (!heap->esmin && (heap->data[i]->felicidad > heap->data[child]->felicidad || (heap->data[i]->felicidad == heap->data[child]->felicidad && heap->data[i]->id < heap->data[child]->id)))) {

            break;
        }



        intercambo(&heap->data[i], &heap->data[child]);
        i = child;
    }
}







void balancearHeaps(Sector* sector) {
    if (sector->max_heap->size > sector->min_heap->size + 1) {

        heapInsert(sector->min_heap, rootRemove(sector->max_heap));
    } 
    
    else if (sector->min_heap->size>sector->max_heap->size) {

        heapInsert(sector->max_heap, rootRemove(sector->min_heap));
    }


}





int mediana(Sector* sector) {
    if (sector->cantidad_actual % 2 == 1) {

        return sector->max_heap->data[0] -> felicidad;
    } 
    
    else {
        int max_heap_root = sector->max_heap->data[0]->felicidad;
        int min_heap_root = sector->min_heap->data[0]->felicidad;

        return (max_heap_root + min_heap_root) / 2;

    }
}








void actualizarMedianas(Sector* sectors[], int S) {
    mejor_mediana = INT_MIN;
    peor_mediana = INT_MAX;
    mejor_sector = -1;
    peor_sector = -1;



    for (int i = 0; i < S; i++) {
        if (sectors[i]->cantidad_actual > 0) {

            int median = mediana(sectors[i]);
            if (median > mejor_mediana || (median == mejor_mediana && i < mejor_sector)) {
                mejor_mediana = median;
                mejor_sector = i;
            }


            if (median < peor_mediana || (median == peor_mediana && i > peor_sector)) {
                peor_mediana = median;
                peor_sector = i;

            }
        }
    }
}








Penguin* elegirMasTriste(Sector* sector) {
    Penguin* penguinMasTriste = NULL;

    if (sector->max_heap->size > 0) {
        penguinMasTriste = sector->max_heap->data[0];
    }


    if (sector->min_heap->size > 0 && (penguinMasTriste == NULL || sector->min_heap->data[0]->felicidad < penguinMasTriste->felicidad || (sector->min_heap->data[0]->felicidad == penguinMasTriste->felicidad && sector->min_heap->data[0]->id < penguinMasTriste->id))) {

        penguinMasTriste = sector->min_heap->data[0];
    }


    for (int i = 0; i < sector->max_heap->size; i++) {
        if (sector->max_heap->data[i]->felicidad < penguinMasTriste->felicidad || (sector->max_heap->data[i]->felicidad == penguinMasTriste->felicidad && sector->max_heap->data[i]->id < penguinMasTriste->id)) {

            penguinMasTriste = sector->max_heap->data[i];
        }
    }


    for (int i = 0; i < sector->min_heap->size; i++) {
        if (sector->min_heap->data[i]->felicidad < penguinMasTriste->felicidad || (sector->min_heap->data[i]->felicidad == penguinMasTriste->felicidad && sector->min_heap->data[i]->id < penguinMasTriste->id)) {

            penguinMasTriste = sector->min_heap->data[i];
        }
    }
    

    return penguinMasTriste;
}










Penguin* elegirMasFeliz(Sector* sector) {
    Penguin* penguinMasFeliz = NULL;


    if (sector->min_heap->size > 0) {

        penguinMasFeliz = sector->min_heap->data[0];
        for (int i = 1; i < sector->min_heap->size; i++) {
            if (sector->min_heap->data[i]->felicidad > penguinMasFeliz->felicidad || (sector->min_heap->data[i]->felicidad == penguinMasFeliz->felicidad && sector->min_heap->data[i]->id < penguinMasFeliz->id)) {

                penguinMasFeliz = sector->min_heap->data[i];
            }
        }
    }



    if (sector->max_heap->size > 0) {
        if (penguinMasFeliz == NULL || sector->max_heap->data[0]->felicidad > penguinMasFeliz->felicidad || (sector->max_heap->data[0]->felicidad == penguinMasFeliz->felicidad && sector->max_heap->data[0]->id < penguinMasFeliz->id)) {

            penguinMasFeliz = sector->max_heap->data[0];
        }
    }



    return penguinMasFeliz;
}





/*--------------------------------------------------EVENTOS-----------------------------------------------------------------------------------*/



void enter(Sector* sector, Penguin* penguin) {
    sector->cantidad_actual++;

    if (sector->max_heap->size == 0 || penguin->felicidad <= sector->max_heap->data[0]->felicidad) {
        heapInsert(sector->max_heap, penguin);
    } 
    
    else {
        heapInsert(sector->min_heap, penguin);
    }


    balancearHeaps(sector);

    sector->mas_triste=elegirMasTriste(sector);
    sector->mas_feliz=elegirMasFeliz(sector);
}







Penguin* leave(Sector* sector) {
    if (sector->cantidad_actual == 0)return NULL;

    Penguin* penguinSalido = sector->mas_triste;
    if (penguinSalido == NULL) return NULL;

    int index = -1;
    for (int i = 0; i < sector->max_heap->size; i++) {
        if (sector->max_heap->data[i] == penguinSalido) {

            index = i;
            break;
        }

    }



    if (index != -1) {
        removeHeap(sector->max_heap, index);
    } 
    
    
    else {

        for (int i = 0; i < sector->min_heap->size; i++) {
            if (sector->min_heap->data[i] == penguinSalido) {
                index = i;
                break;
            }
        }

        if (index != -1) {
            removeHeap(sector->min_heap, index);
        }
    }


    sector->cantidad_actual--;
    balancearHeaps(sector);

    if (sector->cantidad_actual == 0) {
        sector->mas_triste = NULL;
        sector->mas_feliz = NULL;
    } 
    
    else {
        sector->mas_triste = elegirMasTriste(sector);
        sector->mas_feliz = elegirMasFeliz(sector);
    }


    return penguinSalido;
}









void happiness_buff(Sector* sector, int penguin_id, int increase, FILE* output_file) {
    Penguin* penguin = NULL;
    int index = -1;


    for (int i = 0; i < sector->max_heap->size; i++) {

        if (sector->max_heap->data[i]->id == penguin_id) {
            penguin = sector->max_heap->data[i];
            index=i;
            break;
        }
    }


    if (penguin == NULL) {
        for (int i = 0; i < sector->min_heap->size; i++) {

            if (sector->min_heap->data[i]->id == penguin_id) {
                penguin = sector->min_heap->data[i];
                index = i;
                break;

            }
        }
    }



    if (penguin != NULL) {
        if (index != -1) {

            if (penguin->felicidad <= sector->max_heap->data[0]->felicidad) {
                removeHeap(sector->max_heap, index);
            } 
            
            else {
                removeHeap(sector->min_heap, index);
            }
        }


        penguin->felicidad += increase;


        if (penguin->felicidad <= sector->max_heap->data[0]->felicidad || sector->min_heap->size == 0) {
            heapInsert(sector->max_heap, penguin);
        } 
        
        else {
            heapInsert(sector->min_heap, penguin);
        }


        balancearHeaps(sector);


        sector->mas_feliz = elegirMasFeliz(sector);
        sector->mas_triste = elegirMasTriste(sector);

        fprintf(output_file, "Penguin %d happiness updated to %d\n", penguin_id, penguin->felicidad);
    }
}








void festival(FILE* output_file, Sector* sectors[], int S) {
    fprintf(output_file, "Festival Stats\n");

    for (int j = 0; j < S; j++) {

        if (sectors[j]->cantidad_actual > 0) {
            int median = mediana(sectors[j]);
            fprintf(output_file, "\tPlace %d has %d penguin[s]\n", j, sectors[j]->cantidad_actual);
            fprintf(output_file, "\t\t%d %d %d\n", median, sectors[j]->mas_feliz->id, sectors[j]->mas_triste->id);

        }
    }
}





void medians(Sector* sectors[], int S, FILE* output_file) {

    actualizarMedianas(sectors, S);
    fprintf(output_file, "Medians resume:\n");
    fprintf(output_file, "\t%d v/s %d\n", mejor_sector, peor_sector);
    fprintf(output_file, "\t%d - %d\n", mejor_mediana, peor_mediana);

}








<<<<<<< HEAD

=======
>>>>>>> master
/*----------------------------------------------------------------------------------------------------------------------------------------------------*/








/* Función principal */
int main(int argc, char **argv) {
    check_arguments(argc, argv);

    FILE *input_file = fopen(argv[1], "r");
    FILE *output_file = fopen(argv[2], "w");


    int S; // Cantidad de sectores
    int K; // Capacidad de un sector
    int E; // Cantidad de eventos


    int result;
    result = fscanf(input_file, "%d", &S);

    if (result != 1) {
        printf("Error reading tour center capacity");
        return 1;
    }

    Sector* sectors[S];
    for (int i = 0; i < S; i++) {
        result = fscanf(input_file, "%d", &K);
        if (result != 1) {
            printf("Error reading sector capacity");
            return 1;
        }
        sectors[i] = crearSector(K);
    }

    result = fscanf(input_file, "%d", &E);
    if (result != 1) {
        printf("Error reading number of events");
        return 1;
    }



    for (int i = 0; i < E; i++) {
        char command[32];
        result = fscanf(input_file, "%s", command);
        if (result != 1) {
            printf("Error reading event");
            return 1;
        }








        if (string_equals(command, "ENTER")) {
            int sector_id, penguin_id, felicidad;
            result = fscanf(input_file, "%d %d %d", &sector_id, &penguin_id, &felicidad);

            Penguin* new_penguin = (Penguin*)malloc(sizeof(Penguin));

            new_penguin->id = penguin_id;
            new_penguin->felicidad = felicidad;
            enter(sectors[sector_id], new_penguin);
        }

        else if (string_equals(command, "LEAVE")) {
            Penguin* penguinSalido = NULL;
            int leaving_sector = -1;
            int min_median = INT_MAX;


            for (int j = 0; j < S; j++) {
                if (sectors[j]->cantidad_actual > 0) {
                    int sector_median = mediana(sectors[j]);
                    if (sector_median < min_median) {
                        min_median = sector_median;
                        penguinSalido = sectors[j]->mas_triste;
                        leaving_sector = j;
                    }
                }
            }

            if (penguinSalido != NULL) {
                Penguin* saliendo = leave(sectors[leaving_sector]);

                fprintf(output_file, "Penguin %d leaving sector %d\n", saliendo->id, leaving_sector);
                free(saliendo);
            }
        }


        else if (string_equals(command, "FESTIVAL")) {
            festival(output_file, sectors, S);
        }

        else if (string_equals(command, "MEDIANS")) {
            medians(sectors, S, output_file);
        }

        else if (string_equals(command, "HAPPINESS_BUFF")) {
            int sector_id, penguin_id, increase;
            result = fscanf(input_file, "%d %d %d", &sector_id, &penguin_id, &increase);

            happiness_buff(sectors[sector_id], penguin_id, increase, output_file);
        }
    }



    fclose(input_file);
    fclose(output_file);

    for (int i = 0; i < S; i++) {

        for (int j = 0; j < sectors[i]->max_heap->size; j++) {
            free(sectors[i]->max_heap->data[j]);
        }


        for (int j = 0; j < sectors[i]->min_heap->size; j++) {
            free(sectors[i]->min_heap->data[j]);
        }


        free(sectors[i]->max_heap);
        free(sectors[i]->min_heap);
        free(sectors[i]);
    }

    return 0;

}
