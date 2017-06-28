#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXDOC	5	//numero maximo de documentos a insertar 
//estructura que guarda la informacion de cada palabra 
typedef struct _inf_palabras {
	char*	pal;				//palabra
	//  docs[idx] es el contador del documento idx, indicando la frequencia
	int		frecuencia[MAXDOC];		// array de indices a los documentos donde esta palabra se encuentra con la frecuencia de la palabra 
	struct _inf_palabras* sig;	//puntero a la siguiente palabra
} inf_palabras;

//en esta estructura se guardara el indice y la frecuencia 

// es usado para hacer el ordenamiento de los documentos segun la frecuencia
typedef struct _frec_doc {
	int			indice;
	int			frecuencia;
} frec_doc;

int hash (char *palabra);
void 			indexar (inf_palabras **tabla, char* palabra, int ind_documento);
inf_palabras * 	buscar_palabra (inf_palabras **tabla, char* palabra);
void			selectionSort (frec_doc elementos[], int numero_elementos);
char* 			documentos[] = {"docA.txt", "docB.txt", "docC.txt", NULL};
#define			TAMANO_TABLA	4001	// debe ser un numero primo 
#define TAMANOLINEA	2048	// tamaño maximo escogido arbitrariamente
#define SEPARADOR	" \t"	// separador de palabras

int main (void)
{
	FILE*			fp;						// puntero al documento
	inf_palabras	**tabla;				// tabla de palabras que se han creado dinamicamente
	int				ind_documento;			//indice del array documentos
	char			linea[TAMANOLINEA];		//array para almacenar las lineas leidas
	char*			palabra;				// puntero a una palabra
	char*			p;						//puntero temporal
	int				i, j;					// variables temporales
	inf_palabras *	elemento;				//puntero a un elemento de tabla
	frec_doc		docs[MAXDOC];			//array para ordenar los documentos segun su frecuencia

	tabla = (inf_palabras**) malloc (sizeof (inf_palabras) * TAMANO_TABLA);
	//Inicializamos la tabla a null
	for (i = 0; i < TAMANO_TABLA;i++) {
		tabla[i] = (inf_palabras *) NULL;
	}
	//creamos la tabla con un tamaño por defecto
	printf ("Leyendo e insertando...");
	
	//abrimos cada archivo el fina lde este tendra NULL
	for (ind_documento = 0; documentos[ind_documento] != NULL; ind_documento++) {
		fp = fopen (documentos[ind_documento], "r");	/* abrimos el documento */

		if (fp == NULL) {
			fprintf (stderr, "%s: no se puede abrir el archivo\n", documentos[ind_documento]);
			continue;	//intenamos con el proximo
		}
		// leemos linea a linea
		
		while (fgets(linea, TAMANOLINEA, fp) != NULL) {
		
			linea[TAMANOLINEA-1] = '\0';	// Nos aseguramos de que al final tiene NULL
			//extraemos las palabras
			p = strtok (linea, SEPARADOR);
			do {
				palabra = (char*) malloc (strlen (p) + 1); //obtenemos espacio para guardar la palabra
				strcpy (palabra, p);	//copiamos la palabra
				indexar (tabla, palabra, ind_documento);	//insertamos la palabra indicando el documento 
				p = strtok (NULL, SEPARADOR);		//siguiente palabra
			} while (p != NULL);
		}
		fclose (fp); //cerramos el archivo 
	}
	printf (" listo\n");
	//buscamos palabras
	while (1) {	//pedimos palabras hasta que el usuario salga
		//pedimos al usuario la palabra a buscar
		printf ("Indique la palabra a buscar ");
		gets(linea);
		printf ("Ocurrencias de la palabra: %s\n", linea);
		elemento = buscar_palabra (tabla, linea);
		if (elemento != NULL) {
			i = 0;
			for (j = 0;j < MAXDOC;j++) {
				if (!elemento->frecuencia[j])	// la palabra no tiene ocurrencia en el documento j asi que la ignoramos
					continue;
				docs[i].frecuencia = elemento->frecuencia[j]; // frecuencia de la palabra en el documento de indice j 
				docs[i].indice = j;	// y guardamos el indice del documento
				i++;
			}
			//Ordenamos el array
			selectionSort (docs, i);
			//Imprimimos el array
			for (j = 0;j < i;j++) {
				printf ("%s: %d\n", documentos[docs[j].indice], docs[j].frecuencia);
			}
		} else {
			printf ("Ninguna\n");
		}
		printf ("Otra palabra (s/n)? ");
		gets(linea);
		if (strncmp (linea, "s", 1) != 0 && strncmp (linea, "S", 1) != 0)
			break;	//FIN
	}
	//liberamos espacico
	for (i = 0; i < TAMANO_TABLA;i++) {
		if (tabla[i] != NULL) {
			free (tabla[i]->pal);
			free (tabla[i]);
		}
		
	}
	return 0;	
}

//Esta implementa búsqueda con colisiones manejadas a traves de listas

inf_palabras * buscar_palabra (inf_palabras **tabla, char* palabra)
{
	int idx = hash (palabra); //funcion hashing
	int pos;			//posicion calculada de la palabra en la tabla
	inf_palabras *		elemento;		//apuntador de un elemento en la tabla

	pos = (idx) % TAMANO_TABLA;
	if (tabla[pos] == NULL) {
		return NULL;	// no funciona
	}
	elemento = tabla[pos];
	while (elemento != NULL) {
		if (strcmp (palabra, elemento->pal) == 0) {		//encontramos la palabra
			return elemento;
		}
		elemento = elemento->sig;
	}
	return NULL;	//no funciona
}

//insertamos la palabra
void indexar (inf_palabras **tabla, char* palabra, int ind_documento)
{
	int idx;					//guardamos el valor d enuestra funcion hashing
	int i;						//variables para los bucles
	int pos;					//posicion calculada de la palabra en la tabla
	inf_palabras *elemento;		// puntero de un elemento en la tabla

	if ((elemento = buscar_palabra (tabla, palabra)) != NULL) {
		
		// lo registramos y guardamos la frecuencia 
		elemento->frecuencia[ind_documento]++; // aumentamos la frecuenca de la palabra en el documento 
		return;	// guardada 
	}
	idx = hash (palabra); //calculamos el valor de nuestra funcion hashing 
	
	pos = (idx) % TAMANO_TABLA; 
	elemento = malloc (sizeof (inf_palabras));
	elemento->pal = palabra;					//guardamos la palabra 
	elemento->sig = NULL;
	for (i = 0; i < MAXDOC;i++)
		elemento->frecuencia[i] = 0;			// inicializamos las frecuencias de la palabra 
	elemento->frecuencia[ind_documento] = 1;
	if (tabla[pos] != NULL) {	//colision 
		elemento->sig = tabla[pos];	// el nuevo apunta al viejo
		tabla[pos] = elemento;			// actualizamos la cabeza de la lista 
	} else {		// nos encontramos el espacio libre 
		tabla[pos] = elemento;	//la cabeza de la lista es el nuevo elemento 
	}
}

// una función hashing básica que calcula el valor basado en el valor ascii de la palabra 

int hash (char *palabra)
{
	int hashval;

	for (hashval = 0; *palabra != '\0';)
		hashval += *palabra++;
	return hashval;
}


// selectionSort  ordenar los valores de un arreglo en orden descendiente 
 

void selectionSort (frec_doc elementos[], int numero_elementos)
{
	int actual;			// posicion que se esta ordenando en este momento 
	int min_posicion;	//posicion del minimo valor 
	int min_valor;		//posicion del mino valor encontrado
	int index;			//temporal
	int frecuencia, indice;
	for (actual = 0; actual < (numero_elementos-1);actual++)  {
		min_posicion = actual;
		min_valor = elementos[actual].frecuencia;
		for (index = actual + 1; index < numero_elementos; index++)  {
			if(elementos[index].frecuencia> min_valor) {
				min_valor = elementos[index].frecuencia;                                                     
				min_posicion = index;       
		      }
            }
		// hay que intercambiar min_posicion <-> actual 
		frecuencia = elementos[min_posicion].frecuencia;
		indice = elementos[min_posicion].indice;
		elementos[min_posicion].frecuencia = elementos[actual].frecuencia; 
		elementos[min_posicion].indice = elementos[actual].indice;  
		elementos[actual].frecuencia = frecuencia; 
		elementos[actual].indice = indice; 
	}
}
