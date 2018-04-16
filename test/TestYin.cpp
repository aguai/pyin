/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

#include "Yin.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TestYin)

BOOST_AUTO_TEST_CASE(implicitYinOutput)
{
    // this test is just to make sure a YinOutput initialises
    // -- the actual reason for me to write this is to have written 
    // a test, so let's plough on...
    Yin::YinOutput out;
    
    BOOST_CHECK_EQUAL(out.f0, 0);
    BOOST_CHECK_EQUAL(out.periodicity, 0);
    BOOST_CHECK_EQUAL(out.rms, 0);
}

BOOST_AUTO_TEST_CASE(sine128)
{
    
    // a very short frame (8) with maximum frequency (128),
    // assuming a sampling rate of 256 1/sec.
    // Yin should get it approximately right... 
    // but because of post-processing we want to be tolerant

    double in[] = { 1, -1, 1, -1, 1, -1, 1, -1 };
    Yin y(8, 256);
    Yin::YinOutput yo = y.process(in);
    
    BOOST_CHECK(yo.f0 > 114);
    BOOST_CHECK(yo.f0 < 142);
}

BOOST_AUTO_TEST_SUITE_END()

