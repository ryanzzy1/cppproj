#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;

#define A2M_len       2
#define A2M_data_len  12

int A2M_number = 0;

int main()
{
    int test = 6;
    uint8_t x = 0;
    // unsigned char DATA_A2M[A2M_len][A2M_data_len];
    unsigned int DATA_A2M[A2M_len][A2M_data_len];
    char recv[1024] = "hello world.";
    // int send[5];
    char send[1024];

    
    /*
    unsigned char DATA_A2M[A2M_len][A2M_data_len] = {{0x18,0xFF,0x18,0x27,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
												 {0x18,0xFF,0x04,0x27,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11},
												 {0x18,0xFF,0x1B,0x27,0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22},
												 {0x18,0xFF,0x19,0x27,0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33},
												 {0x19,0xFF,0x95,0x27,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44},
												 {0x19,0xFF,0x96,0x27,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55},
												 {0x19,0xFF,0x97,0x27,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66},
												 {0x19,0xFF,0x94,0x27,0x77,0x77,0x77,0x77,0x77,0x77,0x77,0x77}};
    */
    // DATA_A2M[0][0] = 0x19;
    // DATA_A2M[0][1] = 0xFF;
    // DATA_A2M[0][2] = 0x95;
    // DATA_A2M[0][3] = 0x27;
    // DATA_A2M[0][4] = 0xFF;
    // DATA_A2M[0][5] = 0xFF;
    // DATA_A2M[0][6] = 0xFF;
    // DATA_A2M[0][7] = 0xFF;
    // DATA_A2M[0][8] = 0xFF;
    // DATA_A2M[0][9] = 0xFF;
    // DATA_A2M[0][10] = 0xFF;
    // DATA_A2M[0][11] = 0xFF;
  

    // DATA_A2M[1][0] = 0x19;
    // DATA_A2M[1][1] = 0xFF;
    // DATA_A2M[1][2] = 0x96;
    // DATA_A2M[1][3] = 0x27;
    // DATA_A2M[1][4] = 0xFF;
    // DATA_A2M[1][5] = 0xFF;
    // DATA_A2M[1][6] = 0xFF;
    // DATA_A2M[1][7] = 0xFF;
    // DATA_A2M[1][8] = 0xFF;
    // DATA_A2M[1][9] = 0xFF;
    // DATA_A2M[1][10] = 0xFF;
    // DATA_A2M[1][11] = 0xFF;
    // DATA_A2M[0][6] = (2 << 0) & 0x0F + (1 << 2) & 0x0F + (1 << 4) &0xF0 + (1 << 6) & 0xF0;

    DATA_A2M[0][6] = (((1&0x0F) << 0)  + ((1&0x0F) << 2)  + ((1&0x0F)<< 4)  + ((1&0x0F) << 6));
            
                    // = 0010 + 0001 << 2 + 0001 << 4 + 0001 << 6  
    // cout << "set: " << 0x02 + (0x01 << 2) + (0x01 << 4) + (0x01 << 6) << endl;
    // cout << std::hex << setw(2) << "2&0x0F:" << "0x" << (2&0x0F) << "\n" 
    //     << " 1&0x0F:" << "0x" << (1&0x0F) << " 1&0x0F<<2:" << "0x" << ((1&0x0F)<<2) << "\n" 
    //     << " 1&0x0F<<4:" << "0x" << (1&0x0F) << " 1&0x0F<<4:" << "0x" << ((1&0x0F)<<4) << "\n" 
    //     << " 1&0x0F<<6:" << "0x" << ((1&0x0F)<<6) << endl;

    cout << "DATA_A2M[0][6]: 0x" << std::hex << setw(2) << (DATA_A2M[0][6]) << endl;
    // send = recv;
    memset(send, 0, sizeof(send));
    memcpy(send, recv, sizeof(recv));
    // p = recv;

    std::cout << "send:" << send << std::endl;

/*
    // send 10ms, 100ms
    for (A2M_number = 0; A2M_number < A2M_len; A2M_number++) {
        // cout << "DATA_A2M[" << A2M_number << "]: " << " | " << sizeof(DATA_A2M[A2M_number])  << endl;
        for (int j = 0; j < A2M_data_len; j++){
            cout << "DATA_A2M[" << A2M_number << "]+" << j << ":";
            cout << " 0x" << setw(2) << setfill('0') <<std::hex << int(*(DATA_A2M[A2M_number]+j)) << endl;
            // cout << "DATA_A2M[" << A2M_number << "][" << j <<"]:" << std::hex << " 0x" << setw(2) << setfill('0') << int(DATA_A2M[A2M_number][j]) << "\n";
        }    
    }
*/
    // receive 10ms, 20ms, 50ms
    
    return 0;
}