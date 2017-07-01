



#define _USE_MATH_DEFINES
#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>

#ifdef _DEBUG

#pragma comment(lib, "opencv_imgproc2413d.lib")
#pragma comment(lib, "opencv_core2413d.lib")
#pragma comment(lib, "opencv_highgui2413d.lib")

#else

#pragma comment
(lib, "opencv_imgproc2413d.lib")
#pragma comment
(lib, "opencv_core2413d.lib")
#pragma comment
(lib, "opencv_highgui2413d.lib")

#endif // DEBUG


using namespace std;
using namespace cv;
string win_src = "src";
string win_dst = "dst";

#define BLOCK_W 100//ブロック
#define BLOCK_H 50//ブロック
#define SPEED 20//ボールスピード
//ブロックの構造体
struct BLOCKS {
	int x;
	int y;
	int flag;
};

struct BLOCKS block;
//ブロック作成
void make_blocks() {
	block.x = 500;
	block.y = 100;
	block.flag = 1;
}

int main(int argc, char **argv) {
	string file_dst = "image.png";
	Mat img_src;
	Mat src_hsv;
	Mat img_skin;
	int x, y;
	int ball_x = 0, ball_y = 0;//ボールの移動
	int x_direction = 0;//xの方向
	int  y_direction = 0;//yの方向

	vector<cv::Mat> channels;

	int count = 0;	//肌色の画素数カウント
	int x_sum = 0, y_sum = 0; //肌色部分の座標の合計。
	int x_ave = 0, y_ave = 0; //座標の総和の平均
	int x_ave_pre = 0, y_ave_pre = 0; //座標の総和の平均

	VideoCapture cap(0);

	if (!cap.isOpened())
	{
		//読み込み失敗
		return -1;
	}

	cap >> img_src;
	Mat dst(img_src.rows, img_src.cols, CV_8U(1));//肌色検出のマスク画像

	while (1) {
		cap >> img_src;

		cvtColor(img_src, src_hsv, CV_BGR2HSV);//HSV変換

		split(src_hsv, channels);//3チャンネルに分割

		//各チャンネルの値
		//肌色............255
		//それ以外........0 とする

		for (y = 0; y < channels[0].rows; y++) {
			for (x = 0; x < channels[0].cols; x++) {
				if (channels[0].data[y*channels[0].step + x] >= 0 && channels[0].data[y*channels[0].step + x] <= 15 &&
					channels[1].data[y*channels[0].step + x] >= 50 && channels[2].data[y*channels[0].step + x] >= 50) {
					dst.data[y*dst.step + x] = 255;
				}
				else {
					dst.data[y*dst.step + x] = 0;
				}
			}
		}

		img_skin = 0;
		img_src.copyTo(img_skin, dst); //前処理で作ったマスクを使って肌色の部分だけをカメラ画像から切り取る。

		imshow("img_src", img_src);//カメラ画像表示
		

	    //肌色部分を探す
		x_sum = 0;
		y_sum = 0;
		count = 0;
		for (y = 0; y < dst.rows; y++) {
			for (x = 0; x < dst.cols; x++) {
				if (dst.data[y * dst.step + x] == 255) {
					count++;
					x_sum += x;
					y_sum += y;
				}
			}
		}

		if (count == 0) count = 1;
		x_ave = x_sum / count;
		y_ave = y_sum / count;



		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//ブロック生成
		make_blocks();
		if (block.flag == 1) {
			rectangle(img_src, Point(block.x, block.y), Point(block.x+BLOCK_W, block.y+BLOCK_H), Scalar(0, 255, 0), -1, CV_AA);//ブロック矩形描画
		}
		
        //画面下部分にラケットをつくり肌色を検出して動かす
		rectangle(img_src, Point(x_ave-60, 400), Point(x_ave+60, 480), Scalar(0, 0, 255), -1, CV_AA);//描画(画面の下にバーを作る)

		//////////////////////////ボールの処理//////////////////////////////////////////
		//x_direction = 0 .......減少
		//y_direction = 1 .......増加

		circle(img_src, cvPoint(ball_x, ball_y), 15, cvScalar(255, 255, 0), -1, CV_AA);
		if (x_direction == 0) {
			ball_x += SPEED;
		}
		else {
			ball_x -= SPEED;
		}
		if (y_direction == 0) {
			ball_y += SPEED;
		}
		else {
			ball_y -= SPEED;
		}

		///////////////////フレームの当たり判定//////////////////////////
		//ball_x,ball_y............xの範囲 0～640 yの範囲0～480
		//                  ボールの半径 15
		///////////////////ブロックの当たり判定//////////////////////////////
		//
		if (ball_x >= 640) {
			x_direction = 1;
		}
		if (ball_x <= 0) {
			x_direction = 0;
		}
		if (ball_y >= 480 || (ball_y >= 385 && ball_x >= (x_ave - 60) && ball_x <= (x_ave+60))) {
			y_direction = 1;
		}
		if (ball_y <= 0) {
			y_direction = 0;
		}

		//ブロックの当たり判定
		if (block.flag == 1) {
			if (ball_x >= block.x && ball_x <=  block.x + BLOCK_W) {
				if (ball_y == block.y) {
					y_direction = 1;
					block.flag = 0;
					putText(img_src, "hit!!!", Point(250, 310), CV_FONT_HERSHEY_TRIPLEX, 1.2, Scalar(0, 100, 100), 2, CV_AA);//ブロックに当たればhitと表示
				}
				else if (ball_y == block.y + BLOCK_H) {
					y_direction = 0;
					block.flag = 0;
					putText(img_src, "hit!!!", Point(250, 310), CV_FONT_HERSHEY_TRIPLEX, 1.2, Scalar(0, 100, 100), 2, CV_AA);//ブロックに当たればhitと表示
				}

			}
			if (ball_y >= block.x && ball_y <= block.y + BLOCK_H) {
				if (ball_x == block.x) {
					y_direction = 1;
					block.flag = 0;
					putText(img_src, "hit!!!", Point(250, 310), CV_FONT_HERSHEY_TRIPLEX, 1.2, Scalar(0, 100, 100), 2, CV_AA);//ブロックに当たればhitと表示
				}
				else if (ball_x == block.x + BLOCK_H) {
					y_direction = 0;
					block.flag = 0;
					putText(img_src, "hit!!!", Point(250, 310), CV_FONT_HERSHEY_TRIPLEX, 1.2, Scalar(0, 100, 100), 2, CV_AA);//ブロックに当たればhitと表示
				}
			}
		}
				
		imshow("hit", img_src);//カメラ画像表示

		if (waitKey(1) == 'q') {  //qキーで終了
			break;
		}
		else if (waitKey(1) == 's') {
			imwrite(file_dst, img_src);
		}

	}

	return 0;
}
