/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Gleam_MeshGeneration.h"
#include <Gaff_Assert.h>
#include <Gaff_Math.h>
#include <cmath>
#include <cfenv>

NS_GLEAM

void GenerateDebugCylinder(int32_t subdivisions, Vector<glm::vec3>& points, Vector<int16_t>& indices, bool close_off)
{
	const int32_t offset = (close_off) ? 1 : 0;
	const int32_t num_indices = (subdivisions * 6) * (offset + 1);

	points.resize(static_cast<size_t>(subdivisions + offset) * 2);
	indices.resize(num_indices);

	for (int32_t i = 0; i < subdivisions; ++i) {
		const float scale = static_cast<float>(i) / (subdivisions - 1);
		const float x = cosf(2.0f * Gaff::Pi * scale);
		const float z = sinf(2.0f * Gaff::Pi * scale);

		points[2 * offset + subdivisions + i] = glm::vec3(x, -0.5f, z);
		points[offset + i] = glm::vec3(x, 0.5f, z);
	}

	// Generate points to close off the cylinder.
	if (close_off) {
		points[subdivisions + 1] = glm::vec3(0.0f, -0.5f, 0.0f);
		points[0] = glm::vec3(0.0f, 0.5f, 0.0f);
	}

	// Front face clockwise. Current row is top, next row is bottom.
	// *---*
	// | \ |
	// |  \|
	// *---*
	for (int32_t i = 0; i < subdivisions; ++i) {
		const int32_t top_right = (i + 1) % subdivisions + offset;
		const int32_t top_left = i + offset;
		const int32_t bottom_right = top_right + subdivisions + offset;
		const int32_t bottom_left = top_left + subdivisions + offset;

		int32_t index = i * 6 + offset * subdivisions * 3;

		indices[index] = static_cast<int16_t>(bottom_left);
		indices[index + 1] = static_cast<int16_t>(top_left);
		indices[index + 2] = static_cast<int16_t>(bottom_right);
		indices[index + 3] = static_cast<int16_t>(bottom_right);
		indices[index + 4] = static_cast<int16_t>(top_left);
		indices[index + 5] = static_cast<int16_t>(top_right);

		// Generate indices to close off the cylinder.
		if (close_off) {
			index = i * 3;

			indices[index] = 0;
			indices[index + 1] = static_cast<int16_t>(top_right);
			indices[index + 2] = static_cast<int16_t>(top_left);

			index += num_indices - subdivisions * 3;
			indices[index] = static_cast<int16_t>(subdivisions) + 1;
			indices[index + 1] = static_cast<int16_t>(bottom_left);
			indices[index + 2] = static_cast<int16_t>(bottom_right);
		}
	}
}

void GenerateDebugHalfSphere(int32_t subdivisions, Vector<glm::vec3>& points, Vector<int16_t>& indices)
{
	// Ensure at least 2 rows of vertices.
	subdivisions = Gaff::Max(subdivisions, 4);

	// Round subdivisions to the nearest even number.
	const int round_mode = std::fegetround();
	std::fesetround(FE_TONEAREST);

	subdivisions = static_cast<int32_t>(std::nearbyint(subdivisions * 0.5f) * 2.0f);

	std::fesetround(round_mode);


	const int32_t half_subdivisions = subdivisions / 2;
	indices.set_capacity((half_subdivisions - 2) * subdivisions * 6 + subdivisions * 3);
	points.set_capacity((half_subdivisions - 1) * subdivisions + 1);

	// Generate top hemisphere.
	// Generate each elevation level at a time.
	// Start at north pole and work our way down.
	for (int32_t y = half_subdivisions - 1; y >= 0; --y) {
		const float y_scale = static_cast<float>(y) / (half_subdivisions - 1);

		for (int32_t x = 0; x < subdivisions; ++x) {
			const float x_scale = static_cast<float>(x) / (subdivisions - 1);

			points.emplace_back(glm::vec3(
				cosf(2.0f * Gaff::Pi * x_scale) * cosf(Gaff::Pi * 0.5f * y_scale),
				sinf(Gaff::Pi * 0.5f * y_scale),
				sinf(2.0f * Gaff::Pi * x_scale) * cosf(Gaff::Pi * 0.5f * y_scale)
			));

			// Don't generate the same point multiple times. Only need one.
			if (y == (half_subdivisions - 1)) {
				break;
			}
		}
	}

	// Create indices for each triangle strip.
	// Start at north pole and work our way down.
	for (int32_t y = 0; y < half_subdivisions; ++y) {
		// Front face clockwise. Current row is top, next row is bottom.
		// *---*
		// | \ |
		// |  \|
		// *---*
		for (int32_t x = 0; x < subdivisions; ++x) {
			// North pole.
			if (y == 0) {
				const int32_t bottom_right = (x + 1) % subdivisions + 1;
				const int32_t bottom_left = x + 1;

				indices.emplace_back(0); // North pole.
				indices.emplace_back(bottom_right);
				indices.emplace_back(bottom_left);

			// On last row, no row past us.
			} else if (y > (half_subdivisions - 2)) {
				break;

			// Some strip in between north pole and last row.
			} else {
				const int32_t bottom_starting_index = y * subdivisions + 1;
				const int32_t top_starting_index = (y - 1) * subdivisions + 1;
				const int32_t bottom_right = bottom_starting_index + (x + 1) % subdivisions;
				const int32_t bottom_left = bottom_starting_index + x;
				const int32_t top_right = top_starting_index + (x + 1) % subdivisions;
				const int32_t top_left = top_starting_index + x;

				indices.emplace_back(bottom_right);
				indices.emplace_back(bottom_left);
				indices.emplace_back(top_left);

				indices.emplace_back(bottom_right);
				indices.emplace_back(top_left);
				indices.emplace_back(top_right);
			}
		}
	}
}

void GenerateDebugSphere(int32_t subdivisions, Vector<glm::vec3>& points, Vector<int16_t>& indices)
{
	// Ensure at least 3 rows of vertices.
	subdivisions = Gaff::Max(subdivisions, 4);

	// Round subdivisions to the nearest even number.
	const int round_mode = std::fegetround();
	std::fesetround(FE_TONEAREST);

	subdivisions = static_cast<int32_t>(std::nearbyint(subdivisions * 0.5f) * 2.0f);

	std::fesetround(round_mode);


	// Generate top hemisphere.
	GenerateDebugHalfSphere(subdivisions, points, indices);

	const int32_t half_subdivisions = subdivisions / 2;
	const int32_t num_rows = subdivisions - 1;

	indices.set_capacity((num_rows - 2) * subdivisions * 6);
	points.set_capacity((num_rows - 2) * subdivisions + 2);

	// Clone top hemisphere to make bottom hemisphere.
	// Starting at half_subdivisions - 2 because the middle strip
	// is shared between both hemispheres. No need to duplicate that strip.
	for (int32_t y = half_subdivisions - 2; y >= 0; --y) {
		// y == 0 is north pole.
		const int32_t starting_index = (y > 0) ?
			(y - 1) * subdivisions + 1 :
			0;

		// Bottom hemisphere is just top hemisphere with the y-axis inverted.
		for (int32_t x = 0; x < subdivisions; ++x) {
			glm::vec3 point = points[starting_index + x];
			point.y = -point.y;

			points.emplace_back(point);

			// Only one point for top and bottom pole.
			if (y == 0) {
				break;
			}
		}
	}

	// Create indices for each triangle strip.
	// Start at equator and work our way down.
	// Start at half_subdivisions - 1 because GenerateDebugHalfSphere already generated indices for the top hemisphere.
	for (int32_t y = half_subdivisions - 1; y < num_rows; ++y) {
		// Front face clockwise. Current row is top, next row is bottom.
		// *---*
		// | \ |
		// |  \|
		// *---*
		for (int32_t x = 0; x < subdivisions; ++x) {
			// Next strip is south pole.
			if (y == (num_rows - 2)) {
				const int32_t top_starting_index = (y - 1) * subdivisions + 1;
				const int32_t top_right = top_starting_index + (x + 1) % subdivisions;
				const int32_t top_left = top_starting_index + x;
				const int32_t south_pole = y * subdivisions + 1;

				indices.emplace_back(top_left);
				indices.emplace_back(top_right);
				indices.emplace_back(south_pole);

			// On south pole, no row past us.
			} else if (y > (num_rows - 2)) {
				break;

			// Some strip in between north and south pole.
			} else {
				const int32_t bottom_starting_index = y * subdivisions + 1;
				const int32_t top_starting_index = (y - 1) * subdivisions + 1;
				const int32_t bottom_right = bottom_starting_index + (x + 1) % subdivisions;
				const int32_t bottom_left = bottom_starting_index + x;
				const int32_t top_right = top_starting_index + (x + 1) % subdivisions;
				const int32_t top_left = top_starting_index + x;

				indices.emplace_back(bottom_right);
				indices.emplace_back(bottom_left);
				indices.emplace_back(top_left);

				indices.emplace_back(bottom_right);
				indices.emplace_back(top_left);
				indices.emplace_back(top_right);
			}
		}
	}
}

void GenerateDebugBox(Vector<glm::vec3>& points, Vector<int16_t>& indices)
{
	indices.resize(36);
	points.resize(8);

	// Top
	points[0] = glm::vec3(-0.5f, 0.5f, 0.5f);
	points[1] = glm::vec3(0.5f, 0.5f, 0.5f);
	points[2] = glm::vec3(-0.5f, 0.5f, -0.5f);
	points[3] = glm::vec3(0.5f, 0.5f, -0.5f);

	// Bottom
	points[4] = glm::vec3(-0.5f, -0.5f, 0.5f);
	points[5] = glm::vec3(0.5f, -0.5f, 0.5f);
	points[6] = glm::vec3(-0.5f, -0.5f, -0.5f);
	points[7] = glm::vec3(0.5f, -0.5f, -0.5f);

	// Top
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 1;
	indices[5] = 3;

	// Bottom
	indices[6] = 6;
	indices[7] = 5;
	indices[8] = 4;
	indices[9] = 5;
	indices[10] = 6;
	indices[11] = 7;

	// Left
	indices[12] = 0;
	indices[13] = 2;
	indices[14] = 4;
	indices[15] = 4;
	indices[16] = 2;
	indices[17] = 6;

	// Right
	indices[18] = 3;
	indices[19] = 1;
	indices[20] = 7;
	indices[21] = 7;
	indices[22] = 1;
	indices[23] = 5;

	// Forward
	indices[24] = 2;
	indices[25] = 3;
	indices[26] = 6;
	indices[27] = 6;
	indices[28] = 3;
	indices[29] = 7;

	// Back
	indices[30] = 1;
	indices[31] = 0;
	indices[32] = 5;
	indices[33] = 5;
	indices[34] = 0;
	indices[35] = 4;
}

NS_END
