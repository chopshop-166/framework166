/*******************************************************************************
*  Project   		: Framework
*  File Name  		: Runtime166.cpp
*  Owner		   	: Software Group (FIRST Chopshop Team 166)
*  Creation Date	: January 18, 2010
*  File Description	: Class that generates runtime statistics
*******************************************************************************/ 
/*----------------------------------------------------------------------------*/
/*  Copyright (c) MHS Chopshop Team 166, 2010.  All Rights Reserved.          */
/*----------------------------------------------------------------------------*/

#include "Runtime166.h"
#include <cmath>
#include <string>
using namespace std;

Runtime166::Runtime166(int lim) {
	limit = lim;
	r = new char[500]; // Returned string
}

void Runtime166::Start() {
	t.Start();
}

void Runtime166::Stop() {
	t.Stop();
	loop_times.push_back(t.Get()); // Appends the last loop time to the list.
	t.Reset();
	if(limit > 0 && (int)loop_times.size() > limit) { 
		// If we've tracked too many times, delete the oldest ones.
		// Delete from the first element (loop_times.begin()) to the oldest one under the limit.
		loop_times.erase(loop_times.begin(), loop_times.begin()+(loop_times.size() - limit));
	}
}

unsigned int Runtime166::Loops() {
	return loop_times.size();
}

char* Runtime166::GetStats() {
	if(loop_times.size() == 0) {
		return "";
	}
	
	vector<float>::iterator it = loop_times.begin(); 
	float max = *it;
	float min = *it;
	float sx = 0; // Standard deviation
	int n = loop_times.size();
	float avg = 0;
	
	do { // Iterate over every element in the loop_times vector
		if(max < *it) // Dereferencing the iterater returns the "current" element
			max = *it;
		if(min > *it)
			min = *it;
		
		avg += *it;
	} while((++it) != loop_times.end()); // Adding to the iterator moves it along.
	avg /= n;
	
	it = loop_times.begin();
	do {
		sx += pow((avg - *it), 2);
	} while((++it) != loop_times.end());
	
	sx = sqrt(sx / (n-1));
	
	sprintf(r, "[n %d] [max %f] [min %f] [avg %f] [sx %f]", n, max, min, avg, sx);
	return r;
}

void Runtime166::Reset() {
	loop_times.clear();
}
