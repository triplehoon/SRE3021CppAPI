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
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <iostream>
#include <chrono>
#include <thread>
#include <tchar.h>
#include "SRE3021API.h"
#include "pGNUPlotU.h"


using std::cout;
using std::cin;
using std::endl;
using std::this_thread::sleep_for;
using namespace hurel::sre3021;

int main()
{	
	// Initialize device
	SRE3021API sre3021API;
	sre3021API.InitiateSRE3021API();

	// if want to calibrate. Put 22Na source infront of the detector.
	sre3021API.CalibrateEnergySpectrumWith22Na();

	// Start acqusition and trun on high voltage supply
	sre3021API.StartAcqusition();

	// Graph plotting program
	// download and install gnu plot at http://www.gnuplot.info/
	CpGnuplotU plot(_T("C:\\Program Files\\gnuplot\\bin\\wgnuplot.exe")); // basic gnuplot path

	cout << "Loop started..." << endl;

	// Detecting time in minute
	double minutes = 2;

	// loop to draw spectrum
	for (int i = 0; i < 60 * minutes; ++i) {
		cout << i << " seconds: packetcounts = " << sre3021API.GetUdpPacketCount() << endl;

		sleep_for(std::chrono::milliseconds(1000));
		SpectrumEnergy spectrum = sre3021API.GetSpectrum();

		// update spectrum by 5 seconds
		//if (i % 5 == 0)
		{
			FILE* fp = _wfopen(_T("spectrum.dat"), _T("wt"));
			if (fp) {
				for (auto hist : spectrum.HistoEnergies)
				{
					double energy = hist.Energy;
					double count = hist.Count;
					fwprintf(fp, _T("%f, %f \n"), energy, count);
				}
				fclose(fp);
			}

			plot.cmd(_T("plot 'spectrum.dat' with line"));
		}
		
	}

	// peak find fuction (2 nd differential)
	std::vector<double> peaks = sre3021API.GetSpectrum().FindPeaks();
	cout << "Find peaks: ";
	for (double p : peaks)
	{
		cout << p << ", ";
	}
	cout << endl;

	// Stop and trun off high voltage supply
	sre3021API.StopAcqusition();

	
	return 0;

}