#include "stdafx.h"
#include "TrafficLightIdentify.h"


CTrafficLightIdentify::CTrafficLightIdentify()
{
	square3x3 = Mat(3, 3, CV_8U, cv::Scalar(1));

	square5x5 = Mat(5, 5, CV_8U, cv::Scalar(1));
	square7x7 = Mat(7, 7, CV_8U, cv::Scalar(1));
	squareEle = Mat(11, 11, CV_8U, cv::Scalar(1));
	cross3x3 = Mat(3, 3, CV_8U, cv::Scalar(0));
	diamond = Mat(5, 5, CV_8U, cv::Scalar(1));
	diamond.at<uchar>(0, 0) = 0;
	diamond.at<uchar>(0, 1) = 0;
	diamond.at<uchar>(1, 0) = 0;
	diamond.at<uchar>(4, 4) = 0;
	diamond.at<uchar>(3, 4) = 0;
	diamond.at<uchar>(4, 3) = 0;
	diamond.at<uchar>(4, 0) = 0;
	diamond.at<uchar>(4, 1) = 0;
	diamond.at<uchar>(3, 0) = 0;
	diamond.at<uchar>(0, 4) = 0;
	diamond.at<uchar>(0, 3) = 0;
	diamond.at<uchar>(1, 4) = 0;
	cross3x3.at<uchar>(0, 1) = 1;
	cross3x3.at<uchar>(1, 0) = 1;
	cross3x3.at<uchar>(1, 1) = 1;
	cross3x3.at<uchar>(1, 2) = 1;
	cross3x3.at<uchar>(2, 1) = 1;
	cross5x5 = Mat(5, 5, CV_8U, cv::Scalar(0));
	for (int i = 0; i < 5; i++)	{
		cross5x5.at<uchar>(2, i) = 1;
		cross5x5.at<uchar>(i, 2) = 1;
	}

	thr_areaMin = 10;
	thr_areaMax = 200;
	thr_circle = 0.5;
	thr_correlation = 0.7;
	thr_diff = 30;
}


CTrafficLightIdentify::~CTrafficLightIdentify()
{
}


void CTrafficLightIdentify::extractROI(Mat& inMat, Mat& outMat)
{
	if (inMat.channels() != 3)
	{
		AfxMessageBox(_T("请输入3通道彩色图像"));
		return;
	}
	outMat = Mat::zeros(inMat.size(), CV_8U);
	Mat outRedGreeYelloMat = Mat::zeros(inMat.size(), CV_8UC3);

	//extract ROI according to RGB normalization: R./(R+G+B)
	/*float sumRGB;
	float R, G, B;
	unsigned char* outMaskData = (unsigned char*)outMat.data;
	unsigned char* rowImgData;
	for (int i = 0; i < inMat.rows; i++)
	{
		rowImgData = inMat.ptr<unsigned char>(i);
		for (int j = 0; j < inMat.cols; j++)
		{
			B = (float)rowImgData[3 * j];
			G = (float)rowImgData[3 * j + 1];
			R = (float)rowImgData[3 * j + 2];
			
			sumRGB = B + G + R + 0.001;
			B /= sumRGB;
			G /= sumRGB;
			R /= sumRGB;
			
			bool flag1 = R > 0.7843 && B < 0.5882;
			bool flag2 = R < 0.5882 && G > 0.9412 && B > 0.8627;
			if (flag1 || flag2)
				*outMaskData = 255;
			outMaskData++;
		}
	}*/

	//extract ROI according to HSV
	Mat hsvMat;
	cvtColor(inMat, hsvMat, CV_BGR2HSV);

	float H, S;
	bool flagRed, flagGree, flagYello;
	unsigned char* outMaskData = (unsigned char*)outMat.data;
	unsigned char* rowImgData;
	unsigned char* rowRedGreeYelloData;
	for (int i = 0; i < inMat.rows; i++)
	{
		rowImgData = hsvMat.ptr<unsigned char>(i);
		rowRedGreeYelloData = outRedGreeYelloMat.ptr<unsigned char>(i);
		for (int j = 0; j < inMat.cols; j++)
		{
			H = (float)rowImgData[3 * j] / 180.0;
			S = (float)rowImgData[3 * j + 1] / 255.0;

			flagRed = ((H >= 0 && H <= 0.05) || (H >= 0.9 && H <= 1)) && (S >= 0.05);
			flagGree = ((H >= 0.48 && H <= 0.6)) && (S >= 0.2);
			flagYello = ((H >= 0.05 && H <= 0.2)) && (S >= 0.05);
			if (flagRed) rowRedGreeYelloData[3 * j] = 255;
			if (flagGree) rowRedGreeYelloData[3 * j + 1] = 255;
			if (flagYello) rowRedGreeYelloData[3 * j + 2] = 255;

			if (flagGree)// || flagGree || flagYello)
				*outMaskData = 255;
			outMaskData++;
		}
	}
}

void CTrafficLightIdentify::Preprocess_equalizeHist(Mat& equaled_img)
{
	Mat img_channels[3];

	split(m_inMat, img_channels);
	equalizeHist(img_channels[0], img_channels[0]);
	equalizeHist(img_channels[1], img_channels[1]);
	equalizeHist(img_channels[2], img_channels[2]);
	merge(img_channels, 3, equaled_img);
}

void CTrafficLightIdentify::extractROI(Mat& equaled_img, Mat& redMat, Mat& greenMat, Mat& blackMat)
{
	redMat = Mat(equaled_img.rows, equaled_img.cols, CV_8UC1);
	greenMat = Mat(equaled_img.rows, equaled_img.cols, CV_8UC1);
	blackMat = Mat(equaled_img.rows, equaled_img.cols, CV_8UC1);

	Mat result(equaled_img.rows, equaled_img.cols, CV_32FC3);

	Mat_<Vec3b>::iterator it = equaled_img.begin<Vec3b>();
	Mat_<Vec3f>::iterator resultIt = result.begin<Vec3f>();
	Mat_<Vec3b>::iterator itend = equaled_img.end<Vec3b>();
	Mat_<Vec3f>::iterator resultItend = result.end<Vec3f>();
	Mat_<uchar>::iterator blackIt = blackMat.begin<uchar>();
	Mat_<uchar>::iterator blackItend = blackMat.end<uchar>();
	int sum = 0;
	for (; it != itend&&resultIt != resultItend; it++, resultIt++, blackIt++) {
		sum = (*it)[0] + (*it)[1] + (*it)[2];
		(*resultIt)[0] = (float)(*it)[0] / sum;
		(*resultIt)[1] = (float)(*it)[1] / sum;
		(*resultIt)[2] = (float)(*it)[2] / sum;
		if ((*it)[0] < 40 && (*it)[1] < 40 && (*it)[2] < 40) {
			(*blackIt) = 255;
		}
		else {
			(*blackIt) = 0;
		}
	}

	resultIt = result.begin<Vec3f>();
	it = equaled_img.begin<Vec3b>();
	Mat_<uchar>::iterator redIt = redMat.begin<uchar>();
	Mat_<uchar>::iterator redItend = redMat.end<uchar>();
	Mat_<uchar>::iterator greenIt = greenMat.begin<uchar>();
	Mat_<uchar>::iterator greenItend = greenMat.end<uchar>();

	for (; resultIt != resultItend&&redIt != redItend&&greenIt != greenItend; ++resultIt, ++redIt, ++greenIt) {
		if (((*resultIt)[2] - (*resultIt)[1]) > 0.16 && ((*resultIt)[1] - (*resultIt)[0]) < 0.08 && ((*resultIt)[1] < 0.30)) {
			(*redIt) = 255;
		}
		else {
			(*redIt) = 0;
		}
		if (((*resultIt)[2] - (*resultIt)[1]) < /*-0.16*/ -0.16 && ((*resultIt)[1] - (*resultIt)[0]) > /*0.18*/ 0.004 && ((*resultIt)[1] > 0.10)) {
			(*greenIt) = 255;
		}
		else {
			(*greenIt) = 0;
		}
	}

	morphologyEx(redMat, redMat, MORPH_OPEN, square3x3);
	morphologyEx(greenMat, greenMat, MORPH_OPEN, square3x3);

	morphologyEx(redMat, redMat, MORPH_CLOSE, diamond);
	morphologyEx(greenMat, greenMat, MORPH_CLOSE, diamond);
	morphologyEx(blackMat, blackMat, MORPH_CLOSE, square5x5);
	morphologyEx(blackMat, blackMat, MORPH_OPEN, square3x3);
}

void CTrafficLightIdentify::chooseLights_AreaCircleRegiongrow(Mat& equaled_img, Mat& gray_img, Mat& templateMat, Mat& colourMat, VecPotentialLightRects& colour_potentialLightRects, vector<Rect>& rectVec_trafficLights)
{
	rectVec_trafficLights.clear();

	Rect myRect;
	float lightarea;
	bool areaCondition, circleCondition;
	for (int i = 0; i < colour_potentialLightRects.size(); i++)
	{
		lightarea = float(colour_potentialLightRects[i].current_area);
		areaCondition = lightarea >= thr_areaMin && lightarea <= thr_areaMax;
		myRect = colour_potentialLightRects[i].current_rect;
		circleCondition = (max(myRect.width, myRect.height)) / (min(myRect.width, myRect.height)) < 2 && (lightarea / (float)(myRect.area())) >= thr_circle;
		
		//if (areaCondition && circleCondition){
		if (areaCondition && circleCondition && templateMatchCondition(equaled_img, templateMat, colourMat, myRect)){
		//if (areaCondition && circleCondition && expandCondition(gray_img, colourMat, myRect) && templateMatchCondition(equaled_img, templateMat, colourMat, myRect)){
			rectVec_trafficLights.push_back(myRect);
		}
	}

}

bool CTrafficLightIdentify::expandCondition(Mat& gray_img, Mat& colourMat, Rect lightRect)
{
	bool flag = false;

	Rect showRect;
	showRect.x = max(lightRect.x - 3, 0);
	showRect.y = max(0, lightRect.y - lightRect.height - 8);
	showRect.width = min(lightRect.width + 6, gray_img.cols - showRect.x);
	showRect.height = min(lightRect.height + 6, gray_img.rows - showRect.y);
	Mat outlineMat = gray_img(showRect).clone();
	Mat lightMat = gray_img(lightRect).clone();



	//Mat outMat = gray_img(showRect);
	//resize(outMat, outMat, Size(640, 480));
	//imshow("outline", outMat);
	//Mat outMat1 = gray_img(lightRect);
	//resize(outMat1, outMat1, Size(640, 480));
	//imshow("light", outMat1);
	//waitKey();

	

	double outlineMeanV = mean(outlineMat)[0];
	double lightMeanV = mean(lightMat)[0];
	double diffV = lightMeanV - outlineMeanV;

	//Mat img_thresold, img_binary;
	//threshold(gray_img, img_thresold, 0, 255, THRESH_OTSU);
	//morphologyEx(img_thresold, img_binary, cv::MORPH_CLOSE, squareEle);
	//img_binary = 255 - img_binary;
	//img_binary = img_binary / 255;
	//double ratioArea = mean(img_binary(showRect))[0];

	if (diffV > 0 ) //&& ratioArea > 0.9)
		flag = true;

	return flag;
}

bool CTrafficLightIdentify::templateMatchCondition(Mat& equaled_img, Mat& templateMat, Mat& colourMat, Rect lightRect)
{
	bool flag = false;

	Rect showRect;
	showRect.x = max(lightRect.x - 3, 0);
	showRect.y = max(0, lightRect.y - lightRect.height - 8);
	showRect.width = min(lightRect.width + 6, equaled_img.cols - showRect.x);
	showRect.height = min(lightRect.height + 6, equaled_img.rows - showRect.y);

	MatND histMat;
	calcMatROIHist(equaled_img, showRect, histMat);

	double maxVal = 0;

	Mat correlationM = templateMat * histMat.t();	
	minMaxLoc(correlationM, 0, &maxVal);
	if (maxVal >= thr_correlation)
		flag = true;

	//Mat subhistMat;
	//double g_dCompareRecult;
	//for (int i = 0; i < templateMat.rows; i++)
	//{
	//	subhistMat = templateMat.row(i);
	//	g_dCompareRecult = compareHist(histMat, subhistMat, CV_COMP_INTERSECT);

	//	if (g_dCompareRecult > maxVal)
	//		maxVal = g_dCompareRecult;
	//}
	//if (maxVal > 500)
	//	flag = true;

	return flag;
}

void CTrafficLightIdentify::extractTrafficLightsRects(Mat& inMat, Mat& templateMat, vector<vector<Rect>>& rectVec_trafficLights, Mat& redMat, Mat& greenMat)
{
	if (inMat.channels() != 3)
	{
		AfxMessageBox(_T("请输入3通道彩色图像"));
		return;
	}

	m_inMat.release();
	rectVec_trafficLights.clear();
	inMat.copyTo(m_inMat);  //set m_inMat

	Mat equaled_img; 
	Mat blackMat;

	Preprocess_equalizeHist(equaled_img);  //set equaled_img
	extractROI(equaled_img, redMat, greenMat, blackMat);     //set   redMat, greenMat, blackMat

	Mat gray_img;
	cvtColor(equaled_img, gray_img, CV_BGR2GRAY);

	//set red_potentialLightRects and green_potentialLightRects
	VecPotentialLightRects red_potentialLightRects;
	VecPotentialLightRects green_potentialLightRects;
	FindPotentialLightsBoundingRects(redMat, red_potentialLightRects);
	FindPotentialLightsBoundingRects(greenMat, green_potentialLightRects);


	vector<Rect> final_RedLightsRects;
	vector<Rect> final_GreenLightsRects;

	chooseLights_AreaCircleRegiongrow(equaled_img, gray_img, templateMat, redMat, red_potentialLightRects, final_RedLightsRects);
	chooseLights_AreaCircleRegiongrow(equaled_img, gray_img, templateMat, greenMat, green_potentialLightRects, final_GreenLightsRects);
	//imshow("img", blackMat);
	
	rectVec_trafficLights.push_back(final_RedLightsRects);
	rectVec_trafficLights.push_back(final_GreenLightsRects);
}

void CTrafficLightIdentify::FindtrafficLightOutlineRects(Mat& equaled_img, vector<Rect>& rectVec_trafficLightOutline)
{
	rectVec_trafficLightOutline.clear();

	Mat gray_img;
	Mat img_thresold;
	Mat img_binary;
	cvtColor(equaled_img, gray_img, CV_BGR2GRAY);
	threshold(gray_img, img_thresold, 0, 255, cv::THRESH_OTSU);
	
	morphologyEx(img_thresold, img_binary, cv::MORPH_CLOSE, squareEle);
	img_binary = 255 - img_binary;

	VecPotentialLightRects potentialOutlineRects;
	FindPotentialLightsBoundingRects(img_binary, potentialOutlineRects);

	int cmin = 160;  // minimum contour length
	int cmax = 400000; // maximum contour length
	VecPotentialLightRects::const_iterator itc = potentialOutlineRects.begin();
	while (itc != potentialOutlineRects.end()) 
	{
		if (itc->current_area >= cmin && itc->current_area <= cmax) 
		{
			float rectanglerity = (float)(itc->current_area / itc->current_rect.area());
			float mythreshlod = 0.75;

			if (rectanglerity > mythreshlod && itc->current_rect.height > itc->current_rect.width) {

				rectVec_trafficLightOutline.push_back(itc->current_rect);
			}
		}
	}
}

void CTrafficLightIdentify::FindPotentialLightsBoundingRects(Mat& inMat, VecPotentialLightRects& potentialLightRects)
{
	PotentialLightRectsPara lightsRectsStruc;
	Rect LightRect;

	int i, j;
	int loffset;
	int pixel;

	int lWidth = inMat.cols;
	int lHeight = inMat.rows;
	Mat flag = Mat::zeros(inMat.size(), CV_32S);

	int lMark = Eightliantong(inMat, lWidth, lHeight, flag);
	lightsRectsStruc.max_mask = lMark;
	
	
	for (i = 1; i <= lMark; i++)
	{
		Rect boundingRect;
		lightsRectsStruc.current_area = LocateRect(flag, i, boundingRect);
		lightsRectsStruc.current_mask = i;
		lightsRectsStruc.current_rect = boundingRect;

		potentialLightRects.push_back(lightsRectsStruc);
	}
}

int CTrafficLightIdentify::LocateRect(Mat& inMat, int mask, Rect& bondingRect)
{
	int i, j;
	int num = 0;
	int lHeight = inMat.rows;
	int lWidth = inMat.cols;
	int colmin = lWidth, colmax = 0, rowmin = lHeight, rowmax = 0;

	int* flag;
	for (i = 0; i < lHeight; i++)
	{
		flag = inMat.ptr<int>(i);
		for (j = 0; j < lWidth; j++)
		{
			if (*(flag+j) == mask)
			{
				num++;
				if (colmin > j) colmin = j;
				if (colmax < j) colmax = j;
				if (rowmin > i) rowmin = i;
				if (rowmax < i) rowmax = i;
			}
		}
	}
	bondingRect.x = max(colmin-1, 0);
	bondingRect.y = max(rowmin-1, 0);
	bondingRect.width = min(colmax - colmin + 2, inMat.cols - bondingRect.x);
	bondingRect.height = min(rowmax - rowmin + 2, inMat.rows - bondingRect.y);

	return num;
}

/*************************************************************************
* 函数名称：
*   Eightliantong()
* 参数:
*	LPBYTE                   - 图像数据指针
*	int                      - 图像宽度
*	int                      - 图像高度
*   int*                     - 标记位指针
* 返回值:
*   int                      - 区域数目
* 功能:
*   该函数用于八连通域分析，标记区域。
************************************************************************/
void CTrafficLightIdentify::Insert(struct EightAreaTeam *head, int yi, int xi)//入队
{
	struct EightAreaTeam *a, *b;
	a = (struct EightAreaTeam *)malloc(sizeof(struct EightAreaTeam));
	a->x = xi;
	a->y = yi;
	a->next = NULL;
	b = head;
	while (b->next != NULL) b = b->next;
	b->next = a;
}

void CTrafficLightIdentify::Delete(struct EightAreaTeam *head)//出队
{
	struct EightAreaTeam *a;
	a = head->next;
	head->next = a->next;
	delete(a);
}

int CTrafficLightIdentify::Eightliantong(Mat& inMat, int wth, int hth, Mat& flagMat)
{
	int pixel = 255;
	int num = 0;//计数，最后返回值
	int a = 0, b = 0;
	int i, j;

	struct EightAreaTeam *head;
	head = (struct EightAreaTeam *)malloc(sizeof(struct EightAreaTeam));
	head->next = NULL;

	unsigned char* data = (unsigned char*)inMat.data;
	int* flag = (int*)flagMat.data;
	for (i = 0; i<hth; i++)
	{
		for (j = 0; j<wth; j++)
		{
			if (flag[i*wth + j] == 0 && *(data + i*wth + j) == pixel)//是否已经属于别的8连通&&是否灰度是pixelzhi
			{
				Insert(head, i, j);
				num++;
				while (head->next != NULL)
				{
					a = head->next->x;
					b = head->next->y;
					if (b != 0)//不是在y方向上边界上
					{
						if (flag[(b - 1)*wth + a] == 0 && *(data + (b - 1)*wth + a) == pixel)
						{
							Insert(head, b - 1, a);
							flag[(b - 1)*wth + a] = num;//标记
						}
						if (a != 0)//不是在x方向左边界上
						{
							if (flag[(b - 1)*wth + a - 1] == 0 && *(data + (b - 1)*wth + a - 1) == pixel)
							{
								Insert(head, b - 1, a - 1);
								flag[(b - 1)*wth + a - 1] = num;
							}
							if (flag[b*wth + a - 1] == 0 && *(data + b*wth + a - 1) == pixel)
							{
								Insert(head, b, a - 1);
								flag[b*wth + a - 1] = num;
							}

						}

						if (a != wth - 1)//右边界
						{
							if (flag[(b - 1)*wth + a + 1] == 0 && *(data + (b - 1)*wth + a + 1) == pixel)
							{
								Insert(head, b - 1, a + 1);
								flag[(b - 1)*wth + a + 1] = num;
							}
							if (flag[b*wth + a + 1] == 0 && *(data + b*wth + a + 1) == pixel)
							{
								Insert(head, b, a + 1);
								flag[b*wth + a + 1] = num;
							}
						}

					}

					if (b != hth - 1)//y方向的下边界
					{
						if (flag[(b + 1)*wth + a] == 0 && *(data + (b + 1)*wth + a) == pixel)
						{
							Insert(head, b + 1, a);
							flag[(b + 1)*wth + a] = num;
						}
						if (a != 0)
						{
							if (flag[(b + 1)*wth + a - 1] == 0 && *(data + (b + 1)*wth + a - 1) == pixel)
							{
								Insert(head, b + 1, a - 1);
								flag[(b + 1)*wth + a - 1] = num;
							}
							if (flag[b*wth + a - 1] == 0 && *(data + b*wth + a - 1) == pixel)
							{
								Insert(head, b, a - 1);
								flag[b*wth + a - 1] = num;
							}
						}

						if (a != wth - 1)
						{
							if (flag[(b + 1)*wth + a + 1] == 0 && *(data + (b + 1)*wth + a + 1) == pixel)
							{
								Insert(head, b + 1, a + 1);
								flag[(b + 1)*wth + a + 1] = num;
							}
							if (flag[b*wth + a + 1] == 0 && *(data + b*wth + a + 1) == pixel)
							{
								Insert(head, b, a + 1);
								flag[b*wth + a + 1] = num;
							}
						}

					}
					Delete(head);
				}
			}
		}
	}
	return num;
}

void CTrafficLightIdentify::draw_showResultImg(Mat& resultMat, vector<vector<Rect>>& rectVec_trafficLights)
{
	m_inMat.copyTo(resultMat);
	Rect tmpRect;

	vector<Rect> trafficRedLightRect = rectVec_trafficLights[0];
	for (int i = 0; i < trafficRedLightRect.size(); i++) {
		tmpRect = trafficRedLightRect[i];
		//rectangle(img_result, tmpRect, Scalar(0, 0, 255), 3);
		Point center(tmpRect.x + tmpRect.width / 2, tmpRect.y + tmpRect.height / 2);
		circle(resultMat, center, tmpRect.width * 3, Scalar(255, 0, 255), 3);
	}

	vector<Rect> trafficGreenLightRect = rectVec_trafficLights[1];
	for (int i = 0; i < trafficGreenLightRect.size(); i++) {
		tmpRect = trafficGreenLightRect[i];
		//rectangle(img_result, tmpRect, Scalar(0, 255, 0), 3);
		Point center(tmpRect.x + tmpRect.width / 2, tmpRect.y + tmpRect.height / 2);
		circle(resultMat, center, tmpRect.width * 3, Scalar(255, 255, 0), 3);
	}

}

void CTrafficLightIdentify::calcMatROIHist(Mat& inMat, Rect rectROI, MatND& hisMat)
{
	if (inMat.channels() != 3)
	{
		AfxMessageBox(_T("请输入3通道彩色图像"));
		return;
	}

	Mat img_roi = inMat(rectROI).clone();
	resize(img_roi, img_roi, Size(64, 48));

	Mat result;
	cvtColor(img_roi, result, CV_BGR2HSV);
	MatND histM;
	// Quantize the hue to 30 levels
	// and the saturation to 32 levels
	int hbins = 30, sbins = 32;
	int histSize[] = { hbins, sbins };
	// hue varies from 0 to 179, see cvtColor
	float hranges[] = { 0, 180 };
	// saturation varies from 0 (black-gray-white) to
	// 255 (pure spectrum color)
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	// we compute the histogram from the 0-th and 1-st channels
	int channels[] = { 0, 1 };

	calcHist(&result, 1, channels, Mat(), // do not use mask
		histM, 2, histSize, ranges,
		true, // the histogram is uniform
		false);

	normalize(histM, histM);  //L2 normalization

	hisMat = histM.reshape(0, 1);
}

void CTrafficLightIdentify::mergeRows(Mat& A, Mat& B)
{
	if (A.cols != B.cols || A.type() != B.type())
		return;
	int totalRows = A.rows + B.rows;

	Mat mergedDescriptors = Mat(totalRows, A.cols, A.type());
	Mat submat = mergedDescriptors.rowRange(0, A.rows);
	A.copyTo(submat);
	submat = mergedDescriptors.rowRange(A.rows, totalRows);
	B.copyTo(submat);
	A = mergedDescriptors;
}

void CTrafficLightIdentify::setMat(Mat& inMat)
{
	if (inMat.empty())
		return;
	m_inMat.release();
	inMat.copyTo(m_inMat);  //set m_inMat
}