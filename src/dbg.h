/*
 * dbg.h
 *
 *  Created on: Aug 15, 2020
 *      Author: anhpt0135
 */

#ifndef SRC_DBG_H_
#define SRC_DBG_H_

#include <stdio.h>
#include <string.h>

#ifdef NDEBUG
#define Dbg_info(M, ...)
#else
#define Dbg_info(M, ...) printf("[%s: %s: %d]___"M		\
		"\r\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#endif

#endif /* SRC_DBG_H_ */
