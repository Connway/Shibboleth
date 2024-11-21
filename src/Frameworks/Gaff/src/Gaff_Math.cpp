/************************************************************************************
Copyright (C) by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#include "Gaff_Math.h"
#include <cmath>

NS_GAFF

float BinomialCoefficient(float n, float k)
{
	return tgammaf(n + 1.0f) / (tgammaf(k + 1.0f) * tgammaf(n - k + 1.0f));
}

float BernsteinPolynomial(float t, float n, float k)
{
	return BinomialCoefficient(n, k) * powf(t, k) * powf(1.0f - t, n - k);
}

float NormalizeAngleRadiansSigned(float angle)
{
	angle += Pi;
	angle -= floor(angle / (2.0f * Pi)) * (2.0f * Pi);
	angle -= Pi;

	return angle;
}

float NormalizeAngleTurnsSigned(float angle)
{
	angle += 0.5f;
	angle -= floor(angle);
	angle -= 0.5f;

	return angle;
}

float NormalizeAngleRadians(float angle)
{
	angle = fmod(angle, 2.0f * Pi);

	if (angle < 0.0f) {
		angle = 2.0f * Pi + angle;
	}

	return angle;
}

float NormalizeAngleTurns(float angle)
{
	angle = fmod(angle, 1.0f);

	if (angle < 0.0f) {
		angle = 1.0f + angle;
	}

	return angle;
}

float CalculateFOVRadians(float sensor_size, float focal_length)
{
	return 2.0f * atan(0.5f * sensor_size / focal_length);
}

float CalculateFOVDegrees(float sensor_size, float focal_length)
{
	return CalculateFOVRadians(sensor_size, focal_length) * RadianToDegrees;
}

float CalculateFOV(float sensor_size, float focal_length)
{
	return CalculateFOVRadians(sensor_size, focal_length) * RadiansToTurns;
}

float FOVConvertHorizontalToVertical(float horiz_fov, float aspect_ratio)
{
	return 2.0f * atan(tan(horiz_fov * 0.5f) * aspect_ratio);
}

float FOVConvertVerticalToHorizontal(float vert_fov, float aspect_ratio)
{
	return 2.0f * atan(tan(vert_fov * 0.5f) / aspect_ratio);
}

NS_END
