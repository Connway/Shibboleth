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

void GenerateDebugCylinder(int32_t subdivisions, Vector<Vec3>& points, Vector<int16_t>& indices, bool close_off)
{
	const int32_t offset = (close_off) ? 1 : 0;
	const int32_t num_indices = (subdivisions * 6) * (offset + 1);

	points.set_capacity(static_cast<size_t>(subdivisions + offset) * 2);
	indices.set_capacity(num_indices);

	// Top cap.
	if (close_off) {
		points.emplace_back(0.0f, 0.5f, 0.0f);
	}

	for (int32_t i = 0; i < subdivisions; ++i) {
		const float scale = static_cast<float>(i) / (subdivisions - 1);
		const float x = cosf(2.0f * Gaff::Pi * scale) * 0.5f;
		const float z = sinf(2.0f * Gaff::Pi * scale) * 0.5f;

		points.emplace_back(x, 0.5f, z);
	}

	// Bottom cap.
	if (close_off) {
		points.emplace_back(0.0f, -0.5f, 0.0f);
	}

	for (int32_t i = 0; i < subdivisions; ++i) {
		Vec3 point = points[offset + i];
		point.y = -point.y;

		points.emplace_back(point);
	}

	// Front face clockwise. Current row is top, next row is bottom.
	// *---*
	// | \ |
	// |  \|
	// *---*

	// Top cap.
	if (close_off) {
		for (int32_t i = 0; i < subdivisions; ++i) {
			const int32_t top_right = (i + 1) % subdivisions + offset;
			const int32_t top_left = i + offset;

			indices.emplace_back(0);
			indices.emplace_back(top_right);
			indices.emplace_back(top_left);
		}
	}

	for (int32_t i = 0; i < subdivisions; ++i) {
		const int32_t top_right = (i + 1) % subdivisions + offset;
		const int32_t top_left = i + offset;
		const int32_t bottom_right = top_right + subdivisions + offset;
		const int32_t bottom_left = top_left + subdivisions + offset;

		indices.emplace_back(bottom_left);
		indices.emplace_back(top_left);
		indices.emplace_back(bottom_right);
		indices.emplace_back(bottom_right);
		indices.emplace_back(top_left);
		indices.emplace_back(top_right);
	}

	// Bottom cap.
	if (close_off) {
		for (int32_t i = 0; i < subdivisions; ++i) {
			const int32_t top_right = (i + 1) % subdivisions + offset;
			const int32_t top_left = i + offset;
			const int32_t bottom_right = top_right + subdivisions + offset;
			const int32_t bottom_left = top_left + subdivisions + offset;

			indices.emplace_back(subdivisions + 1);
			indices.emplace_back(bottom_left);
			indices.emplace_back(bottom_right);
		}
	}
}

void GenerateDebugHalfSphere(int32_t subdivisions, Vector<Vec3>& points, Vector<int16_t>& indices)
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

			points.emplace_back(Vec3(
				cosf(2.0f * Gaff::Pi * x_scale) * cosf(Gaff::Pi * 0.5f * y_scale) * 0.5f,
				sinf(Gaff::Pi * 0.5f * y_scale) * 0.5f,
				sinf(2.0f * Gaff::Pi * x_scale) * cosf(Gaff::Pi * 0.5f * y_scale) * 0.5f
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

void GenerateDebugSphere(int32_t subdivisions, Vector<Vec3>& points, Vector<int16_t>& indices)
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
			Vec3 point = points[starting_index + x];
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

void GenerateDebugCone(int32_t subdivisions, Vector<Vec3>& points, Vector<int16_t>& indices)
{
	indices.set_capacity(subdivisions * 6);
	points.set_capacity(subdivisions + 2);

	points.emplace_back(0.0f, 0.5f, 0.0f);
	points.emplace_back(0.0f, -0.5f, 0.0f);

	for (int32_t x = 0; x < subdivisions; ++x) {
		const float x_scale = static_cast<float>(x) / (subdivisions - 1);

		points.emplace_back(Vec3(
			cosf(2.0f * Gaff::Pi * x_scale) * 0.5f,
			-0.5f,
			sinf(2.0f * Gaff::Pi * x_scale) * 0.5f
		));

		const int32_t bottom_right = (x + 1) % subdivisions + 2;
		const int32_t bottom_left = x + 2;

		indices.emplace_back(0);
		indices.emplace_back(bottom_right);
		indices.emplace_back(bottom_left);

		indices.emplace_back(1);
		indices.emplace_back(bottom_left);
		indices.emplace_back(bottom_right);
	}
}

void GenerateDebugPlane(int32_t subdivisions, Vector<Vec3>& points, Vector<int16_t>& indices)
{
	subdivisions = Gaff::Max(1, subdivisions);

	const int32_t rowsAndColumns = subdivisions + 1;

	indices.set_capacity(6 * subdivisions * subdivisions);
	points.set_capacity(rowsAndColumns * rowsAndColumns);

	for (int32_t row = 0; row < rowsAndColumns; ++row) {
		for (int32_t column = 0; column < rowsAndColumns; ++column) {
			points.emplace_back(
				static_cast<float>(column) / (rowsAndColumns - 1),
				0.0f,
				static_cast<float>(row) / (rowsAndColumns - 1)
			);

			if (row < (rowsAndColumns - 1) && column < (rowsAndColumns - 1)) {
				const int32_t top_left = row * rowsAndColumns + column;
				const int32_t top_right = top_left + 1;
				const int32_t bottom_left = (row + 1) * rowsAndColumns + column;
				const int32_t bottom_right = bottom_left + 1;

				indices.emplace_back(top_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_left);

				indices.emplace_back(bottom_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_right);
			}
		}
	}
}

void GenerateDebugBox(int32_t subdivisions, Vector<Vec3>& points, Vector<int16_t>& indices)
{
	subdivisions = Gaff::Max(1, subdivisions);

	const int32_t num_points_per_axis = subdivisions + 1;
	const int32_t num_points_per_face = num_points_per_axis * num_points_per_axis;
	const int32_t num_indices_per_face = subdivisions * subdivisions * 6;

	indices.set_capacity(num_indices_per_face * 6);
	points.set_capacity(num_points_per_face * 6);
	
	// Top
	// Forward to back.
	for (int32_t z_slice = 0; z_slice < num_points_per_axis; ++z_slice) {
		const float z = 0.5f - (static_cast<float>(z_slice) / (num_points_per_axis - 1));

		// Left to right.
		for (int32_t x_slice = 0; x_slice < num_points_per_axis; ++x_slice) {
			const float x = -0.5f + (static_cast<float>(x_slice) / (num_points_per_axis - 1));
			points.emplace_back(x, 0.5f, z);

			// Don't push indices if we already did the row before last.
			if (x_slice < (num_points_per_axis - 1) && z_slice < (num_points_per_axis - 1)) {
				const int32_t top_left = x_slice + z_slice * num_points_per_axis;
				const int32_t bottom_left = top_left + num_points_per_axis;
				const int32_t bottom_right = bottom_left + 1;
				const int32_t top_right = top_left + 1;

				indices.emplace_back(top_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_left);

				indices.emplace_back(bottom_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_right);
			}
		}
	}

	// Bottom
	// Forward to back.
	for (int32_t z_slice = 0; z_slice < num_points_per_axis; ++z_slice) {
		const float z = 0.5f - (static_cast<float>(z_slice) / (num_points_per_axis - 1));

		// Right to left.
		for (int32_t x_slice = 0; x_slice < num_points_per_axis; ++x_slice) {
			const float x = 0.5f - (static_cast<float>(x_slice) / (num_points_per_axis - 1));
			points.emplace_back(x, -0.5f, z);

			// Don't push indices if we already did the row before last.
			if (x_slice < (num_points_per_axis - 1) && z_slice < (num_points_per_axis - 1)) {
				const int32_t top_left = x_slice + z_slice * num_points_per_axis + num_points_per_face;
				const int32_t bottom_left = top_left + num_points_per_axis;
				const int32_t bottom_right = bottom_left + 1;
				const int32_t top_right = top_left + 1;

				indices.emplace_back(top_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_left);

				indices.emplace_back(bottom_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_right);
			}
		}
	}

	// Left
	// Top to bottom.
	for (int32_t y_slice = 0; y_slice < num_points_per_axis; ++y_slice) {
		const float y = 0.5f - (static_cast<float>(y_slice) / (num_points_per_axis - 1));

		// Forward to back.
		for (int32_t z_slice = 0; z_slice < num_points_per_axis; ++z_slice) {
			const float z = 0.5f - (static_cast<float>(z_slice) / (num_points_per_axis - 1));
			points.emplace_back(-0.5f, y, z);

			// Don't push indices if we already did the row before last.
			if (y_slice < (num_points_per_axis - 1) && z_slice < (num_points_per_axis - 1)) {
				const int32_t top_left = z_slice + y_slice * num_points_per_axis + num_points_per_face * 2;
				const int32_t bottom_left = top_left + num_points_per_axis;
				const int32_t bottom_right = bottom_left + 1;
				const int32_t top_right = top_left + 1;

				indices.emplace_back(top_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_left);

				indices.emplace_back(bottom_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_right);
			}
		}
	}

	// Right
	// Top to bottom.
	for (int32_t y_slice = 0; y_slice < num_points_per_axis; ++y_slice) {
		const float y = 0.5f - (static_cast<float>(y_slice) / (num_points_per_axis - 1));

		// Back to forward.
		for (int32_t z_slice = 0; z_slice < num_points_per_axis; ++z_slice) {
			const float z = -0.5f + (static_cast<float>(z_slice) / (num_points_per_axis - 1));
			points.emplace_back(0.5f, y, z);

			// Don't push indices if we already did the row before last.
			if (y_slice < (num_points_per_axis - 1) && z_slice < (num_points_per_axis - 1)) {
				const int32_t top_left = z_slice + y_slice * num_points_per_axis + num_points_per_face * 3;
				const int32_t bottom_left = top_left + num_points_per_axis;
				const int32_t bottom_right = bottom_left + 1;
				const int32_t top_right = top_left + 1;

				indices.emplace_back(top_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_left);

				indices.emplace_back(bottom_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_right);
			}
		}
	}

	// Forward
	// Top to bottom.
	for (int32_t y_slice = 0; y_slice < num_points_per_axis; ++y_slice) {
		const float y = 0.5f - (static_cast<float>(y_slice) / (num_points_per_axis - 1));

		// Right to left.
		for (int32_t x_slice = 0; x_slice < num_points_per_axis; ++x_slice) {
			const float x = 0.5f - (static_cast<float>(x_slice) / (num_points_per_axis - 1));
			points.emplace_back(x, y, 0.5f);

			// Don't push indices if we already did the row before last.
			if (x_slice < (num_points_per_axis - 1) && y_slice < (num_points_per_axis - 1)) {
				const int32_t top_left = x_slice + y_slice * num_points_per_axis + num_points_per_face * 4;
				const int32_t bottom_left = top_left + num_points_per_axis;
				const int32_t bottom_right = bottom_left + 1;
				const int32_t top_right = top_left + 1;

				indices.emplace_back(top_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_left);

				indices.emplace_back(bottom_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_right);
			}
		}
	}

	// Back
	// Top to bottom.
	for (int32_t y_slice = 0; y_slice < num_points_per_axis; ++y_slice) {
		const float y = 0.5f - (static_cast<float>(y_slice) / (num_points_per_axis - 1));

		// Left to right.
		for (int32_t x_slice = 0; x_slice < num_points_per_axis; ++x_slice) {
			const float x = -0.5f + (static_cast<float>(x_slice) / (num_points_per_axis - 1));
			points.emplace_back(x, y, -0.5f);

			// Don't push indices if we already did the row before last.
			if (x_slice < (num_points_per_axis - 1) && y_slice < (num_points_per_axis - 1)) {
				const int32_t top_left = x_slice + y_slice * num_points_per_axis + num_points_per_face * 5;
				const int32_t bottom_left = top_left + num_points_per_axis;
				const int32_t bottom_right = bottom_left + 1;
				const int32_t top_right = top_left + 1;

				indices.emplace_back(top_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_left);

				indices.emplace_back(bottom_left);
				indices.emplace_back(top_right);
				indices.emplace_back(bottom_right);
			}
		}
	}
}

NS_END
