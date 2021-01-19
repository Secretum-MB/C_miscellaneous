/* The below implementation of MergeSort takes as input an array
   of pointers to integers (int *array[]). 

   This ability to use an array of pointers to our data rather 
   than an array of data is important when you want to do some 
   processing on the data w/o mutating the original array and 
   you dont want to copy the array because memory considerations
   (i.e. array of strings is very large; array of pointers to 
   strings is only size of the pointer (4 or 8 bytes) x num pointers).

   Here, the original array is left alone, and in the array of
   pointers, the pointers are shuffled around so that iterating
   over this array returns pointers to increasingly larger ints
   in the original array.
*/

#include <stdio.h>


void merge(int **array, int start, int mid, int end) {

  int L_len = mid - start + 1;
  int R_len = end - mid;

  int *L[L_len];
  int *R[R_len];

  int L_i = start;
  int R_i = mid + 1;
  int arr_i = start;

  for (int i = 0; i < L_len; i++) {
	L[i] = array[L_i];
	L_i += 1;
  }
  for (int i = 0; i < R_len; i++) {
	R[i] = array[R_i];
	R_i += 1;
  }
  
  L_i = 0;
  R_i = 0;
  while ( L_i < L_len && R_i < R_len )
  {
  	if ( *L[L_i] < *R[R_i] ) {  // only line in function that would be different if not for array of pointers
	  array[arr_i] = L[L_i];
	  arr_i += 1; L_i += 1;
	} else {
	  array[arr_i] = R[R_i];
	  arr_i += 1; R_i += 1;
	}
  }
  while ( R_i < R_len ) {
	array[arr_i] = R[R_i];
	arr_i += 1; R_i += 1;
  }
  while ( L_i < L_len ) {
	array[arr_i] = L[L_i];
	arr_i += 1; L_i += 1;
  }
}

void mergeSort(int **array, int start, int end) {

  if (start >= end) {
	return;
  }

  int mid = (start + end) / 2;

  mergeSort(array, start, mid);
  mergeSort(array, mid+1, end);
  merge(array, start, mid, end);
}



// take notice to how we instantiate the array of pointers.
// if you declare this array and work with it you may be accessing
// locations in memory that are unsafe.
//
int main(void) {

  int arr[] = { -6, -24, 0, -8, 5, 22, 45, -17, 31, 36, 26, -26, 37, 0, 31, 36, 21, -26, -44, -21, -21, 12, -8, -25, 48 };
  int arr_size = sizeof(arr) / sizeof(arr[0]);
  
  int *result[arr_size];
  for (int i = 0; i < arr_size; i++) {
	result[i] = &arr[i];
  }

  mergeSort(result, 0, arr_size - 1);

  for (int i = 0; i < arr_size; i++) {
	printf("%d, ", arr[i]);
  }

  printf("\n");

  for (int i = 0; i < arr_size; i++) {
	printf("%d, ", *result[i]);
  }

  return 0;
}

