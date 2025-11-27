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
//#define TAMTOKEN 50
//#define MAXPALABRAS 5000
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
    /*
    //Sustituya estas lineas por su código
    strcpy_s(szListaFinal[0], szPalabrasSugeridas[0]); //la palabra candidata
    iPeso[0] = iEstadisticas[0];			// el peso de la palabra candidata

    iNumLista = 1;							//Una sola palabra candidata
    */
/*****************************************************************************************************************
    ClonaPalabras: toma una palabra y obtiene todas las combinaciones y permutaciones requeridas por el metodo
    char *	szPalabraLeida,						// Palabra a clonar
    char	szPalabrasSugeridas[][TAMTOKEN], 	//Lista de palabras clonadas
    int &	iNumSugeridas)						//Numero de elementos en la lista
******************************************************************************************************************/
void	ClonaPalabras(char* szPalabraLeida,	// Palabra a clonar
    char	szPalabrasSugeridas[][TAMTOKEN], 	//Lista de palabras clonadas
    int& iNumSugeridas)						//Numero de elementos en la lista
{
    //Funcion que ignora palabras vacias o en blanco 
    if (szPalabraLeida == NULL || strlen(szPalabraLeida) == 0)
    {
        return;
    }
    int palabraALeer = strlen(szPalabraLeida);
    // 1. Eliminación de caracter
    for (int i = 0; i < palabraALeer; i++)
    {
        char Aux[TAMTOKEN];
        strcpy_s(Aux, TAMTOKEN, szPalabraLeida); // copiamos la palabra original
        // eliminar el caracter temp[i]
        for (int k = i; k < palabraALeer; k++) 
        {
            Aux[k] = Aux[k + 1];  // recorre a la izquierda
        }
        // aquí temp contiene la palabra con el carácter i eliminado
        //char* szPalabraLeida = Aux;
        // guardar en la lista de sugerencias
        strcpy_s(szPalabrasSugeridas[iNumSugeridas], Aux);
        iNumSugeridas++; // incrementar número de sugerencias
    }
    //Sustituya estas lineas por su código
    strcpy_s(szPalabrasSugeridas[iNumSugeridas], szPalabraLeida); //lo que sea que se capture, es sugerencia
}
