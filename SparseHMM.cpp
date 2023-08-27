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

#include "SparseHMM.h"
#include <vector>
#include <cstdio>
#include <iostream>

using std::vector;
using std::pair;

SparseHMM::SparseHMM(int fixedLag) :
    m_fixedLag(fixedLag),
    m_nState(0),
    m_nTrans(0),
    m_init(0),
    m_from(0),
    m_to(0),
    m_transProb(0),
    m_scale(0),
    m_psi(0),
    m_delta(0),
    m_oldDelta(0)
{

}

void
SparseHMM::build()
{ }

std::vector<int> 
SparseHMM::decodeViterbi(std::vector<vector<double> > obsProb) 
{
    int nFrame = obsProb.size();
    if (nFrame < 1) {
        return vector<int>();
    }

    initialise(obsProb[0]);

    // rest of forward step
    for (int iFrame = 1; iFrame < nFrame; ++iFrame)
    {
        process(obsProb[iFrame]);
    }

    vector<int> path = track();
    return(path);
}

void
SparseHMM::reset()
{
    m_scale.clear();
    m_psi.clear();
    for (int i = 0; i < int(m_delta.size()); ++i) m_delta[i] = 0;
    for (int i = 0; i < int(m_oldDelta.size()); ++i) m_oldDelta[i] = 0;
}

void
SparseHMM::initialise(vector<double> firstObs)
{
    reset();

    double deltasum = 0;

    // initialise first frame
    for (int iState = 0; iState < m_nState; ++iState)
    {
        m_oldDelta[iState] = m_init[iState] * firstObs[iState];
        deltasum += m_oldDelta[iState];
    }

    for (int iState = 0; iState < m_nState; ++iState)
    {
        m_oldDelta[iState] /= deltasum; // normalise (scale)
    }

    m_scale.push_back(1.0/deltasum);
    m_psi.push_back(vector<int>(m_nState,0));
}

int
SparseHMM::process(vector<double> newObs)
{
    vector<int> tempPsi = vector<int>(m_nState,0);

    // calculate best previous state for every current state
    int fromState;
    int toState;
    double currentTransProb;
    double currentValue;
    
    // this is the "sparse" loop
    for (int iTrans = 0; iTrans < m_nTrans; ++iTrans)
    {
        fromState = m_from[iTrans];
        toState = m_to[iTrans];
        currentTransProb = m_transProb[iTrans];
        
        currentValue = m_oldDelta[fromState] * currentTransProb;
        if (currentValue > m_delta[toState])
        {
            // will be multiplied by the right obs later!
            m_delta[toState] = currentValue;
            tempPsi[toState] = fromState;
        }
    }
    m_psi.push_back(tempPsi);

    double deltasum = 0;
    for (int jState = 0; jState < m_nState; ++jState)
    {
        m_delta[jState] *= newObs[jState];
        deltasum += m_delta[jState];
    }

    if (deltasum > 0)
    {
        for (int iState = 0; iState < m_nState; ++iState)
        {
            m_oldDelta[iState] = m_delta[iState] / deltasum;// normalise (scale)
            m_delta[iState] = 0;
        }
        m_scale.push_back(1.0/deltasum);
    } else
    {
//        std::cerr << "WARNING: Viterbi has been fed some zero "
//            "probabilities, at least they become zero "
//            "in combination with the model." << std::endl;
        for (int iState = 0; iState < m_nState; ++iState)
        {
            m_oldDelta[iState] = 1.0/m_nState;
            m_delta[iState] = 0;
        }
        m_scale.push_back(1.0);
    }

    if (m_fixedLag > 0 && int(m_psi.size()) > m_fixedLag)
    {
        m_psi.pop_front();
        m_scale.pop_front();
    }

    // std::cerr << m_fixedLag << " " << m_psi.size() << std::endl;

    return 0;
}

vector<int>
SparseHMM::track()
{
    // initialise backward step
    int nFrame = m_psi.size();

    // The final output path (current assignment arbitrary, makes sense only for 
    // Chordino, where nChord-1 is the "no chord" label)
    vector<int> path = vector<int>(nFrame, m_nState-1);

    double bestValue = 0;
    for (int iState = 0; iState < m_nState; ++iState)
    {
        double currentValue = m_oldDelta[iState];
        if (currentValue > bestValue)
        {
            bestValue = currentValue;            
            path[nFrame-1] = iState;
        }
    }

    // Rest of backward step
    for (int iFrame = nFrame-2; iFrame != -1; --iFrame)
    {
        path[iFrame] = m_psi[iFrame+1][path[iFrame+1]];
    }
        
    return path;
}
