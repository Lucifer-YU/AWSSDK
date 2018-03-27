/*
 * Foundation.h
 *
 *  Created on: Nov 26, 2014
 *      Author: Lucifer
 */

#ifndef __BFX_FOUNDATION_H__
#define __BFX_FOUNDATION_H__

#ifdef	WIN32
#ifdef	_DEBUG
//#include <vld.h>	// Visual Leak Detector
#endif
#endif

#ifndef BFX_ASSERT
#include <assert.h>
#define	BFX_ASSERT	assert
#endif
#ifndef BFX_MAX
#define BFX_MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef BFX_MIN
#define BFX_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#include "Logger.h"
#include "Holder.h"
#include "Lock.h"
#include "REF.h"
#include "ArrayList.h"
#include "REFAutoreleasePool.h"
#include "Buffer.h"
#include "StringT.h"
#include "LinkedList.h"
#include "HashMap.h"
#include "TreeMap.h"
#include "DateTime.h"

#endif /* __BFX_FOUNDATION_H__ */
