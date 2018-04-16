/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */
/*
    This file is Copyright (c) 2012 Chris Cannam
  
    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "MeanFilter.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TestMeanFilter)

BOOST_AUTO_TEST_CASE(simpleFilter)
{
    double in[] = { 1.0, 2.0, 3.0 };
    double out[3];
    MeanFilter(3).filter(in, out, 3);
    BOOST_CHECK_EQUAL(out[0], 1.5);
    BOOST_CHECK_EQUAL(out[1], 2.0);
    BOOST_CHECK_EQUAL(out[2], 2.5);
}

BOOST_AUTO_TEST_CASE(simpleSubset)
{
    double in[] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
    double out[3];
    MeanFilter(3).filterSubsequence(in, out, 5, 3, 1);
    BOOST_CHECK_EQUAL(out[0], 2.0);
    BOOST_CHECK_EQUAL(out[1], 3.0);
    BOOST_CHECK_EQUAL(out[2], 4.0);
}

BOOST_AUTO_TEST_CASE(flenExceedsArraySize)
{
    double in[] = { 1.0, 2.0, 3.0 };
    double out[3];
    MeanFilter(5).filter(in, out, 3);
    BOOST_CHECK_EQUAL(out[0], 2.0);
    BOOST_CHECK_EQUAL(out[1], 2.0);
    BOOST_CHECK_EQUAL(out[2], 2.0);
}

BOOST_AUTO_TEST_CASE(flenIs1)
{
    double in[] = { 1.0, 2.0, 3.0 };
    double out[3];
    MeanFilter(1).filter(in, out, 3);
    BOOST_CHECK_EQUAL(out[0], in[0]);
    BOOST_CHECK_EQUAL(out[1], in[1]);
    BOOST_CHECK_EQUAL(out[2], in[2]);
}

BOOST_AUTO_TEST_CASE(arraySizeIs1)
{
    double in[] = { 1.0 };
    double out[1];
    MeanFilter(3).filter(in, out, 1);
    BOOST_CHECK_EQUAL(out[0], in[0]);
}

BOOST_AUTO_TEST_CASE(subsequenceLengthIs1)
{
    double in[] = { 1.0, 2.0, 3.0 };
    double out[1];
    MeanFilter(3).filterSubsequence(in, out, 3, 1, 2);
    BOOST_CHECK_EQUAL(out[0], 2.5);
}

BOOST_AUTO_TEST_SUITE_END()

