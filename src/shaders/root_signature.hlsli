/**
 * Root signature layout
 * slot 0: b0 - Vertex shader constant buffer
 * slot 1: b1 - Pixel shader light buffer
 * slot 2: b2 - Pixel shader model color
 */

#define ROOTSIG \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0), " \
    "CBV(b1), " \
    "RootConstants(num32BitConstants=3, b2)" \
