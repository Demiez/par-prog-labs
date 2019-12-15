// Lab07.cpp : ПРАКТИКА СТВОРЕННЯ ПАРАЛЕЛЬНИХ ПРОГРАМ (ПОБУДОВА КОЛЬОРОВОГО ЗОБРАЖЕННЯ МНОЖИНИ МАНДЕЛЬБРОТА)
// для побудови зображення планується використання формату файлу PPM.

#include <iostream>
#include <fstream> // для маніпуляції з файлами
#include <complex> // для складних чисельних значень

// Послідовний алгоритм формування кольорового зображення множини Мандельброта
using namespace std;

float width = 600; // Задамо розмір зображення
float height = 600;


int value(int x, int y) {
	// В допоміжному методі додаємо градієнт до зображення
	complex<float> point((float)x / width - 1.5, (float)y / height - 0.5);
	complex<float> z(0, 0);
	int nb_iter = 0;
	while (abs(z) < 2 && nb_iter <= 20) {
		z = z * z + point;
		nb_iter++;
	}
	if (nb_iter < 20)
		return (255 * nb_iter) / 20;
	else
		return 0;
}

void Mandelbrot()
{
	// Будемо використовувати червоний колір (RGB 255 0 0).
	ofstream my_Image("mandelbrot.ppm");


	if (my_Image.is_open()) {
		my_Image << "P3\n" << width << " " << height << " 255\n";
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				int val = value(i, j);
				my_Image << val << ' ' << 0 << ' ' << 0 << "\n";
			}
		}
		my_Image.close();
	}
	else
		cout << "Could not open the file";
}

int main() {
	// Файл mandelbrot.ppm буде збережено в корньовій папці проекту
	Mandelbrot();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu