#include <omp.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

#ifdef __cplusplus
}
#endif


void merge(int * arr, int l, int mid, int r, int * temp, int n1) 
	
//void merge(int * arr, int start, int mid, int end, int * temp, int n)
{
	// short circuits
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
	while (i<n) {
		arr[k++] = temp[i++];
	}
}	

void mergesort(int* arr, int left, int right, int n, int * temp) 
{
	//if (left < right) {
	if(n<1000)
	{
		//mergesort_serial (arr, n, temp);
		
		int mid = (left+right)/2;
		//int mid = left+(right-left)/2;
	
		mergesort(arr, left, mid, n, temp);
		mergesort(arr, mid+1, right, n, temp);
		merge(arr, left, mid+1, right,temp,n);
	}
	else
	{
		if (left < right) {	
			int mid = (left+right)/2;
			//int mid = left+(right-left)/2;
		
			#pragma omp task 
			mergesort(arr, left, mid, n, temp);
			
			#pragma omp task 
			mergesort(arr, mid+1, right, n, temp);
				
			
			#pragma omp taskwait
			merge(arr, left, mid+1, right,temp,n);
		}
	}
}

int main (int argc, char* argv[]) {

	//forces openmp to create the threads beforehand
	#pragma omp parallel
	{
		int fd = open (argv[0], O_RDONLY);
		if (fd != -1) {
			close (fd);
		}
		else {
			std::cerr<<"something is amiss"<<std::endl;
		}
	}

	if (argc < 3) { std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
		return -1;
	}

	int nbthreads = atoi(argv[2]);
	omp_set_num_threads(nbthreads);
	
	int n = atoi(argv[1]);

	// get arr data
	int * arr = new int [n];
	int * temp = new int [n];
	
	// get arr data
	generateMergeSortData (arr, n);
	for(int i = 0;i<n;i++)
		std::cout<<arr[i]<<std::endl;
	//insert sorting code here.
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	#pragma omp parallel
	{
		#pragma omp single
		{
			mergesort(arr,  0, n-1, n , temp);
		}	
	}
	std::cout<<"---------------------"<<std::endl;
	for(int i = 0;i<n;i++)
		std::cout<<arr[i]<<std::endl;
	
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end-start;

	std::cerr<<elapsed_seconds.count()<<std::endl;
	
	checkMergeSortResult (arr, n);

	delete[] arr;
	delete[] temp;

	return 0;
}
