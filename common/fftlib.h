long FFTInit(long *fftMptr, long fftN, float *Utbl);
/* Compute cosine table and check size for complex ffts	*/
/* INPUTS */
/* fftN = size of fft	*/
/* OUTPUTS */
/* *fftMptr = log2 of fft size	*/
/* *Utbl = cosine table with fftN/4 + 1 entries (angles = 0 to pi/2 inclusive)	*/
/* RETURNS */
/* 1 if fftN is invalid, 0 otherwise	*/

long rFFTInit(long *fftMptr, long fftN, float *Utbl);
/* Compute cosine table and check size for a real input fft	*/
/* INPUTS */
/* fftN = size of fft	*/
/* OUTPUTS */
/* *fftMptr = log2 of fft size	*/
/* *Utbl = cosine table with fftN/4 + 1 entries (angles = 0 to pi/2 inclusive)	*/
/* RETURNS */
/* 1 if fftN is invalid, 0 otherwise	*/

void ffts(float *ioptr, long M, long Rows, float *Utbl);
/* Compute in-place complex fft on the rows of the input array	*/
/* INPUTS */
/* M = log2 of fft size	*/
/* *ioptr = input data array	*/
/* *Utbl = cosine table	*/
/* Rows = number of rows in ioptr array (use Rows of 1 if ioptr is a 1 dimensional array)	*/
/* OUTPUTS */
/* *ioptr = output data array	*/

void iffts(float *ioptr, long M, long Rows, float *Utbl);
/* Compute in-place inverse complex fft on the rows of the input array	*/
/* INPUTS */
/* M = log2 of fft size	*/
/* *ioptr = input data array	*/
/* *Utbl = cosine table	*/
/* Rows = number of rows in ioptr array (use Rows of 1 if ioptr is a 1 dimensional array)	*/
/* OUTPUTS */
/* *ioptr = output data array	*/

void rffts(float *ioptr, long M, long Rows, float *Utbl);
/* Compute in-place real fft on the rows of the input array	*/
/* INPUTS */
/* M = log2 of fft size	*/
/* *ioptr = real input data array	*/
/* *Utbl = cosine table	*/
/* Rows = number of rows in ioptr array (use Rows of 1 if ioptr is a 1 dimensional array)	*/
/* OUTPUTS */
/* *ioptr = output data array	in the following order */
/* Re(x[0]), Re(x[N/2]), Re(x[1]), Im(x[1]), Re(x[2]), Im(x[2]), ... Re(x[N/2-1]), Im(x[N/2-1]). */


void riffts(float *ioptr, long M, long Rows, float *Utbl);
/* Compute in-place real ifft on the rows of the input array	*/
/* INPUTS */
/* M = log2 of fft size	*/
/* *ioptr = input data array in the following order	*/
/* Re(x[0]), Re(x[N/2]), Re(x[1]), Im(x[1]), Re(x[2]), Im(x[2]), ... Re(x[N/2-1]), Im(x[N/2-1]). */
/* *Utbl = cosine table	*/
/* Rows = number of rows in ioptr array (use Rows of 1 if ioptr is a 1 dimensional array)	*/
/* OUTPUTS */
/* *ioptr = real output data array	*/
