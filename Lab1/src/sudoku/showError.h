/*
 * @Author: your name
 * @Date: 2020-03-28 23:50:13
 * @LastEditTime: 2020-03-29 00:01:19
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CloudComputingLabs/Lab1/src/sudoku/showError.h
 */

#ifndef  SHOWERROR_H
#define SHOWERROR_H
#include <iostream>

/* Status Code
 * 1 - about File > 100、101、102……
 * 2 - about ? > 200、201、202……
 * 3 - about ? > 300、301、302……
 */
void showError(short statusCode)
{
    switch(statusCode)
    {
        case 100: printf("Error <100> | File reads nothing.");break;
	case 101: printf("Error <101> | ?.");break;
	default: break;
    }
}


#endif
