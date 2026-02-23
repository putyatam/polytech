#include "GrayCode.h"

vector<string> GrayCode(int n) {
    vector<string> grayCode;

    int totalCodes = 1 << n;

    for (int i = 0; i < totalCodes; ++i) {
        int gray = i ^ (i >> 1);

        string binary = "";
        for (int j = n - 1; j >= 0; --j) {
            binary += (gray & (1 << j)) ? '1' : '0';
        }

        grayCode.push_back(binary);
    }

    return grayCode;
}