/*****************************************************************************
  Copyright (c) 2014, Intel Corp.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Intel Corporation nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************
* Contents: Native middle-level C interface to LAPACK function sspgvd
* Author: Intel Corporation
*****************************************************************************/

#include "lapacke_utils.h"

lapack_int LAPACKE_sspgvd_work(int matrix_layout, lapack_int itype, char jobz, char uplo, lapack_int n, float *ap, float *bp, float *w,
                               float *z, lapack_int ldz, float *work, lapack_int lwork, lapack_int *iwork, lapack_int liwork) {
    lapack_int info = 0;
    if (matrix_layout == LAPACK_COL_MAJOR) {
        /* Call LAPACK function and adjust info */
        LAPACK_sspgvd(&itype, &jobz, &uplo, &n, ap, bp, w, z, &ldz, work, &lwork, iwork, &liwork, &info);
        if (info < 0) {
            info = info - 1;
        }
    } else if (matrix_layout == LAPACK_ROW_MAJOR) {
        lapack_int ldz_t = MAX(1, n);
        float *z_t = NULL;
        float *ap_t = NULL;
        float *bp_t = NULL;
        /* Check leading dimension(s) */
        if (ldz < n) {
            info = -10;
            LAPACKE_xerbla("LAPACKE_sspgvd_work", info);
            return info;
        }
        /* Query optimal working array(s) size if requested */
        if (liwork == -1 || lwork == -1) {
            LAPACK_sspgvd(&itype, &jobz, &uplo, &n, ap, bp, w, z, &ldz_t, work, &lwork, iwork, &liwork, &info);
            return (info < 0) ? (info - 1) : info;
        }
        /* Allocate memory for temporary array(s) */
        if (LAPACKE_lsame(jobz, 'v')) {
            z_t = (float *)LAPACKE_malloc(sizeof(float) * ldz_t * MAX(1, n));
            if (z_t == NULL) {
                info = LAPACK_TRANSPOSE_MEMORY_ERROR;
                goto exit_level_0;
            }
        }
        ap_t = (float *)LAPACKE_malloc(sizeof(float) * (MAX(1, n) * MAX(2, n + 1)) / 2);
        if (ap_t == NULL) {
            info = LAPACK_TRANSPOSE_MEMORY_ERROR;
            goto exit_level_1;
        }
        bp_t = (float *)LAPACKE_malloc(sizeof(float) * (MAX(1, n) * MAX(2, n + 1)) / 2);
        if (bp_t == NULL) {
            info = LAPACK_TRANSPOSE_MEMORY_ERROR;
            goto exit_level_2;
        }
        /* Transpose input matrices */
        LAPACKE_ssp_trans(matrix_layout, uplo, n, ap, ap_t);
        LAPACKE_ssp_trans(matrix_layout, uplo, n, bp, bp_t);
        /* Call LAPACK function and adjust info */
        LAPACK_sspgvd(&itype, &jobz, &uplo, &n, ap_t, bp_t, w, z_t, &ldz_t, work, &lwork, iwork, &liwork, &info);
        if (info < 0) {
            info = info - 1;
        }
        /* Transpose output matrices */
        if (LAPACKE_lsame(jobz, 'v')) {
            LAPACKE_sge_trans(LAPACK_COL_MAJOR, n, n, z_t, ldz_t, z, ldz);
        }
        LAPACKE_ssp_trans(LAPACK_COL_MAJOR, uplo, n, ap_t, ap);
        LAPACKE_ssp_trans(LAPACK_COL_MAJOR, uplo, n, bp_t, bp);
        /* Release memory and exit */
        LAPACKE_free(bp_t);
    exit_level_2:
        LAPACKE_free(ap_t);
    exit_level_1:
        if (LAPACKE_lsame(jobz, 'v')) {
            LAPACKE_free(z_t);
        }
    exit_level_0:
        if (info == LAPACK_TRANSPOSE_MEMORY_ERROR) {
            LAPACKE_xerbla("LAPACKE_sspgvd_work", info);
        }
    } else {
        info = -1;
        LAPACKE_xerbla("LAPACKE_sspgvd_work", info);
    }
    return info;
}
