#pragma once
#ifndef ZTECHALLEGE2020_UTILITIES_H
#define ZTECHALLEGE2020_UTILITIES_H

#define INF 0x3f3f3f3f

#include <string>
#include <vector>
#include <cmath>

using namespace std;

void split(const string& s, vector<string>& tokens, const string& delimiters = " ");

const double EPS = 1e-6;

bool dgreater(double a, double b) { return a > b + EPS; }

bool dequal(double a, double b) { return fabs(a - b) < EPS; }

#endif // !ZTECHALLEGE2020_UTILITIES_H
