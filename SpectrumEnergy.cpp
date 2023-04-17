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

#include "SpectrumEnergy.h"

SpectrumEnergy::SpectrumEnergy(double binSize, double maxEnergy)
{
    BinSize = binSize;
    MaxEnergy = maxEnergy;
    int binCount = (int)(MaxEnergy / binSize);
    for (int i = 0; i < binCount; ++i)
    {
        double energy = i * binSize;

        EnergyBin.push_back(energy);
        HistoEnergies.push_back(HistoEnergy(energy));
    }
}

void SpectrumEnergy::AddEnergy(double energy)
{
    for (int i = 0; i < EnergyBin.size() - 1; ++i)
    {
        if (energy < EnergyBin[i + 1] && energy > EnergyBin[i])
        {
            ++HistoEnergies[i].Count;
            break;
        }
    }
}

void SpectrumEnergy::Reset()
{
    for(HistoEnergy data : HistoEnergies)
    {
        data.Count = 0;
    }
}

std::vector<double> SpectrumEnergy::FindPeaks(double diffLimit)
{
    std::vector<double> spectAvg;

    std::vector<double> numericDiff;
    std::vector<double> numericSecondDiff;


    std::vector<double> peaks;

    numericDiff.reserve(EnergyBin.size());
    numericSecondDiff.reserve(EnergyBin.size());

    numericDiff.push_back(HistoEnergies[1].Count - HistoEnergies[0].Count);
    numericDiff.push_back(HistoEnergies[2].Count - HistoEnergies[1].Count);
    numericSecondDiff.push_back(numericDiff[1] - numericDiff[0]);
    double dataMin = 500000;
    //Find min
    bool flagIsDecending = true;
    spectAvg.push_back((HistoEnergies[1].Count + HistoEnergies[0].Count) / 2);
    for (int i = 1; i < EnergyBin.size() - 2; ++i)
    {
        spectAvg.push_back((HistoEnergies[i + 1].Count + HistoEnergies[i].Count + HistoEnergies[i - 1].Count) / 3);
        numericDiff.push_back(HistoEnergies[i + 1].Count - HistoEnergies[i].Count);
        numericSecondDiff.push_back(numericDiff[i] - numericDiff[i - 1]);

        double data = numericSecondDiff[i];
        double dataFoward = numericSecondDiff[i - 1];
        if (data < diffLimit)
        {

            if (flagIsDecending && (data - dataFoward) > 0)
            {
                //peaks.push_back(HistoEnergies[i - 1].Energy);
                //printf("Spectrum diff min: {%f}, E: {%f}", data, HistoEnergies[i - 1].Energy);
            }
        }
        if (data < dataMin)
        {
            dataMin = data;
        }

        flagIsDecending = dataFoward - data > 0;
    }

    std::vector<double> numericDiffAvg;
    std::vector<double> numericSecondDiffAvg;
    numericDiffAvg.push_back((numericDiff[1] + numericDiff[0])/2);
    
    numericDiffAvg.push_back((numericDiff[1] + numericDiff[0]) / 2);
    numericSecondDiffAvg.push_back(numericDiffAvg[1] - numericDiffAvg[0]);
    for (int i = 1; i < spectAvg.size() - 2; ++i)
    {
        numericDiffAvg.push_back((spectAvg[i + 1] + spectAvg[i] + spectAvg[i - 1])/3);
        numericSecondDiffAvg.push_back(numericDiffAvg[i] - numericDiffAvg[i - 1]);

        double data = numericSecondDiffAvg[i];
        double dataFoward = numericSecondDiffAvg[i - 1];
        if (data < diffLimit)
        {
         
            if (flagIsDecending && (data - dataFoward) > 0)
            {
                if (i - 3 > 0)
                {
                    peaks.push_back(HistoEnergies[i - 2].Energy);
                }                
                //printf("Spectrum avg diff min: {%f}, E: {%f}", data, HistoEnergies[i - 1].Energy);
            }
        }
        if (data < dataMin)
        {
            dataMin = data;
        }

        flagIsDecending = dataFoward - data > 0;
    }
    //printf("\n");
    return peaks;
}

void SpectrumEnergy::PrintSpectrum()
{
    if (HistoEnergies.size() == 0)
    {
        return;
    }
    else 
    {
        std::cout << std::setw(11) << "   Energy   ";
        std::cout << std::setw(11) << "   Counts   " << std::endl;
        for (auto hist : HistoEnergies)
        {
            std::cout << std::setw(11) << hist.Energy;
            std::cout << std::setw(11) << hist.Count << std::endl;
        }
    }
}

void SpectrumEnergy::AddEnergy(std::vector<double> energy)
{
    for(double d : energy)
    {
        AddEnergy(d);
    }
}



SpectrumEnergy::SpectrumEnergy(const SpectrumEnergy& spectrum)
{
    BinSize = spectrum.BinSize;
    MaxEnergy = spectrum.MaxEnergy;
    EnergyBin = spectrum.EnergyBin;
    HistoEnergies = spectrum.HistoEnergies;
}
