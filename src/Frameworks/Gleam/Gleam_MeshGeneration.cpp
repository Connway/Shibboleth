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

void GenerateDebugSphere(int32_t subdivisions, Vector<glm::vec3>& points, Vector<int16_t>& indices)
{
	// Ensure at least 3 rows of vertices.
	subdivisions = Gaff::Max(subdivisions, 4);

	// Round subdivisions to the nearest even number.
	const int round_mode = std::fegetround();
	std::fesetround(FE_TONEAREST);

	subdivisions = static_cast<int32_t>(std::nearbyint(subdivisions * 0.5f) * 2.0f);

	std::fesetround(round_mode);


	const int32_t half_subdivisions = subdivisions / 2;

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

	points.shrink_to_fit();

	const int32_t num_rows = subdivisions - 1;

	// Create indices for each triangle strip.
	// Start at north pole and work our way down.
	for (int32_t y = 0; y < num_rows; ++y) {
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

			// Next strip is south pole.
			} else if (y == (num_rows - 2)) {
				const int32_t top_starting_index = (y - 1) * subdivisions + 1;
				const int32_t top_right = top_starting_index + (x + 1) % subdivisions;
				const int32_t top_left = top_starting_index + x;
				const int32_t south_pole = y * subdivisions + 1;

				indices.emplace_back(top_left);
				indices.emplace_back(top_right);
				indices.emplace_back(south_pole);

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

	indices.shrink_to_fit();
}

NS_END
