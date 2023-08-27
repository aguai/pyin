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

#include "MonoPitchHMM.h"

#include <boost/math/distributions.hpp>

#include <cstdio>
#include <cmath>
#include <iostream>

using std::vector;
using std::pair;

MonoPitchHMM::MonoPitchHMM(int fixedLag) :
    SparseHMM(fixedLag),
    m_minFreq(61.735),
    m_nBPS(5),
    m_nPitch(0),
    m_transitionWidth(0),
    m_selfTrans(0.99),
    m_yinTrust(.5),
    m_freqs(0)
{
    m_transitionWidth = 5*(m_nBPS/2) + 1;
    m_nPitch = 69 * m_nBPS;
    m_nState = 2 * m_nPitch; // voiced and unvoiced
    m_freqs = vector<double>(2*m_nPitch);
    for (int iPitch = 0; iPitch < m_nPitch; ++iPitch)
    {
        m_freqs[iPitch] = m_minFreq * std::pow(2, iPitch * 1.0 / (12 * m_nBPS));
        m_freqs[iPitch+m_nPitch] = -m_freqs[iPitch];
    }
    build();
}

vector<double>
MonoPitchHMM::calculateObsProb(const vector<pair<double, double> > &pitchProb)
{
    vector<double> out = vector<double>(2*m_nPitch+1);
    double probYinPitched = 0;
    int nPair = int(pitchProb.size());
    
    // BIN THE PITCHES
    for (int iPair = 0; iPair < nPair; ++iPair)
    {
        double freq = 440. * std::pow(2, (pitchProb[iPair].first - 69)/12);
        if (freq <= m_minFreq) continue;
        double d = 0;
        double oldd = 1000;
        for (int iPitch = 0; iPitch < m_nPitch; ++iPitch)
        {
            d = std::abs(freq-m_freqs[iPitch]);
            if (oldd < d && iPitch > 0)
            {
                // previous bin must have been the closest
                out[iPitch-1] = pitchProb[iPair].second;
                probYinPitched += out[iPitch-1];
                break;
            }
            oldd = d;
        }
    }
    
    double probReallyPitched = m_yinTrust * probYinPitched;
    // std::cerr << probReallyPitched << " " << probYinPitched << std::endl;
    // damn, I forget what this is all about...
    for (int iPitch = 0; iPitch < m_nPitch; ++iPitch)
    {
        if (probYinPitched > 0) out[iPitch] *= (probReallyPitched/probYinPitched) ;
        out[iPitch+m_nPitch] = (1 - probReallyPitched) / m_nPitch;
    }
    // out[2*m_nPitch] = m_yinTrust * (1 - probYinPitched);
    return(out);
}

void
MonoPitchHMM::build()
{
    // INITIAL VECTOR
    m_init = vector<double>(2*m_nPitch, 1.0 / 2*m_nPitch);
    
    // TRANSITIONS
    for (int iPitch = 0; iPitch < int(m_nPitch); ++iPitch)
    {
        int theoreticalMinNextPitch = iPitch-m_transitionWidth/2;
        int minNextPitch = iPitch>m_transitionWidth/2 ? iPitch-m_transitionWidth/2 : 0;
        int maxNextPitch = iPitch<m_nPitch-m_transitionWidth/2 ? iPitch+m_transitionWidth/2 : m_nPitch-1;
        
        // WEIGHT VECTOR
        double weightSum = 0;
        vector<double> weights;
        for (int i = minNextPitch; i <= maxNextPitch; ++i)
        {
            if (i <= iPitch)
            {
                weights.push_back(i-theoreticalMinNextPitch+1);
                // weights.push_back(i-theoreticalMinNextPitch+1+m_transitionWidth/2);
            } else {
                weights.push_back(iPitch-theoreticalMinNextPitch+1-(i-iPitch));
                // weights.push_back(iPitch-theoreticalMinNextPitch+1-(i-iPitch)+m_transitionWidth/2);
            }
            weightSum += weights[weights.size()-1];
        }
        
        // std::cerr << minNextPitch << "  " << maxNextPitch << std::endl;
        // TRANSITIONS TO CLOSE PITCH
        for (int i = minNextPitch; i <= maxNextPitch; ++i)
        {
            m_from.push_back(iPitch);
            m_to.push_back(i);
            m_transProb.push_back(weights[i-minNextPitch] / weightSum * m_selfTrans);

            m_from.push_back(iPitch);
            m_to.push_back(i+m_nPitch);
            m_transProb.push_back(weights[i-minNextPitch] / weightSum * (1-m_selfTrans));

            m_from.push_back(iPitch+m_nPitch);
            m_to.push_back(i+m_nPitch);
            m_transProb.push_back(weights[i-minNextPitch] / weightSum * m_selfTrans);
            // transProb.push_back(weights[i-minNextPitch] / weightSum * 0.5);
            
            m_from.push_back(iPitch+m_nPitch);
            m_to.push_back(i);
            m_transProb.push_back(weights[i-minNextPitch] / weightSum * (1-m_selfTrans));
            // transProb.push_back(weights[i-minNextPitch] / weightSum * 0.5);
        }

        // TRANSITION TO UNVOICED
        // from.push_back(iPitch+m_nPitch);
        // to.push_back(2*m_nPitch);
        // transProb.push_back(1-m_selfTrans);
        
        // TRANSITION FROM UNVOICED TO PITCH
        // from.push_back(2*m_nPitch);
        // to.push_back(iPitch+m_nPitch);
        // transProb.push_back(1.0/m_nPitch);
    }
    // UNVOICED SELFTRANSITION
    // from.push_back(2*m_nPitch);
    // to.push_back(2*m_nPitch);
    // transProb.push_back(m_selfTrans);
    
    // for (int i = 0; i < from.size(); ++i) {
    //     std::cerr << "P(["<< from[i] << " --> " << to[i] << "]) = " << transProb[i] << std::endl;
    // }
    m_nTrans = m_transProb.size();
    m_delta = vector<double>(m_nState);
    m_oldDelta = vector<double>(m_nState);
}

/*
Takes a state number and a pitch-prob vector, then finds the pitch that would
have been closest to the pitch of the state. Easy to understand? ;)
*/
float
MonoPitchHMM::nearestFreq(int state, const vector<pair<double, double> > &pitchProb)
{
    float hmmFreq = m_freqs[state];
    // std::cerr << "hmmFreq " << hmmFreq << std::endl;
    float bestFreq = 0;
    float leastDist = 10000;
    if (hmmFreq > 0)
    {
        // This was a Yin estimate, so try to get original pitch estimate back
        // ... a bit hacky, since we could have direclty saved the frequency
        // that was assigned to the HMM bin in hmm.calculateObsProb -- but would
        // have had to rethink the interface of that method.

        // std::cerr << "pitch prob size " << pitchProb.size() << std::endl;

        for (size_t iPt = 0; iPt < pitchProb.size(); ++iPt)
        {
            float freq = 440. * 
                         std::pow(2, 
                                  (pitchProb[iPt].first - 69)/12);
            float dist = std::abs(hmmFreq-freq);
            if (dist < leastDist)
            {
                leastDist = dist;
                bestFreq = freq;
            }
        }
    } else {
        bestFreq = hmmFreq;
    }
    return bestFreq;
}
