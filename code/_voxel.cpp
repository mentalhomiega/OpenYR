/*******************************************************************************
 *                                O P E N  T S
 *******************************************************************************
 * SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright 2026 OpenTS contributors
 *
 * See LICENSE.md for applicable additional terms and warranty disclaimers.
 ******************************************************************************/

#include "always.h"

#include "_voxel.h"

#include "matrix3d.h"
#include "stbuffer.h"

bool UseVoxelCache;
Vector3 VoxelShadowLightVector;
float VoxelLightAngle;
Vector3 VoxelLightSource;
float VoxelCameraAngle;
Matrix3D VoxelCameraMatrix;
// Turret and barrel images share the buffer with the hulls, and their keys name the hull facing as well
// as the turret facing, so the buffer is 8 MB; a full buffer drops every image and renders them afresh.
StaticBufferClass VoxelStaticBuffer(8000000);

/// Both use the truncated pi the voxel code carries, not M_PI.
const float DefaultLightAngle = (float)(45 * (3.1415 / 180.0));
const float DefaultCameraAngle = (float)(55 * (3.1415 / 180.0));

