#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

class PGM {
   public:

    int width;
    int height;
    int size_;

    vector<int> pixel, temp;
    vector<char> tes;

    void open(std::string);
    void write(std::string);
    void init();
    void canny(int t1 = 50, int t2 = 200);
    void nms();
    void doublethresh(int , int );
    void hysteresis();
    void sobel();
    void gaussian3();
};

//sobel
const int8_t Gx[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
const int8_t Gy[] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
const float G3[] = {0.0625, 0.125,  0.0625, 0.125, 0.25, 0.125,  0.0625, 0.125,  0.0625};

void PGM::init() {
    size_ = width * height;
    tes.resize(size_);
    pixel.resize(size_);
    temp.resize(size_);
}

void PGM::canny(int t1, int t2) {
    gaussian3();
    write("./1.pgm");
    sobel();
    write("./2.pgm");
    nms();
    write("./3.pgm");
    doublethresh(50,200);
    write("./4.pgm");
    hysteresis();
    write("./original.pgm");
}

void PGM::gaussian3() {
    int offset_xy = 1;  // for kernel = 3

    /* gaussian filter
     * get cobv from pixel
     * copy to pixel
     * at very end finally copy to pixel
     */

    temp = pixel;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int pos = x + (y * width);
            if (x < offset_xy || x >= (width - offset_xy) || y < offset_xy ||
                y >= (height - offset_xy)) {
                continue;
            }

            float convolve = 0;
            int k = 0;
            for (int kx = -offset_xy; kx <= offset_xy; kx++) {
                for (int ky = -offset_xy; ky <= offset_xy; ky++) {
                    convolve +=
                        pixel.at(pos + (kx + (ky * width))) * G3[k];
                    k++;
                }
            }
            if (convolve > 255) convolve = 255;
            if (convolve < 0) convolve = 0;
            int i = convolve;
            int val = i;
            temp.at(pos) = val;
        }
    }

    pixel = temp;
}

void PGM::sobel() {
    /* apply sobel kernels
     * get conv from temp
     * apply on pixel after getting magnitude (sqrt(gx)^2+(gy)^2)
     */
    int offset_xy = 1;  // 3x3
    for (int x = offset_xy; x < width - offset_xy; x++) {
        for (int y = offset_xy; y < height - offset_xy; y++) {
            double convolve_X = 0.0;
            double convolve_Y = 0.0;
            int k = 0;
            int pos = x + (y * width);

            for (int ky = -offset_xy; ky <= offset_xy; ky++) {
                for (int kx = -offset_xy; kx <= offset_xy; kx++) {
                    convolve_X += temp.at(pos + (kx + (ky * width))) * Gx[k];
                    convolve_Y += temp.at(pos + (kx + (ky * width))) * Gy[k];
                    k++;
                }
            }

            // sobel filter; gradient magnitude & direction
            int segment = 0;

            if (convolve_X == 0.0 || convolve_Y == 0.0) {
                pixel.at(pos) = 0;
            } else {
                double mag = ((std::sqrt((convolve_X * convolve_X) +
                                         (convolve_Y * convolve_Y))));

                if (mag > 255)
                    pixel.at(pos) = 255;
                else
                    pixel.at(pos) = mag;

                double angle = std::atan2(convolve_Y, convolve_X);
                angle = angle * (360.0 / (2.0 * M_PI));

                if ((angle <= 22.5 && angle >= -22.5) || (angle <= -157.5) ||
                    (angle >= 157.5)) {
                    segment = 1;  // "-"
                } else if ((angle > 22.5 && angle <= 67.5) ||
                           (angle > -157.5 && angle <= -112.5)) {
                    segment = 2;  // "/"
                } else if ((angle > 67.5 && angle <= 112.5) ||
                           (angle >= -112.5 && angle < -67.5)) {
                    segment = 3;  // "|"
                } else if ((angle >= -67.5 && angle < -22.5) ||
                           (angle > 112.5 && angle < 157.5)) {
                    segment = 4;  // "\"
                } else {
                    cerr << "error " << angle << endl;
                }
            }

            tes.at(pos) = (unsigned char)segment;
        }
    }
}

void PGM::nms() {
    /* local maxima: non maxima suppression
     * get suppressed pixels from temp
     * copy into pixel
     */

    temp = pixel;

    for (int x = 1; x < width - 1; x++) {
        for (int y = 1; y < height - 1; y++) {
            int pos = x + (y * width);

            switch (tes.at(pos)) {
                case 1:
                    if (temp.at(pos - 1) >= temp.at(pos) ||
                        temp.at(pos + 1) > temp.at(pos)) {
                        pixel.at(pos) = 0;
                    }
                    break;
                case 2:
                    if (temp.at(pos - (width - 1)) >= temp.at(pos) ||
                        temp.at(pos + (width - 1)) > temp.at(pos)) {
                        pixel.at(pos) = 0;
                    }
                    break;
                case 3:
                    if (temp.at(pos - (width)) >= temp.at(pos) ||
                        temp.at(pos + (width)) > temp.at(pos)) {
                        pixel.at(pos) = 0;
                    }
                    break;
                case 4:
                    if (temp.at(pos - (width + 1)) >= temp.at(pos) ||
                        temp.at(pos + (width + 1)) > temp.at(pos)) {
                        pixel.at(pos) = 0;
                    }
                    break;
                default:
                    pixel.at(pos) = 0;
                    break;
            }
        }
    }

}

void PGM::doublethresh(int t1, int t2) {
    /* double thresholding
     * compare values from temp
     * copy into pixels
     */
    temp = pixel;
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int pos = x + (y * width);
            if (temp.at(pos) > t2) {
                pixel.at(pos) = 255;
            } else if (temp.at(pos) > t1) {
                pixel.at(pos) = 100;
            } else {
                pixel.at(pos) = 0;
            }
        }
    }
}

void PGM::hysteresis() {
    /* track edges with hysteresis
     * prolly can't be parallelized, copy pixel vals into pixel
     */
    for (int x = 1; x < width - 1; x++) {
        for (int y = 1; y < height - 1; y++) {
            int pos = x + (y * width);

            if (pixel.at(pos) == 127) {
                if (pixel.at(pos - 1) == 255 || pixel.at(pos + 1) == 255 ||
                    pixel.at(pos - 1 - width) == 255 ||
                    pixel.at(pos + 1 - width) == 255 ||
                    pixel.at(pos + width) == 255 ||
                    pixel.at(pos + width - 1) == 255 ||
                    pixel.at(pos + width + 1) == 255) {
                    pixel.at(pos) = 255;
                } else {
                    pixel[pos] = 0;
                }
            }
        }
    }
}

/**
 * Opens the PGM image file for write.
 * @param Path of the PGM image file being read.
 **/
void PGM::write(std::string filename) {
    ofstream writer(filename);

    writer << "P2\n";
    writer << width << " " << height << "\n";
    writer << "255\n";

    // print in 17 pixel long rows.
    int numval = 0;
    int flag = 0;
    int counter = 0;
    for (int x = 0; x < width; x++)
        for (int y = 0; y < height; y++) {
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
 * Sets: int width, int height, size_, std::vector<u_int8_t> pixel
 * @param Path of the PGM image file being read.
 **/
void PGM::open(std::string filename) {
    ifstream file;
    file.exceptions(ifstream::badbit);  // attach badbit exception (hacky
                                        // cpp exception attachment)
    string read;
    try {
        file.open(filename, ios::in);
        file >> read;
        if (read.compare("P2") == 0) {
            file >> width >> height;
            init();
            pixel.resize(size_);
            file >> read;
            int flag = 0;
            if (read == "255") {
                int counter = 0;
                int numval = 0;
                for (int x = 0; x < width; x++)
                    for (int y = 0; y < height; y++) {
                        file >> read;
                        int pos = counter;
                        pixel.at(pos) = stoi(read);
                        numval++;
                        if (numval % 17 == 0 && flag == 1) flag = 1;
                        counter++;
                    }
                cout << width << " " << height << " " << size_ << endl;
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
    PGM opened;
    opened.open("./input.pgm");
    opened.canny();
    cout << "All files written." << endl;
    cout << "succesfully exited";
    return 0;
}

