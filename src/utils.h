/*
 * utils.h
 *
 *  Created on: 13 déc. 2018
 *      Author: guilhem
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <math.h>

template<class T> constexpr const T& clamp(const T& lo,const T& v, const T& hi )
{
    return min(hi, max(lo, v));
}


#endif /* UTILS_H_ */
