
Programacion 1 - Optimizacion de produccion de una fabrica de componentes electronicos
  // Alexis Escobar
 
#include <stdio.h>

#define MAX_PRODUCTOS 5
#define LONG_NOMBRE   30

/* ---------- Funciones de texto (sin string.h) ---------- */

void copiarTexto(char *destino, char *origen) {
    int i = 0;
    while (origen[i] != '\0') {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

/* Devuelve 1 si los textos son iguales (sin distinguir mayusculas), 0 si no */
int textosIguales(char *a, char *b) {
    int i = 0;
    char x, y;
    while (a[i] != '\0' && b[i] != '\0') {
        x = a[i];
        y = b[i];
        if (x >= 'A' && x <= 'Z') x = x + 32;
        if (y >= 'A' && y <= 'Z') y = y + 32;
        if (x != y) return 0;
        i++;
    }
    return a[i] == b[i];
}

/* ---------- Lectura de datos ---------- */

/* Lee un numero y lo vuelve a pedir si es invalido o menor que el minimo */
double leerNumero(char *mensaje, double minimo) {
    double x;
    printf("%s", mensaje);
    while (scanf("%lf", &x) != 1 || x < minimo) {
        while (getchar() != '\n');          /* limpia lo que se escribio mal */
        printf("  Valor invalido. Intente de nuevo: ");
    }
    return x;
}

void leerNombre(char *mensaje, char *nombre) {
    printf("%s", mensaje);
    scanf(" %29[^\n]", nombre);             /* permite nombres con espacios */
}

/* ---------- Busqueda lineal ---------- */

/* Devuelve la posicion del producto, o -1 si no existe */
int buscarProducto(char nombres[][LONG_NOMBRE], int n, char *buscado) {
    int i;
    for (i = 0; i < n; i++) {
        if (textosIguales(nombres[i], buscado)) {
            return i;
        }
    }
    return -1;
}

/* ---------- Opciones del menu ---------- */

void ingresarProducto(char nombres[][LONG_NOMBRE], double *tiempos,
                      double *recursos, int *demandas, int *n) {
    char nombre[LONG_NOMBRE];
    if (*n == MAX_PRODUCTOS) {
        printf("\n  Ya se registraron los %d productos permitidos.\n", MAX_PRODUCTOS);
        return;
    }
    printf("\n--- Ingreso del producto %d de %d ---\n", *n + 1, MAX_PRODUCTOS);
    leerNombre("Nombre del producto: ", nombre);
    if (buscarProducto(nombres, *n, nombre) != -1) {
        printf("  Ya existe un producto con ese nombre.\n");
        return;
    }
    copiarTexto(nombres[*n], nombre);
    tiempos[*n]  = leerNumero("Tiempo de fabricacion por unidad (horas): ", 0.01);
    recursos[*n] = leerNumero("Recursos requeridos por unidad: ", 0.01);
    demandas[*n] = (int) leerNumero("Cantidad demandada (unidades): ", 0);
    *n = *n + 1;
    printf("  Producto registrado correctamente.\n");
}

void listarProductos(char nombres[][LONG_NOMBRE], double *tiempos,
                     double *recursos, int *demandas, int n) {
    int i;
    if (n == 0) {
        printf("\n  No hay productos registrados.\n");
        return;
    }
    printf("\n  %-3s %-30s %12s %14s %10s\n", "N.", "Producto",
           "Horas/unid.", "Recursos/unid.", "Demanda");
    for (i = 0; i < n; i++) {
        printf("  %-3d %-30s %12.2f %14.2f %10d\n", i + 1, nombres[i],
               tiempos[i], recursos[i], demandas[i]);
    }
}

void editarProducto(char nombres[][LONG_NOMBRE], double *tiempos,
                    double *recursos, int *demandas, int n) {
    char buscado[LONG_NOMBRE];
    int pos;
    leerNombre("\nNombre del producto a editar: ", buscado);
    pos = buscarProducto(nombres, n, buscado);
    if (pos == -1) {
        printf("  Producto no encontrado.\n");
        return;
    }
    printf("  Editando: %s\n", nombres[pos]);
    tiempos[pos]  = leerNumero("Nuevo tiempo por unidad (horas): ", 0.01);
    recursos[pos] = leerNumero("Nuevos recursos por unidad: ", 0.01);
    demandas[pos] = (int) leerNumero("Nueva demanda (unidades): ", 0);
    printf("  Producto actualizado.\n");
}

void eliminarProducto(char nombres[][LONG_NOMBRE], double *tiempos,
                      double *recursos, int *demandas, int *n) {
    char buscado[LONG_NOMBRE], resp;
    int pos, i;
    leerNombre("\nNombre del producto a eliminar: ", buscado);
    pos = buscarProducto(nombres, *n, buscado);
    if (pos == -1) {
        printf("  Producto no encontrado.\n");
        return;
    }
    printf("  Seguro que desea eliminar '%s'? (s/n): ", nombres[pos]);
    scanf(" %c", &resp);
    if (resp != 's' && resp != 'S') {
        printf("  Eliminacion cancelada.\n");
        return;
    }
    /* Se mueve cada elemento una posicion hacia atras */
    for (i = pos; i < *n - 1; i++) {
        copiarTexto(nombres[i], nombres[i + 1]);
        tiempos[i]  = tiempos[i + 1];
        recursos[i] = recursos[i + 1];
        demandas[i] = demandas[i + 1];
    }
    *n = *n - 1;
    printf("  Producto eliminado.\n");
}

void configurarDisponibilidad(double *tiempoDisp, double *recursosDisp) {
    printf("\n--- Disponibilidad de la fabrica ---\n");
    *tiempoDisp   = leerNumero("Tiempo de produccion disponible (horas): ", 0.01);
    *recursosDisp = leerNumero("Recursos disponibles (unidades): ", 0.01);
    printf("  Disponibilidad registrada.\n");
}

void calcularYEvaluar(char nombres[][LONG_NOMBRE], double *tiempos,
                      double *recursos, int *demandas, int n,
                      double tiempoDisp, double recursosDisp) {
    int i;
    double tiempoTotal = 0, recursosTotal = 0, t, r;

    if (n == 0) {
        printf("\n  Registre al menos un producto antes de calcular.\n");
        return;
    }
    printf("\n  %-30s %14s %16s\n", "Producto", "Tiempo (h)", "Recursos");
    for (i = 0; i < n; i++) {
        t = demandas[i] * tiempos[i];
        r = demandas[i] * recursos[i];
        tiempoTotal = tiempoTotal + t;
        recursosTotal = recursosTotal + r;
        printf("  %-30s %14.2f %16.2f\n", nombres[i], t, r);
    }
    printf("  %-30s %14.2f %16.2f\n", "TOTAL REQUERIDO", tiempoTotal, recursosTotal);

    if (tiempoDisp == 0 || recursosDisp == 0) {
        printf("\n  Aun no se configuro la disponibilidad (opcion 5).\n");
        return;
    }
    printf("\n  Disponible: %.2f h y %.2f recursos\n", tiempoDisp, recursosDisp);
    printf("  Uso de tiempo:   %.1f %%\n", tiempoTotal / tiempoDisp * 100);
    printf("  Uso de recursos: %.1f %%\n", recursosTotal / recursosDisp * 100);

    if (tiempoTotal <= tiempoDisp && recursosTotal <= recursosDisp) {
        printf("\n  RESULTADO: la fabrica PUEDE cumplir con la demanda.\n");
    } else {
        printf("\n  RESULTADO: la fabrica NO puede cumplir con la demanda.\n");
        if (tiempoTotal > tiempoDisp)
            printf("  - Faltan %.2f horas de produccion.\n", tiempoTotal - tiempoDisp);
        if (recursosTotal > recursosDisp)
            printf("  - Faltan %.2f unidades de recursos.\n", recursosTotal - recursosDisp);
    }
}

/* Ordenamiento burbuja: se ordenan las posiciones de mayor a menor tiempo total */
void rankingPorTiempo(char nombres[][LONG_NOMBRE], double *tiempos,
                      int *demandas, int n) {
    int orden[MAX_PRODUCTOS], i, j, aux;
    if (n == 0) {
        printf("\n  No hay productos registrados.\n");
        return;
    }
    for (i = 0; i < n; i++) {
        orden[i] = i;
    }
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (demandas[orden[j]] * tiempos[orden[j]] <
                demandas[orden[j + 1]] * tiempos[orden[j + 1]]) {
                aux = orden[j];
                orden[j] = orden[j + 1];
                orden[j + 1] = aux;
            }
        }
    }
    printf("\n  Productos por tiempo total de fabricacion (mayor a menor):\n");
    for (i = 0; i < n; i++) {
        printf("  %d. %-30s %10.2f h\n", i + 1, nombres[orden[i]],
               demandas[orden[i]] * tiempos[orden[i]]);
    }
}

/* ---------- Programa principal ---------- */

int main(void) {
    char nombres[MAX_PRODUCTOS][LONG_NOMBRE];
    double tiempos[MAX_PRODUCTOS];    /* horas por unidad    */
    double recursos[MAX_PRODUCTOS];   /* recursos por unidad */
    int demandas[MAX_PRODUCTOS];      /* unidades demandadas */
    int n = 0, opcion;
    double tiempoDisp = 0, recursosDisp = 0;

    do {
        printf("\n========== OPTIMIZACION DE PRODUCCION ==========\n");
        printf(" 1. Ingresar producto\n");
        printf(" 2. Listar productos\n");
        printf(" 3. Editar producto\n");
        printf(" 4. Eliminar producto\n");
        printf(" 5. Configurar tiempo y recursos disponibles\n");
        printf(" 6. Calcular y evaluar cumplimiento de la demanda\n");
        printf(" 7. Ranking de productos por tiempo total\n");
        printf(" 0. Salir\n");
        opcion = (int) leerNumero("Seleccione una opcion: ", 0);

        if (opcion == 1) ingresarProducto(nombres, tiempos, recursos, demandas, &n);
        else if (opcion == 2) listarProductos(nombres, tiempos, recursos, demandas, n);
        else if (opcion == 3) editarProducto(nombres, tiempos, recursos, demandas, n);
        else if (opcion == 4) eliminarProducto(nombres, tiempos, recursos, demandas, &n);
        else if (opcion == 5) configurarDisponibilidad(&tiempoDisp, &recursosDisp);
        else if (opcion == 6) calcularYEvaluar(nombres, tiempos, recursos, demandas, n,
                                               tiempoDisp, recursosDisp);
        else if (opcion == 7) rankingPorTiempo(nombres, tiempos, demandas, n);
        else if (opcion != 0) printf("\n  Opcion invalida.\n");
    } while (opcion != 0);

    printf("\nHasta luego.\n");
    return 0;
}