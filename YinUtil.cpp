/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    pYIN - A fundamental frequency estimator for monophonic audio
    Centre for Digital Music, Queen Mary, University of London.
    
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#include "YinUtil.h"

#include <vector>

#include <cstdio>
#include <cmath>
#include <algorithm>

YinUtil::YinUtil(int yinBufferSize) :
    m_yinBufferSize(yinBufferSize),
    m_fft(yinBufferSize * 2)
{
}

YinUtil::~YinUtil()
{
}

void 
YinUtil::slowDifference(const double *in, double *yinBuffer) 
{
    yinBuffer[0] = 0;
    double delta ;
    int startPoint = 0;
    int endPoint = 0;
    for (int i = 1; i < int(m_yinBufferSize); ++i) {
        yinBuffer[i] = 0;
        startPoint = m_yinBufferSize/2 - i/2;
        endPoint = startPoint + m_yinBufferSize;
        for (int j = startPoint; j < endPoint; ++j) {
            delta = in[i+j] - in[j];
            yinBuffer[i] += delta * delta;
        }
    }    
}

void 
YinUtil::fastDifference(const double *in, double *yinBuffer) 
{
    // DECLARE AND INITIALISE
    // initialisation of most of the arrays here was done in a separate function,
    // with all the arrays as members of the class... moved them back here.
    
    int frameSize = 2 * m_yinBufferSize;
    int halfSize = m_yinBufferSize;

    double *audioTransformedComplex = new double[frameSize + 2];
    double *audioOutReal = new double[frameSize];
    double *kernel = new double[frameSize];
    double *kernelTransformedComplex = new double[frameSize + 2];
    double *yinStyleACFComplex = new double[frameSize + 2];
    double *powerTerms = new double[m_yinBufferSize];
    
    // POWER TERM CALCULATION
    // ... for the power terms in equation (7) in the Yin paper
    powerTerms[0] = 0.0;
    for (int j = 0; j < m_yinBufferSize; ++j) {
        powerTerms[0] += in[j] * in[j];
    }

    // now iteratively calculate all others (saves a few multiplications)
    for (int tau = 1; tau < m_yinBufferSize; ++tau) {
        powerTerms[tau] = powerTerms[tau-1] -
            in[tau-1] * in[tau-1] +
            in[tau+m_yinBufferSize] * in[tau+m_yinBufferSize];  
    }

    // YIN-STYLE AUTOCORRELATION via FFT
    // 1. data
    m_fft.forward(in, audioTransformedComplex);
    
    // 2. half of the data, disguised as a convolution kernel
    for (int j = 0; j < m_yinBufferSize; ++j) {
        kernel[j] = in[m_yinBufferSize-1-j];
    }
    for (int j = m_yinBufferSize; j < frameSize; ++j) {
        kernel[j] = 0.;
    }
    m_fft.forward(kernel, kernelTransformedComplex);

    // 3. convolution via complex multiplication -- written into
    for (int j = 0; j <= halfSize; ++j) {
        yinStyleACFComplex[j*2] = // real
            audioTransformedComplex[j*2] * kernelTransformedComplex[j*2] -
            audioTransformedComplex[j*2+1] * kernelTransformedComplex[j*2+1];
        yinStyleACFComplex[j*2+1] = // imaginary
            audioTransformedComplex[j*2] * kernelTransformedComplex[j*2+1] +
            audioTransformedComplex[j*2+1] * kernelTransformedComplex[j*2];
    }

    m_fft.inverse(yinStyleACFComplex, audioOutReal);
    
    // CALCULATION OF difference function
    // ... according to (7) in the Yin paper.
    for (int j = 0; j < m_yinBufferSize; ++j) {
        yinBuffer[j] = powerTerms[0] + powerTerms[j] - 2 *
            audioOutReal[j+m_yinBufferSize-1];
    }
    delete [] audioTransformedComplex;
    delete [] audioOutReal;
    delete [] kernel;
    delete [] kernelTransformedComplex;
    delete [] yinStyleACFComplex;
    delete [] powerTerms;
}


void 
YinUtil::cumulativeDifference(double *yinBuffer)
{    
    int tau;
    
    yinBuffer[0] = 1;
    
    double runningSum = 0;
    
    for (tau = 1; tau < m_yinBufferSize; ++tau) {
        runningSum += yinBuffer[tau];
        if (runningSum == 0)
        {
            yinBuffer[tau] = 1;
        } else {
            yinBuffer[tau] *= tau / runningSum;
        }
    }    
}

int 
YinUtil::absoluteThreshold(const double *yinBuffer, double thresh)
{
    int tau;
    int minTau = 0;
    double minVal = 1000.;
    
    // using Joren Six's "loop construct" from TarsosDSP
    tau = 2;
    while (tau < m_yinBufferSize)
    {
        if (yinBuffer[tau] < thresh)
        {
            while (tau+1 < m_yinBufferSize && yinBuffer[tau+1] < yinBuffer[tau])
            {
                ++tau;
            }
            return tau;
        } else {
            if (yinBuffer[tau] < minVal)
            {
                minVal = yinBuffer[tau];
                minTau = tau;
            }
        }
        ++tau;
    }
    if (minTau > 0)
    {
        return -minTau;
    }
    return 0;
}

#pragma GCC diagnostic ignored "-Wfloat-conversion"

static float uniformDist[100] = {0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000,0.0100000};
static float betaDist1[100] = {0.028911,0.048656,0.061306,0.068539,0.071703,0.071877,0.069915,0.066489,0.062117,0.057199,0.052034,0.046844,0.041786,0.036971,0.032470,0.028323,0.024549,0.021153,0.018124,0.015446,0.013096,0.011048,0.009275,0.007750,0.006445,0.005336,0.004397,0.003606,0.002945,0.002394,0.001937,0.001560,0.001250,0.000998,0.000792,0.000626,0.000492,0.000385,0.000300,0.000232,0.000179,0.000137,0.000104,0.000079,0.000060,0.000045,0.000033,0.000024,0.000018,0.000013,0.000009,0.000007,0.000005,0.000003,0.000002,0.000002,0.000001,0.000001,0.000001,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000};
static float betaDist2[100] = {0.012614,0.022715,0.030646,0.036712,0.041184,0.044301,0.046277,0.047298,0.047528,0.047110,0.046171,0.044817,0.043144,0.041231,0.039147,0.036950,0.034690,0.032406,0.030133,0.027898,0.025722,0.023624,0.021614,0.019704,0.017900,0.016205,0.014621,0.013148,0.011785,0.010530,0.009377,0.008324,0.007366,0.006497,0.005712,0.005005,0.004372,0.003806,0.003302,0.002855,0.002460,0.002112,0.001806,0.001539,0.001307,0.001105,0.000931,0.000781,0.000652,0.000542,0.000449,0.000370,0.000303,0.000247,0.000201,0.000162,0.000130,0.000104,0.000082,0.000065,0.000051,0.000039,0.000030,0.000023,0.000018,0.000013,0.000010,0.000007,0.000005,0.000004,0.000003,0.000002,0.000001,0.000001,0.000001,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000};
static float betaDist3[100] = {0.006715,0.012509,0.017463,0.021655,0.025155,0.028031,0.030344,0.032151,0.033506,0.034458,0.035052,0.035331,0.035332,0.035092,0.034643,0.034015,0.033234,0.032327,0.031314,0.030217,0.029054,0.027841,0.026592,0.025322,0.024042,0.022761,0.021489,0.020234,0.019002,0.017799,0.016630,0.015499,0.014409,0.013362,0.012361,0.011407,0.010500,0.009641,0.008830,0.008067,0.007351,0.006681,0.006056,0.005475,0.004936,0.004437,0.003978,0.003555,0.003168,0.002814,0.002492,0.002199,0.001934,0.001695,0.001481,0.001288,0.001116,0.000963,0.000828,0.000708,0.000603,0.000511,0.000431,0.000361,0.000301,0.000250,0.000206,0.000168,0.000137,0.000110,0.000088,0.000070,0.000055,0.000043,0.000033,0.000025,0.000019,0.000014,0.000010,0.000007,0.000005,0.000004,0.000002,0.000002,0.000001,0.000001,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000};
static float betaDist4[100] = {0.003996,0.007596,0.010824,0.013703,0.016255,0.018501,0.020460,0.022153,0.023597,0.024809,0.025807,0.026607,0.027223,0.027671,0.027963,0.028114,0.028135,0.028038,0.027834,0.027535,0.027149,0.026687,0.026157,0.025567,0.024926,0.024240,0.023517,0.022763,0.021983,0.021184,0.020371,0.019548,0.018719,0.017890,0.017062,0.016241,0.015428,0.014627,0.013839,0.013068,0.012315,0.011582,0.010870,0.010181,0.009515,0.008874,0.008258,0.007668,0.007103,0.006565,0.006053,0.005567,0.005107,0.004673,0.004264,0.003880,0.003521,0.003185,0.002872,0.002581,0.002312,0.002064,0.001835,0.001626,0.001434,0.001260,0.001102,0.000959,0.000830,0.000715,0.000612,0.000521,0.000440,0.000369,0.000308,0.000254,0.000208,0.000169,0.000136,0.000108,0.000084,0.000065,0.000050,0.000037,0.000027,0.000019,0.000014,0.000009,0.000006,0.000004,0.000002,0.000001,0.000001,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000,0.000000};
static float single10[100] = {0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,1.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000};
static float single15[100] = {0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,1.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000};
static float single20[100] = {0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,1.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000,0.00000};

std::vector<double>
YinUtil::yinProb(const double *yinBuffer, int prior, int minTau0, int maxTau0) 
{
    int minTau = 2;
    int maxTau = m_yinBufferSize;

    // adapt period range, if necessary
    if (minTau0 > 0 && minTau0 < maxTau0) minTau = minTau0;
    if (maxTau0 > 0 && maxTau0 < m_yinBufferSize && maxTau0 > minTau) maxTau = maxTau0;

    double minWeight = 0.01;
    int tau;
    std::vector<float> thresholds;
    std::vector<float> distribution;
    std::vector<double> peakProb = std::vector<double>(m_yinBufferSize);
    
    int nThreshold = 100;
    int nThresholdInt = nThreshold;
    
    for (int i = 0; i < nThresholdInt; ++i)
    {
        switch (prior) {
            case 0:
                distribution.push_back(uniformDist[i]);
                break;
            case 1:
                distribution.push_back(betaDist1[i]);
                break;
            case 2:
                distribution.push_back(betaDist2[i]);
                break;
            case 3:
                distribution.push_back(betaDist3[i]);
                break;
            case 4:
                distribution.push_back(betaDist4[i]);
                break;
            case 5:
                distribution.push_back(single10[i]);
                break;
            case 6:
                distribution.push_back(single15[i]);
                break;
            case 7:
                distribution.push_back(single20[i]);
                break;
            default:
                distribution.push_back(uniformDist[i]);
        }
        thresholds.push_back(0.01 + i*0.01);
    }
    
    
    int currThreshInd = nThreshold-1;
    tau = minTau;
    
    // double factor = 1.0 / (0.25 * (nThresholdInt+1) * (nThresholdInt + 1)); // factor to scale down triangular weight
    int minInd = 0;
    float minVal = 42.f;
    // while (currThreshInd != -1 && tau < maxTau)
    // {
    //     if (yinBuffer[tau] < thresholds[currThreshInd])
    //     {
    //         while (tau + 1 < maxTau && yinBuffer[tau+1] < yinBuffer[tau])
    //         {
    //             tau++;
    //         }
    //         // tau is now local minimum
    //         // std::cerr << tau << " " << currThreshInd << " "<< thresholds[currThreshInd] << " " << distribution[currThreshInd] << std::endl;
    //         if (yinBuffer[tau] < minVal && tau > 2){
    //             minVal = yinBuffer[tau];
    //             minInd = tau;
    //         }
    //         peakProb[tau] += distribution[currThreshInd];
    //         currThreshInd--;
    //     } else {
    //         tau++;
    //     }
    // }
    // double nonPeakProb = 1;
    // for (int i = minTau; i < maxTau; ++i)
    // {
    //     nonPeakProb -= peakProb[i];
    // }
    // 
    // std::cerr << tau << " " << currThreshInd << " "<< thresholds[currThreshInd] << " " << distribution[currThreshInd] << std::endl;
    float sumProb = 0;
    while (tau+1 < maxTau)
    {
        if (yinBuffer[tau] < thresholds[thresholds.size()-1] && yinBuffer[tau+1] < yinBuffer[tau])
        {
            while (tau + 1 < maxTau && yinBuffer[tau+1] < yinBuffer[tau])
            {
                tau++;
            }
            // tau is now local minimum
            // std::cerr << tau << " " << currThreshInd << " "<< thresholds[currThreshInd] << " " << distribution[currThreshInd] << std::endl;
            if (yinBuffer[tau] < minVal && tau > 2){
                minVal = yinBuffer[tau];
                minInd = tau;
            }
            currThreshInd = nThresholdInt-1;
            while (currThreshInd > -1 && thresholds[currThreshInd] > yinBuffer[tau]) {
                // std::cerr << distribution[currThreshInd] << std::endl;
                peakProb[tau] += distribution[currThreshInd];
                currThreshInd--;
            }
            // peakProb[tau] = 1 - yinBuffer[tau];
            sumProb += peakProb[tau];
            tau++;
        } else {
            tau++;
        }
    }
    
    if (peakProb[minInd] > 1) {
        std::cerr << "WARNING: yin has prob > 1 ??? I'm returning all zeros instead." << std::endl;
        return(std::vector<double>(m_yinBufferSize));
    }
    
    double nonPeakProb = 1;
    if (sumProb > 0) {
        for (int i = minTau; i < maxTau; ++i)
        {
            peakProb[i] = peakProb[i] / sumProb * peakProb[minInd];
            nonPeakProb -= peakProb[i];
        }
    }
    if (minInd > 0)
    {
        // std::cerr << "min set " << minVal << " " << minInd << " " << nonPeakProb << std::endl; 
        peakProb[minInd] += nonPeakProb * minWeight;
    }
    
    return peakProb;
}

double
YinUtil::parabolicInterpolation(const double *yinBuffer, int tau) 
{
    // this is taken almost literally from Joren Six's Java implementation
    if (tau == m_yinBufferSize) // not valid anyway.
    {
        return static_cast<double>(tau);
    }
    
    double betterTau = 0.0;
    if (tau > 0 && tau < m_yinBufferSize-1) {
        float s0, s1, s2;
        s0 = yinBuffer[tau-1];
        s1 = yinBuffer[tau];
        s2 = yinBuffer[tau+1];
        
        double adjustment = (s2 - s0) / (2 * (2 * s1 - s2 - s0));
        
        if (abs(adjustment)>1) adjustment = 0;
        
        betterTau = tau + adjustment;
    } else {
        // std::cerr << "WARNING: can't do interpolation at the edge (tau = " << tau << "), will return un-interpolated value.\n";
        betterTau = tau;
    }
    return betterTau;
}

double 
YinUtil::sumSquare(const double *in, int start, int end)
{
    double out = 0;
    for (int i = start; i < end; ++i)
    {
        out += in[i] * in[i];
    }
    return out;
}
