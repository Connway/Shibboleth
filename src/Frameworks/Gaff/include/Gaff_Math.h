/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#pragma once

#include "Gaff_Defines.h"

NS_GAFF

constexpr float Pi = 3.141592654f;
constexpr float DegreesToRadians = Pi / 180.0f;
constexpr float RadianToDegrees = 180.0f / Pi;
constexpr float TurnsToDegrees = 360.0f;
constexpr float TurnsToRadians = 2.0f * Pi;
constexpr float DegreesToTurns = 1.0f / 360.0f;
constexpr float RadiansToTurns = 1.0f / (2.0f * Pi);
constexpr float Epsilon = 0.0001f;

template <class T>
constexpr const T& Min(const T& v1, const T& v2)
{
	return (v1 < v2) ? v1 : v2;
}

template <class T>
constexpr const T& Min(const T& v)
{
	return v;
}

template <class First, class... Rest>
constexpr const First& Min(const First& first, const Rest&... rest)
{
	return Min(first, Min(rest...));
}


template <class T>
constexpr const T& Max(const T& v1, const T& v2)
{
	return (v1 > v2) ? v1 : v2;
}

template <class T>
constexpr const T& Max(const T& v)
{
	return v;
}

template <class First, class... Rest>
constexpr const First& Max(const First& first, const Rest&... rest)
{
	return Max(first, Max(rest...));
}


template <class T>
constexpr const T& Clamp(const T& val, const T& min_bound, const T& max_bound)
{
	return Min(Max(val, min_bound), max_bound);
}

template <class T>
constexpr const T& Saturate(const T& val)
{
	return Clamp(val, static_cast<T>(0), static_cast<T>(1));
}

template <class T>
constexpr T Lerp(const T& begin, const T& end, float t)
{
	return begin + t * (end - begin);
}

// Check if val is in range [min_val, max_val]
template <class T>
constexpr bool Between(const T& val, const T& min_val, const T& max_val)
{
	return val >= min_val && val <= max_val;
}

// Check if val is in range [min_val, max_val)
template <class T>
constexpr bool ValidIndex(const T& val, const T& size)
{
	return val >= 0 && val < size;
}

template <class T>
constexpr T Sign(T value)
{
	static_assert(std::is_signed<T>::value, "SignMask was not given a signed number.");
	return value < static_cast<T>(1) ? static_cast<T>(-1) : static_cast<T>(1);
}

float BinomialCoefficient(float n, float k);
float BernsteinPolynomial(float t, float n, float k);

float NormalizeAngleRadiansSigned(float angle); // Normalize angle to [-Pi, Pi]
float NormalizeAngleTurnsSigned(float angle); // Normalize angle to [-0.5, 0.5]
float NormalizeAngleRadians(float angle); // Normalize angle to [0, 2*Pi]
float NormalizeAngleTurns(float angle); // Normalize angle to [0, 1]

float CalculateFOVRadians(float sensor_size, float focal_length);
float CalculateFOVDegrees(float sensor_size, float focal_length);
float CalculateFOV(float sensor_size, float focal_length);

float FOVConvertHorizontalToVertical(float horiz_fov, float aspect_ratio);
float FOVConvertVerticalToHorizontal(float vert_fov, float aspect_ratio);

NS_END
