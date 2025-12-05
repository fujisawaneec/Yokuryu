#include "Spline.h"
#include <windows.h>
#include <sysinfoapi.h>

void Spline::Initialize() {
	isEnd = false;
	t = 0.0f;
}

Vector3 Spline::Update(std::vector<Vector3>& points, float val) {

	t = t + val;

	//timeRateが1.0f以上になったら、次の区間に進む
	if (t >= 1.0f) {
		if (startIndex < points.size() - 3) {

			startIndex++;

			t -= 1.0f;
			integer += 1.0f;
		}
		else {
			isEnd = true;
			t = 1.0f;
		}
	}

	Vector3 pos = SplinePosition(points, startIndex, t);

	t += (float)0.002;
	//float integer = (float)floor(val);
	//float few = (float)fmod(val, 1);

	//Vector3 pos = SplinePosition(points, (size_t)integer, few);

	//if (integer >= (float)points.size() - 3.0f) {
	//	isEnd = true;
	//}
	
	return pos;
}

Vector3 Spline::EnemyPosition(std::vector<Vector3>& points, float val)
{
	float integer = (float)floor(val);
	float few = (float)fmod(val, 1);

	Vector3 pos = SplinePosition(points, (size_t)integer, few);

	return pos;
}

Vector3 Spline::pointCal(std::vector<Vector3>& points) {

	const int INDEXMAX = (int)points.size() - 1;

	float nowPoint = (float)startIndex / (float)INDEXMAX;

	Vector3 pos = SplinePosition(points, startIndex, t);

	if (t >= 1) {
		startIndex++;
		t = 0;
	}
	else {
		t += 0.01f;
	}

	return pos;
}

void Spline::Reset()
{			 
	t = 0;
	integer = 0;
	startIndex = 1;
}

Vector3 Spline::SplinePosition(const std::vector<Vector3>& points, size_t startIndex, float t) {
	//補間すべき点
	size_t n = points.size() - 2;

	if (startIndex > n) return points[n];
	if (startIndex < 1) return points[1];

	//p0~p3の制御点を取得する
	Vector3 p0_ = points[startIndex - 1];
	Vector3 p1_ = points[startIndex];
	Vector3 p2_ = points[startIndex + 1];
	Vector3 p3_ = points[startIndex + 2];

	//Catmull-Rom の式による補間
	Vector3 position = 0.5 * (p1_ * 2 + (-p0_ + p2_) *
							  t + (p0_ * 2 - p1_ * 5 + p2_ * 4 - p3_) *
							  (t * t) + (-p0_ + p1_ * 3 - p2_ * 3 + p3_) *
							  (t * t * t));

	return position;
}