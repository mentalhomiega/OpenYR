/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#pragma once


/// <summary>
/// Returns the specular highlight of a voxel surface, as a brightness to add to its diffuse term.
/// </summary>
/// <remarks>
/// A surface turned away from the halfway direction takes no highlight, whatever the strength.
/// Below a strength of 0.5 the formula turns positive again for the most turned-away surfaces,
/// so they are cut off before it is applied.
/// </remarks>
/// <param name="halfway_dot">The cosine of the angle between the surface normal and the halfway
/// direction of the light and the viewer.</param>
/// <param name="strength">The VoxelSpecularStrength rule. Zero or less turns the highlight
/// off.</param>
/// <returns>float; The highlight, never negative.</returns>
inline float Voxel_Specular_Term(float halfway_dot, float strength)
{
	if (!(strength > 0.0f) || !(halfway_dot > 0.0f)) {
		return(0.0f);
	}
	return(halfway_dot / (strength - halfway_dot * strength + halfway_dot));
}


/// <summary>
/// Returns the palette row a voxel surface of the given brightness is drawn from.
/// </summary>
/// <remarks>
/// The voxel drawers index their palette lookup with this row unchecked, so the row is held to the
/// lookup's range whatever the scale is. A scale or brightness that is not a number gives row 0.
/// </remarks>
/// <param name="brightness">The diffuse term plus the highlight.</param>
/// <param name="scale">The VoxelShadeScale rule: rows per unit of brightness.</param>
/// <param name="row_count">The number of rows in the palette lookup.</param>
/// <returns>unsigned char; The brightness times the scale, truncated, from 0 to row_count - 1.</returns>
inline unsigned char Voxel_Shade_Row(float brightness, float scale, int row_count)
{
	double const row = static_cast<double>(brightness) * static_cast<double>(scale);
	if (!(row > 0.0)) {
		return(0);
	}
	if (!(row < static_cast<double>(row_count - 1))) {
		return(static_cast<unsigned char>(row_count - 1));
	}
	return(static_cast<unsigned char>(row));
}
