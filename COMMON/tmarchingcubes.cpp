#pragma unmanaged
#include "tmarchingcubes.h"

#include <time.h>
#include <iostream>
#include <vector>
#include <map>
#include <omp.h>

#include "tqueue.h"

using namespace marchingcubes;


//the table is from http://paulbourke.net/geometry/polygonise/

static const int mcEdgeTable[256] = {
0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0 };

static const int mcTriTable[256][16] =
{ {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1} };






/*  sampling cube edge

      4_________5
     /        / |
    /        /  |
 7 /_______6/   |  vertex index
   |        |   |
   |  0     |  / 1
   |        | /
  3|________|/ 2


      ____4_____
     /        / |
    /7       /5 |9
   /___6____/   |  edge index
   |        |   |
 11|     0  |10/
   | 3      | /1
   |________|/
        2
*/


//
//
//  CellEdgeVtx
//     y
//     |
//     |
//     |e1                edge id は e0 = 0 残りの9本は隣のvoxelにある
//     |                             e1 = 8
//     |____e0_____x                 e2 = 3
//     \
//       \
//         \ e2
//           \ z


class CellEdgeVtx 
{
public:
  int m_x, m_y, m_z, m_thread_i;
  CellEdgeVtx (  )
  {
    m_x = -1;
    m_y = -1;
    m_z = -1;
    m_thread_i = 0;
  }
  
  CellEdgeVtx ( const CellEdgeVtx &src ){
    Copy(src);
  }

  CellEdgeVtx &operator=(const CellEdgeVtx &src)
  {
    Copy(src);
    return *this;
  }

  void Copy( const CellEdgeVtx &src ){
    m_x = src.m_x;
    m_y = src.m_y;
    m_z = src.m_z;
    m_thread_i = src.m_thread_i;
  }

  void Set(int _x, int _y, int _z) { m_x = _x; m_y = _y; m_z = _z; }
};



inline EVec3f getPosX(const int &x, const int &y, int const &z, const EVec3f &pitch, const double &t)
{
  return EVec3f((x + (float)t + 0.5f) * pitch[0], (y + 0.5f) * pitch[1], (z + 0.5f) * pitch[2]);
}

inline EVec3f getPosY(const int &x, const int &y, int const &z, const EVec3f &pitch, const double &t)
{
  return EVec3f((x + 0.5f) * pitch[0], (y + (float)t + 0.5f) * pitch[1], (z + 0.5f) * pitch[2]);
}

inline EVec3f getPosZ(const int &x, const int &y, int const &z, const EVec3f &pitch, const double &t)
{
  return EVec3f((x + 0.5f) * pitch[0], (y + 0.5f) * pitch[1], (z + (float)t + 0.5f) * pitch[2]);
}


/*

//voxel size : WxHxD
//cell  size : (W+1)x(H+1)x(D+1)

void t_MarchingCubesParallel_CalcVertex(
    const EVec3f &pitch,
    const int W, 
    const int H, 
    const int D, 
    const short thresh,
    const short *volume, 
    const byte  *v_flg ,

    CellEdgeVtx *cell_edges, //allocated
    vector<TQueue<EVec3f>> &verts_thread
){
  const int WH  = W*H;
  const int cW  = W+1;
  const int cWH = (W+1)*(H+1);


#pragma omp parallel for schedule(dynamic)
  for( int z = 0; z < D - 1; ++z ) 
  {
    const int thread_i = omp_get_thread_num();
    TQueue<EVec3f> &v = verts_thread[thread_i];

    for( int y = 0; y < H - 1; ++y )
    {
      for( int x = 0; x < W - 1; ++x ) 
      {
        const int vi = x + y * W + z * WH;
        const int ci = (x+1) + (y+1) *cW + (z+1) *cWH;
        CellEdgeVtx *cell = &cell_edges[ci];
        cell->m_thread_i = thread_i;

        if ( v_flg[ vi ] !=  v_flg[ vi + 1] ){
          cell->m_x = (int)v.size(); 
          float t = (thresh - volume[vi]) / (float) (volume[vi+1] - volume[vi]);
          v.push_back( EVec3f((x + t + 0.5f) * pitch[0], (y + 0.5f) * pitch[1], (z + 0.5f) * pitch[2] )); 
        }
        if ( v_flg[ vi ] !=  v_flg[ vi + W] ){
          cell->m_y = (int)v.size(); 
          float t = (thresh - volume[vi]) / (float)(volume[vi+W] - volume[vi]);
          v.push_back( EVec3f((x + 0.5f) * pitch[0], (y + t + 0.5f) * pitch[1], (z + 0.5f) * pitch[2] )); 
        }
        if ( v_flg[ vi ] !=  v_flg[ vi + WH] ){
          cell->m_z = (int)v.size(); 
          float t = (thresh - volume[vi]) / (float)(volume[vi+WH] - volume[vi]);
          v.push_back( EVec3f( (x + 0.5f) * pitch[0], (y + 0.5f) * pitch[1], (z + t + 0.5f) * pitch[2] )); 
        }
      }
    }
  }
  
  //cX == 0, cX == cW-1, 
#pragma omp parallel for schedule(dynamic)
  for( int z = -1; z < D ; ++z ) 
  {
    const int thread_i = omp_get_thread_num();
    TQueue<EVec3f> &v = verts_thread[thread_i];

    for( int y = -1; y < H ; ++y ) 
    {
      for( int kk = 0; kk < 2 ; ++kk )
      {
        int x = (kk == 0) ? -1 : W-1;
        const int vi = x + y * W + z * WH;
        CellEdgeVtx *cell = &cell_edges[(x+1) + (y+1) *cW + (z+1) *cWH];
        cell->m_thread_i = thread_i;

        byte flg_p = (x < 0     || y < 0      || z < 0     ) ?  false   :  v_flg[vi     ];
        byte flg_x = (x == W - 1|| y < 0      || z < 0     ) ?  false   :  v_flg[vi + 1 ];
        byte flg_y = (x < 0     || y == H - 1 || z < 0     ) ?  false   :  v_flg[vi + W ];
        byte flg_z = (x < 0     || y < 0      || z == D - 1) ?  false   :  v_flg[vi + WH];
        short v_p = (x < 0      || y < 0      || z < 0     ) ?  SHRT_MIN: volume[vi     ];
        short v_x = (x == W - 1 || y < 0      || z < 0     ) ?  SHRT_MIN: volume[vi + 1 ];
        short v_y = (x < 0      || y == H - 1 || z < 0     ) ?  SHRT_MIN: volume[vi + W ];
        short v_z = (x < 0      || y < 0      || z == D - 1) ?  SHRT_MIN: volume[vi + WH];

        if ( flg_p != flg_x ) {
          cell->m_x = (int)v.size(); 
          float t = (thresh - v_p) / (float)(v_x - v_p);
          v.push_back( EVec3f((x + t + 0.5f) * pitch[0], (y + 0.5f) * pitch[1], (z + 0.5f) * pitch[2] ) ); 
        }
        if ( flg_p != flg_y ) {
          cell->m_y = (int)v.size(); 
          float t = (thresh - v_p) / (float)(v_y - v_p);
          v.push_back( EVec3f((x + 0.5f) * pitch[0], (y + t + 0.5f) * pitch[1], (z + 0.5f) * pitch[2] ) ); 
        }
        if ( flg_p !=  flg_z ) {
          cell->m_z = (int)v.size(); 
          float t = (thresh - v_p) / (float)(v_z - v_p);
          v.push_back( EVec3f((x + 0.5f) * pitch[0], (y + 0.5f) * pitch[1], (z + t + 0.5f) * pitch[2] ) ); 
        }
      }
    }
  }


  
  //cY == 0, cY == cW-1 (x = -1, x = W -1 are already done)
#pragma omp parallel for schedule(dynamic)
  for( int z = -1; z < D ; ++z ) 
  {
    const int thread_i = omp_get_thread_num();
    TQueue<EVec3f> &v = verts_thread[thread_i];

    for( int x = 0; x < W - 1 ; ++x ) 
    {
      for( int kk = 0; kk < 2 ; ++kk )
      {
        int y = (kk == 0) ? -1 : H -1;
        const int vi = x + y * W + z * WH;
        CellEdgeVtx *cell = &cell_edges[(x+1) + (y+1) *cW + (z+1) *cWH];
        cell->m_thread_i = thread_i;
               
        byte flg_p = (x < 0     || y < 0      || z < 0     ) ?  false   :  v_flg[vi     ];
        byte flg_x = (x == W - 1|| y < 0      || z < 0     ) ?  false   :  v_flg[vi + 1 ];
        byte flg_y = (x < 0     || y == H - 1 || z < 0     ) ?  false   :  v_flg[vi + W ];
        byte flg_z = (x < 0     || y < 0      || z == D - 1) ?  false   :  v_flg[vi + WH];
        short v_p = (x < 0      || y < 0      || z < 0     ) ?  SHRT_MIN: volume[vi     ];
        short v_x = (x == W - 1 || y < 0      || z < 0     ) ?  SHRT_MIN: volume[vi + 1 ];
        short v_y = (x < 0      || y == H - 1 || z < 0     ) ?  SHRT_MIN: volume[vi + W ];
        short v_z = (x < 0      || y < 0      || z == D - 1) ?  SHRT_MIN: volume[vi + WH];

        if ( flg_p != flg_x ) {
          cell->m_x = (int)v.size(); 
          float t = (thresh - v_p) / (float)(v_x - v_p);
          v.push_back( EVec3f((x + t + 0.5f) * pitch[0], (y + 0.5f) * pitch[1], (z + 0.5f) * pitch[2] ) ); 
        }
        if ( flg_p != flg_y ) {
          cell->m_y = (int)v.size(); 
          float t = (thresh - v_p) / (float)(v_y - v_p);
          v.push_back( EVec3f((x + 0.5f) * pitch[0], (y + t + 0.5f) * pitch[1], (z + 0.5f) * pitch[2] ) ); 
        }
        if ( flg_p !=  flg_z ) {
          cell->m_z = (int)v.size(); 
          float t = (thresh - v_p) / (float)(v_z - v_p);
          v.push_back( EVec3f((x + 0.5f) * pitch[0], (y + 0.5f) * pitch[1], (z + t + 0.5f) * pitch[2] ) ); 
        }
      }
    }
  }

  
  //cZ == 0, cZ == cZ-1, (x = -1, x = W -1, y = -1, y = H-1 are already done)
#pragma omp parallel for schedule(dynamic)
  for( int y = 0; y < H - 1; ++y ) 
  {
    const int thread_i = omp_get_thread_num();
    TQueue<EVec3f> &v = verts_thread[thread_i];

    for( int x = 0; x < W - 1 ; ++x ) 
    {
      for( int kk = 0; kk < 2 ; ++kk )
      {
        int z = (kk == 0) ? -1 : D -1;
        const int vi = x + y * W + z * WH;
        CellEdgeVtx *cell = &cell_edges[(x+1) + (y+1) *cW + (z+1) *cWH];
        cell->m_thread_i = thread_i;

        byte flg_p = (x < 0     || y < 0      || z < 0     ) ?  false   :  v_flg[vi     ];
        byte flg_x = (x == W - 1|| y < 0      || z < 0     ) ?  false   :  v_flg[vi + 1 ];
        byte flg_y = (x < 0     || y == H - 1 || z < 0     ) ?  false   :  v_flg[vi + W ];
        byte flg_z = (x < 0     || y < 0      || z == D - 1) ?  false   :  v_flg[vi + WH];
        short v_p = (x < 0      || y < 0      || z < 0     ) ?  SHRT_MIN: volume[vi     ];
        short v_x = (x == W - 1 || y < 0      || z < 0     ) ?  SHRT_MIN: volume[vi + 1 ];
        short v_y = (x < 0      || y == H - 1 || z < 0     ) ?  SHRT_MIN: volume[vi + W ];
        short v_z = (x < 0      || y < 0      || z == D - 1) ?  SHRT_MIN: volume[vi + WH];

        if ( flg_p != flg_x ) {
          cell->m_x = (int)v.size(); 
          float t = (thresh - v_p) / (float)(v_x - v_p);
          v.push_back( EVec3f((x + t + 0.5f) * pitch[0], (y + 0.5f) * pitch[1], (z + 0.5f) * pitch[2] ) ); 
        }
        if ( flg_p != flg_y ) {
          cell->m_y = (int)v.size(); 
          float t = (thresh - v_p) / (float)(v_y - v_p);
          v.push_back( EVec3f((x + 0.5f) * pitch[0], (y + t + 0.5f) * pitch[1], (z + 0.5f) * pitch[2] ) ); 
        }
        if ( flg_p !=  flg_z ) {
          cell->m_z = (int)v.size(); 
          float t = (thresh - v_p) / (float)(v_z - v_p);
          v.push_back( EVec3f((x + 0.5f) * pitch[0], (y + 0.5f) * pitch[1], (z + t + 0.5f) * pitch[2] ) ); 
        }
      }
    }
  }
}







  
void t_MarchingCubesParallel(
    const EVec3i &vRes,
    const EVec3f &pitch,
    const short *volume,
    const short  thresh,
    const int    *minIdx,
    const int    *maxIdx,

    vector<EVec3f> &Vs, //output vertices
    vector<TPoly > &Ps  //output polygons 
)
{
  clock_t t0 = clock();

  //1. binalize volume 
  const int W = vRes[0];
  const int H = vRes[1];
  const int D = vRes[2];
  const int WH = W*H, WHD = W*H*D;

  byte *v_flg = new byte[WHD];

#pragma omp parallel for   
  for( int i=0; i < WHD; ++i ) 
    v_flg[i] = volume[i] > thresh;

  clock_t t1 = clock();

  //2. compute cell_edge 
  const int thread_num = omp_get_max_threads();
  const int cW = W + 1;
  const int cH = H + 1;
  const int cD = D + 1;
  const int cWH = cW * cH, cWHD = cW*cH*cD;

  CellEdgeVtx *cell_edges = new CellEdgeVtx[cWHD];
  vector<TQueue<EVec3f>> verts_thread( thread_num, TQueue<EVec3f>(W*H*6, W*H*6 ) );
  
  t_MarchingCubesParallel_CalcVertex(pitch, W,H,D,thresh, volume, v_flg, cell_edges, verts_thread);

  //3. vertexをまとめる
  int sum_verts = 0;
  for( int i=0; i < thread_num; ++i) sum_verts += verts_thread[i].size();
  
  Vs.resize( sum_verts );

  vector<int> verts_thread_offset(thread_num ,0);
  for( int i=0; i < thread_num; ++i) 
  {
    int ofst = (i == 0) ? 0 : verts_thread_offset[i-1] + verts_thread[i-1].size();
    
    TQueue<EVec3f> &v = verts_thread[i];
    for( int j = 0; j < v.size(); ++j) Vs[ ofst + j ] = v[j];

    verts_thread_offset[i] = ofst;
  }


  //4. generate polygon 

  vector<TQueue<TPoly>> polys_thread( thread_num, TQueue<TPoly>(W*H*2, W*H*2 ) );

#pragma omp parallel for schedule(dynamic)
  for (int cz = 0; cz < cD; ++cz)
  {
    TQueue<TPoly> &p = polys_thread[omp_get_thread_num()];
    for (int cy = 0; cy < cH; ++cy)
    {
      for (int cx = 0; cx < cW; ++cx)
      {
        //sampling 8 points on the cell (x,y,z)
        const int x = cx - 1, y = cy - 1, z = cz - 1;
        const int vI = x  + y * W   + z  *  WH;


        short caseID = 0;
        if (x >= 0  && y >= 0  && z >= 0  && v_flg[vI       ] ) caseID |= 1  ;
        if (x < W-1 && y >= 0  && z >= 0  && v_flg[vI+1     ] ) caseID |= 2  ;
        if (x < W-1 && y >= 0  && z < D-1 && v_flg[vI+1  +WH] ) caseID |= 4  ;
        if (x >= 0  && y >= 0  && z < D-1 && v_flg[vI    +WH] ) caseID |= 8  ;
        if (x >= 0  && y < H-1 && z >= 0  && v_flg[vI  +W   ] ) caseID |= 16 ;
        if (x < W-1 && y < H-1 && z >= 0  && v_flg[vI+1+W   ] ) caseID |= 32 ;
        if (x < W-1 && y < H-1 && z < D-1 && v_flg[vI+1+W+WH] ) caseID |= 64 ;
        if (x >= 0  && y < H-1 && z < D-1 && v_flg[vI  +W+WH] ) caseID |= 128;
        
        short flg = mcEdgeTable[caseID];
        if (flg == 0) continue;

        const int cI = cx + cy * cW + cz * cWH;

        int v[12];
        v[0]  = cell_edges[cI          ].x_;
        v[2]  = cell_edges[cI      +cWH].x_;
        v[4]  = cell_edges[cI + cW     ].x_;
        v[6]  = cell_edges[cI + cW +cWH].x_;
        v[1]  = cell_edges[cI + 1      ].z_;
        v[3]  = cell_edges[cI          ].z_;
        v[5]  = cell_edges[cI + 1 + cW ].z_;
        v[7]  = cell_edges[cI + cW     ].z_;
        v[8]  = cell_edges[cI          ].y_;
        v[9]  = cell_edges[cI + 1      ].y_;
        v[10] = cell_edges[cI + 1 + cWH].y_;
        v[11] = cell_edges[cI     + cWH].y_;

        //polygon生成
        for (int i = 0; mcTriTable[caseID][i] != -1; i += 3)
        {
          p.push_back( TPoly(v[mcTriTable[caseID][i]], v[mcTriTable[caseID][i + 1]], v[mcTriTable[caseID][i + 2]]));
        }
      }
    }
  }

  


  clock_t t2 = clock();

  int sum_polys = 0;
  for( int i=0; i < thread_num; ++i) sum_polys += polys_thread[i].size();

  cout << "AAA Mesh size vtx: " << sum_verts << " polys : " << sum_polys << " time: " << (t2-t0)/(double)CLOCKS_PER_SEC << "\n";

  delete[] v_flg;
  delete[] cell_edges;

}


  
template<class T>
void MarchingCubesParallel(
    const EVec3i &vRes,
    const EVec3f &vPitch,
    const T      *vol,
    const T       thresh,
    const int    *minIdx,
    const int    *maxIdx,
    TMesh &mesh)
{
  vector<EVec3f> Vs;
  vector<TPoly > Ps;
  MarchingCubesParallel<T>(vRes, vPitch, vol, thresh, minIdx, maxIdx, Vs, Ps);

  mesh.initialize(Vs, Ps);

}
*/






//
// Marching cubes法
//
// threshold と 画素値が同じ場合に
// 複数頂点が一点に縮退する場合がある．
// この時は一部の面の法線が計算できなくなるので注意

void marchingcubes::t_MarchingCubes(
    const EVec3i &vRes,
    const EVec3f &vPitch,
    const short *volume,
    const short  thresh,
    const int    *minIdx,
    const int    *maxIdx,

    std::vector<EVec3f> &Vs, //output vertices
    std::vector<TPoly > &Ps  //output polygons 
){
  clock_t t0 = clock();

  //volume resolution and Cell resolution
  const int W = vRes[0];
  const int H = vRes[1];
  const int D = vRes[2];
  const int WH = W*H, WHD = W*H*D;

  const int cW = W + 1;
  const int cH = H + 1;
  const int cD = D + 1;
  int cellXs = 0, cellXe = cW;
  int cellYs = 0, cellYe = cH;
  int cellZs = 0, cellZe = cD;

  const int cWH = cW * cH, cWHD = cW*cH*cD;

  if (minIdx != 0 && maxIdx != 0)
  {
    cellXs = std::max(0, minIdx[0]);  cellXe = std::min(maxIdx[0] + 2, cellXe);
    cellYs = std::max(0, minIdx[1]);  cellYe = std::min(maxIdx[1] + 2, cellYe);
    cellZs = std::max(0, minIdx[2]);  cellZe = std::min(maxIdx[2] + 2, cellZe);
  }

  CellEdgeVtx *edgePiv = new CellEdgeVtx[cWH];
  CellEdgeVtx *edgeNex = new CellEdgeVtx[cWH];
  for (int i = 0; i < cWH; ++i) edgePiv[i].Set(-1, -1, -1);
  for (int i = 0; i < cWH; ++i) edgeNex[i].Set(-1, -1, -1);

  //reserve Vs and Ps 
  int preCount = 0;
  for (int i = 0; i < WHD; ++i) if (volume[i] > thresh) preCount++;
  Vs.reserve(preCount);
  Ps.reserve(preCount);


  for (int cz = cellZs; cz < cellZe; ++cz)
  {
    std::swap(edgeNex, edgePiv);
    for (int i = 0; i < cWH; ++i) edgeNex[i].Set(-1, -1, -1);
    if (cz % 100 == 0) 
      std::cout << cz << "/" << cellZe - cellZs << "\n";

    double p[8];

    for (int cy = cellYs; cy < cellYe; ++cy)
    {
      for (int cx = cellXs; cx < cellXe; ++cx)
      {
        //sampling 8 points on the cell (x,y,z)
        const int x = cx - 1, y = cy - 1, z = cz - 1;
        const int vI = x + y * W + z*WH;

        p[0] = (x < 0      || y < 0      || z < 0     ) ? -DBL_MAX : volume[vI             ];
        p[1] = (x == W - 1 || y < 0      || z < 0     ) ? -DBL_MAX : volume[vI + 1         ];
        p[2] = (x == W - 1 || y < 0      || z == D - 1) ? -DBL_MAX : volume[vI + 1 +     WH];
        p[3] = (x < 0      || y < 0      || z == D - 1) ? -DBL_MAX : volume[vI +         WH];
        p[4] = (x < 0      || y == H - 1 || z <  0    ) ? -DBL_MAX : volume[vI +     W     ];
        p[5] = (x == W - 1 || y == H - 1 || z <  0    ) ? -DBL_MAX : volume[vI + 1 + W     ];
        p[6] = (x == W - 1 || y == H - 1 || z == D - 1) ? -DBL_MAX : volume[vI + 1 + W + WH];
        p[7] = (x < 0      || y == H - 1 || z == D - 1) ? -DBL_MAX : volume[vI +     W + WH];

        short caseID = 0;
        if (p[0] > thresh) caseID |= 1  ;
        if (p[1] > thresh) caseID |= 2  ;
        if (p[2] > thresh) caseID |= 4  ;
        if (p[3] > thresh) caseID |= 8  ;
        if (p[4] > thresh) caseID |= 16 ;
        if (p[5] > thresh) caseID |= 32 ;
        if (p[6] > thresh) caseID |= 64 ;
        if (p[7] > thresh) caseID |= 128;

        short flg = mcEdgeTable[caseID];
        if (flg == 0) continue;

        const int eI = cx + cy * cW;
        
        //x e0, e2, e4, e6
        if (flg & 1 && edgePiv[eI].m_x < 0) {
          edgePiv[eI].m_x = (int)Vs.size(); 
          Vs.push_back( getPosX(x, y, z, vPitch, (thresh - p[0]) / (p[1] - p[0])) ); 
        }
        if (flg & 4 && edgeNex[eI].m_x < 0) { 
          edgeNex[eI].m_x = (int)Vs.size(); 
          Vs.push_back( getPosX(x, y, z + 1, vPitch, (thresh - p[3]) / (p[2] - p[3])) ); 
        }
        if (flg & 16 && edgePiv[eI + cW].m_x < 0) { 
          edgePiv[eI + cW].m_x = (int)Vs.size(); 
          Vs.push_back( getPosX(x, y + 1, z, vPitch, (thresh - p[4]) / (p[5] - p[4])) ); 
        }
        if (flg & 64 && edgeNex[eI + cW].m_x < 0) { 
          edgeNex[eI + cW].m_x = (int)Vs.size(); 
          Vs.push_back( getPosX(x, y + 1, z + 1, vPitch, (thresh - p[7]) / (p[6] - p[7]))); 
        }
        //z e1, e3, e5, e7
        if (flg & 2 && edgePiv[eI + 1].m_z < 0) { 
          edgePiv[eI + 1].m_z = (int)Vs.size(); 
          Vs.push_back( getPosZ(x + 1, y, z, vPitch, (thresh - p[1]) / (p[2] - p[1]))); 
        }
        if (flg & 8 && edgePiv[eI].m_z < 0) { 
          edgePiv[eI].m_z = (int)Vs.size(); 
          Vs.push_back( getPosZ(x, y, z, vPitch, (thresh - p[0]) / (p[3] - p[0])));
        }
        if (flg & 32 && edgePiv[eI + 1 + cW].m_z < 0) { 
          edgePiv[eI + 1 + cW].m_z = (int)Vs.size(); 
          Vs.push_back( getPosZ(x + 1, y + 1, z, vPitch, (thresh - p[5]) / (p[6] - p[5]))); 
        }
        if (flg & 128 && edgePiv[eI + cW].m_z < 0) { 
          edgePiv[eI + cW].m_z = (int)Vs.size(); 
          Vs.push_back( getPosZ(x, y + 1, z, vPitch, (thresh - p[4]) / (p[7] - p[4]))); 
        }

        //y e8, e9, e10, e11
        if (flg & 256 && edgePiv[eI].m_y < 0) { 
          edgePiv[eI].m_y = (int)Vs.size(); 
          Vs.push_back( getPosY(x, y, z, vPitch, (thresh - p[0]) / (p[4] - p[0]))); 
        }
        if (flg & 512 && edgePiv[eI + 1].m_y < 0) { 
          edgePiv[eI + 1].m_y = (int)Vs.size(); 
          Vs.push_back( getPosY(x + 1, y, z, vPitch, (thresh - p[1]) / (p[5] - p[1]))); 
        }
        if (flg & 1024 && edgeNex[eI + 1].m_y < 0) { 
          edgeNex[eI + 1].m_y = (int)Vs.size(); 
          Vs.push_back( getPosY(x + 1, y, z + 1, vPitch, (thresh - p[2]) / (p[6] - p[2]))); 
        }
        if (flg & 2048 && edgeNex[eI].m_y < 0) { 
          edgeNex[eI].m_y = (int)Vs.size(); 
          Vs.push_back( getPosY(x, y, z + 1, vPitch, (thresh - p[3]) / (p[7] - p[3]))); 
        }

        int v[12];
        v[0]  =             edgePiv[eI     ].m_x;
        v[2]  =             edgeNex[eI     ].m_x;
        v[4]  = (cy<cH-1) ? edgePiv[eI  +cW].m_x : -1;
        v[6]  = (cy<cH-1) ? edgeNex[eI  +cW].m_x : -1;
        v[1]  = (cx<cW-1) ? edgePiv[eI+1   ].m_z : -1;
        v[3]  =             edgePiv[eI     ].m_z ;
        v[5]  = (cx<cW-1 && cy<cH-1)
                          ? edgePiv[eI+1+cW].m_z : -1;
        v[7]  = (cy<cH-1) ? edgePiv[eI  +cW].m_z : -1;
        v[8]  =             edgePiv[eI     ].m_y;
        v[9]  = (cx<cW-1) ? edgePiv[eI+1   ].m_y : -1;
        v[10] = (cx<cW-1) ? edgeNex[eI+1   ].m_y : -1;
        v[11] =             edgeNex[eI     ].m_y;

        //polygon生成
        for (int i = 0; mcTriTable[caseID][i] != -1; i += 3)
        {
          Ps.push_back( TPoly(v[mcTriTable[caseID][i]], v[mcTriTable[caseID][i + 1]], v[mcTriTable[caseID][i + 2]]));
        }
      }
    }
  }


  clock_t t1 = clock();

  std::cout << "Mesh size vtx: " << Vs.size() 
            << " polys : "       << Ps.size() 
            << " time: " << (t1-t0)/(double)CLOCKS_PER_SEC << "\n";

  delete[] edgePiv;
  delete[] edgeNex;
}











/*  sampling cube edge

      4_________5
     /        / |
    /        /  |
 7 /_______6/   |  vertex index
   |        |   |
   |  0     |  / 1
   |        | /
  3|________|/ 2


      ____4_____
     /        / |
    /7       /5 |9
   /___6____/   |  edge index
   |        |   |
 11|     0  |10/
   | 3      | /1
   |________|/
        2
*/


void marchingcubes::t_MarchingCubes_PolygonSoup(
    const EVec3i &vRes,
    const EVec3f &vPitch,
    const short  *volume,
    const short   thresh,
    const int    *minIdx,
    const int    *maxIdx,

    int     &polygon_verts_num, 
    EVec3f* &polygon_verts     //v0,v1,v2,  v0,v1,v2, ...
)
{
  clock_t t0 = clock();

  //volume resolution and Cell resolution
  const int W   = vRes[0];
  const int H   = vRes[1];
  const int D   = vRes[2];
  const int cW  = W + 1;
  const int cH  = H + 1;
  const int cD  = D + 1;
  const int WH  = W*H, WHD = W*H*D;
  const int cWH = cW * cH, cWHD = cW*cH*cD;

  int cellXs = 0, cellXe = W + 1;
  int cellYs = 0, cellYe = H + 1;
  int cellZs = 0, cellZe = D + 1;

  if (minIdx != 0 && maxIdx != 0)
  {
    cellXs = std::max(0, minIdx[0]);  cellXe = std::min(maxIdx[0] + 2, cellXe);
    cellYs = std::max(0, minIdx[1]);  cellYe = std::min(maxIdx[1] + 2, cellYe);
    cellZs = std::max(0, minIdx[2]);  cellZe = std::min(maxIdx[2] + 2, cellZe);
  }

  const int thread_num = omp_get_max_threads();
  std::vector<TQueue<EVec3f>> verts_thread( thread_num, TQueue<EVec3f>(W*H*3*6, W*H*3*6 ) );
  
#pragma omp parallel for schedule(dynamic)
  for (int cz = cellZs; cz < cellZe; ++cz)
  {
    TQueue<EVec3f> &vs = verts_thread[omp_get_thread_num()];

    double p[8 ];
    EVec3f v[12]; // 01,12,23,30, 45,56,67,70,  04,15,26,37,

    for (int cy = cellYs; cy < cellYe; ++cy)
    {
      for (int cx = cellXs; cx < cellXe; ++cx)
      {
        //sampling 8 points on the cell (x,y,z)
        const int x  = cx - 1, y = cy - 1, z = cz - 1;
        const int vI = x + y * W + z*WH;

        p[0] = (x < 0      || y < 0      || z < 0     ) ? -DBL_MAX : volume[vI             ];
        p[1] = (x == W - 1 || y < 0      || z < 0     ) ? -DBL_MAX : volume[vI + 1         ];
        p[2] = (x == W - 1 || y < 0      || z == D - 1) ? -DBL_MAX : volume[vI + 1 +     WH];
        p[3] = (x < 0      || y < 0      || z == D - 1) ? -DBL_MAX : volume[vI +         WH];
        p[4] = (x < 0      || y == H - 1 || z <  0    ) ? -DBL_MAX : volume[vI +     W     ];
        p[5] = (x == W - 1 || y == H - 1 || z < 0     ) ? -DBL_MAX : volume[vI + 1 + W     ];
        p[6] = (x == W - 1 || y == H - 1 || z == D - 1) ? -DBL_MAX : volume[vI + 1 + W + WH];
        p[7] = (x < 0      || y == H - 1 || z == D - 1) ? -DBL_MAX : volume[vI +     W + WH];

        short caseID = 0;
        if (p[0] > thresh) caseID |= 1  ;
        if (p[1] > thresh) caseID |= 2  ;
        if (p[2] > thresh) caseID |= 4  ;
        if (p[3] > thresh) caseID |= 8  ;
        if (p[4] > thresh) caseID |= 16 ;
        if (p[5] > thresh) caseID |= 32 ;
        if (p[6] > thresh) caseID |= 64 ;
        if (p[7] > thresh) caseID |= 128;

        short flg = mcEdgeTable[caseID];
        if (flg == 0) continue;

        const int eI = cx + cy * cW;
        //0-1, 1-2, 2-3, 3-4 
        if (flg &  1  ) v[ 0] = getPosX( x ,  y , z  , vPitch, (thresh - p[0]) / (p[1] - p[0]) ) ;
        if (flg &  2  ) v[ 1] = getPosZ(x+1,  y , z  , vPitch, (thresh - p[1]) / (p[2] - p[1]) ) ; 
        if (flg &  4  ) v[ 2] = getPosX( x ,  y , z+1, vPitch, (thresh - p[3]) / (p[2] - p[3]) ) ;
        if (flg &  8  ) v[ 3] = getPosZ( x ,  y , z  , vPitch, (thresh - p[0]) / (p[3] - p[0]) ) ;
        //4-5, 5-6, 6-7, 7-0 
        if (flg &  16 ) v[ 4] = getPosX( x , y+1, z  , vPitch, (thresh - p[4]) / (p[5] - p[4]) ) ; 
        if (flg &  32 ) v[ 5] = getPosZ(x+1, y+1, z  , vPitch, (thresh - p[5]) / (p[6] - p[5]) ) ; 
        if (flg &  64 ) v[ 6] = getPosX( x , y+1, z+1, vPitch, (thresh - p[7]) / (p[6] - p[7]) ) ; 
        if (flg & 128 ) v[ 7] = getPosZ( x , y+1, z  , vPitch, (thresh - p[4]) / (p[7] - p[4]) ) ; 
        //0-4, 1-5, 2-6, 3-7 
        if (flg & 256 ) v[ 8] = getPosY( x ,  y ,  z , vPitch, (thresh - p[0]) / (p[4] - p[0]) ) ; 
        if (flg & 512 ) v[ 9] = getPosY(x+1,  y ,  z , vPitch, (thresh - p[1]) / (p[5] - p[1]) ) ; 
        if (flg & 1024) v[10] = getPosY(x+1,  y , z+1, vPitch, (thresh - p[2]) / (p[6] - p[2]) ) ; 
        if (flg & 2048) v[11] = getPosY( x ,  y , z+1, vPitch, (thresh - p[3]) / (p[7] - p[3]) ) ; 

        //polygon生成
        for (int i = 0; mcTriTable[caseID][i] != -1; i += 3)
        {
          vs.push_back( v[mcTriTable[caseID][ i ]] );
          vs.push_back( v[mcTriTable[caseID][i+1]] );
          vs.push_back( v[mcTriTable[caseID][i+2]] );
        }
      }
    }
  }
 

  polygon_verts_num = 0;
  for( const auto &vs : verts_thread)
    polygon_verts_num += vs.size();

  polygon_verts = new EVec3f[polygon_verts_num];
  int idx = 0;
  for( const auto &vs : verts_thread)
    for( int i = 0; i < vs.size(); ++i)
    {
      polygon_verts[idx] = vs[i];
      ++idx;
    }

  clock_t t1 = clock();

  std::cout << "AAAA: polys : " << polygon_verts_num 
            << "time: "         << (t1-t0)/(double)CLOCKS_PER_SEC << "\n";
}


void marchingcubes::t_MarchingCubes_PolygonSoup(
    const EVec3i &vRes,
    const EVec3f &vPitch,
    const short   *volume,
    const short   thresh,
    const int    *minIdx,
    const int    *maxIdx,
    
    TTriangleSoup &triangle_soup
)
{
  int num_tri_verts = 0;
  EVec3f *tri_verts = 0;
  t_MarchingCubes_PolygonSoup(vRes, vPitch, volume, thresh, minIdx, maxIdx, num_tri_verts, tri_verts );

  triangle_soup.Allocate(num_tri_verts/3, tri_verts);

  if( num_tri_verts != 0 ) delete[] tri_verts;
}













void marchingcubes::t_MarchingCubes(
    const EVec3i &vRes,
    const EVec3f &vPitch,
    const short *vol,
    const short  thresh,
    const int    *minIdx,
    const int    *maxIdx,
    TMesh &mesh)
{
  std::vector<EVec3f> Vs;
  std::vector<TPoly > Ps;
  t_MarchingCubes(vRes, vPitch, vol, thresh, minIdx, maxIdx, Vs, Ps);
  mesh.Initialize(Vs, Ps);
}


#pragma managed
