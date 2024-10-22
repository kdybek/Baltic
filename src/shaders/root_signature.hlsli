/**
 * Root signature layout
 * slot 0: b0 - Vertex shader constant buffer
 * slot 1: b0 - Pixel shader light buffer
 * slot 2: b1 - Model buffer
 * slot 3: b2 - Absolute time modulo 2pi
 */

#define ROOTSIG \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX), " \
    "CBV(b0, visibility = SHADER_VISIBILITY_PIXEL), " \
    "CBV(b1), " \
    "RootConstants(num32BitConstants = 1, b2), " \
