# Einsums in C++

Provides compile-time contraction pattern analysis to determine optimal operation to perform.

## Requirements
A C++ compiler with C++17 support.

The following libraries are required to build EinsumsInCpp:

* BLAS and LAPACK.

On my personal development machine, I use MKL for the above requirements. On GitHub Actions, stock BLAS, LAPACK, and FFTW3 are used.

Optional requirements:

* A Fast Fourier Transform library, either FFTW3 or DFT from MKL.
* For call stack backtracing, refer to the requirements listed [here](https://github.com/bombela/backward-cpp).

## Examples
This will optimize at compile-time to a BLAS dgemm call.
```C++
#include "einsums/TensorAlgebra.hpp"

using einsums;  // Provides Tensor and create_random_tensor
using einsums::TensorAlgebra;  // Provides einsum and Indices
using einsums::TensorAlgrebra::Index;  // Provides i, j, k

Tensor<2> A = create_random_tensor("A", 7, 7);
Tensor<2> B = create_random_tensor("B", 7, 7);
Tensor<2> C{"C", 7, 7};

einsum(Indices{i, j}, &C, Indices{i, k}, A, Indices{k, j}, B);
```

Two-Electron Contribution to the Fock Matrix
```C++
#include "einsums/TensorAlgebra.hpp"

using namespace einsums;

void build_Fock_2e_einsum(Tensor<2> *F,
                          const Tensor<4> &g,
                          const Tensor<2> &D) {
    using namespace einsums::TensorAlgebra;
    using namespace einsums::TensorAlgebra::Index;

    // Will compile-time optimize to BLAS gemv
    einsum(1.0, Indices{p, q}, F,
           2.0, Indices{p, q, r, s}, g, Indices{r, s}, D);

    // As written cannot be optimized.
    // A generic arbitrary contraction function will be used.
    einsum(1.0, Indices{p, q}, F,
          -1.0, Indices{p, r, q, s}, g, Indices{r, s}, D);
}
```

![einsum Performance](/images/Performance.png)

W Intermediates in CCD
```C++
Wmnij = g_oooo;
// Compile-time optimizes to gemm
einsum(1.0,  Indices{m, n, i, j}, &Wmnij,
       0.25, Indices{i, j, e, f}, t_oovv,
             Indices{m, n, e, f}, g_oovv);

Wabef = g_vvvv;
// Compile-time optimizes to gemm
einsum(1.0,  Indices{a, b, e, f}, &Wabef,
       0.25, Indices{m, n, e, f}, g_oovv,
             Indices{m, n, a, b}, t_oovv);

Wmbej = g_ovvo;
// As written uses generic arbitrary contraction function
einsum(1.0, Indices{m, b, e, j}, &Wmbej,
      -0.5, Indices{j, n, f, b}, t_oovv,
            Indices{m, n, e, f}, g_oovv);
```

CCD Energy
```C++
/// Compile-time optimizes to a dot product
einsum(0.0,  Indices{}, &e_ccd,
       0.25, Indices{i, j, a, b}, new_t_oovv,
             Indices{i, j, a, b}, g_oovv);
```
