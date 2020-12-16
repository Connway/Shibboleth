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
	GAFF_ASSERT(subdivisions > 1);

	// Round subdivisions to the nearest even number.
	const int round_mode = std::fegetround();
	std::fesetround(FE_TONEAREST);

	subdivisions = static_cast<int32_t>(std::nearbyint(subdivisions * 0.5f) * 2.0f);

	std::fesetround(round_mode);


	const int32_t half_subdivisions = subdivisions / 2;

	// Generate top hemisphere.
	// Generate each elevation level at a time.
	for (int32_t y = 0; y < half_subdivisions; ++y) {
		const float y_scale = static_cast<float>(y) / half_subdivisions;

		for (int32_t x = 0; x < subdivisions; ++x) {
			const float x_scale = static_cast<float>(x) / subdivisions;

			points.emplace_back(glm::vec3(
				cosf(2.0f * Gaff::Pi * x_scale) * cosf(Gaff::Pi * y_scale),
				sinf(Gaff::Pi * y_scale),
				sinf(2.0f * Gaff::Pi * x_scale) * cosf(Gaff::Pi * y_scale)
			));

			// Don't generate the same point multiple times. Only need one.
			if (y == (half_subdivisions - 1)) {
				break;
			}
		}
	}

	// Elevation 0 will be duplicates, but easier to deal with a duplicate row of points than special case it.
	// Clone top hemisphere to make bottom hemisphere.
	for (int32_t y = 0; y < half_subdivisions; ++y) {
		const int32_t starting_index = y * subdivisions;

		// Bottom hemisphere is just top hemisphere with the y-axis inverted.
		for (int32_t x = 0; x < subdivisions; ++x) {
			glm::vec3 point = points[starting_index + x];
			point.y = -point.y;

			// Only one point for top and bottom pole.
			if (y == (half_subdivisions - 1)) {
				break;
			}
		}
	}

	points.shrink_to_fit();

	// Create indices for each triangle strip.

	// Top hemisphere indices.
	for (int32_t y = 0; y < half_subdivisions; ++y) {
		const int32_t top_starting_index = (y + 1) * subdivisions;
		const int32_t bottom_starting_index = y * subdivisions;

		// Front face clockwise.
		// *---*
		// | \ |
		// |  \|
		// *---*
		for (int32_t x = 0; x < subdivisions; ++x) {
			const int32_t bot_right = bottom_starting_index + (x + 1) % subdivisions;
			const int32_t bot_left = bottom_starting_index + x;

			indices.emplace_back(bot_right);
			indices.emplace_back(bot_left);

			// Reached north pole.
			if (y == (half_subdivisions - 2)) {
				indices.emplace_back(top_starting_index);

			} else {
				const int32_t top_right = top_starting_index + (x + 1) % subdivisions;
				const int32_t top_left = top_starting_index + x;

				indices.emplace_back(top_left);

				indices.emplace_back(bot_right);
				indices.emplace_back(top_left);
				indices.emplace_back(top_right);
			}
		}
	}

	// Bottom hemisphere indices.
	const int32_t offset = subdivisions * (half_subdivisions - 1) + 1;

	for (int32_t y = 0; y < half_subdivisions; ++y) {
		const int32_t bottom_starting_index = (y + 1) * subdivisions;
		const int32_t top_starting_index = y * subdivisions;

		// Front face clockwise.
		// *---*
		// | \ |
		// |  \|
		// *---*
		for (int32_t x = 0; x < subdivisions; ++x) {
			const int32_t top_right = top_starting_index + (x + 1) % subdivisions + offset;
			const int32_t top_left = top_starting_index + x + offset;

			indices.emplace_back(top_left);
			indices.emplace_back(top_right);

			// Reached south pole.
			if (y == (half_subdivisions - 2)) {
				indices.emplace_back(bottom_starting_index + offset);

			} else {
				const int32_t bottom_right = bottom_starting_index + (x + 1) % subdivisions + offset;
				const int32_t bottom_left = bottom_starting_index + x + offset;

				indices.emplace_back(bottom_right);

				indices.emplace_back(top_left);
				indices.emplace_back(bottom_right);
				indices.emplace_back(bottom_left);
			}
		}
	}
}

NS_END
