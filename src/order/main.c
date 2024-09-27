#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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





typedef struct {
    int id;
    int age;
    int membership;
    int orden_entrada;
} Penguin;

typedef struct {
    Penguin *penguins;
    int cantidad;
    int capacidad;
} Cola;

typedef struct {
    char* tipo1;
    char* orden1;
    char* tipo2;
    char* orden2;
} Atributos;


Cola* create_cola(int capacidad) {
    Cola* cola = (Cola*)malloc(sizeof(Cola));
    cola->penguins = (Penguin*)malloc(sizeof(Penguin) * capacidad);
    cola->cantidad = 0;
    cola->capacidad =capacidad;
    return cola;
}

void borrar_cola(Cola* cola) {
    free(cola->penguins);
    free(cola);

}




static int orden_entrada_actual = 0; 

void enter(Cola* cola, int id, int age, int membership) {
    if (cola->cantidad == cola->capacidad) {
        cola->capacidad *= 2;
        cola->penguins = (Penguin*)realloc(cola->penguins, sizeof(Penguin) * cola->capacidad);
    }

    Penguin nuevoPinguino = {id, age, membership, orden_entrada_actual++};
    cola->penguins[cola->cantidad++] = nuevoPinguino;

}




void leave(Cola* cola, int id) {
    for (int i = 0; i < cola->cantidad; i++) {
        if (cola->penguins[i].id == id) {
            for (int j = i; j < cola->cantidad - 1; j++) {
                cola->penguins[j] = cola->penguins[j + 1];
            }
            cola->cantidad--;
            break;

        }
    }
}




int ordenar_age_asc(const void* a, const void* b) {
    Penguin* penguin1 = (Penguin*)a;
    Penguin* penguin2 = (Penguin*)b;

    if (penguin1->age == penguin2->age) {
        return penguin1->orden_entrada - penguin2->orden_entrada; 
    }
    
    return penguin1->age - penguin2->age;
}

int ordenar_age_des(const void* a, const void* b) {
    Penguin* penguin1 = (Penguin*)a;
    Penguin* penguin2 = (Penguin*)b;

    if (penguin1->age == penguin2->age) {
        return penguin1->orden_entrada - penguin2->orden_entrada;
    }
    return penguin2->age - penguin1->age;
}

int ordenar_membership_asc(const void* a, const void* b) {
    Penguin* penguin1 = (Penguin*)a;
    Penguin* penguin2 = (Penguin*)b;

    if (penguin1->membership == penguin2->membership) {
        return penguin1->orden_entrada - penguin2->orden_entrada;
    }

    return penguin1->membership - penguin2->membership;
}

int ordenar_membership_des(const void* a, const void* b) {
    Penguin* penguin1 = (Penguin*)a;
    Penguin* penguin2 = (Penguin*)b;

    if (penguin1->membership == penguin2->membership) {
        return penguin1->orden_entrada - penguin2->orden_entrada; 
    }

    return penguin2->membership - penguin1->membership;
}



void sort_by(Cola* cola, char* type, char* order) {
    if (string_equals(type, "age")) {
        if (string_equals(order, "asc")) {
        qsort(cola->penguins, cola->cantidad, sizeof(Penguin), ordenar_age_asc);
        } 
        else {
        qsort(cola->penguins, cola->cantidad, sizeof(Penguin), ordenar_age_des);
        }
    } 
    
    else if (string_equals(type, "membership")) {
        if (string_equals(order, "asc")) {
        qsort(cola->penguins, cola->cantidad, sizeof(Penguin), ordenar_membership_asc);
        } 
        else {
        qsort(cola->penguins, cola->cantidad, sizeof(Penguin), ordenar_membership_des);
        }
    }
}









Atributos atributos_globales;
int comparacion_doble(const void* a, const void* b) {
    Penguin* penguin1 = (Penguin*)a;
    Penguin* penguin2 = (Penguin*)b;

    int resultado1 = 0;
    int resultado2 = 0;

    if (string_equals(atributos_globales.tipo1, "age")) {
        resultado1 = penguin1->age - penguin2->age;
    } 

    else if (string_equals(atributos_globales.tipo1, "membership")) {
        resultado1 = penguin1->membership - penguin2->membership;
    }
    if (string_equals(atributos_globales.orden1, "desc")) {
        resultado1 = -resultado1;
    }



    if (resultado1 == 0) {
        if (string_equals(atributos_globales.tipo2, "age")) {
            resultado2 = penguin1->age - penguin2->age;
        } 

        else if (string_equals(atributos_globales.tipo2, "membership")) {
            resultado2 = penguin1->membership - penguin2->membership;
        }
        if (string_equals(atributos_globales.orden2, "desc")) {
            resultado2 = -resultado2;
        }


        if (resultado2 == 0) {
            return penguin1->orden_entrada - penguin2->orden_entrada;
        }
        return resultado2;
    }


    return resultado1;
}




void double_sort_by(Cola* cola, char* tipo1, char* orden1, char* tipo2, char* orden2) {

    atributos_globales.tipo1 = tipo1;
    atributos_globales.orden1 = orden1;
    atributos_globales.tipo2 = tipo2;
    atributos_globales.orden2 = orden2;


    qsort(cola->penguins, cola->cantidad, sizeof(Penguin), comparacion_doble);
}










/*---------------------------------------------------------------------------------------------------------------------------------------*/













int main(int argc, char **argv) {
    check_arguments(argc, argv);

    FILE *input_file = fopen(argv[1], "r");
    FILE *output_file = fopen(argv[2], "w");

    int E; // Cantidad de eventos
    
    int result;
    result = fscanf(input_file, "%d", &E);
    if (result != 1) {
        printf("Error reading events");
        return 1;
    }

    Cola* cola = create_cola(10);

    char command[32];
    for (int i = 0; i < E; i++) {
        result = fscanf(input_file, "%s", command);
        if (result != 1) {
            printf("Error reading events");
            return 1;
        }

        if (string_equals(command, "ENTER")) {
            int id, age, membership;
            fscanf(input_file, "%d %d %d", &id, &age, &membership);
            fprintf(output_file, "Penguin %d with age %d has entered\n", id, age);
            enter(cola, id, age, membership);

        } 
        else if (string_equals(command, "LEAVE")) {
            int id;
            fscanf(input_file, "%d", &id);
            fprintf(output_file, "Penguin %d has left\n", id);
            leave(cola, id);

        } 
        else if (string_equals(command, "SORT-BY")) {
            char type[16], order[16];
            fscanf(input_file, "%s %s", type, order);
            sort_by(cola, type, order);
            fprintf(output_file, "Penguins sorted by %s in %s order:\n", type, order);
            for (int j = 0; j < cola->cantidad; j++) {
                if (j != 0) fprintf(output_file, ", ");
                fprintf(output_file, "%d", cola->penguins[j].id);
            }
            fprintf(output_file, "\n");

        } 
        else if (string_equals(command, "DOUBLE-SORT-BY")) {
            char tipo1[16], orden1[16], tipo2[16], orden2[16];
            fscanf(input_file, "%s %s %s %s", tipo1, orden1, tipo2, orden2);

            double_sort_by(cola, tipo1, orden1, tipo2, orden2);
            fprintf(output_file, "Penguins sorted by %s, %s in %s, %s order:\n", tipo1, tipo2, orden1, orden2);
            for (int j = 0; j < cola->cantidad; j++) {
                if (j != 0) fprintf(output_file, ", ");
                fprintf(output_file, "%d", cola->penguins[j].id);
            }
            fprintf(output_file, "\n");
        }


    }
/* Cerramos los archivos */
    fclose(input_file);
    fclose(output_file);


/* Liberamos memoria */
    borrar_cola(cola);

    return 0;
}
