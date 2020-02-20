#include<iostream>

using namespace std;
#ifndef __FANCS_H
#define __FANCS_H

const int MAX_RAND = 16384;
const int HALF_MAX_RAND = 8192;

int chance(int MAX);

//Perlin Noise
float Noise(int x, int y);
float Smoothed(float x, float y);
float Interpolated(float v0, float v1, float v2, float v3, float f);
float InterpolatedNoise(float x, float y);
float PerlinNoise(float x, float y, float p, int oct);
//Some fancs
int readdigit(string str);
string readFUntilDigit(FILE * fail);
int readFdigit(FILE * file);
string myto_string(int in);
string myto_string(bool bo);
unsigned int to_gen(unsigned int t);
unsigned int to_gen_typ(unsigned int t);

bool readBinStr(unsigned int s, int b, int e);

#endif;