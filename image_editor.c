/// Plamadeala Victoria 315CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct copy {
	int w, h;
	int type;
	unsigned char ***p;
	int pixeli;
};

struct image {
	int width, height;
	int type, color;
	unsigned char ***pixels; /// RGB - P2, P3
	struct copy *c;
	int selection;
	int x1, x2, y1, y2;
};

void free_matrix(unsigned char ***pixeli, int type, int h)
{
	for (int i = 0; i < ((type == 3 || type == 6) ? 3 : 1); i++) {
		for (int j = 0; j < h; j++)
			free(pixeli[i][j]);
		free(pixeli[i]);
	}
	free(pixeli);
}

void free_img(struct image *img)
{
	for (int c = 0; c < ((img->type == 3 || img->type == 6) ? 3 : 1); c++) {
		for (int i = 0; i < img->height; i++)
			free(img->pixels[c][i]);
		free(img->pixels[c]);
	}
	///// VERIFIC DACA SE ALOCA IN MALLOC !!
	free(img->pixels);
	free(img);
}

struct image *create(int w, int h, int type, int color)
{
	struct image *img = (struct image *)malloc(sizeof(struct image) * 1);
	if (!img)
		return NULL; /// sa dai free la img !
	img->width = w;
	img->height = h;
	img->type = type;
	img->color = color;
	if (color) { /// P6, P3
		img->pixels = malloc(3 * sizeof(unsigned char **));
		for (int i = 0; i < 3; i++) {
			img->pixels[i] = malloc(h * sizeof(unsigned char *));
			for (int j = 0; j < h; j++)
				img->pixels[i][j] = malloc(w * sizeof(unsigned char));
		}
	} else {
		img->pixels = malloc(1 * sizeof(unsigned char **));
		img->pixels[0] = malloc(h * sizeof(unsigned char *));
		for (int j = 0; j < h; j++)
			img->pixels[0][j] = malloc(w * sizeof(unsigned char));
	}
	return img;
}

struct image *read(const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if (!file) {
		printf("Failed to load %s\n", filename);
		return NULL;
	}
	int width, height, type, ascii;
	fscanf(file, "P%d %d %d %d", &type, &width, &height, &ascii);
	char space;
	fscanf(file, "%c", &space);
	int color = (type == 3 || type == 6) ? 1 : 0;
	struct image *img = create(width, height, type, color);
	if (type < 4) {
		if (!color) {
			for (int c = 0; c < ((color == 1) ? 3 : 1); c++)
				for (int i = 0; i < height; i++)
					for (int j = 0; j < width; j++)
						fscanf(file, "%hhd", &img->pixels[c][i][j]);
		} else {
			for (int i = 0; i < height; i++)
				for (int j = 0; j < width; j++) {
					fscanf(file, "%hhd", &img->pixels[0][i][j]);
					fscanf(file, "%hhd", &img->pixels[1][i][j]);
					fscanf(file, "%hhd", &img->pixels[2][i][j]);
				}
		}
	} else {
		if (color == 1) {
			for (int i = 0; i < img->height; i++)
				for (int j = 0; j < img->width; j++) {
					fread(&img->pixels[0][i][j], 1, 1, file);
					fread(&img->pixels[1][i][j], 1, 1, file);
					fread(&img->pixels[2][i][j], 1, 1, file);
				}
		} else {
			for (int i = 0; i < img->height; i++)
				for (int j = 0; j < img->width; j++)
					fread(&img->pixels[0][i][j], 1, 1, file);
		}
	}
	fclose(file);
	printf("Loaded %s\n", filename);
	img->selection = 0;
	img->x1 = 0;
	img->y1 = 0;
	img->x2 = img->width;
	img->y2 = img->height;
	img->c = NULL;
	return img;
}

void coord_check(char *cuvant, int *param, int *coord)
{
	int digit = 1;
	char cuv[10];
	strcpy(cuv, cuvant);
	for (unsigned long i = 0; i < strlen(cuv); i++)
		if (cuv[i] >= 'a' && cuv[i] <= 'z')
			digit = 0;
	if (digit == 1)
		*param = atoi(cuvant);
	else
		*coord = 0;
}

void ord_coord(int *a, int *b)
{
	if (*a > *b) {
		int aux = *a;
		*a = *b;
		*b = aux;
	}
}

void selection(struct image *img, char prop[20])
{
	if (img) {
		char *cuvant = strtok(prop, "\n ");
		if (!strcmp(cuvant, "ALL")) {
			if (img) {
				img->x1 = 0;
				img->x2 = img->width;
				img->y1 = 0;
				img->y2 = img->height;
				printf("Selected ALL\n");
				img->selection = 1;
			} else {
				printf("No image loaded\n");
			}
		} else {
			int coord = 1;
			int x1_c, x2_c, y1_c, y2_c;
			if (cuvant) /// x1
				coord_check(cuvant, &x1_c, &coord);
			else
				coord = 0;
			cuvant = strtok(NULL, " ");
			if (cuvant && coord == 1) /// y1
				coord_check(cuvant, &y1_c, &coord);
			else
				coord = 0;
			cuvant = strtok(NULL, " ");
			if (cuvant && coord == 1) /// x2
				coord_check(cuvant, &x2_c, &coord);
			else
				coord = 0;
			cuvant = strtok(NULL, "\n");
			if (cuvant && coord == 1) /// y2
				coord_check(cuvant, &y2_c, &coord);
			else
				coord = 0;
			if (coord == 1) {
				ord_coord(&x1_c, &x2_c);
				ord_coord(&y1_c, &y2_c);
				int comp = 1;
				if (x1_c < 0 || y1_c < 0)
					comp = 0;
				if (x2_c > img->width || y2_c > img->height)
					comp = 0;
				if (x1_c == x2_c || y1_c == y2_c)
					comp = 0;
				if (comp == 1) {
					img->x1 = x1_c;
					img->x2 = x2_c;
					img->y1 = y1_c;
					img->y2 = y2_c;
					printf("Selected %d %d", img->x1, img->y1);
					printf(" %d %d\n", img->x2, img->y2);
					img->selection = 1;
			} else {
				printf("Invalid set of coordinates\n");
			}
			} else {
				printf("Invalid command\n");
			}
		}
	} else {
		printf("No image loaded\n");
	}
}

void hist_valid(struct image *img, int x, int y)
{
	if (img->type == 3 || img->type == 6) {
		printf("Black and white image needed\n");
	} else {
		double freq[256] = {0};
		for (int i = 0; i < img->height; i++)
			for (int j = 0; j < img->width; j++)
				freq[img->pixels[0][i][j]]++;
		int *hist = calloc(y, sizeof(double));
		if (!hist)
			printf("nu s-a alocat hist\n");
		int maxim = 0;
		for (int i = 0; i < y; i++) {
			for (int j = 0; j < 256 / y; j++)
				hist[i] = hist[i] + freq[(256 / y) * i + j];
			if (maxim < hist[i])
				maxim = hist[i];
		}
		for (int i = 0; i < y; i++) {
			float aux = (float)hist[i] / maxim;
			hist[i] = (int)(aux * x);
		}
		for (int i = 0; i < y; i++) {
			printf("%d\t|\t", hist[i]);
			for (int j = 0; j < hist[i]; j++)
				printf("*");
			printf("\n");
		}
		free(hist);
	}
}

void histogram(struct image *img, int *ok, char prop[20])
{
	*ok = 1;
	if (img) {
		if (!prop) {
			printf("Invalid command\n");
		} else {
			char *cuvant = strtok(prop, "\n ");
			int x, y;
			int count = 0;
			while (cuvant) {
				count++;
				if (count == 1)
					x = atoi(cuvant);
				if (count == 2)
					y = atoi(cuvant);
				cuvant = strtok(NULL, "\n ");
			}
			if (count == 2)
				hist_valid(img, x, y);
			else
				printf("Invalid command\n");
		}
	} else {
		printf("No image loaded\n");
	}
}

void save_ascii(int h, int w, int type, unsigned char ***pixels, char file[50])
{
	FILE *filename = fopen(file, "w");
	fprintf(filename, "P%d\n", type);
	fprintf(filename, "%d %d\n", w, h);
	fprintf(filename, "255\n");
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			fprintf(filename, "%d ", pixels[0][i][j]);
			if (type == 3 || type == 6)
				fprintf(filename, "%d %d ", pixels[1][i][j], pixels[2][i][j]);
		}
		fprintf(filename, "\n");
	}
	fclose(filename);
	printf("Saved %s\n", file);
}

void save_binary(int h, int w, int type, unsigned char ***pixels, char file[50])
{
	FILE *filename = fopen(file, "wb");
	fprintf(filename, "P%d\n", type);
	fprintf(filename, "%d %d\n", w, h);
	fprintf(filename, "255\n");
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			fwrite(&pixels[0][i][j], sizeof(unsigned char), 1, filename);
			if (type == 3 || type == 6) {
				fwrite(&pixels[1][i][j], sizeof(unsigned char), 1, filename);
				fwrite(&pixels[2][i][j], sizeof(unsigned char), 1, filename);
			}
		}
	}
	fclose(filename);
	printf("Saved %s\n", file);
}

void save(struct image *img, char prop[50])
{
	char file[50];
	int ok = 0;
	char *p = strtok(prop, "\n ");
	strcpy(file, p);
	p = strtok(NULL, "\n");
	if (p)
		ok = 1;
	if (img) {
		int h = img->height;
		int w = img->width;
		if (ok) {
			if (img->type < 4)
				save_ascii(h, w, img->type, img->pixels, file);
			else
				save_ascii(h, w, img->type - 3, img->pixels, file);
		} else {
			if (img->type < 4)
				save_binary(h, w, img->type + 3, img->pixels, file);
			else
				save_binary(h, w, img->type, img->pixels, file);
		}
	} else {
		printf("No image loaded\n");
	}
}

struct copy *create_copy(struct copy *c, int x1, int x2, int y1, int y2, int t)
{
	if (!c)
		c = malloc(1 * sizeof(struct copy));
	if (!c)
		printf("nu s-a alocat pt img.c\n");
	c->h = y2 - y1;
	c->w = x2 - x1;
	c->type = t;
	if (t % 3 == 0)
		c->p = malloc(sizeof(unsigned char **) * 3);
	else
		c->p = malloc(sizeof(unsigned char **));
	if (!c->p) {
		perror("Failed to allocate c->p");
		exit(EXIT_FAILURE);
	}
	for (int ch = 0; ch < ((t == 3 || t == 6) ? 3 : 1); ch++) {
		c->p[ch] = malloc(c->h * sizeof(unsigned char *));
		for (int i = 0; i < c->h; i++)
			c->p[ch][i] = malloc(c->w * sizeof(unsigned char));
	}
	if (!c->p[0]) {
		perror("Failed to allocate c->p[0]");
		exit(EXIT_FAILURE);
	}
	return c;
}

void free_copy(struct copy *c)
{
	for (int i = 0; i < ((c->type == 3 || c->type == 6) ? 3 : 1); i++) {
		for (int j = 0; j < c->h; j++)
			free(c->p[i][j]);
		free(c->p[i]);
	}
	free(c->p);
	free(c);
	c = NULL;
}

void copying(unsigned char ***a, unsigned char ***b, int h, int w, int type)
{
	for (int i = 0; i < ((type == 3 || type == 6) ? 3 : 1); i++)
		for (int j = 0; j < h; j++)
			for (int k = 0; k < w; k++)
				a[i][j][k] = b[i][j][k];
}

void crop(struct image *img)
{
	if (img) {
		img->c = malloc(1 * sizeof(struct copy));
		if (!img->c)
			printf("nu s-a alocat pt img.c\n");
		int type = img->type;
		img->c = create_copy(img->c, img->x1, img->x2, img->y1, img->y2, type);
		for (int i = 0; i < img->c->h; i++)
			for (int j = 0; j < img->c->w; j++) {
				img->c->p[0][i][j] = img->pixels[0][img->y1 + i][img->x1 + j];
				if (img->type == 3 || img->type == 6) {
					int lin = img->y1 + i;
					int col = img->x1 + j;
					img->c->p[1][i][j] = img->pixels[1][lin][col];
					img->c->p[2][i][j] = img->pixels[2][lin][col];
				}
			}
		img->c->h = img->y2 - img->y1;
		img->c->w = img->x2 - img->x1;
		img->c->pixeli = 1; /// imaginea cropata e in img.c
		for (int i = 0; i < ((img->type == 3 || img->type == 6) ? 3 : 1); i++) {
			for (int j = 0; j < img->height; j++)
				free(img->pixels[i][j]);
			free(img->pixels[i]);
		}
		free(img->pixels);
		if (img->color) { /// P6, P3
			int size = sizeof(unsigned char);
			img->pixels = malloc(3 * sizeof(unsigned char **));
			for (int i = 0; i < 3; i++) {
				img->pixels[i] = malloc(img->c->h * sizeof(unsigned char *));
				for (int j = 0; j < img->c->h; j++)
					img->pixels[i][j] = malloc(img->c->w * size);
			}
		} else {
			img->pixels = malloc(1 * sizeof(unsigned char **));
			img->pixels[0] = malloc(img->c->h * sizeof(unsigned char *));
			for (int j = 0; j < img->c->h; j++)
				img->pixels[0][j] = malloc(img->c->w * sizeof(unsigned char));
		}
		copying(img->pixels, img->c->p, img->c->h, img->c->w, img->type);
		img->height = img->c->h;
		img->width = img->c->w;
		free_copy(img->c);
		img->x1 = 0;
		img->y1 = 0;
		img->x2 = img->width;
		img->y2 = img->height;
		printf("Image cropped\n");
	} else {
		printf("No image loaded\n");
	}
}

void kernel_edge(double kernel[3][3])
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			if (i == 1 && j == 1)
				kernel[i][j] = 8;
			else
				kernel[i][j] = -1;
}

void kernel_sharpen(double kernel[3][3])
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			if (i == 1 && j == 1)
				kernel[i][j] = 5;
			else if (i + j == 1 || i + j == 3)
				kernel[i][j] = -1;
			else
				kernel[i][j] = 0;
		}
}

void kernel_blur(double kernel[3][3])
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			///kernel[i][j] = 1.0 / 9;
			kernel[i][j] = 1.0;
}

void kernel_g_blur(double kernel[3][3])
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++) {
			if (i == 1 && j == 1)
				kernel[i][j] = 0.25;
			else if (i + j == 1 || i + j == 3)
				kernel[i][j] = 0.125;
			else
				kernel[i][j] = 0.0625;
	}
}

void a(struct image *img, double k[3][3], unsigned char ***co, char com[50])
{
	for (int c = 0; c < 3; c++) {
		for (int i = img->y1; i < img->y2; i++) {
			for (int j = img->x1; j < img->x2; j++) {
				double result = 0;
				int h = img->height - 1;
				int w = img->width - 1;
				if (i != 0 && j != 0 && i != h && j != w) {
					result =
						1.0 * k[0][0] * (double)co[c][i - 1][j - 1] +
						1.0 * k[0][1] * (double)co[c][i - 1][j] +
						1.0 * k[0][2] * (double)co[c][i - 1][j + 1] +
						1.0 * k[1][0] * (double)co[c][i][j - 1] +
						1.0 * k[1][1] * (double)co[c][i][j] +
						1.0 * k[1][2] * (double)co[c][i][j + 1] +
						1.0 * k[2][0] * (double)co[c][i + 1][j - 1] +
						1.0 * k[2][1] * (double)co[c][i + 1][j] +
						1.0 * k[2][2] * (double)co[c][i + 1][j + 1];
				if (!strcmp(com, "BLUR")) /// round !
					result = result / 9;
				if (result > 255)
					result = 255;
				if (result < 0)
					result = 0;
				img->pixels[c][i][j] = result;
				}
			}
		}
	}
}

void apply_val(struct image *img, char com[50], double kernel[3][3])
{
	if (img->type == 3 || img->type == 6) {
		unsigned char ***copie;
		copie = malloc(sizeof(unsigned char **) * 3);
		for (int i = 0; i < 3; i++) {
			copie[i] = malloc(sizeof(unsigned char *) * img->height);
			for (int j = 0; j < img->height; j++)
				copie[i][j] = malloc(sizeof(unsigned char) * img->width);
		}
		copying(copie, img->pixels, img->height, img->width, img->type);
		a(img, kernel, copie, com);
		free_matrix(copie, img->type, img->height);
		printf("APPLY %s done\n", com);
	} else {
		printf("Easy, Charlie Chaplin\n");
	}
}

void apply(struct image *img, char prop[50])
{
	if (img) {
		if (prop[0] == '\0') {
			printf("Invalid command\n");
		} else {
			char *cuvant = strtok(prop, " ");
			char com[50];
			int count = 0;
			while (cuvant) {
				count++;
				if (count == 1)
					strcpy(com, cuvant);
				cuvant = strtok(NULL, "\n ");
			}
			int okay = 0;
			double kernel[3][3] = {0};
			if (!strcmp(com, "EDGE")) {
				okay = 1;
				kernel_edge(kernel);
			}
			if (!strcmp(com, "SHARPEN")) {
				okay = 1;
				kernel_sharpen(kernel);
			}
			if (!strcmp(com, "BLUR")) {
				okay = 1;
				kernel_blur(kernel);
			}
			if (!strcmp(com, "GAUSSIAN_BLUR")) {
				okay = 1;
				kernel_g_blur(kernel);
			}
			if (okay && count == 1)
				apply_val(img, com, kernel);
			else
				printf("APPLY parameter invalid\n");
		}
	} else {
		printf("No image loaded\n");
	}
}

void equalize(struct image *img)
{
	if (img) {
		if (img->type == 3 || img->type == 6) {
			printf("Black and white image needed\n");
		} else {
			double area = img->height * img->width;
			int freq[256] = {0}, suma = 0;
			for (int i = 0; i < img->height; i++)
				for (int j = 0; j < img->width; j++)
					freq[img->pixels[0][i][j]]++;
			int eq[256] = {0};
			for (int i = 0; i < 256; i++) {
				suma = suma + freq[i];
				eq[i] = 255 / area * suma;
			}
			for (int i = 0; i < img->height; i++)
				for (int j = 0; j < img->width; j++)
					img->pixels[0][i][j] = eq[img->pixels[0][i][j]];
			printf("Equalize done\n");
		}
	} else {
		printf("No image loaded\n");
	}
}

void rotate_cleanup(struct image *img, int all, int w, int h)
{
	if (all == 1) {
		int type = img->type;
		int color = ((type == 3 || type == 6) ? 1 : 0);
		for (int c = 0; c < ((color == 1) ? 3 : 1); c++) {
			for (int i = 0; i < img->height; i++)
				free(img->pixels[c][i]);
			free(img->pixels[c]);
			img->pixels[c] = malloc(h * sizeof(unsigned char *));
			for (int i = 0; i < h; i++)
				img->pixels[c][i] = malloc(w * sizeof(unsigned char));
		}
		img->x1 = 0;
		img->y1 = 0;
		img->x2 = w;
		img->y2 = h;
		img->height = h;
		img->width = w;
	}

	for (int c = 0; c < ((img->type % 3 == 0) ? 3 : 1); c++)
		for (int i = img->y1; i < img->y2; i++)
			for (int j = img->x1; j < img->x2; j++) {
				int lin = i - img->y1;
				int col = j - img->x1;
				img->pixels[c][i][j] = img->c->p[c][lin][col];
			}
	img->c->h = h;
	free_copy(img->c);
}

void rotate_matrix(unsigned char ***matrix, int h, int w, int type)
{ /// o data la dreapta
	int ***aux = malloc(3 * sizeof(int **));
	for (int c = 0; c < 3 ; c++) {
		aux[c] = malloc(w * sizeof(int *));
		for (int i = 0; i < w; i++)
			aux[c][i] = malloc(h * sizeof(int));
	}

	for (int c = 0; c < ((type == 3 || type == 6) ? 3 : 1); c++)
		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++) {
			///printf("[c][i][j] = [%d][%d][%d]\n", c, i, j);
				aux[c][i][j] = matrix[c][h - 1 - j][i]; /// formula ??
			}

	for (int c = 0; c < ((type == 3 || type == 6) ? 3 : 1); c++) {
		for (int i = 0; i < h; i++)
			free(matrix[c][i]);
		free(matrix[c]);
		///matrix[c] = NULL;
		matrix[c] = malloc(w * sizeof(unsigned char *));
		for (int i = 0; i < w; i++)
			matrix[c][i] = malloc(h * sizeof(unsigned char));
		}

	for (int c = 0; c < ((type == 3 || type == 6) ? 3 : 1); c++)
		for (int i = 0; i < w; i++)
			for (int j = 0; j < h; j++)
				matrix[c][i][j] = aux[c][i][j];

	for (int c = 0; c < 3 ; c++) {
		for (int i = 0; i < w; i++)
			free(aux[c][i]);
		free(aux[c]);
	}
	free(aux);
}

void rot_copy(struct image *img)
{
	img->c = (struct copy *)malloc(1 * sizeof(struct copy));
	if (!img->c)
		printf("nu s-a alocat pt img.c\n");
	int type = img->type;
	img->c = create_copy(img->c, img->x1, img->x2, img->y1, img->y2, type);
		/// w = x2 - x1 si h = y2 - y1
	for (int c = 0; c < ((img->type == 3 || img->type == 6) ? 3 : 1); c++)
		for (int i = 0; i < img->y2 - img->y1; i++)
			for (int j = 0; j < img->x2 - img->x1; j++) {
				int lin = i + img->y1;
				int col = j + img->x1;
				img->c->p[c][i][j] = img->pixels[c][lin][col];
			}
}

void rotate(struct image *img, int unghi)
{
	if (img) {
		int all = 1;
		if (img->x1 != 0 || img->y1 != 0)
			all = 0;
		if (img->x2 != img->width || img->y2 != img->height)
			all = 0;
		if ((img->y2 - img->y1 == img->x2 - img->x1) || all == 1) {
			if (unghi % 90) {
				printf("Unsupported rotation angle\n");
			} else {
				rot_copy(img);
				int unghi_poz = unghi;
				if (unghi < 0)
					unghi_poz = 360 + unghi;
				int o = unghi_poz / 90;
				int h = img->y2 - img->y1;
				int w = img->x2 - img->x1;
				while (o != 0) {
					o--;
					rotate_matrix(img->c->p, h, w, img->type);
					/// w = y2 - y1 si h = x2 - x1
					int aux = h;
					h = w;
					w = aux;
				}
				rotate_cleanup(img, all, w, h);
				printf("Rotated %d\n", unghi);
			}
		} else {
			printf("The selection must be square\n");
		}
	} else {
		printf("No image loaded\n");
	}
}

int main(void)
{
	char comanda[15];
	char filename[25], sep[3] = "\n ";
	int done = 0, ok = 0;
	struct image *img = NULL;
	while (done == 0) {
		char comm_line[100];
		fgets(comm_line, 100, stdin);
		char *d = strtok(comm_line, sep);
		strcpy(comanda, d);
		ok = 0;
		if (!strcmp(comanda, "LOAD")) {
			if (img)
				free_img(img);
			d = strtok(NULL, "\n");
			strcpy(filename, d);
			img = read(filename);
			ok = 1;
		}
		if (!strcmp(comanda, "SELECT")) {
			char prop[20];
			strcpy(prop, comm_line + strlen("SELECT "));
			selection(img, prop);
			ok = 1;
		}
		if (!strcmp(comanda, "HISTOGRAM")) {
			char prop[20];
			d = strtok(NULL, "\n");
			ok = 1;
			if (img && !d)
				ok = 0;
			if (!img && !d)
				printf("No image loaded\n");
			if (d) {
				strcpy(prop, d);
				histogram(img, &ok, prop);
			}
		}
		if (!strcmp(comanda, "SAVE")) {
			d = strtok(NULL, "\n");
			char prop[50];
			strcpy(prop, d);
			save(img, prop); ok = 1;
		}
		if (!strcmp(comanda, "CROP")) {
			crop(img);
			ok = 1;
		}
		if (!strcmp(comanda, "EXIT")) {
			if (!img)
				printf("No image loaded\n");
			done = 1;
			ok = 1;
		}
		if (!strcmp(comanda, "APPLY")) {
			d = strtok(NULL, "\n");
			char prop[50];
			if (d)
				strcpy(prop, d);
			else
				prop[0] = '\0';
			apply(img, prop);
			ok = 1;
		}
		if (!strcmp(comanda, "EQUALIZE"))
			equalize(img), ok = 1;
		if (!strcmp(comanda, "ROTATE")) {
			d = strtok(NULL, "\n");
			char cuv[5];
			strcpy(cuv, d);
			rotate(img, atoi(cuv));
			ok = 1;
		}
		if (!ok)
			printf("Invalid command\n");
	}
	if (img)
		free_img(img);
	return 0;
}
