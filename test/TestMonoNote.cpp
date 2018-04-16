/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

#include "MonoNote.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

using std::vector;
using std::pair;

BOOST_AUTO_TEST_SUITE(TestMonoNote)

BOOST_AUTO_TEST_CASE(instantiate)
{
    MonoNote mn;
    vector<vector<pair<double, double> > > pitchProb;
    size_t n = 8;

    vector<double> pitch;
    pitch.push_back(48);
    pitch.push_back(51);
    pitch.push_back(50);
    pitch.push_back(51);
    pitch.push_back(60);
    pitch.push_back(71);
    pitch.push_back(51);
    pitch.push_back(62);
    
    for (size_t i = 0; i < n; ++i)
    {
        vector<pair<double, double> > temp;
        temp.push_back(pair<double, double>(pitch[i], 1.0));
        pitchProb.push_back(temp);
    }

    mn.process(pitchProb);
}


BOOST_AUTO_TEST_SUITE_END()

