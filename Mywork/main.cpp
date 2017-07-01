



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

#define BLOCK_W 100//�u���b�N
#define BLOCK_H 50//�u���b�N
#define SPEED 20//�{�[���X�s�[�h
//�u���b�N�̍\����
struct BLOCKS {
	int x;
	int y;
	int flag;
};

struct BLOCKS block;
//�u���b�N�쐬
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
	int ball_x = 0, ball_y = 0;//�{�[���̈ړ�
	int x_direction = 0;//x�̕���
	int  y_direction = 0;//y�̕���

	vector<cv::Mat> channels;

	int count = 0;	//���F�̉�f���J�E���g
	int x_sum = 0, y_sum = 0; //���F�����̍��W�̍��v�B
	int x_ave = 0, y_ave = 0; //���W�̑��a�̕���
	int x_ave_pre = 0, y_ave_pre = 0; //���W�̑��a�̕���

	VideoCapture cap(0);

	if (!cap.isOpened())
	{
		//�ǂݍ��ݎ��s
		return -1;
	}

	cap >> img_src;
	Mat dst(img_src.rows, img_src.cols, CV_8U(1));//���F���o�̃}�X�N�摜

	while (1) {
		cap >> img_src;

		cvtColor(img_src, src_hsv, CV_BGR2HSV);//HSV�ϊ�

		split(src_hsv, channels);//3�`�����l���ɕ���

		//�e�`�����l���̒l
		//���F............255
		//����ȊO........0 �Ƃ���

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
		img_src.copyTo(img_skin, dst); //�O�����ō�����}�X�N���g���Ĕ��F�̕����������J�����摜����؂���B

		imshow("img_src", img_src);//�J�����摜�\��
		

	    //���F������T��
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
		//�u���b�N����
		make_blocks();
		if (block.flag == 1) {
			rectangle(img_src, Point(block.x, block.y), Point(block.x+BLOCK_W, block.y+BLOCK_H), Scalar(0, 255, 0), -1, CV_AA);//�u���b�N��`�`��
		}
		
        //��ʉ������Ƀ��P�b�g�����蔧�F�����o���ē�����
		rectangle(img_src, Point(x_ave-60, 400), Point(x_ave+60, 480), Scalar(0, 0, 255), -1, CV_AA);//�`��(��ʂ̉��Ƀo�[�����)

		//////////////////////////�{�[���̏���//////////////////////////////////////////
		//x_direction = 0 .......����
		//y_direction = 1 .......����

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

		///////////////////�t���[���̓����蔻��//////////////////////////
		//ball_x,ball_y............x�͈̔� 0�`640 y�͈̔�0�`480
		//                  �{�[���̔��a 15
		///////////////////�u���b�N�̓����蔻��//////////////////////////////
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

		//�u���b�N�̓����蔻��
		if (block.flag == 1) {
			if (ball_x >= block.x && ball_x <=  block.x + BLOCK_W) {
				if (ball_y == block.y) {
					y_direction = 1;
					block.flag = 0;
					putText(img_src, "hit!!!", Point(250, 310), CV_FONT_HERSHEY_TRIPLEX, 1.2, Scalar(0, 100, 100), 2, CV_AA);//�u���b�N�ɓ������hit�ƕ\��
				}
				else if (ball_y == block.y + BLOCK_H) {
					y_direction = 0;
					block.flag = 0;
					putText(img_src, "hit!!!", Point(250, 310), CV_FONT_HERSHEY_TRIPLEX, 1.2, Scalar(0, 100, 100), 2, CV_AA);//�u���b�N�ɓ������hit�ƕ\��
				}

			}
			if (ball_y >= block.x && ball_y <= block.y + BLOCK_H) {
				if (ball_x == block.x) {
					y_direction = 1;
					block.flag = 0;
					putText(img_src, "hit!!!", Point(250, 310), CV_FONT_HERSHEY_TRIPLEX, 1.2, Scalar(0, 100, 100), 2, CV_AA);//�u���b�N�ɓ������hit�ƕ\��
				}
				else if (ball_x == block.x + BLOCK_H) {
					y_direction = 0;
					block.flag = 0;
					putText(img_src, "hit!!!", Point(250, 310), CV_FONT_HERSHEY_TRIPLEX, 1.2, Scalar(0, 100, 100), 2, CV_AA);//�u���b�N�ɓ������hit�ƕ\��
				}
			}
		}
				
		imshow("hit", img_src);//�J�����摜�\��

		if (waitKey(1) == 'q') {  //q�L�[�ŏI��
			break;
		}
		else if (waitKey(1) == 's') {
			imwrite(file_dst, img_src);
		}

	}

	return 0;
}
