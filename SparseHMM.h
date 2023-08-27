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

#ifndef _SPARSEHMM_H_
#define _SPARSEHMM_H_

#include <vector>
#include <queue>
#include <cstdio>

using std::vector;
using std::deque;
using std::pair;

class SparseHMM
{
public:
    SparseHMM(int fixedLag); // set fixedLag == 0 when doing full Viterbi
    
    virtual std::vector<double> calculateObsProb
    (const vector<pair<double, double> > &) = 0;
    
    virtual void           build();
    std::vector<int>       decodeViterbi(std::vector<vector<double> > obs);
    void                   reset();
    void                   initialise(vector<double> firstObs);
    int                    process(vector<double> newObs);
    vector<int>            track();
    
    // "sparse" HMM definition
    int m_fixedLag;
    int m_nState;
    int m_nTrans;
    vector<double> m_init;
    vector<size_t> m_from;
    vector<size_t> m_to;
    vector<double> m_transProb;

    // variables for decoding
    deque<double> m_scale;
    deque<vector<int> > m_psi;
    vector<double> m_delta;
    vector<double> m_oldDelta;
};

#endif
