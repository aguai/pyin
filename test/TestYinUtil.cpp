/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

#include "YinUtil.h"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(TestYin)

BOOST_AUTO_TEST_CASE(difference1)
{
    // difference always needs to be non-negative
    double in[] = { 1, -1, 1, -1, 1, -1, 1, -1 };
    double yinBuffer[4];
    
    YinUtil::difference(in, yinBuffer, 4);
    
    BOOST_CHECK(yinBuffer[0] >= 0);
    BOOST_CHECK(yinBuffer[1] >= 0);
    BOOST_CHECK(yinBuffer[2] >= 0);
    BOOST_CHECK(yinBuffer[3] >= 0);
}

// BOOST_AUTO_TEST_CASE(difference2)
// {
//     // difference for fast sinusoid
//     double in[] = { 1, -1, 1, -1, 1, -1, 1, -1 };
//     double yinBuffer[4];
//     
//     Yin y(8, 256);
//     
//     y.difference(in, yinBuffer);
//     
//     BOOST_CHECK_EQUAL(yinBuffer[0], 0);
//     BOOST_CHECK_EQUAL(yinBuffer[1], 16);
//     BOOST_CHECK_EQUAL(yinBuffer[2], 0);
//     BOOST_CHECK_EQUAL(yinBuffer[3], 16);
// }
// 
// BOOST_AUTO_TEST_CASE(difference3)
// {
//     // difference for contaminated fast sinusoid
//     double in[] = { 2, -1, 1, -1, 1, -1, 1, -1 };
//     double yinBuffer[4];
//     
//     Yin y(8, 256);
//     
//     y.difference(in, yinBuffer);
//     
//     BOOST_CHECK_EQUAL(yinBuffer[0], 0);
//     BOOST_CHECK_EQUAL(yinBuffer[1], 21);
//     BOOST_CHECK_EQUAL(yinBuffer[2], 1);
//     BOOST_CHECK_EQUAL(yinBuffer[3], 21);
// }
// 
// BOOST_AUTO_TEST_CASE(difference4)
// {
//     // 
//     double in[2048];
//     double yinBuffer[1024];
//     for (size_t i = 0; i < 2048; i = i + 2) {
//         in[i] = 1;
//         in[i+1] = -1;
//     }
//     Yin y(2048, 44100);
//     
//     y.difference(in, yinBuffer);
//     
//     BOOST_CHECK(yinBuffer[0] >= 0);
//     BOOST_CHECK(yinBuffer[1] >= 0);
// }
// 
// BOOST_AUTO_TEST_CASE(cumulativeDifference1)
// {
//     // test against matlab implementation
//     double yinBuffer[] = {0, 21, 1, 21};
//     
//     Yin y(8, 256);
//     
//     y.cumulativeDifference(yinBuffer);
//     
//     BOOST_CHECK_EQUAL(yinBuffer[0], 1);
//     BOOST_CHECK_EQUAL(yinBuffer[1], 1); // this is just chance!
//     BOOST_CHECK(yinBuffer[2] >= 0.0909 && yinBuffer[2] < 0.091);
//     BOOST_CHECK(yinBuffer[3] >= 1.4651 && yinBuffer[3] < 1.466);
// 
// }



BOOST_AUTO_TEST_SUITE_END()

