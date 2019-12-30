//
//  phaseOne.hpp
//  TRSPV_7
//
//  Created by Emin Bari on 01.12.2019.
//  Copyright © 2019 Emin Bari. All rights reserved.
//

#ifndef phaseOne_hpp
#define phaseOne_hpp

#include <stdio.h>

void swap(int* a, int* b);
int partition (int arr[], int low, int high);
void quickSort(int arr[], int low, int high, int pivot);
void printArray(int arr[], int size);
void printSubArray();
int countPivot(int quatroArr[]);


#endif /* phaseOne_hpp */
