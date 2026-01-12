
void multiplyMatricesBlocked(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int blockSize) {
    int n = A.size();
    int p = B.size();
    int m = B[0].size();
    // initialize C to 0
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) C[i][j] = 0;
    }
    for (int ii = 0; ii < n; ii += blockSize) {
        for (int kk = 0; kk < p; kk += blockSize) {
            for (int jj = 0; jj < m; jj += blockSize) {
                int iMax = min(ii + blockSize, n);
                int kMax = min(kk + blockSize, p);
                int jMax = min(jj + blockSize, m);
                for (int i = ii; i < iMax; ++i) {
                    for (int k = kk; k < kMax; ++k) {
                        int a = A[i][k];
                        for (int j = jj; j < jMax; ++j) {
                            C[i][j] += a * B[k][j];
                        }
                    }
                }
            }
        }
    }
}




