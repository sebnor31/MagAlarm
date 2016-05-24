// TestCalibration.cpp : Defines the entry point for the console application.
// Calibration for the sensors
// Author: Sahadat
// Email: m.n.sahadat@gmail.com
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int main(void)
{
	double datai1[2000][15];
	string data[2000][15];
	unsigned int nrow = 2000, ncol = 15;
	
	ifstream myfile("calibration.txt");
	ofstream outp1("projection1.txt");
	ofstream outp2("projection2.txt");
	ofstream outp3("projection3.txt");
	ofstream outp4("projection4.txt");

	cout << "The program is running" << endl;

	// copying file to a 2D array
	if (myfile.is_open())
	{
		for (unsigned int i = 0; i < nrow;i++)
		{
			for (unsigned int j = 0; j < ncol; j++)
			{
				myfile >> data[i][j];
	
				datai1[i][j] = stod(data[i][j],NULL);
				//cout << datai1[i][j] << ' ';
			}
			
			//cout << endl;
			
		}
		myfile.close();
		
		Mat CalData = Mat(2000, 15, CV_64FC1, datai1);				// initialize matrix with the 2D array

		//cout << CalData;
		// get the submatrixes from the main matrix
		Mat sen1(CalData(Range(0, 1500), Range(0, 3)));
		Mat sen2(CalData(Range(0, 1500), Range(3, 6)));
		Mat sen3(CalData(Range(0, 1500), Range(6, 9)));
		Mat sen4(CalData(Range(0, 1500), Range(9, 12)));
		Mat sen5(CalData(Range(0, 1500), Range(12, 15)));
		Mat onem(1500, 1, CV_64FC1, 1);

		//cout << onem << endl;
		// LSE operation to generate matrixes
		Mat w(sen5.rows, sen5.cols + 1, CV_64FC1);
		hconcat(onem,sen5,w);									// w = [refsensor ones(#r,1)]
		Mat proj1 = (((w.t())*w).inv(DECOMP_SVD))*(w.t())*sen1; // proj1 = ((wT*w)-1)*wT*sensor
		Mat proj2 = (((w.t())*w).inv(DECOMP_SVD))*(w.t())*sen2;
		Mat proj3 = (((w.t())*w).inv(DECOMP_SVD))*(w.t())*sen3;
		Mat proj4 = (((w.t())*w).inv(DECOMP_SVD))*(w.t())*sen4;
		
		// saving to the files
		outp1 <<proj1<<endl;
		outp2 << proj2 << endl;
		outp3 << proj3 << endl;
		outp4 << proj4 << endl;

		// closing files
		outp1.close();
		outp2.close();
		outp3.close();
		outp4.close();
		
	}
	else
	{
		cout << "can't open the file" << endl;
	}

	cout << "Calculation completed" << endl;

	return 0;
}

