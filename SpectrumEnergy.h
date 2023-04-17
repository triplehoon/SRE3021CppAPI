// ----------------------------------------------------------------------------
// -                        SRE3021API C++ version                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2022-2022 Choi, Sehoon (triplehoon95@hanyang.ac.kr)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------
#pragma once

#include <iostream>
#include <vector>
#include<iomanip>

class HistoEnergy
{
public:
    double Energy;
    int Count;
    HistoEnergy(double energy)
    {
        Energy = energy;
        Count = 0;
    };
    HistoEnergy(double energy, int count)
    {
        Energy = energy;
        Count = count;
    };
};

class SpectrumEnergy
{
public:
    std::vector<HistoEnergy> HistoEnergies;

    std::vector<double> EnergyBin;

    double BinSize = 0;
    double MaxEnergy = 0;
    SpectrumEnergy() {};
    SpectrumEnergy(double binSize, double maxEnergy);
    SpectrumEnergy(const SpectrumEnergy &spectrum);

    void AddEnergy(double energy);
    void AddEnergy(std::vector<double> energy);

    void Reset();

    void PrintSpectrum();

    std::vector<double> FindPeaks(double diffLimit = -20);
};

