/**
 * @file manipulations.hpp
 * @brief Core image augmentation operations used in augmento.
 * @author Emmanuel Butsana
 * @date Initial release: May 27, 2025
 *
 * Contains transformation and filtering functions for brightness, contrast,
 * rotation, noise injection, and other common image manipulations.
 */

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <stdexcept>
#include <string>
#include <random>
#include <cmath>

#define PI 3.14159

// ===============================
// Geometric Operations
// ===============================

/**
 * @brief Rotate an image without cropping any region.
 * @param im Input image.
 * @param deg Rotation angle in degrees.
 * @return Rotated image with adjusted bounds.
 */
cv::Mat rotateImageNoCrop(const cv::Mat& im, double deg);

/**
 * @brief Rotate an image with cropping to avoid fill-in regions.
 * @param im Input image.
 * @param deg Rotation angle in degrees.
 * @return Cropped and rotated image.
 */
cv::Mat rotateImageCrop(const cv::Mat& im, double deg);

/**
 * @brief Rotate an image and clip parts that fall outside the frame.
 * @param im Input image.
 * @param deg Rotation angle in degrees.
 * @return Clipped rotated image.
 */
cv::Mat rotateImage(const cv::Mat& im, double deg);

/**
 * @brief Reflect an image horizontally (across vertical axis).
 * @param im Input/output image (modified in-place).
 * @return 0 on success, -1 on failure.
 */
int reflectImageHorizontal(cv::Mat& im);

/**
 * @brief Reflect an image vertically (across horizontal axis).
 * @param im Input/output image (modified in-place).
 * @return 0 on success, -1 on failure.
 */
int reflectImageVertical(cv::Mat& im);

/**
 * @brief Resize an image to the specified dimensions.
 * @param im Input image to resize.
 * @param width Target width in pixels.
 * @param height Target height in pixels.
 * @return Resized image with the specified dimensions.
 */
cv::Mat resizeImage(const cv::Mat& im, int width, int height);

/**
 * @brief Resize an image using a scale factor.
 * @param im Input image to resize.
 * @param scale Scale factor.
 * @return Resized image with the specified scale.
 */
cv::Mat resizeImage(const cv::Mat& im, int scale);

/**
 * @brief Crop a fixed region from the input image.
 * @param im Input image.
 * @param x Top-left x-coordinate of the crop.
 * @param y Top-left y-coordinate of the crop.
 * @param width Width of the crop region.
 * @param height Height of the crop region.
 * @return Cropped image of specified size.
 */
cv::Mat cropImage(const cv::Mat& im, int x, int y, int width, int height);

/**
 * @brief Randomly crop a region from the input image.
 * @param im Input image to crop from.
 * @param crop_width Width of the crop region.
 * @param crop_height Height of the crop region.
 * @return Cropped image of specified size.
 */
cv::Mat randomCrop(const cv::Mat& im, int crop_width, int crop_height);

/**
 * @brief Apply an affine transformation to the input image.
 * @param im Input image.
 * @param matrix 2×3 affine transformation matrix (CV_32F or CV_64F).
 * @return Transformed image.
 */
cv::Mat affineTransform(const cv::Mat& im, const cv::Mat& matrix);

// ===============================
// Color & Intensity Adjustments
// ===============================

/**
 * @brief Apply random brightness, contrast, saturation, and hue adjustments.
 * @param im Input/output image (modified in-place).
 * @param brightness Brightness adjustment factor (±range).
 * @param contrast Contrast scaling factor (±range).
 * @param saturation Saturation scaling factor (±range).
 * @param hue Hue shift range in degrees (±range).
 * @return 0 on success, -1 on failure.
 */
int colorJitter(cv::Mat& im, double brightness, double contrast, double saturation, int hue);

/**
 * @brief Apply histogram equalization on image intensity.
 * @param im Input/output color image (modified in-place).
 * @return 0 on success, -1 on failure.
 */
int histogramEqualization(cv::Mat& im);

/**
 * @brief Apply white balancing using the gray-world assumption.
 * @param im Input/output image (modified in-place).
 * @return 0 on success, -1 on failure.
 */
int whiteBalance(cv::Mat& im);

/**
 * @brief Convert a color image to grayscale.
 * @param im Input/output image (modified in-place).
 * @return 0 on success, -1 on failure.
 */
int toGrayscale(cv::Mat& im);

/**
 * @brief Adjust image brightness by adding a constant.
 * @param im Input/output image (modified in-place).
 * @param val Scalar value to add to each pixel.
 * @return 0 on success, -1 on failure.
 */
int adjustBrightness(cv::Mat& im, double val);

/**
 * @brief Adjust image contrast by scaling pixel intensities.
 * @param im Input/output image (modified in-place).
 * @param val Scaling factor.
 * @return 0 on success, -1 on failure.
 */
int adjustContrast(cv::Mat& im, double val);

/**
 * @brief Adjust image saturation in HSV space.
 * @param im Input/output image (modified in-place).
 * @param val Saturation scaling factor.
 * @return 0 on success, -1 on failure.
 */
int adjustSaturation(cv::Mat& im, double val);

/**
 * @brief Adjust image hue in HSV space.
 * @param im Input/output image (modified in-place).
 * @param val Hue shift (±180).
 * @return 0 on success, -1 on failure.
 */
int adjustHue(cv::Mat& im, int val);

// ===============================
// Noise & Filtering
// ===============================

/**
 * @brief Add Gaussian noise to the input image.
 * @param im Input/output image (modified in-place).
 * @param mean Mean of the Gaussian distribution.
 * @param stdev Standard deviation of the Gaussian distribution.
 * @return 0 on success, -1 on failure.
 */
int injectNoise(cv::Mat& im, double mean, double stdev);

/**
 * @brief Blur the image using a square averaging kernel.
 * @param im Input/output image (modified in-place).
 * @param ksize Size of the kernel (must be odd).
 * @return 0 on success, -1 on failure.
 */
int blurImage(cv::Mat& im, int ksize);

/**
 * @brief Sharpen the image using Laplacian enhancement.
 * @param im Input/output image (modified in-place).
 * @return 0 on success, -1 on failure.
 */
int sharpenImage(cv::Mat& im);

/**
 * @brief Randomly erases a rectangular region within the image.
 * @param im Input/output image (modified in-place).
 * @param min_h Minimum height of the erase region (in pixels).
 * @param max_h Maximum height of the erase region (in pixels).
 * @param min_w Minimum width of the erase region (in pixels).
 * @param max_w Maximum width of the erase region (in pixels).
 * @return 0 on success, -1 on failure.
 */
int randomErase(cv::Mat& im, int min_h, int max_h, int min_w, int max_w);

