/******************************************************************************************************************
 *                     This piece of software is original from Koivisto UCI engine.                               *
 * Using the code must be explicitly granted by the authors as well as the rights to use the code can be revoked. *
 *                          Removing this header from the files is forbidden.                                     *
 *           Any changes to the code must be marked as those and do not change the copyright.                     *
 *                                                                                                                *
 * Written by: Finn Eggers                                                                                        *
 ******************************************************************************************************************/

#ifndef KOIVISTO_DATA_H
#define KOIVISTO_DATA_H

#include <cassert>
#include <iomanip>
#include <new>
#include <ostream>
class Data {

    private:
    int width;
    int height;
    int size;

    float* values;
    Data*  gradient = nullptr;

    public:
    Data(int width, bool createGradients) {
        this->width  = width;
        this->height = 1;
        this->size   = width;

        assert(this->width % 8 == 0);

        this->values = new (std::align_val_t(32)) float[size] {};

        if (createGradients) {
            this->gradient = new Data(this->width, false);
        }
    }

    Data(int width, int height, bool createGradients) {
        this->width  = width;
        this->height = height;
        this->size   = width * height;

        assert(this->width % 8 == 0);
        assert(this->height % 8 == 0 || this->height == 1);

        this->values = new (std::align_val_t(32)) float[size] {};
        if (createGradients) {
            this->gradient = new Data(this->width, this->height, false);
        }
    }

    virtual ~Data() {
        //        delete[](std::align_val_t(32), values);
        if (gradient != nullptr) {
            delete gradient;
        }
    }

    bool hasGradient() { return gradient != nullptr; }

    float  get(int width) const { return values[width]; }
    float& get(int width) { return values[width]; }
    float  get(int width, int height) const { return values[width + height * this->width]; }
    float& get(int width, int height) { return values[width + height * this->width]; }

    float  operator()(int width) const { return get(width); }
    float& operator()(int width) { return get(width); }
    float  operator()(int width, int height) const { return get(width, height); }
    float& operator()(int width, int height) { return get(width, height); }

    friend std::ostream& operator<<(std::ostream& os, const Data& data) {

        os << std::fixed << std::setprecision(3);
        for (int i = 0; i < data.height; i++) {
            for (int n = 0; n < data.width; n++) {
                os << std::setw(10) << data(n, i);
            }
            os << "\n";
        }
        return os;
    }

    void randomise(float lower, float upper) {
        for (int i = 0; i < size; i++) {
            this->values[i] = static_cast<float>(rand()) / RAND_MAX * (upper - lower) + lower;
        }
    }

    float* getValues() const { return values; }

    [[nodiscard]] int getWidth() const { return width; }
    [[nodiscard]] int getHeight() const { return height; }
    [[nodiscard]] int getSize() const { return size; }

    Data* getGradient() const { return gradient; }
};

#endif    // KOIVISTO_DATA_H
