#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>

#define DEBUG 0

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);
  
#ifdef __cplusplus
}
#endif


void merge(int * arr, int l, int mid, int r, int * temp) {
  
#if DEBUG
  std::cout<<l<<" "<<mid<<" "<<r<<std::endl;
#endif

  /*// short circuits
	if (l == r) return;
	if (r-l == 1) {
		if (arr[l] > arr[r]) {
			int temp = arr[l];
			arr[l] = arr[r];
			arr[r] = temp;
		}
		return;
	}

	int i, j, k;
	int n = mid - l;

	// declare and init temp arrays

	for (i=0; i<n; ++i)
	temp[i] = arr[l+i];

	i = 0;    // temp left half
	j = mid;  // right half
	k = l;    // write to 

	// merge
	while (i<n && j<=r) {
		if (temp[i] <= arr[j] ) {
			arr[k++] = temp[i++];
		} else {
			arr[k++] = arr[j++];
		}
	}

	// exhaust temp 
	#pragma omp parallel for 
	for (int i = 0; i < n; i++)
	{	
		arr[i] = temp[i];
	} */
  
	

}

void insertionSort(int arr[], int l, int r) 
{ 
	if ((r - l) < 1) { return; }
	
	#pragma omp parallel for shared(arr)
	for (long int i = (l + 1); i <= r; i++) {
		for (long int j = i; j > l; j--) {
			if (arr[j] < arr[j - 1]) 
				std::swap(arr[j], arr[j - 1]); 
			else { 
				break; 
			}
		}
	}
} 

void mergesort(int * arr, int l, int r) {
	int s = 1000;
	
	//if (l < r) {
	if (l-r > s) {
		int *temp = new int[r-l+1];
		
		int mid = (l+r)/2;
		#pragma omp task 
		mergesort(arr, l, mid);
		
		#pragma omp task 
		mergesort(arr, mid+1, r);

		#pragma omp taskwait 
		merge(arr, l, mid+1, r,temp);
		
		delete[] temp;
	}
	else
	{
		//int n = (r-l+1);
		#pragma omp task 
		insertionSort(arr,l,r);
	}
	
}




int main (int argc, char* argv[]) {

	if (argc < 2) { std::cerr<<"Usage: "<<argv[0]<<" <n>"<<std::endl;
		return -1;
	}


	// command line parameter
	
	int nbthreads = atoi(argv[2]);
	omp_set_num_threads(nbthreads);
	int n;
	n = atoi(argv[1]);

	// get arr data
	int * arr = new int [n];
	generateMergeSortData (arr, n);

	#if DEBUG
	for (int i=0; i<n; ++i) 
		std::cout<<arr[i]<<" ";
		std::cout<<std::endl;
	#endif

	// begin timing
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

	// sort
	#pragma omp parallel
	{
		#pragma omp single
		{
			mergesort(arr, 0, n-1);
		}
	}
	// end timing
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	std::chrono::duration<double> elpased_seconds = end-start;

	// display time to cerr
	std::cerr<<elpased_seconds.count()<<std::endl;
	checkMergeSortResult (arr, n);

	#if DEBUG
	for (int i=0; i<n; ++i) 
		std::cout<<arr[i]<<" ";
		std::cout<<std::endl;
	#endif

	delete[] arr;

	return 0;
}
