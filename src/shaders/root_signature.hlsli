/**
 * Root signature layout
 * slot 0: b0 - Vertex shader constant buffer
 * slot 1: b1 - Pixel shader light buffer
 * slot 2: b2 - Pixel shader model color
 * slot 3: b3 - Vertex shader absolute time modulo 2pi
 */

#define ROOTSIG \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX), " \
    "RootConstants(num32BitConstants = 1, b1, visibility = SHADER_VISIBILITY_VERTEX), " \
    "CBV(b0, visibility = SHADER_VISIBILITY_PIXEL), " \
    "RootConstants(num32BitConstants = 3, b1, visibility = SHADER_VISIBILITY_PIXEL), " \
