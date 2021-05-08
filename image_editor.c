// MITROI EDUARD IONUT 312CA
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include "image_editor.h"

int main(void)
{
// CITESC COMANDA INTRODUSA
	char *operation[WORD_LEN];
	int n = read_operation(operation);
	coordinates val;
	int load_image = 0;
	image picture;

// SE REPETA CITIREA PANA LA INTALNIREA COMENZII EXIT
	while (strcmp(operation[0], "EXIT") != 0) {
		int ok = 0;
		check_operation(operation, &ok); // VERIFIC CORECTITUDINEA ACESTEIA

		if (!load_image && ok && strcmp(operation[0], "LOAD") != 0) {
			printf("No image loaded\n");
			free_operation(n, operation);
			n = read_operation(operation);
			continue;
		}

// DACA EXISTA IMAGINE MEMORATA, PRIMA DATA SE VA ELIBERA MEMORIA OCUPATA
// DUPA CARE SE VA MEMORA NOUA IMAGINE
		if (strcmp(operation[0], "LOAD") == 0) {
			if (load_image) {
				free_image(&picture);
				load_image = load(operation, &val, &picture, &load_image);
			} else {
				load_image = load(operation, &val, &picture, &load_image);
			}
		}

		if (strcmp(operation[0], "SELECT") == 0 &&
			strcmp(operation[1], "ALL") != 0) {
			if (n != 5)
				ok = 0;
			else
				select_coordinates(operation, &val, &picture);
		}

		if (strcmp(operation[0], "SELECT") == 0 &&
			strcmp(operation[1], "ALL") == 0) {
			select_all(&val, &picture);
			printf("Selected ALL\n");
		}

		if (strcmp(operation[0], "ROTATE") == 0)
			rotate(operation[1], &val, &picture);

		if (strcmp(operation[0], "CROP") == 0)
			crop(&val, &picture);

		if (strcmp(operation[0], "GRAYSCALE") == 0)
			grayscale(&val, &picture);

		if (strcmp(operation[0], "SEPIA") == 0)
			sepia(&val, &picture);

		if (strcmp(operation[0], "SAVE") == 0)
			save(n, operation, &picture);

		if (!ok)
			printf("Invalid command\n");
		ok = 0;
		free_operation(n, operation);
		n = read_operation(operation);
	}

// VERIFIC DACA EXISTA IMAGINE MEMORATA IN MOMENTUL FINALIZARII PROGRAMULUI
	if (load_image)
		free_image(&picture);
	else
		printf("No image loaded\n");
	free_operation(n, operation);

	return 0;
}
