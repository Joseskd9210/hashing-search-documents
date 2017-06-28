#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// estructura para guardar información de cada palabra 
typedef struct _inf_palabras {
	char*	pal;				// palabra a guardar
	int		docs[5];			// array con los indices a los documentos donde se encuentra la palabra
	int		numdoc;				//numero de documentos donde se encuentra la palabra como un contador
} inf_palabras;

int hash (char *palabra);
void 			indexar (inf_palabras **tabla, char* palabra, int ind_documento);
int 			buscar_palabra (inf_palabras **tabla, char* palabra);
int				mystrcmp (char *s1, char*s2);
char* 			documentos[] = {"docA.txt", "docB.txt", "docC.txt", NULL};
#define			TAMANO_TABLA	4001	//tiene que ser numero primo
#define TAMANOLINEA	2048	// tamaño de la linea escogido arbitrariamente
#define SEPARADOR	" \t"	// separador de palabras  

int main (void)
{
	FILE*			fp;						// puntero que apunta al documento
	inf_palabras	**tabla;				// tabla de palabras creado dinámicamente
	int				ind_documento;			// indice en el array documentos[] 
	char			linea[TAMANOLINEA];		// array para almacenar las líneas leídas del archivo 
	char*			palabra;				// puntero a una palabra
	char*			p;						// puntero temporal  
	int				i, j;					// variables temporales 

	tabla = (inf_palabras**) malloc (sizeof (inf_palabras) * TAMANO_TABLA);
	//iniciamos la tabla a NULL
	for (i = 0; i < TAMANO_TABLA;i++) {
		tabla[i] = (inf_palabras *) NULL;
	}
	// creamos la tabla con un tamaño por defecto
	printf ("Leyendo e insertando...");
	
	/// abrimos cada archivo, el final del array 'documentos' tiene NULL 
	for (ind_documento = 0; documentos[ind_documento] != NULL; ind_documento++) {
		fp = fopen (documentos[ind_documento], "r");	// abrimos el documento

		if (fp == NULL) {
			fprintf (stderr, "%s: no se puede abrir el archivo\n", documentos[ind_documento]);
			continue; //intenamos abrir el proximo documento
		}
		//se lee linea por linea
		
		while (fgets(linea, TAMANOLINEA, fp) != NULL) {
		
			linea[TAMANOLINEA-1] = '\0';	// nos aseguramos que el archivo tiene null al final 
			// extraemos las palabras
			p = strtok (linea, SEPARADOR);
			do {
				palabra = (char*) malloc (strlen (p) + 1); //obtenemos espacio para guardar la palabra
				strcpy (palabra, p);	//copiamos la palabra
				indexar (tabla, palabra, ind_documento);	// introducimos la palabra indicando el documento
				p = strtok (NULL, SEPARADOR);		// pasamos a la siguiente palabra
			} while (p != NULL);
		}
		fclose (fp); //cerramos el puntero
	}
	printf (" listo\n");
	// buscar palabras
	while (1) {	//pedimos palabras hasta que el usuario se salga
		//pedimos las palabras a buscar
		printf ("Indique la palabra a buscar ");
		gets(linea);
		printf ("Ocurrencias de la palabra: %s\n", linea);
		i = buscar_palabra (tabla, linea);
		if (i != -1) {
			for (j = 0;j < tabla[i]->numdoc;j++) {
				printf ("%s", documentos[tabla[i]->docs[j]]);
				if (j < tabla[i]->numdoc - 1) // introducir o no coma
					printf (", ");
			}
			printf ("\n");
		} else {
			printf ("Ninguna\n");
		}
		printf ("Otra palabra (s/n)? ");
		gets(linea);
		if (strncmp (linea, "s", 1) != 0 && strncmp (linea, "S", 1) != 0)
			break;	//FIN
	}
	// liberamos espacio
	for (i = 0; i < TAMANO_TABLA;i++) {
		if (tabla[i] != NULL) {
			free (tabla[i]->pal);
			free (tabla[i]);
		}
		
	}
	return 0;	
}


int buscar_palabra (inf_palabras **tabla, char* palabra)
{
	int idx = hash (palabra); //funcion hashing cuadratica en este caso
	int i;
	int pos, oldpos;			//posicion calculada en la tabla
	
	pos = -1;
	for (i = 0; i < TAMANO_TABLA;i++) {
		oldpos = pos; //guardamos la posicion calculada previamente
		pos = (idx + i*i) % TAMANO_TABLA; 
		if (tabla[pos] == NULL) {
			return -1;	//si no funciona
		}
		if (strcmp (palabra, tabla[pos]->pal) == 0) {		//la encontramos
			return pos;
		}
	}
	return -1;	//no la encontramos 
}

// insertar palabra en la tabla 
void indexar (inf_palabras **tabla, char* palabra, int ind_documento)
{
	int idx;					//guarda el valor de nuestra funciona hashing
	int i, j;					// variables para los bucles
	int pos, oldpos;			// posicion de la tabla calcualda 
	
	if ((pos = buscar_palabra (tabla, palabra)) != -1) {
		for (j = 0;j < tabla[pos]->numdoc;j++) {
			//validad que el documento no este registrado para esta palabra
			if (tabla[pos]->docs[j] == ind_documento) {	
				return; // si esta registrado volvemos
			}
		}
		//si no lo registramos 
		tabla[pos]->docs[tabla[pos]->numdoc] = ind_documento; // guardamos su indice
		tabla[pos]->numdoc++; //incrementamos los documentos guardados
		return;	// una vez guardado volvemos
	}
	pos = -1;
	idx = hash (palabra); //calculamos el valor de nuestra funcion hashing 
	//la palabra no se ha guardado
	for (i = 0; i < TAMANO_TABLA;i++) {
		oldpos = pos; //guardamos la posicion que hemos calculado previamente 
		pos = (idx + i*i) % TAMANO_TABLA; 
		if (oldpos == pos) {
			// comprobacion de un valor ya comprobado
			break;
		}
		if (tabla[pos] == NULL) {		//encontramos espacio libre
			tabla[pos] = malloc (sizeof (inf_palabras));
			tabla[pos]->pal = palabra;					//guardamos la palabra
			tabla[pos]->docs[0] = ind_documento;
			tabla[pos]->numdoc = 1;
			return;
		}
		
	}
	printf ("Insertar: no se puede encontrar un espacio libre\n");
}

//funcion que calcula el valor hash basado en ascci 

int hash (char *palabra)
{
	int hashval;

	for (hashval = 0; *palabra != '\0';)
		hashval += *palabra++;
	return (hashval % TAMANO_TABLA);
}
