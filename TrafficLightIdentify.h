#pragma once
#include "stdafx.h"

class CTrafficLightIdentify
{
public:
	CTrafficLightIdentify();
	~CTrafficLightIdentify();

	void setMat(Mat& inMat);
	void Preprocess_equalizeHist(Mat& equaled_img);

	void extractTrafficLightsRects(Mat& inMat, Mat& templateMat, vector<vector<Rect>>& rectVec_trafficLights, Mat& redMat, Mat& greenMat);
	void draw_showResultImg(Mat& resultMat, vector<vector<Rect>>& rectVec_trafficLights);
	void calcMatROIHist(Mat& inMat, Rect rectROI, Mat& hisMat);
	void mergeRows(Mat& A, Mat& B);

private:
	
	void extractROI(Mat& inMat, Mat& outMat);
	void extractROI(Mat& equaled_img, Mat& redMat, Mat& greenMat, Mat& blackMat);

	void FindtrafficLightOutlineRects(Mat& equaled_img, vector<Rect>& rectVec_trafficLightOutline);
	void FindPotentialLightsBoundingRects(Mat& inMat, VecPotentialLightRects& potentialLightRects);
	//八连通域分析，标记区域
	void Insert(struct EightAreaTeam *head, int yi, int xi);
	void Delete(struct EightAreaTeam  *head);
	int Eightliantong(Mat& inMat, int wth, int hth, Mat& flagMat);
	int LocateRect(Mat& inMat, int mask, Rect& bondingRect);

	void chooseLights_AreaCircleRegiongrow(Mat& equaled_img, Mat& gray_img, Mat& templateMat, Mat& colourMat, VecPotentialLightRects& colour_potentialLightRects, vector<Rect>& rectVec_trafficLights);
	bool expandCondition(Mat& gray_img, Mat& colourMat, Rect lightRect);
	bool templateMatchCondition(Mat& equaled_img, Mat& templateMat, Mat& colourMat, Rect lightRect);

private:
	Mat square3x3;
	Mat square5x5;
	Mat square7x7;
	Mat squareEle;
	Mat cross3x3;
	Mat cross5x5;
	Mat xElement;
	Mat diamond;

	int thr_areaMin;
	int thr_areaMax;
	float thr_circle;
	double thr_correlation;
	double thr_diff;
	Mat m_inMat;

};

