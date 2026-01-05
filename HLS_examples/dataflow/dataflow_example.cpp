#include <math.h>
#include <hls_stream.h>
#define N_VALS 500

/// Note: Also looked at the streams example from the Vitis introductory examples (https://github.com/Xilinx/Vitis-HLS-Introductory-Examples/blob/master/Task_level_Parallelism/Control_driven/Channels/using_stream_of_blocks/diamond.cpp).
void process_A(hls::stream<double>& A, hls::stream<double>& A_prime)
{
process_A_loop:
    for(int i = 0; i < N_VALS; i++)
    {
        A_prime.write(log(A.read()));
    }
}

void process_B(hls::stream<double>& B, hls::stream<double>& B_prime)
{
    double tmp_1, tmp_2;
    tmp_1 = 1;
process_B_loop:
    for(int i = 0; i < N_VALS; i++)
    {
        tmp_2 = B.read();
        B_prime.write(sqrt(tmp_1 * tmp_2));
        tmp_1 = tmp_2;
    }
}

void v_add(hls::stream<double>& B_prime, hls::stream<double>& A_prime, hls::stream<double>& C)
{
output_loop:
    for(int i = 0; i < N_VALS; i++)
    {
        C.write(B_prime.read() + A_prime.read());
    }
}

extern "C"
{
    void dataflow_kernel(hls::stream<double>& A, hls::stream<double>& B, hls::stream<double>& out)
    {
#pragma HLS interface m_axi port=A bundle=memblock_1
#pragma HLS interface m_axi port=B bundle=memblock_2
#pragma HLS interface m_axi port=out bundle=memblock_1

        //add a pragma here to declare this function a dataflow region

        // We don't want to alter global memory in place! So we have local
        // arrays to hold the result of processing the input arrays 
//       double B_prime[N_VALS], A_prime[N_VALS];
    	hls::stream<double> A_prime;
    	hls::stream<double> B_prime;
       // convert these to streams to allow data to flow efficiently through your design

        // We apply some procesing function to the two input arrays, which can 
        // be done in parallel
        process_A(A, A_prime);
        process_B(B, B_prime);

        // Add the resulting arrays together. 
        v_add(B_prime, A_prime, out);
    }
}
