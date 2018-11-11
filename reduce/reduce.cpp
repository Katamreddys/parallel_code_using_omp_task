#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

  void generateReduceData (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

int calculate_reduce(int * arr,int n, int nbthreads)
{
	
	int * thread_data = new int [nbthreads];
	
	#pragma omp parallel
	#pragma omp single nowait
	for(int i=0;i<n;i++)
	{
		//#pragma omp atomic
		#pragma omp shared(thread_data)
		{
			int tid = omp_get_thread_num();
			thread_data[tid] += arr[i];
		}	
	}
	
	
	int result = 0;
	//#pragma omp taskwait
	#pragma omp parallel
	for(int j=1;j<=nbthreads;j++)
	{
		#pragma omp shared(result)
		result += thread_data[j];
	}
	return result;
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

	if (argc < 3) {
		std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
		return -1;
	}

	int n = atoi(argv[1]);
	int * arr = new int [n];

	generateReduceData (arr, n);

	int nbthreads = atoi(argv[2]);
	//set number of threads to be used
	omp_set_num_threads(nbthreads);
	
	//insert reduction code here
	// start timing
	std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
	int result = 0;
	
	#pragma omp parallel
	{
		#pragma omp single
		{
			result = calculate_reduce(arr,n, nbthreads);
		}	
	}
	
	//#pragma omp taskwait
	
	
	
	// end time
	std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapased_seconds = end-start;

	// display result
	std::cout<<result;
	std::cerr<<elapased_seconds.count();

	delete[] arr;

	return 0;
}
