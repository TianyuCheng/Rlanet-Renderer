#include "NoiseGenerator.h"

noise::module::Perlin NoiseGenerator::perlin = noise::module::Perlin();

void NoiseGenerator::ConvertNoiseMapToQImage(QImage &out, int width, int height, noise::utils::Image &noise) {

    // Build and write each horizontal line to the file.
    for (int y = 0; y < height; y++) {
        noise::utils::Color* pSource = noise.GetSlabPtr (y);
        for (int x = 0; x < width; x++) {
            int r = pSource->red;
            int b = pSource->blue;
            int g = pSource->green;
            ++pSource;
            out.setPixel(x, y, qRgb(r, g, b));
        }
    }
}

void NoiseGenerator::Heightmap(QImage &out, int width, int height, int seed) {
    if (seed) NoiseGenerator::perlin.SetSeed(seed);

    noise::utils::NoiseMap noiseMap;
    noise::utils::NoiseMapBuilderSphere noiseMapBuilder;
    noiseMapBuilder.SetSourceModule (perlin);
    noiseMapBuilder.SetDestNoiseMap (noiseMap);
    noiseMapBuilder.SetDestSize (width, height);
    noiseMapBuilder.SetBounds (-90.0, 90.0, -90.0, 90.0);
    noiseMapBuilder.Build();

    noise::utils::RendererImage renderer;
    noise::utils::Image image;
    renderer.SetSourceNoiseMap (noiseMap);
    renderer.SetDestImage (image);
    renderer.Render();
    ConvertNoiseMapToQImage(out, width, height, image);
}

void NoiseGenerator::SphericalHeightmap(QImage &out, int radius, int seed) {
    int width = int(radius * 2 * M_PI);
    int height = int(radius * M_PI);

    if (seed) NoiseGenerator::perlin.SetSeed(seed);

    noise::utils::NoiseMap noiseMap;
    noise::utils::NoiseMapBuilderSphere noiseMapBuilder;
    noiseMapBuilder.SetSourceModule (perlin);
    noiseMapBuilder.SetDestNoiseMap (noiseMap);
    noiseMapBuilder.SetDestSize (width, height);
    noiseMapBuilder.SetBounds (-90.0, 90.0, -180.0, 180.0);
    noiseMapBuilder.Build();

    noise::utils::RendererImage renderer;
    noise::utils::Image image;
    renderer.SetSourceNoiseMap (noiseMap);
    renderer.SetDestImage (image);
    renderer.Render();
    ConvertNoiseMapToQImage(out, width, height, image);
}
