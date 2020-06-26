#include <stdio.h>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <tesseract/baseapi.h>
#include <opencv2/photo.hpp>
#include <string>
#include "sudoku.h"

using namespace std;
using namespace cv;

Mat img, img_proc, game_img;
int contour_thresh = 255;

void findContour();
void preProcess(char* path);
game_wrapper extract_board();

int main(int argc, char** argv) {

	if (argc != 2) {
		cout << "Please supply an image as second argument" << endl;
		return -1;
	}

	preProcess(argv[1]);
	findContour();
	solve(extract_board());
}

void preProcess(char* path) {

	srand(time(NULL));
	img = imread(path, 1);
	img_proc = img.clone();
	cvtColor(img_proc, img_proc, COLOR_BGR2GRAY);
	fastNlMeansDenoising(img_proc, img_proc, 16, 11, 7);
	img_proc.convertTo(img_proc, -1, 3, 0);
	medianBlur(img_proc, img_proc, 9);
	adaptiveThreshold(img_proc, img_proc, contour_thresh,
		ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 5, 2);
	bitwise_not(img_proc, img_proc);
	uint kernel_data[25] = { 0, 1, 1, 1, 0,
							 1, 1, 1, 1, 1, 
							 1, 1, 1, 1, 1,
							 1, 1, 1, 1, 1,
							 0, 1, 1, 1, 0 };
	Mat kernel = Mat(5, 5, CV_8U, kernel_data);
	dilate(img_proc, img_proc, kernel, Point(-1,-1),1);
	float r = (float)img_proc.cols / img_proc.rows;
	resize(img_proc, img_proc, Size(floor(600*r), 600));
	imshow("post", img_proc);
	waitKey(0);

}

int getDistance(Point p1, Point p2) {
	
	double x_dist = abs(p1.x - p2.x);
	double y_dist = abs(p1.y - p2.y);

	return ceil(sqrt(x_dist * x_dist + y_dist * y_dist));
}

void findContour() {
	Mat canny_out;
	vector<vector<Point>> contours;

	findContours(img_proc, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	sort(contours.begin(), contours.end(), [](const vector<Point>& c1, const vector<Point>& c2) {
		return contourArea(c1, false) < contourArea(c2, false); 
	});

	vector<Point> gameContour = contours[contours.size() - 1];

	//find corners of contour
	Point top_left = Point(10000, 10000);
	Point top_right = Point(0, 10000);
	Point bot_left = Point(10000, 0);
	Point bot_right = Point(0, 0);

	for (Point p : gameContour) {
		int coordSum = p.x + p.y;
		int coordDif = p.x - p.y;

		if (coordSum < (top_left.x + top_left.y)) top_left = p;
		if (coordDif > (top_right.x - top_right.y)) top_right = p;
		if (coordSum >= (bot_right.x + bot_right.y)) bot_right = p;
		if (coordDif <= (bot_left.x - bot_right.y)) bot_left = p;

	}

	vector<int> side_lengths = {getDistance(top_left, top_right), getDistance(top_right, bot_right),
		getDistance(bot_right, bot_left), getDistance(bot_left, top_left)};
	int max_side_length = *max_element(side_lengths.begin(), side_lengths.end());

	Point2f src[3] = { top_left, top_right, bot_left };
	Point2f transf[3] = { Point(0,0), Point(max_side_length-1, 0), Point(0, max_side_length-1) };
	Mat transf_mat = getPerspectiveTransform(src, transf);
	warpPerspective(img_proc, game_img, transf_mat, Size(max_side_length, max_side_length), 
		INTER_LINEAR, BORDER_CONSTANT);

	imshow("postwarp", game_img);
	waitKey(0);

}

game_wrapper extract_board() {

	cv::resize(game_img, game_img, Size(450,450));
	Mat square;

	game_wrapper game;
 
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {

			square = game_img(Rect((j*50)+5, (i*50)+5, 40, 40));
			cv::resize(square, square, Size(200,200));
			bitwise_not(square, square);

			tesseract::TessBaseAPI* ocr = new tesseract::TessBaseAPI();
			ocr->Init(NULL, "digits", tesseract::OEM_LSTM_ONLY);
			ocr->SetPageSegMode(tesseract::PSM_SINGLE_WORD);
			ocr->SetImage(square.data, square.cols, square.rows, 1, square.step);
			string digit = string(ocr->GetUTF8Text());
			cout << "digit: " << digit << endl;
			imshow("number", square);
			waitKey(0);
			if (digit == "") {
				game.board.at(i).at(j).val = 0;
			} else {
				game.board.at(i).at(j).val = stoi(digit);
				game.board.at(i).at(j).valid_vals = {game.board.at(i).at(j).val};
			}
		}
	}

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << game.board.at(i).at(j).val << "\t";
		}
		cout << endl;
	}

	return game;
}