#ifndef NOISEGENERATOR_H
#define NOISEGENERATOR_H

#include <QImage>
#include <QColor>
#include <QDebug>

#include <cmath>
#include <noise/noise.h>
#include <noiseutils.h>

class NoiseGenerator {
public:
    static void Heightmap(QImage &out, int width, int height, int seed = 0);
    static void SphericalHeightmap(QImage &out, int radius, int seed = 0);
private:
    static void ConvertNoiseMapToQImage(QImage &image, int width, int height, noise::utils::Image &noise);

};

#endif /* end of include guard: NOISEGENERATOR_H */
