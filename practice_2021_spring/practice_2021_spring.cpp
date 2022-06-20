#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include <cmath>
using namespace cv;
using namespace std;
const double PI = 3.141592653589793238462;

void set_resolution(const int w, const int h, int& w1, int& h1, double& f_gr, double& f_rad, double& f) {
	// преобразуем угол поворота, чтобы он был от 0 до 360 градусов
	while (f_gr < 0) {
		f_gr += 360;
	}
	int int_f = int(f_gr);
	f_gr -= int_f;
	int_f = int_f % 360;
	f_gr += int_f;
	f = PI * f_gr / 180.0; // перевод в радианы
	if (f_gr == 0 || f_gr == 180) {
		w1 = w;
		h1 = h;
	}
	else if (f_gr == 90 || f_gr == 270) {
		w1 = h;
		h1 = w;
	}
	else {
		if (f_gr < 90) {
			w1 = abs(w * cos(f) + h * sin(f) + 1);// размеры нового изображения
			h1 = abs(w * sin(f) + h * cos(f) + 1);
		}
		else if (f_gr < 180) {
			f_gr = 90 - (f_gr - 90);
			f_rad = PI * f_gr / 180.0; // перевод в радианы
			w1 = abs(w * cos(f_rad) + h * sin(f_rad) + 1);// размеры нового изображения
			h1 = abs(w * sin(f_rad) + h * cos(f_rad) + 1);
		}
		else if (f_gr < 270) {
			f_gr = f_gr - 180;
			f_rad = PI * f_gr / 180.0; // перевод в радианы
			w1 = abs(w * cos(f_rad) + h * sin(f_rad) + 1);// размеры нового изображения
			h1 = abs(w * sin(f_rad) + h * cos(f_rad) + 1);
		}
		else {
			f_gr = 360 - f_gr;
			f_rad = PI * f_gr / 180.0; // перевод в радианы
			w1 = abs(w * cos(f_rad) + h * sin(f_rad) + 1);// размеры нового изображения
			h1 = abs(w * sin(f_rad) + h * cos(f_rad) + 1);
		}
	}
}

void rotation_180(const int h, const int w, Mat rotation, Mat image) {
	for (int i = 0; i < h; i++) { // вывод 
		for (int j = 0; j < w; j++) {
			rotation.at<Vec3b>(h - i - 1, w - j - 1)[0] = image.at<Vec3b>(i, j)[0];
			rotation.at<Vec3b>(h - i - 1, w - j - 1)[1] = image.at<Vec3b>(i, j)[1];
			rotation.at<Vec3b>(h - i - 1, w - j - 1)[2] = image.at<Vec3b>(i, j)[2];
		}
	}
}

void rotation_90(const int h, const int w, Mat rotation, Mat image) {
	for (int i = 0; i < h; i++) { // вывод 
		for (int j = 0; j < w; j++) {
			rotation.at<Vec3b>(j, h - i - 1)[0] = image.at<Vec3b>(i, j)[0];
			rotation.at<Vec3b>(j, h - i - 1)[1] = image.at<Vec3b>(i, j)[1];
			rotation.at<Vec3b>(j, h - i - 1)[2] = image.at<Vec3b>(i, j)[2];
		}
	}
}

void rotation_270(const int h, const int w, Mat rotation, Mat image) {
	for (int i = 0; i < h; i++) { // вывод 
		for (int j = 0; j < w; j++) {
			rotation.at<Vec3b>(w - j - 1, i)[0] = image.at<Vec3b>(i, j)[0];
			rotation.at<Vec3b>(w - j - 1, i)[1] = image.at<Vec3b>(i, j)[1];
			rotation.at<Vec3b>(w - j - 1, i)[2] = image.at<Vec3b>(i, j)[2];
		}
	}
}

void count_pixel_centers(double*** centre, const int h, const int w, const double h1_w2, const double w1_h2, const double x1, const double y1) {
	double x_, x__, y_, y__; // координаты центра пикселя из первого столбца для создания таблицы центров пикселей
	for (int i = 0; i < h; i++) {
		centre[i] = new double* [w];
		for (int j = 0; j < w; j++) {
			centre[i][j] = new double[2];
		}
	}
	x_ = x1;
	x__ = x1;
	y_ = y1;
	y__ = y1;
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			centre[i][j][0] = x_;
			centre[i][j][1] = y_;
			x_ = x_ + w1_h2;
			y_ = y_ + h1_w2;
		}
		y__ += w1_h2;
		x__ -= h1_w2;
		x_ = x__;
		y_ = y__;
	}
}

void calculate_angles(double f, int h, int w, int h1, int w1, double& cx, double& cy, double& cx1, double& cy1, 
	double& x1, double& y1, double& x2, double& y2, double& x3, double& y3, double& x4, double& y4, double& h1_w2, double& w1_h2) {
	cx1 = w1 / 2;
	cy1 = h1 / 2;// центр  повернутого изображения
	cx = w / 2;
	cy = h / 2; // центр исходного изображения
	x1 = cx1 + (0.5 - cx) * cos(f) - (0.5 - cy) * sin(f); // x1 - y4 - координаты углов картинки после поворота
	y1 = cy1 + (0.5 - cy) * cos(f) + (0.5 - cx) * sin(f);
	x2 = cx1 + (w - 0.5 - cx) * cos(f) - (0.5 - cy) * sin(f);
	y2 = cy1 + (0.5 - cy) * cos(f) + (w - 0.5 - cx) * sin(f);
	x3 = cx1 + (0.5 - cx) * cos(f) - (h - 0.5 - cy) * sin(f);
	y3 = cy1 + (h - 0.5 - cy) * cos(f) + (0.5 - cx) * sin(f);
	x4 = cx1 + (w - 0.5 - cx) * cos(f) - (h - 0.5 - cy) * sin(f);
	y4 = cy1 + (h - 0.5 - cy) * cos(f) + (w - 0.5 - cx) * sin(f);
	h1_w2 = (y2 - y1) / (w - 1); // разница между центрами пикселей
	w1_h2 = (y3 - y1) / (h - 1); // разница между центрами пикселей
}

void write_matrix(double*** centre, const int h, const int w, Mat rotation, Mat image) {
	
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			rotation.at<Vec3b>(int(centre[i][j][1]), int(centre[i][j][0]))[0] = image.at<Vec3b>(i, j)[0];
			rotation.at<Vec3b>(int(centre[i][j][1]), int(centre[i][j][0]))[1] = image.at<Vec3b>(i, j)[1];
			rotation.at<Vec3b>(int(centre[i][j][1]), int(centre[i][j][0]))[2] = image.at<Vec3b>(i, j)[2];
		}
	}

}

void remove_black_dots(int w1, int h1, Mat rotation) {
	bool** black_p = new bool* [h1];// инициализация массива черных точек
	for (int i = 0; i < h1; i++) {
		black_p[i] = new bool[w1];
	}

	for (int i = 0; i < h1; i++) { // обнуление массива
		for (int j = 0; j < w1; j++) {
			black_p[i][j] = false;
		}
	}

	//поиск черных точек
	for (int i = 1; i < h1 - 1; i++) {
		for (int j = 1; j < w1 - 1; j++) {
			if (rotation.at<Vec3b>(i, j)[0] == 0 && rotation.at<Vec3b>(i, j)[1] == 0 && rotation.at<Vec3b>(i, j)[2] == 0) {
				if ((rotation.at<Vec3b>(i, j - 1)[0] != 0 || rotation.at<Vec3b>(i, j - 1)[1] != 0 || rotation.at<Vec3b>(i, j - 1)[2] != 0) &&
					(rotation.at<Vec3b>(i, j + 1)[0] != 0 || rotation.at<Vec3b>(i, j + 1)[1] != 0 || rotation.at<Vec3b>(i, j + 1)[2] != 0)) {
					black_p[i][j] = true;
				}
			}
		}
	}

	//изменение черных точек интерполяцией
	int num_p = 0, sum_r = 0, sum_g = 0, sum_b = 0;
	for (int i = 1; i < h1 - 1; i++) {
		for (int j = 1; j < w1 - 1; j++) {
			if (black_p[i][j]) {

				if (!black_p[i - 1][j - 1]) {
					sum_r += rotation.at<Vec3b>(i - 1, j - 1)[0];
					sum_g += rotation.at<Vec3b>(i - 1, j - 1)[1];
					sum_b += rotation.at<Vec3b>(i - 1, j - 1)[2];
					num_p += 1;
				}
				if (!black_p[i - 1][j]) {
					sum_r += rotation.at<Vec3b>(i - 1, j)[0];
					sum_g += rotation.at<Vec3b>(i - 1, j)[1];
					sum_b += rotation.at<Vec3b>(i - 1, j)[2];
					num_p += 1;
				}
				if (!black_p[i - 1][j + 1]) {
					sum_r += rotation.at<Vec3b>(i - 1, j + 1)[0];
					sum_g += rotation.at<Vec3b>(i - 1, j + 1)[1];
					sum_b += rotation.at<Vec3b>(i - 1, j + 1)[2];
					num_p += 1;
				}
				if (!black_p[i][j - 1]) {
					sum_r += rotation.at<Vec3b>(i, j - 1)[0];
					sum_g += rotation.at<Vec3b>(i, j - 1)[1];
					sum_b += rotation.at<Vec3b>(i, j - 1)[2];
					num_p += 1;
				}
				if (!black_p[i][j + 1]) {
					sum_r += rotation.at<Vec3b>(i, j + 1)[0];
					sum_g += rotation.at<Vec3b>(i, j + 1)[1];
					sum_b += rotation.at<Vec3b>(i, j + 1)[2];
					num_p += 1;
				}
				if (!black_p[i + 1][j - 1]) {
					sum_r += rotation.at<Vec3b>(i + 1, j - 1)[0];
					sum_g += rotation.at<Vec3b>(i + 1, j - 1)[1];
					sum_b += rotation.at<Vec3b>(i + 1, j - 1)[2];
					num_p += 1;
				}
				if (!black_p[i + 1][j]) {
					sum_r += rotation.at<Vec3b>(i + 1, j)[0];
					sum_g += rotation.at<Vec3b>(i + 1, j)[1];
					sum_b += rotation.at<Vec3b>(i + 1, j)[2];
					num_p += 1;
				}
				if (!black_p[i + 1][j + 1]) {
					sum_r += rotation.at<Vec3b>(i + 1, j + 1)[0];
					sum_g += rotation.at<Vec3b>(i + 1, j + 1)[1];
					sum_b += rotation.at<Vec3b>(i + 1, j + 1)[2];
					num_p += 1;
				}

				rotation.at<Vec3b>(i, j)[0] = sum_r / num_p;
				rotation.at<Vec3b>(i, j)[1] = sum_g / num_p;
				rotation.at<Vec3b>(i, j)[2] = sum_b / num_p;

				num_p = 0;
				sum_r = 0;
				sum_g = 0;
				sum_b = 0;
			}
		}
	}

	for (int i = 0; i < h1; i++) {
		delete[] black_p[i];
	}
	delete[] black_p;
}

void del_centre(double*** centre, int h, int w) {
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			delete[] centre[i][j];
		}
		delete[] centre[i];
	}
	delete[] centre;
}

int main()
{
	setlocale(LC_ALL, "Russian");
	string imageName;
	Mat image;
	//imageName = "cat.jpg";
	cout << "Введите имя исходного файла: ";
	cin >> imageName;
	image = imread(imageName.c_str(), IMREAD_COLOR);
	if (image.empty())
	{
		cout << "Ошибка открытия файла " << endl;
		return -1;
	}
	const int w = image.size().width;
	const int h = image.size().height;
	int h1, w1;
	double f_gr; // угол поворота в градусах
	double f, f_rad; // угол поворота в радианах
	cout << "Введите угол поворота: ";
	cin >> f_gr;
	set_resolution(w, h, w1, h1, f_gr, f_rad, f);
	Mat rotation(h1, w1, CV_8UC3, Scalar(0, 0, 0));
	if (f_gr == 0) {
		rotation = image;
	}
	else if (f_gr == 180) {
		rotation_180(h, w, rotation, image);
	}
	else if (f_gr == 90) {
		rotation_90(h, w, rotation, image);
	}
	else if (f_gr == 270) {
		rotation_270(h, w, rotation, image);
	}
	else {
		double cx, cy; // центр исходного изображения
		double cx1, cy1;// центр  повернутого изображения
		double x1, y1, x2, y2, x3, y3, x4, y4; // x1 - y4 - координаты углов картинки после поворота
		double h1_w2, w1_h2; // разница между центрами пикселей
		//расчет координат углов повернутого изображения
		calculate_angles(f, h, w, h1, w1, cx, cy, cx1, cy1, x1, y1, x2, y2, x3, y3, x4, y4, h1_w2, w1_h2);

		double*** centre = new double** [h];// инициализация массива центров пикселей
		// вычисляем координаты центров пикселей
		count_pixel_centers(centre, h, w, h1_w2, w1_h2, x1, y1);
		// запись в матрицу изображения
		write_matrix(centre, h, w, rotation, image);
		// удаляем черные точки
		remove_black_dots(w1, h1, rotation);
		//очистка памяти
		del_centre(centre, h, w);
	}

	imshow("rotation", rotation);

	imshow("image", image);

	imwrite("./output.jpg", rotation);
	waitKey(0);
	return 0;
}