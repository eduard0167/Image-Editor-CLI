// MITROI EDUARD IONUT 312CA
#ifndef __IMAGE_EDITOR_H__
#define __IMAGE_EDITOR_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#define OP_LEN 150
#define WORD_LEN 15

typedef struct {
	char *format;
	int width, height;
	int max_value;
	unsigned char **R;
	unsigned char **G;
	unsigned char **B;
} image;

typedef struct {
	int x1, y1, x2, y2;
} coordinates;

int read_operation(char *word[WORD_LEN]);
void check_operation(char *word[WORD_LEN], int *ok);
void free_operation(int n, char *word[WORD_LEN]);

void free_image(image *picture);

unsigned char **alloc_matrix(int height, int width, unsigned char **a);
void free_matrix(int height, unsigned char **a);

int load(char **operation, coordinates *val, image *picture, int *load_image);
int check_file(char *filename, image *picture);
int load_ascii(char *filename, image *picture, int pozition);
int load_binary(char *filename, image *picture, int pozition);

void change_values(int *a, int *b);
void select_all(coordinates *val, image *picture);
int check_coordinates(char **operation, int x1, int y1, int x2, int y2);
void select_coordinates(char **operation, coordinates *val, image *picture);

void save(int n, char **filename, image *picture);
int save_ascii(char *filename, image *picture);
int save_binary(char *filename, image *picture);

void rotate(char *operation, coordinates *val, image *picture);
void rotate_full_image(coordinates *val, image *picture, int grades);
void rotate_selection(coordinates *val, image *picture, int grades);

void crop(coordinates *val, image *picture);

void change_values(int *a, int *b);

void grayscale(coordinates *val, image *picture);
void sepia(coordinates *val, image *pic);

int min(int a, int b);

#endif
