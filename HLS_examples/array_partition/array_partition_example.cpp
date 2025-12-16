#define DATA_TYPE double

double sum_vec(double vec[8])
{
    double sum = 0;
    double M_int1[4], M_int2[2];
#pragma HLS ARRAY_PARTITION variable=M_int1 type=block factor=2
#pragma HLS ARRAY_PARTITION variable=M_int2	type=complete
//    naive_sum_loop:
//    for(int i = 0; i < 8; i++)
//    {
//        sum += vec[i];
//    }
    parallel_sum_loop_1:
	for (int i = 0; i < 4; i++)
	{
		M_int1[i] = vec[2 * i] + vec[2 * i + 1];
	}
	parallel_sum_loop_2:
	for (int i = 0; i < 2; i++)
	{
		M_int2[i] = M_int1[2 * i] + M_int1[2 * i + 1];
	}
	sum = M_int2[0] + M_int2[1];
    return sum;
}

extern "C"{
    void matrix_multiply_kernel(DATA_TYPE *A, DATA_TYPE *B, DATA_TYPE *OUT)
    {
// Setting up interfaces to global memory: see interface examples for more info!
#pragma HLS INTERFACE m_axi port=A bundle=gmem1
#pragma HLS INTERFACE m_axi port=B bundle=gmem2
#pragma HLS INTERFACE m_axi port=OUT bundle=gmem1

        double M1[64], M2[64], M3[64];
        // partition pragmas go here for these matrices 
#pragma HLS ARRAY_PARTITION variable=M1 type=block factor=8
#pragma HLS ARRAY_PARTITION variable=M2 type=block factor=8

        assign_matrix_loop:
        for(int i = 0; i < 64; i++)
        {
            M1[i] = A[i];
            M2[i] = B[i];
        }

        // A variable to store the results of multiplications (8 multiplications per row-column product)
        double row_col_prod[8];
        //partition pragma goes here for this array 
#pragma HLS ARRAY_PARTITION variable=row_col_prod type=block factor=8
        //Trivial implementation of Matrix multiply
        mult_loop_i:
        for(int i = 0; i < 8; i++)
        {
            mult_loop_j:
            for(int j = 0; j < 8; j++)
            {
                // do all the multiplications
                mult_loop_k:
                for(int k = 0; k < 8; k++)
                {
                    row_col_prod[k] = M1[i*8 + k] * M2[k*8 + j];
                }
                // add the products together to get one element of the matrix product
                M3[i*8 + j] = sum_vec(row_col_prod);
            }
        }

        output_loop:
        for(int i = 0; i < 64; i++)
        {
            OUT[i] = M3[i];
        }
    }
}
