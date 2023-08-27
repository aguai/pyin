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

#include "MonoNote.h"
#include <vector>

#include <cstdio>
#include <cmath>
#include <complex>

using std::vector;
using std::pair;

MonoNote::MonoNote(bool fixedLag) :
    m_fixedLag(fixedLag),
    hmm(m_fixedLag ? 1000 : 0)
{
}

MonoNote::~MonoNote()
{
}

const vector<MonoNote::FrameOutput>
MonoNote::process(const vector<vector<pair<double, double> > > pitchProb)
{
    // Previously, this built up a single matrix of probabilities, by
    // calling calculateObsProb to get a column for each frame in
    // pitchProb.
    //
    // The number of distinct states depends on MonoNoteParameters,
    // but the defaults have 3 states per pitch, 3 pitches per MIDI
    // note, and 69 MIDI notes, giving 681 states per frame. With a
    // frame step size of 256 at 44100Hz sample rate, a 3-minute song
    // has about 30K frames leading to a 20 million element
    // probability matrix.
    //
    // Since the matrix is very sparse, we can avoid some of this by
    // feeding the (sparse implementation of) HMM one column at a
    // time.

    vector<int> path;
    path.reserve(pitchProb.size());
    
    if (!pitchProb.empty()) {

        hmm.initialise(hmm.calculateObsProb(pitchProb[0]));
        
        for (size_t iFrame = 1; iFrame < pitchProb.size(); ++iFrame)
        {
            if (m_fixedLag && (int(iFrame) >= hmm.m_fixedLag)) 
            {
                vector<int> rawPath = hmm.track();
                path.push_back(rawPath[0]);
            }

            hmm.process(hmm.calculateObsProb(pitchProb[iFrame]));
        }

        vector<int> rawPath = hmm.track();
        path.insert(path.end(), rawPath.begin(), rawPath.end());
    }
    
    vector<MonoNote::FrameOutput> out;
    out.reserve(path.size());

    for (size_t iFrame = 0; iFrame < path.size(); ++iFrame)
    {
        double currPitch = -1;
        int stateKind = 0;

        currPitch = hmm.par.minPitch + (path[iFrame]/hmm.par.nSPP) * 1.0/hmm.par.nPPS;
        stateKind = (path[iFrame]) % hmm.par.nSPP + 1;

        out.push_back(FrameOutput(iFrame, currPitch, stateKind));
    }
    
    return(out);
}
