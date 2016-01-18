

// Fortran
// http://en.wikibooks.org/wiki/Fortran/Fortran_examples
/*
function gauss_sparse(num_iter, tol, b, A, x, actual_iter)

!  This function solves a system of equations (Ax = b) by using the Gauss-Seidel Method

   implicit none

   real ::  gauss_sparse

!  Input: its value cannot be modified from within the function
   integer, intent(in) :: num_iter
   real, intent(in) :: tol
   real, intent(in), dimension(1:) :: b
   real, intent(in), dimension(1:,1:) :: A

!  Input/Output: its input value is used within the function, and can be modified
   real, intent(inout), dimension(1:) :: x

!  Output: its value is modified from within the function, only if the argument is required
   integer, optional, intent(out) :: actual_iter

!  Locals
   integer :: i, n, iter
   real :: tol_max, xk

!  Initialize values
   n = ubound(b, dim = 1)  ! Size of array, obtained using ubound intrinsic function
   tol_max = 2. * tol
   iter = 0

!  Compute solution until convergence
   convergence_loop: do while (tol_max >= tol .and. iter < num_iter); iter = iter + 1

	  tol_max = -1.  ! Reset the tolerance value

!	 Compute solution for the k-th iteration
	  iteration_loop: do i = 1, n

!		Compute the current x-value
		 xk = (b(i) - dot_product(A(i,1:i-1),x(1:i-1)) - dot_product(A(i,i+1:n),x(i+1:n))) / A(i, i)

!		Compute the error of the solution
!		dot_product(a,v)=a'b
		 tol_max = max((abs(x(i) - xk)/(1. + abs(xk))) ** 2, abs(A(i, i) * (x(i) - xk)), tol_max)
		 x(i) = xk
	  end do iteration_loop
   end do convergence_loop

   if (present(actual_iter)) actual_iter = iter
   gauss_sparse = tol_max

end function gauss_sparse


*/

(
var dot_product = { |a, b| (a * b).sum };
var gauss_sparse = { | num_iter, tol, b, a, x |
	var gauss_sparse;
	// Locals
  	var i, n, iter;
   	var tol_max, xk;
	// Initialize values
	tol_max = 2.0 * tol;
	iter = 0;
	// Compute solution until convergence
	while { (tol_max >= tol) and: (iter < num_iter) }
	{
		iter = iter + 1;

		tol_max = -1.0;  // Reset the tolerance value
		// Compute solution for the k-th iteration
		i = 1;
		n.do {
			//		Compute the current x-value
			xk = (b[i] - dot_product(a[i][i-1], x[i-1]) - dot_product(a[i][i+1], x[i+1])) / a[i][i];
			//	   Compute the error of the solution
			//		dot_product(a,v)=a'b
				tol_max = max((abs(x[i] - xk)/(1.0 + abs(xk))) ** 2, abs(a[i][i] * (x[i] - xk)), tol_max);
		 		x[i] = xk;
		}

	};
	tol_max
};
gauss_sparse.(8, 2, [0, 1, 2, 3, 4], [[0, 1, 2], [1, 2, 3]], [0, 1, 2, 3, 4]);
)
