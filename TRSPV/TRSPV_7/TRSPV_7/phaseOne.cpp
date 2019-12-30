//
//  phaseOne.cpp
//  TRSPV_7
//
//  Created by Emin Bari on 01.12.2019.
//  Copyright © 2019 Emin Bari. All rights reserved.
//

#include "phaseOne.hpp"
#include <iostream>
#include <cmath>
#include <mpi.h>
#include <vector>
#include <stdlib.h>

using namespace std;

int countPivot(int quatroArr[]){
    int pivot = 0;
    for (int i = 0; i < 4; i++) {
       pivot += quatroArr[i];
    }
    pivot = ceil(pivot/4);
    
    pivotElementsCount = pivot == 0 ? 1 : int (log10 (pivot) + 1);
    return(pivot);
}

void printSubArray(){
    cout<<"Process "<<processRank<<endl;
    for (int i = 0; i < 4; i++) {
        cout<<subArr[i]<<" ";
    }
    cout<<endl;
}

void printArray(int arr[], int size)
{
    for (int i = 0; i < size; i++)
        cout << arr[i] << " ";
    cout << endl;
}

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

int partition (int arr[], int low, int high, int pivot)
{
    int i = low - 1;
    int j = high;
    
    for(;;)
    {
      while(arr[++i] < pivot)
          ;

      while(pivot < arr[--j])
        if(j == low)
          break;

      if( i >= j)
        break;

      swap(&arr[i], &arr[j]);
    }

    swap(&arr[i], &arr[high]);
    
    return i;
}

void quickSort(int arr[], int low, int high, int pivot)
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
        at right place */
//        int pi = partition(arr, low, high, pivot);
        partition(arr, low, high, pivot);
//
//        cout<<"leftSide"<<endl;
//        for (int i = 0; i< pi; i++) {
//           cout<<arr[i]<<" ";
//
//        }
//
//        cout<<"rightSide"<<endl;
//        for (int i = pi; i< high+1; i++) {
//            cout<<arr[i]<<" ";
//        }
//        cout<<endl;
//
        
        for (int i = 0; i< 4; i++) {
            if (arr[i] < pivot) {
                leftVec.push_back(arr[i]);
            }
            else{
                rightVec.push_back(arr[i]);
            }
        }
        
        cout<<endl;
        cout<<"left SubArray:"<<endl;
        for(int i:leftVec)
            cout<<i<<" ";
        cout<<endl;
        cout<<"right SubArray:"<<endl;
        for(int i:rightVec)
            cout<<i<<" ";
        cout<<endl;
        
        leftArr = new int[leftVec.size()];
        rightArr = new int[rightVec.size()];
        
        //vector to array
        copy(leftVec.begin(), leftVec.end(), leftArr);
        copy(rightVec.begin(), rightVec.end(), rightArr);
        
//        cout<<"leftArr"<<endl;
//        for (int i = 0; i<leftVec.size(); i++) {
//            cout<<leftArr[i]<<" ";
//        }
//
//        cout<<"rightArr"<<endl;
//        for (int i = 0; i<rightVec.size(); i++) {
//            cout<<rightArr[i]<<" ";
//        }
        
        cout<<endl;
        cout<<"Sorted SubArray"<<endl;
        printArray(subArr, 4);
    }
}

