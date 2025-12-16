#define N_VALS 500

extern "C"
{
    void v_add_kernel(double *A, double *B, double *out)
    {
        //pragmas go here!
        //assign A and B to separate bundles so they have independent interfaces
#pragma HLS INTERFACE mode=m_axi port=A bundle=a_bundle
#pragma HLS INTERFACE mode=m_axi port=B bundle=b_bundle
#pragma HLS INTERFACE mode=m_axi port=out bundle=a_bundle
V_ADD_LOOP:
        for(int i = 0; i < N_VALS; i++)
        {
            out[i] = A[i] + B[i];
        }
    }
}
