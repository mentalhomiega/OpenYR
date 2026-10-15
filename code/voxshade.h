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
