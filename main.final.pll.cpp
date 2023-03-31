#include <bits/stdc++.h>
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

class PGM {
   public:
    PGM() { cout << endl; };
    virtual ~PGM();

    int w_;
    int h_;
    int size_;

    vector<int> pixel;
    vector<char> tes;

    void open(std::string);
    void write(std::string);
    void init();
    void edges(int weak_threshold = 50, int strong_threshold = 200);
};

// clang-format off

    //Sobel
    const int8_t Gx[] = {-1, 0, 1,
                         -2, 0, 2,
                         -1, 0, 1};

    const int8_t Gy[] = { 1, 2, 1,
                          0, 0, 0,
                         -1,-2,-1};

    //Gausian blur
    //3 x 3 kernel
    const float Gaus3x3[] =  {0.0625, 0.125, 0.0625, 0.125, 0.25, 0.125, 0.0625, 0.125, 0.0625};

// clang-format on
PGM::~PGM() {}

void PGM::init() {
    size_ = w_ * h_;
    tes.resize(size_);
}

void PGM::edges(int weak_threshold, int strong_threshold) {
    double start, end;

    start = omp_get_wtime();

    int offset_xy = 1;  // for kernel = 3

    vector<int> temp(size_, 0);
    int a[size_];

    // gaussian filter
    // get cobv from pixel
    // copy to pixel
    // at very end finally copy to pixel

#pragma omp parallel for
    for (int x = 0; x < w_; x++) {
        for (int y = 0; y < h_; y++) {
            int pos = x + (y * w_);
            if (x < offset_xy || x >= (w_ - offset_xy) || y < offset_xy ||
                y >= (h_ - offset_xy)) {
                temp[pos] = pixel[pos];
                a[pos] = pixel[pos];
                continue;
            }

            float convolve = 0;
            int k = 0;
            for (int kx = -offset_xy; kx <= offset_xy; kx++) {
                for (int ky = -offset_xy; ky <= offset_xy; ky++) {
                    convolve += pixel[pos + (kx + (ky * w_))] * Gaus3x3[k];
                    k++;
                }
            }
            if (convolve > 255) convolve = 255;
            if (convolve < 0) convolve = 0;
            int i = convolve;
            a[pos] = i;
            temp[pos] = a[pos];
        }
    }

    pixel = temp;
    write("./1.pgm");

    end = omp_get_wtime();
    cout << end - start << " seconds\n";
    start = omp_get_wtime();

    // apply sobel kernels
    // get conv from temp
    // apply on pixel after getting magnitude (sqrt(gx)^2+(gy)^2)

    offset_xy = 1;  // 3x3
#pragma omp parallel for
    for (int x = offset_xy; x < w_ - offset_xy; x++) {
        for (int y = offset_xy; y < h_ - offset_xy; y++) {
            double convolve_X = 0.0;
            double convolve_Y = 0.0;
            int k = 0;
            int src_pos = x + (y * w_);

            for (int ky = -offset_xy; ky <= offset_xy; ky++) {
                for (int kx = -offset_xy; kx <= offset_xy; kx++) {
                    convolve_X += temp[src_pos + (kx + (ky * w_))] * Gx[k];
                    convolve_Y += temp[src_pos + (kx + (ky * w_))] * Gy[k];
                    k++;
                }
            }

            // gradient hypot & direction
            int segment = 0;

            if (convolve_X == 0.0 || convolve_Y == 0.0) {
                pixel.at(src_pos) = 0;
            } else {
                double mag = ((std::sqrt((convolve_X * convolve_X) +
                                         (convolve_Y * convolve_Y))));

                if (mag > 255)
                    pixel.at(src_pos) = 255;
                else
                    pixel.at(src_pos) = mag;

                double theta = std::atan2(
                    convolve_Y, convolve_X);  // radians. atan2 range: -PI,+PI,
                                              // theta : 0 - 2PI
                theta = theta * (360.0 / (2.0 * M_PI));  // degrees

                if ((theta <= 22.5 && theta >= -22.5) || (theta <= -157.5) ||
                    (theta >= 157.5)) {
                    segment = 1;  // "-"
                } else if ((theta > 22.5 && theta <= 67.5) ||
                           (theta > -157.5 && theta <= -112.5)) {
                    segment = 2;  // "/"
                } else if ((theta > 67.5 && theta <= 112.5) ||
                           (theta >= -112.5 && theta < -67.5)) {
                    segment = 3;  // "|"
                } else if ((theta >= -67.5 && theta < -22.5) ||
                           (theta > 112.5 && theta < 157.5)) {
                    segment = 4;  // "\"
                } else {
                    cerr << "error " << theta << endl;
                }
            }

            tes.at(src_pos) = (unsigned char)segment;
        }
    }

    write("./2.pgm");
    end = omp_get_wtime();
    cout << end - start << " seconds\n";

    start = omp_get_wtime();

    // local maxima: non maxima suppression
    // get suppressed pixels from temp
    // copy into pixel

    temp = pixel;

#pragma omp parallel for
    for (int x = 1; x < w_ - 1; x++) {
        for (int y = 1; y < h_ - 1; y++) {
            int pos = x + (y * w_);

            switch (tes[pos]) {
                case 1:
                    if (temp[pos - 1] >= temp[pos] ||
                        temp[pos + 1] > temp[pos]) {
                        pixel[pos] = 0;
                    }
                    break;
                case 2:
                    if (temp[pos - (w_ - 1)] >= temp[pos] ||
                        temp[pos + (w_ - 1)] > temp[pos]) {
                        pixel[pos] = 0;
                    }
                    break;
                case 3:
                    if (temp[pos - (w_)] >= temp[pos] ||
                        temp[pos + (w_)] > temp[pos]) {
                        pixel[pos] = 0;
                    }
                    break;
                case 4:
                    if (temp[pos - (w_ + 1)] >= temp[pos] ||
                        temp[pos + (w_ + 1)] > temp[pos]) {
                        pixel[pos] = 0;
                    }
                    break;
                default:
                    pixel[pos] = 0;
                    break;
            }
        }
    }

    temp = pixel;

    write("./3.pgm");

    end = omp_get_wtime();
    cout << end - start << " seconds\n";
    start = omp_get_wtime();

    // double threshold
    // compare values from temp
    // copy into pixels

#pragma omp parallel for
        for (int x = 0; x < w_; x++) {
            for (int y = 0; y < h_; y++) {
                int src_pos = x + (y * w_);
                if (temp[src_pos] > strong_threshold) {
                    pixel[src_pos] = 255;
                } else if (temp[src_pos] > weak_threshold) {
                    pixel[src_pos] = 100;
                } else {
                    pixel[src_pos] = 0;
                }
            }
        }

    write("./4.pgm");

    end = omp_get_wtime();
    cout << end - start << " seconds\n";

    start = omp_get_wtime();

    temp = pixel;

#pragma omp parallel for

    // edges with hysteresis
    // prolly can't be parallelized, copy pixel vals into pixel
    for (int x = 1; x < w_ - 1; x++) {
        for (int y = 1; y < h_ - 1; y++) {
            int src_pos = x + (y * w_);
            if (temp[src_pos] == 255) {
                pixel[src_pos] = 255;
            } else if (temp[src_pos] == 100) {
                if (temp[src_pos - 1] == 255 || temp[src_pos + 1] == 255 ||
                    temp[src_pos - 1 - w_] == 255 ||
                    temp[src_pos + 1 - w_] == 255 ||
                    temp[src_pos + w_] == 255 ||
                    temp[src_pos + w_ - 1] == 255 ||
                    temp[src_pos + w_ + 1] == 255) {
                    pixel[src_pos] = 255;
                } else {
                    pixel[src_pos] = 0;
                }
            } else {
                pixel[src_pos] = 0;
            }
        }
    }

    write("./5.pgm");

    end = omp_get_wtime();
    cout << end - start << " seconds\n";

    cout << "All files written." << endl;
}

/**
 * Opens the PGM image file for write.
 * @param Path of the PGM image file being read.
 **/

void PGM::write(std::string filename) {
    ofstream writer(filename);

    writer << "P2\n";
    writer << w_ << " " << h_ << "\n";
    writer << "255\n";

    // print in 17 pixel long rows.
    int numval = 0;
    int flag = 0;
    int counter = 0;
    for (int x = 0; x < w_; x++)
        for (int y = 0; y < h_; y++) {
            int pos = counter;
            writer << +pixel.at(pos);
            numval++;
            if (numval % 17 == 0 && flag == 1)
                writer << "\n";
            else
                writer << " ";
            flag = 1;
            counter++;
        }

    writer << endl;
    writer.close();
}

/**
 * Opens the PGM image file for reading.
 * Sets: int w_, int h_, size_, std::vector<u_int8_t> pixel
 * @param Path of the PGM image file being read.
 **/
void PGM::open(std::string filename) {
    ifstream file;
    file.exceptions(ifstream::badbit);  // attach badbit exception (hacky cpp
                                        // exception attachment)
    string read;
    try {
        file.open(filename, ios::in);
        file >> read;
        if (read.compare("P2") == 0) {
            file >> w_ >> h_;
            init();
            pixel.resize(size_);
            file >> read;
            int flag = 0;
            if (read == "255") {
                int counter = 0;
                int numval = 0;
                for (int x = 0; x < w_; x++)
                    for (int y = 0; y < h_; y++) {
                        file >> read;
                        int pos = counter;
                        pixel.at(pos) = stoi(read);
                        cout << +pixel.at(pos);
                        numval++;
                        if (numval % 17 == 0 && flag == 1) flag = 1;
                        counter++;
                    }
                cout << w_ << " " << h_ << " " << size_ << endl;
                cout << "\nFile read." << endl;
            } else {
                string err{"file format error, not 8-bit!"};
                err.append(filename);
            }

        } else {
            string err{"file format error, not a pgm file(plain or otherwise)"};
            err.append(filename);
        }
    } catch (const ifstream::failure& e) {
        string err{"Read error?! Check read permissions!"};
        err.append(filename);
        std::cerr << err;
    }
    file.close();
}

int main() {
    double start, end;
    start = omp_get_wtime();
    PGM opened;
    opened.open("./01.pgm");
    opened.edges();
    cout << "succesfully exited";
    end = omp_get_wtime();
    cout << "Total time " << end - start << " seconds.\n";
    return 0;
}
