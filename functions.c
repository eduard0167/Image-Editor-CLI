// MITROI EDUARD IONUT 312CA
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include "image_editor.h"

// CITIREA OPERATIEI SI STOCAREA CUVINTELOR DIN ACEASTA
int read_operation(char *word[WORD_LEN])
{
	char string[OP_LEN];
	fgets(string, OP_LEN, stdin);

	int n = 0;
	char *delimitator = "\n ";

	char *token = strtok(string, delimitator);
	while (token) {
		word[n] = (char *)strdup(token);

		if (!word[n])
			return -1;

		n++;
		token = (char *)strtok(NULL, delimitator);
	}

	return n;
}

// VERIFIC DACA INSTRUCTIUNEA ESTE CORECTA
void check_operation(char *operation[WORD_LEN], int *ok)
{
	if (strcmp(operation[0], "LOAD") == 0)
		*ok = 1;
	if (strcmp(operation[0], "SELECT") == 0 && strcmp(operation[1], "ALL") != 0)
		*ok = 1;
	if (strcmp(operation[0], "SELECT") == 0 && strcmp(operation[1], "ALL") == 0)
		*ok = 1;
	if (strcmp(operation[0], "ROTATE") == 0)
		*ok = 1;
	if (strcmp(operation[0], "CROP") == 0)
		*ok = 1;
	if (strcmp(operation[0], "GRAYSCALE") == 0)
		*ok = 1;
	if (strcmp(operation[0], "SEPIA") == 0)
		*ok = 1;
	if (strcmp(operation[0], "SAVE") == 0)
		*ok = 1;
}

void free_operation(int n, char *word[WORD_LEN])
{
	for (int i = 0; i < n; i++)
		free(word[i]);
}

void free_image(image *picture)
{
	free(picture->format);
	for (int i = 0; i <  picture->height; i++) {
		free(picture->R[i]);
		free(picture->G[i]);
		free(picture->B[i]);
	}
	free(picture->R);
	free(picture->G);
	free(picture->B);
}

unsigned char **alloc_matrix(int height, int width, unsigned char **a)
{
	a = (unsigned char **)malloc(height * sizeof(char *));
	if (!a) {
		fprintf(stderr, "malloc() failed\n");
		return NULL;
	}

	for (int i = 0; i < height; i++) {
		a[i] = (unsigned char *)malloc(width * sizeof(char));
		if (!a[i]) {
			fprintf(stderr, "malloc() failed\n");

			while (i-- >= 0)
				free(a[i]);
			free(a);

			return NULL;
		}
	}

	return a;
}

void free_matrix(int height, unsigned char **a)
{
	for (int i = 0; i < height; i++)
		free(a[i]);
	free(a);
}

int load(char **operation, coordinates *val, image *picture, int *load_image)
{
	char *filename = strdup(operation[1]);

	int pozition = check_file(filename, picture);
	if (!pozition) {
		printf("Failed to load %s\n", filename);
		free(filename);
		*load_image = 0;
		return 0;
	}

// IN FUNCTIE DE TIPUL FISIERULUI, CONTINUI CITIREA TEXT/BINARA
// DE LA POZITIA RAMASA
	if (strcmp(picture->format, "P2") == 0 ||
		strcmp(picture->format, "P3") == 0)
		load_ascii(filename, picture, pozition);
	else
		load_binary(filename, picture, pozition);

//DUPA CITIRE, INTREAGA IMAGINE ESTE SELECTATA
	val->x1 = 0;
	val->y1 = 0;
	val->x2 = picture->width;
	val->y2 = picture->height;
	free(filename);
	return 1;
}

// VERIFIC DACA FISIERUL POATE FI DESCHIS.
// DACA DA, IMI RETURNEAZA POZITIA LA CARE A RAMAS CURSORUL
int check_file(char *filename, image *picture)
{
	FILE *in = fopen(filename, "rt");
	if (!in)
		return 0;

	char buff[1000];
	char *corect[3];
	int n = 0;

// REALIZEZ CITIREA FISIERULUI LINIE CU LINE PANA GASESC PRIMELE 3 LINII
// CARE NU CONTIN COMENTARII
	while (n < 3) {
		fgets(buff, sizeof(buff), in);
		if (buff[0] == '#')
			continue;
		else
			corect[n++] = strdup(buff);
	}

	picture->format = strdup(corect[0]);
	picture->format[strlen(picture->format) - 1] = '\0';

	char *token = strtok(corect[1], " ");
	picture->width = atoi(token);
	token = strtok(NULL, " ");
	picture->height = atoi(token);

	picture->max_value = atoi(corect[2]);

	for (int i = 0; i < n; i++)
		free(corect[i]);

// VERIFIC DACA EXITA COMENTARIU INTRE VALOARE MAXIMA SI IMAGINE
	n = ftell(in);
	fgets(buff, sizeof(buff), in);
	if (buff[0] == '#')
		n = ftell(in);

	fclose(in);
	return n;
}

// INCARCAREA UNEI IMAGINI ASCII
int load_ascii(char *filename, image *picture, int pozition)
{
	FILE *in = fopen(filename, "rt");
	if (!in) {
		printf("Failed to load %s\n", filename);
		return 0;
	}

	fseek(in, pozition, SEEK_SET);

	picture->R = alloc_matrix(picture->height, picture->width, picture->R);
	picture->G = alloc_matrix(picture->height, picture->width, picture->G);
	picture->B = alloc_matrix(picture->height, picture->width, picture->B);

// CITIREA MATRICEA O REALIZEZ INTREG CU INTREG, IAR DUPA ACEEA STOCHEZ
// VALORILE INTR-O MATRICE UNSIGNED CHAR

	if (strcmp(picture->format, "P3") == 0) {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++) {
				int x;
				fscanf(in, "%d", &x);
				picture->R[i][j] = (unsigned char)x;

				fscanf(in, "%d", &x);
				picture->G[i][j] = (unsigned char)x;

				fscanf(in, "%d", &x);
				picture->B[i][j] = (unsigned char)x;
			}
		}
	} else {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++) {
				int x;

				fscanf(in, "%d", &x);
				picture->R[i][j] = (unsigned char)x;
				picture->G[i][j] = (unsigned char)x;
				picture->B[i][j] = (unsigned char)x;
			}
		}
	}

	fclose(in);
	printf("Loaded %s\n", filename);

	return 1;
}

// INCARCAREA UNEI IMAGINI IN FORMAT BINAR
int load_binary(char *filename, image *picture, int pozition)
{
	FILE *in = fopen(filename, "rb");
	if (!in) {
		printf("Failed to load %s\n", filename);
		return 0;
	}

	fseek(in, pozition, SEEK_SET);

	picture->R = alloc_matrix(picture->height, picture->width, picture->R);
	picture->G = alloc_matrix(picture->height, picture->width, picture->G);
	picture->B = alloc_matrix(picture->height, picture->width, picture->B);

// SPRE DEOSIBIRE DE INCARCAREA IMAGINII ASCII, LA BINAR REALIZEZ CITIREA
// CARACTER CU CARACTER
	if ((strcmp(picture->format, "P6") == 0)) {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++) {
				fread(&picture->R[i][j], sizeof(char), 1, in);
				fread(&picture->G[i][j], sizeof(char), 1, in);
				fread(&picture->B[i][j], sizeof(char), 1, in);
			}
		}
	} else {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++) {
				unsigned char aux;
				fread(&aux, sizeof(char), 1, in);

				picture->R[i][j] = aux;
				picture->G[i][j] = aux;
				picture->B[i][j] = aux;
			}
		}
	}
	fclose(in);
	printf("Loaded %s\n", filename);

	return 1;
}

// SE VERIFICA TIPUL DE FISIER CARE TREBUIE SALVAT
void save(int n, char **operation, image *picture)
{
	char *filename = strdup(operation[1]);

	if (n == 3 && strcmp(operation[2], "ascii") == 0)
		save_ascii(filename, picture);
	else
		save_binary(filename, picture);

	free(filename);
}

// SALVAREA UNEI IMAGINI IN BINAR
int save_binary(char *filename, image *picture)
{
	FILE *out = fopen(filename, "wt");
	if (!out)
		return -1;

	if (strcmp(picture->format, "P2") == 0)
		fprintf(out, "P5\n");
	else if (strcmp(picture->format, "P3") == 0)
		fprintf(out, "P6\n");
	else
		fprintf(out, "%s\n", picture->format);

	fprintf(out, "%d %d\n", picture->width, picture->height);
	fprintf(out, "%d\n", picture->max_value);

	fclose(out);

// DUPA SCRIE METADATELOR TEXT, IMAGINEA PROPRIU ZISA ESTE SALVATA IN BINAR
	out = fopen(filename, "ab");
	if (!out)
		return -1;

	if (strcmp(picture->format, "P6") == 0 ||
		strcmp(picture->format, "P3") == 0) {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++) {
				fwrite(&picture->R[i][j], sizeof(char), 1, out);
				fwrite(&picture->G[i][j], sizeof(char), 1, out);
				fwrite(&picture->B[i][j], sizeof(char), 1, out);
			}
		}
	} else {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++)
				fwrite(&picture->G[i][j], sizeof(char), 1, out);
		}
	}

	fclose(out);

	printf("Saved %s\n", filename);
	return 1;
}

//SALVAREA UNEI IMAGINI IN FORMAT ASCII
int save_ascii(char *filename, image *picture)
{
	FILE *out = fopen(filename, "wt");
	if (!out)
		return -1;

	if (strcmp(picture->format, "P5") == 0)
		fprintf(out, "P2\n");
	else if (strcmp(picture->format, "P6") == 0)
		fprintf(out, "P3\n");
	else
		fprintf(out, "%s\n", picture->format);

	fprintf(out, "%d %d\n", picture->width, picture->height);
	fprintf(out, "%d\n", picture->max_value);

	if (strcmp(picture->format, "P3") == 0 ||
		strcmp(picture->format, "P6") == 0) {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++) {
				int x = (int)picture->R[i][j];
				fprintf(out, "%d ", x);

				x = (int)picture->G[i][j];
				fprintf(out, "%d ", x);

				x = (int)picture->B[i][j];
				fprintf(out, "%d ", x);
			}
			fprintf(out, "\n");
		}
	} else {
		for (int i = 0; i < picture->height; i++) {
			for (int j = 0; j < picture->width; j++) {
				int x = (int)picture->G[i][j];
				fprintf(out, "%d ", x);
				}
			fprintf(out, "\n");
		}
	}
	fclose(out);

	printf("Saved %s\n", filename);

	return 1;
}

void change_values(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void select_all(coordinates *val, image *picture)
{
	val->x1 = 0;
	val->y1 = 0;
	val->x2 = picture->width;
	val->y2 = picture->height;
}

// VERIFICAREA CORECTITUDINII COORDONATELOR
int check_coordinates(char **operation, int x1, int y1, int x2, int y2)
{
	if (x1 == 0 && strcmp(operation[1], "0") != 0)
		return 0;

	if (x2 == 0 && strcmp(operation[3], "0") != 0)
		return 0;

	if (y1 == 0 && strcmp(operation[2], "0") != 0)
		return 0;

	if (y2 == 0 && strcmp(operation[4], "0") != 0)
		return 0;

	return 1;
}

// VERIFIC DACA COORDONATELE SUNT VALIDE SI DACA AU FOST TRANSMISE IN
// ORDINEA CORECTA, DOAR DACA VARIABILELE AUXILIARE INDEPLINESC CERINTELE
// COORDONOTALE ISI SCHIMBA VALOARE
void select_coordinates(char **operation, coordinates *val, image *picture)
{
	int x1 = atoi(operation[1]);
	int y1 = atoi(operation[2]);
	int x2 = atoi(operation[3]);
	int y2 = atoi(operation[4]);

	if (!check_coordinates(operation, x1, y1, x2, y2)) {
		printf("Invalid command\n");
		return;
	}

	if (x1 == x2 || y1 == y2) {
		printf("Invalid set of coordinates\n");
		return;
	}

	if (x1 > x2)
		change_values(&x1, &x2);
	if (y1 > y2)
		change_values(&y1, &y2);

	if (y1 >= 0 && x1 >= 0 && y2 <= picture->height && x2 <= picture->width) {
		val->x1 = x1;
		val->x2 = x2;
		val->y1 = y1;
		val->y2 = y2;
		printf("Selected %d %d %d %d\n", val->x1, val->y1, val->x2, val->y2);
	} else {
		printf("Invalid set of coordinates\n");
	}
}

// VERIFIC IN CE CAZ SE INCADREAZA SELECTIA CURENTA:
void rotate(char *operation, coordinates *val, image *picture)
{
	int grades = atoi(operation);

	if (val->y2 - val->y1 != val->x2 - val->x1 &&
		val->y2 != picture->height && val->x2 != picture->width) {
		printf("The selection must be square\n");
		return;
	}

	if (grades == 360 || grades == -360 || grades == 0) {
		printf("Rotated %d\n", grades);
		return;
	}

	if (grades != 180 && grades != -180 &&
		val->y2 - val->y1 == picture->height &&
		val->x2 - val->x1 == picture->width)
		rotate_full_image(val, picture, grades);
	else
		rotate_selection(val, picture, grades);
}

// ROTIREA PENTRU O IMAGINE INTREAGA IN CAZUL IN CARE ACEASTA ISI SCHIMBA
// DIMENSIUNEA (+-90/+-270)
void rotate_full_image(coordinates *val, image *picture, int grades)
{
	int n = picture->height, m = picture->width;

	unsigned char **r = NULL;
	r = alloc_matrix(m, n, r);
	unsigned char **g = NULL;
	g = alloc_matrix(m, n, g);
	unsigned char **b = NULL;
	b = alloc_matrix(m, n, b);

	if (grades == 90 || grades == -270) {
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++) {
				r[i][j] = picture->R[n - 1 - j][i];
				g[i][j] = picture->G[n - 1 - j][i];
				b[i][j] = picture->B[n - 1 - j][i];
			}
		}
	} else if (grades == -90 || grades == 270) {
		for (int i = 0; i < m; i++) {
			for (int j = 0; j < n; j++) {
				r[i][j] = picture->R[j][m - i - 1];
				g[i][j] = picture->G[j][m - i - 1];
				b[i][j] = picture->B[j][m - i - 1];
			}
		}
	} else {
		free_matrix(m, r);
		free_matrix(m, g);
		free_matrix(m, b);
		printf("Unsupported rotation angle\n");
		return;
	}

	free_matrix(picture->height, picture->R);
	free_matrix(picture->height, picture->G);
	free_matrix(picture->height, picture->B);

	picture->R = r;
	picture->G = g;
	picture->B = b;

// NOILE VALORILE ALE INALTIMII, RESPECTIV LATIMII
	picture->height = m;
	picture->width = n;

// SE CONSIDERA SELECTATA INTREAGA IMAGINE CU NOILE DIMENSIUNI
	select_all(val, picture);

	printf("Rotated %d\n", grades);
}

// ROTIREA INTREGII IMAGINII CU +-180  SAU A UNEI PORTIUNI
void rotate_selection(coordinates *val, image *picture, int grades)
{
	int n = val->y2 - val->y1;
	int m = val->x2 - val->x1;

	unsigned char **r = NULL;
	r = alloc_matrix(n, m, r);
	unsigned char **g = NULL;
	g = alloc_matrix(n, m, g);
	unsigned char **b = NULL;
	b = alloc_matrix(n, m, b);

	int rotation = 0;
	if (grades == 90 || grades == -270)
		rotation = 1;
	if (grades == -90 || grades == 270)
		rotation = 2;
	if (grades == 180)
		rotation = 3;
	if (grades == -180)
		rotation = 4;

	switch (rotation) {
	case 1:
	for (int i = val->y1, p = 0; i < val->y2; i++, p++) {
		for (int j = val->x1, q = 0; j < val->x2; j++, q++) {
			r[p][q] = picture->R[val->y2 - q - 1][val->x1 + p];
			g[p][q] = picture->G[val->y2 - q - 1][val->x1 + p];
			b[p][q] = picture->B[val->y2 - q - 1][val->x1 + p];
		}
	}
		break;
	case 2:
	for (int i = val->y1, p = 0; i < val->y2; i++, p++) {
		for (int j = val->x1, q = 0; j < val->x2; j++, q++) {
			r[p][q] = picture->R[val->y1 + q][val->x2 - p - 1];
			g[p][q] = picture->G[val->y1 + q][val->x2 - p - 1];
			b[p][q] = picture->B[val->y1 + q][val->x2 - p - 1];
		}
	}
		break;
	case 3:
	for (int i = val->y1, p = 0; i < val->y2; i++, p++) {
		for (int j = val->x1, q = 0; j < val->x2; j++, q++) {
			r[p][q] = picture->R[val->y2 - p - 1][val->x2 - q - 1];
			g[p][q] = picture->G[val->y2 - p - 1][val->x2 - q - 1];
			b[p][q] = picture->B[val->y2 - p - 1][val->x2 - q - 1];
		}
	}
		break;
	case 4:
	for (int i = val->y1, p = 0; i < val->y2; i++, p++) {
		for (int j = val->x1, q = 0; j < val->x2; j++, q++) {
			r[p][q] = picture->R[val->y2 - p - 1][val->x2 - q - 1];
			g[p][q] = picture->G[val->y2 - p - 1][val->x2 - q - 1];
			b[p][q] = picture->B[val->y2 - p - 1][val->x2 - q - 1];
		}
	}
	break;
	default:
	free_matrix(n, r);
	free_matrix(n, g);
	free_matrix(n, b);
	printf("Unsupported rotation angle\n");
	return;
		break;
	}

	for (int i = val->y1, p = 0; i < val->y2; i++, p++) {
		for (int j = val->x1, q = 0; j < val->x2; j++, q++) {
			picture->R[i][j] = r[p][q];
			picture->G[i][j] = g[p][q];
			picture->B[i][j] = b[p][q];
		}
	}
	printf("Rotated %d\n", grades);

	free_matrix(n, r);
	free_matrix(n, g);
	free_matrix(n, b);
}

// DECUPAREA SELECTIEI CURENTE
void crop(coordinates *val, image *picture)
{
	int n = val->y2 - val->y1;
	int m = val->x2 - val->x1;

	unsigned char **r = NULL;
	r = alloc_matrix(n, m, r);
	unsigned char **g = NULL;
	g = alloc_matrix(n, m, g);
	unsigned char **b = NULL;
	b = alloc_matrix(n, m, b);

	for (int i = val->y1, p = 0; i < val->y2; i++, p++) {
		for (int j = val->x1, q = 0; j < val->x2; j++, q++) {
			r[p][q] = picture->R[i][j];
			g[p][q] = picture->G[i][j];
			b[p][q] = picture->B[i][j];
		}
	}

	free_matrix(picture->height, picture->R);
	free_matrix(picture->height, picture->G);
	free_matrix(picture->height, picture->B);

	picture->R = r;
	picture->G = g;
	picture->B = b;

// NOILE VALORILE ALE INALTIMII, RESPECTIV LATIMII
	picture->height = n;
	picture->width = m;

// SE CONSIDERA SELECTATA INTREAGA IMAGINE CU NOILE DIMENSIUNI
	select_all(val, picture);

	printf("Image cropped\n");
}

// APLICAREA FILTRULUI GRAYSCALE:
// UTILIZEZ VARIABILA DOUBLE, NOUA VALOAREA A PIXELILOR SE
// OBTINE PRIN APLICAREA UNUI CAST LA UNSIGNED CHAR A VARIABILE DOUBLE
void grayscale(coordinates *val, image *picture)
{
	if (strcmp(picture->format, "P3") == 0 ||
		strcmp(picture->format, "P6") == 0) {
		for (int i = val->y1; i < val->y2; i++) {
			for (int j = val->x1; j < val->x2; j++) {
				double x = ((double)picture->R[i][j] +
				(double)picture->G[i][j] +
				(double)picture->B[i][j]) / 3;

				picture->R[i][j] = (unsigned char)x;
				picture->G[i][j] = (unsigned char)x;
				picture->B[i][j] = (unsigned char)x;
			}
		}
		printf("Grayscale filter applied\n");
	} else {
		printf("Grayscale filter not available\n");
	}
}

// APLICAREA FILTRULUI SEPIA:
// APLIC FORMULA LUCRAND DOAR CU VARIABILE DOUBLE, NOUA VALOAREA SE
// OBTINE PRIN ROTUNJIREA VALORILOR DOUBLE (SI A FUNCTIE MIN DACA ESTE CAZUL)
//  SI APLICAREA UNUI CAST LA UNSIGNED CHAR
void sepia(coordinates *val, image *pic)
{
	if (strcmp(pic->format, "P3") == 0 ||
		strcmp(pic->format, "P6") == 0) {
		for (int i = val->y1; i < val->y2; i++) {
			for (int j = val->x1; j < val->x2; j++) {
				double newR = (double)pic->R[i][j] * 0.393 +
				(double)pic->G[i][j] * 0.769 +
				(double)pic->B[i][j] * 0.189;

				double newG = (double)pic->R[i][j] * 0.349 +
				(double)pic->G[i][j] * 0.686 +
				(double)pic->B[i][j] * 0.168;

				double newB = (double)pic->R[i][j] * 0.272 +
				 (double)pic->G[i][j] * 0.534 + (double)pic->B[i][j] * 0.131;

				newR = round(newR);
				newG = round(newG);
				newB = round(newB);

				if (pic->max_value < newR)
					pic->R[i][j] = (unsigned char)min(pic->max_value, newR);
				else
					pic->R[i][j] = (unsigned char)newR;

				if (pic->max_value < newG)
					pic->G[i][j] = (unsigned char)min(pic->max_value, newG);
				else
					pic->G[i][j] = (unsigned char)newG;

				if (pic->max_value < newB)
					pic->B[i][j] = (unsigned char)min(pic->max_value, newB);
				else
					pic->B[i][j] = (unsigned char)newB;
			}
		}
		printf("Sepia filter applied\n");
	} else {
		printf("Sepia filter not available\n");
	}
}

int min(int a, int b)
{
	if (a < b)
		return a;
	else
		return b;
}
