#pragma once

#include "Vector3.h"
#include <Vector>

class Spline 
{
public:
	//初期化
	void Initialize();

	//更新
	Vector3 Update(std::vector<Vector3>& points,float val);

	Vector3 EnemyPosition(std::vector<Vector3>& points, float val);

	//更新
	Vector3 pointCal(std::vector<Vector3>& points);

	void Reset();

	//制御点の集合(vectorコンテナ)、補間する区間の添字、時間経過率
	Vector3 SplinePosition(const std::vector<Vector3>& point, size_t startIndex, float t);

	bool GetIsEnd() { return isEnd; }
	float GetT() { return t + integer; }

private:
	//P1からスタートする
	size_t startIndex = 1;
	bool isEnd = false;
	float t = 0;
	float integer = 0;


	//球の位置
	Vector3 position;
};