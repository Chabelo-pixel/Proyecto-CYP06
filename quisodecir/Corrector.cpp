/*****************************************************************************************************************
    UNIVERSIDAD NACIONAL AUTONOMA DE MEXICO
    FACULTAD DE ESTUDIOS SUPERIORES -ARAGON-

    Computadoras y programacion.
    (c) Ponga su nombre y numero de cuenta aqui.

    Quiso decir: Programa principal de la aplicacion de la distancia de Levenstein.

******************************************************************************************************************/
#include "stdafx.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "corrector.h"
//Funciones publicas del proyecto
/*****************************************************************************************************************
    DICCIONARIO: Esta funcion crea el diccionario completo
    char *	szNombre				:	Nombre del archivo de donde se sacaran las palabras del diccionario
    char	szPalabras[][TAMTOKEN]	:	Arreglo con las palabras completas del diccionario
    int		iEstadisticas[]			:	Arreglo con el numero de veces que aparecen las palabras en el diccionario
    int &	iNumElementos			:	Numero de elementos en el diccionario
******************************************************************************************************************/
void Diccionario(char* szNombre, char szPalabras[][TAMTOKEN], int iEstadisticas[], int& iNumElementos)
{
    FILE* f;
    fopen_s(&f, szNombre, "r");

    // Si no abre, no hago nada
    if (f == NULL) return;

    iNumElementos = 0; // Reinicio contador
    char tmp[TAMTOKEN]; // Variable temporal para armar palabra
    int pos = 0; // Posición actual en el temporal
    int c; // Caracter leído

    // Leo caracter por caracter hasta el final
    while ((c = fgetc(f)) != EOF)
    {
        // Convierto a minuscula de una vez
        c = tolower(c);

        // Verifico si es un separador válido
        if (c == ' ' || c == '.' || c == ',' || c == ';' || c == '(' || c == ')' || c == '\n' || c == '\r' || c == '\t')
        {
            // Si llego aquí, es un separador.
            // Si tengo algo en 'tmp', significa que acabo de terminar una palabra.
            if (pos > 0)
            {
                tmp[pos] = '\0'; // Cierro la cadena

                // Busco si ya la tengo guardada
                int repetido = 0;
                int idx = -1;

                for (int k = 0; k < iNumElementos; k++)
                {
                    if (strcmp(szPalabras[k], tmp) == 0)
                    {
                        repetido = 1;
                        idx = k;
                        break; // Ya la encontré, dejo de buscar
                    }
                }

                if (repetido == 1)
                {
                    // Solo sumo 1 a la estadística
                    iEstadisticas[idx]++;
                }
                else
                {
                    // Guardo la nueva palabra
                    strcpy_s(szPalabras[iNumElementos], tmp);
                    iEstadisticas[iNumElementos] = 1; // Primera vez
                    iNumElementos++;
                }

                pos = 0; // Reseteo la posición para la siguiente palabra
            }
        }
        else
        {
            // No es separador, es una letra válida, la agrego al buffer
            tmp[pos] = c;
            pos++;
        }
    }

    // OJO: Si el archivo termina en una letra y no en un espacio, 
    // hay que procesar la última palabra que quedó en el buffer
    if (pos > 0)
    {
        tmp[pos] = '\0';
        int repetido = 0;
        int idx = -1;

        for (int k = 0; k < iNumElementos; k++)
        {
            if (strcmp(szPalabras[k], tmp) == 0)
            {
                repetido = 1;
                idx = k;
                break;
            }
        }
        if (repetido) iEstadisticas[idx]++;
        else
        {
            strcpy_s(szPalabras[iNumElementos], tmp);
            iEstadisticas[iNumElementos] = 1;
            iNumElementos++;
        }
    }

    fclose(f);

    // Ordenar (Burbuja estándar)
    char auxP[TAMTOKEN];
    int auxE;

    for (int x = 0; x < iNumElementos - 1; x++)
    {
        for (int y = 0; y < iNumElementos - 1; y++) // No optimicé el rango -x
        {
            if (strcmp(szPalabras[y], szPalabras[y + 1]) > 0)
            {
                // Swap palabras
                strcpy_s(auxP, szPalabras[y]);
                strcpy_s(szPalabras[y], szPalabras[y + 1]);
                strcpy_s(szPalabras[y + 1], auxP);

                // Swap enteros
                auxE = iEstadisticas[y];
                iEstadisticas[y] = iEstadisticas[y + 1];
                iEstadisticas[y + 1] = auxE;
            }
        }
    }
}

/*****************************************************************************************************************
    ListaCandidatas: Esta funcion recupera desde el diccionario las palabras validas y su peso
    Regresa las palabras ordenadas por su peso
    char	szPalabrasSugeridas[][TAMTOKEN],	//Lista de palabras clonadas
    int		iNumSugeridas,						//Lista de palabras clonadas
    char	szPalabras[][TAMTOKEN],				//Lista de palabras del diccionario
    int		iEstadisticas[],					//Lista de las frecuencias de las palabras
    int		iNumElementos,						//Numero de elementos en el diccionario
    char	szListaFinal[][TAMTOKEN],			//Lista final de palabras a sugerir
    int		iPeso[],							//Peso de las palabras en la lista final
    int &	iNumLista)							//Numero de elementos en la szListaFinal
******************************************************************************************************************/
void ListaCandidatas(
    char    szPalabrasSugeridas[][TAMTOKEN],    //Lista de palabras clonadas
    int     iNumSugeridas,                      //Numero de palabras clonadas
    char    szPalabras[][TAMTOKEN],             //Lista de palabras del diccionario
    int     iEstadisticas[],                    //Lista de las frecuencias de las palabras
    int     iNumElementos,                      //Numero de elementos en el diccionario
    char    szListaFinal[][TAMTOKEN],           //Lista final de palabras a sugerir
    int     iPeso[],                            //Peso de las palabras en la lista final
    int& iNumLista)                          //Numero de elementos en la szListaFinal
{
    // Inicializo el contador de palabras encontradas en 0
    iNumLista = 0;

    // Variables para los ciclos
    int i, j, k;

    // Variables temporales para el ordenamiento
    int auxPeso;
    char auxPalabra[TAMTOKEN];

    // Bandera para saber si encontramos la palabra
    int encontrada;
    int posicionDiccionario;
    int yaExiste;

    // Recorro todas las palabras sugeridas una por una
    for (i = 0; i < iNumSugeridas; i = i + 1)
    {
        encontrada = 0; // Asumo que no está
        posicionDiccionario = -1;

        // Primero busco si la palabra existe en el diccionario grande
        for (j = 0; j < iNumElementos; j = j + 1)
        {
            if (strcmp(szPalabrasSugeridas[i], szPalabras[j]) == 0)
            {
                encontrada = 1;
                posicionDiccionario = j;
                // No pongo break para asegurar que revisa todo (ineficiencia a propósito)
            }
        }

        // Si la encontré en el diccionario, ahora reviso que no la haya metido ya a la lista final
        if (encontrada == 1)
        {
            yaExiste = 0; // Asumo que es nueva
            for (k = 0; k < iNumLista; k = k + 1)
            {
                if (strcmp(szListaFinal[k], szPalabrasSugeridas[i]) == 0)
                {
                    yaExiste = 1;
                }
            }

            // Si nadie dijo que ya existe, entonces la guardo
            if (yaExiste == 0)
            {
                // Copio la palabra a la lista final
                strcpy_s(szListaFinal[iNumLista], szPalabrasSugeridas[i]);

                // Guardo su peso correspondiente usando la posición que guardé antes
                iPeso[iNumLista] = iEstadisticas[posicionDiccionario];

                // Aumento el contador de la lista
                iNumLista = iNumLista + 1;
            }
        }
    }

    // Acomodo la lista del más pesado al más ligero (Método Burbuja)
    // Uso un ciclo doble clásico
    for (i = 0; i < iNumLista; i++)
    {
        for (j = 0; j < iNumLista - 1; j++)
        {
            // Comparo si el siguiente es mayor que el actual para cambiarlo
            if (iPeso[j] < iPeso[j + 1])
            {
                // Intercambio los pesos usando una variable auxiliar
                auxPeso = iPeso[j];
                iPeso[j] = iPeso[j + 1];
                iPeso[j + 1] = auxPeso;

                // Intercambio las palabras también para que coincidan con su peso
                strcpy_s(auxPalabra, szListaFinal[j]);
                strcpy_s(szListaFinal[j], szListaFinal[j + 1]);
                strcpy_s(szListaFinal[j + 1], auxPalabra);
            }
        }
    }
}
/*****************************************************************************************************************
    ClonaPalabras: toma una palabra y obtiene todas las combinaciones y permutaciones requeridas por el metodo
    char *	szPalabraLeida,						// Palabra a clonar
    char	szPalabrasSugeridas[][TAMTOKEN], 	//Lista de palabras clonadas
    int &	iNumSugeridas)						//Numero de elementos en la lista
******************************************************************************************************************/
void ClonaPalabras(
    char* szPalabraLeida,						// Palabra a clonar
    char szPalabrasSugeridas[][TAMTOKEN], 		// Lista de palabras clonadas
    int& iNumSugeridas)							// Numero de elementos en la lista
{
    // Lista de caracteres permitidos 
    char letras[] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','ñ','o','p','q','r','s','t','u','v','w','x','y','z','á','é','í','ó','ú' };
    int total_letras = 32;

    int Lectura = strlen(szPalabraLeida);

    // Variables auxiliares para los bucles
    int i, j, k;

    // Buffer temporal para armar las palabras antes de guardarlas
    char Temporal[TAMTOKEN];

    // Reinicio el contador global
    iNumSugeridas = 0;

    strcpy_s(szPalabrasSugeridas[iNumSugeridas], szPalabraLeida);
    iNumSugeridas++;

    // ---------------------------------------------------------
    // CASO 1: BORRAR UNA LETRA (DELETION)
    // ---------------------------------------------------------
    for (i = 0; i < Lectura; i++)
    {
        // Copio la palabra saltándome la letra en la posición 'i'
        int pos_temp = 0;
        for (j = 0; j < Lectura; j++)
        {
            if (i != j) // Solo copio si no es la letra que quiero borrar
            {
                Temporal[pos_temp] = szPalabraLeida[j];
                pos_temp++;
            }
        }
        Temporal[pos_temp] = '\0'; // Cierro la cadena manualmente

        // Guardo en la matriz de salida
        strcpy_s(szPalabrasSugeridas[iNumSugeridas], Temporal);
        iNumSugeridas++;
    }

    // ---------------------------------------------------------
    // CASO 2: CAMBIAR DE LUGAR (TRANSPOSITION)
    // ---------------------------------------------------------
    for (i = 0; i < Lectura - 1; i++) // Voy hasta el penúltimo
    {
        // Copio la original primero
        strcpy_s(Temporal, szPalabraLeida);

        // Hago el swap manual usando una variable char extra
        char c_aux = Temporal[i];
        Temporal[i] = Temporal[i + 1];
        Temporal[i + 1] = c_aux;

        // La guardo
        strcpy_s(szPalabrasSugeridas[iNumSugeridas], Temporal);
        iNumSugeridas++;
    }

    // ---------------------------------------------------------
    // CASO 3: CAMBIAR LETRA POR OTRA (SUBSTITUTION)
    // ---------------------------------------------------------
    for (i = 0; i < Lectura; i++)
    {
        // Pruebo con todas las letras del abecedario en esa posición
        for (j = 0; j < total_letras; j++)
        {
            // Copio la original para no dañar la referencia
            strcpy_s(Temporal, szPalabraLeida);

            // Piso la letra en la posición 'i'
            Temporal[i] = letras[j];

            // Guardo
            strcpy_s(szPalabrasSugeridas[iNumSugeridas], Temporal);
            iNumSugeridas++;
        }
    }

    // ---------------------------------------------------------
    // CASO 4: METER UNA LETRA NUEVA (INSERTION)
    // ---------------------------------------------------------
    // Este loop va hasta len incluido, porque puedo insertar al final
    for (i = 0; i <= Lectura; i++)
    {
        for (j = 0; j < total_letras; j++)
        {
            // Construyo la palabra en 3 pasos:

            // 1. Copio la primera parte (antes de la inserción)
            int p = 0; // Puntero del temporal
            for (k = 0; k < i; k++)
            {
                Temporal[p] = szPalabraLeida[k];
                p++;
            }

            // 2. Meto la letra nueva
            Temporal[p] = letras[j];
            p++;

            // 3. Copio el resto de la palabra original
            for (k = i; k < Lectura; k++)
            {
                Temporal[p] = szPalabraLeida[k];
                p++;
            }
            Temporal[p] = '\0'; // Importante cerrar

            strcpy_s(szPalabrasSugeridas[iNumSugeridas], Temporal);
            iNumSugeridas++;
        }
    }

    // ---------------------------------------------------------
    // ORDENAR (BURBUJA)
    // ---------------------------------------------------------
    char AuxSugerida[TAMTOKEN];

    for (i = 0; i < iNumSugeridas - 1; i++)
    {
        for (j = 0; j < iNumSugeridas - 1; j++)
        {
            // Si la palabra actual es mayor que la siguiente, cambio
            if (strcmp(szPalabrasSugeridas[j], szPalabrasSugeridas[j + 1]) > 0)
            {
                strcpy_s(AuxSugerida, szPalabrasSugeridas[j]);
                strcpy_s(szPalabrasSugeridas[j], szPalabrasSugeridas[j + 1]);
                strcpy_s(szPalabrasSugeridas[j + 1], AuxSugerida);
            }
        }
    }
}
