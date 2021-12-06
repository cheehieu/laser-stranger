import argparse
import numpy as np
import cv2

def show_image(image):
    cv2.imshow('image', image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

def invert_image(image):
    return (255 - image)


# Parse command line arguments
ap = argparse.ArgumentParser()
ap.add_argument("-i", "--input", required=True,
	help="Path to input image")
ap.add_argument("-o", "--output", required=True,
	help="Path to output image")
ap.add_argument("-t", "--threshold", type = int, default = 40,
	help = "Threshold value")
args = vars(ap.parse_args())

# Load input image
image = cv2.imread(args["input"])
mask = cv2.imread('mask.png', 0)
mask_inv = invert_image(mask)

# Mask and threshold image
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
masked_img = cv2.bitwise_and(gray, mask)
idx = (mask==0)
masked_img[idx] = mask_inv[idx]
thresh = cv2.threshold(masked_img, args["threshold"], 255, cv2.THRESH_BINARY)[1]

# Thresholding
# methods = [
# 	("THRESH_BINARY", cv2.THRESH_BINARY),
# 	("THRESH_BINARY_INV", cv2.THRESH_BINARY_INV),
# 	("THRESH_TRUNC", cv2.THRESH_TRUNC),
# 	("THRESH_TOZERO", cv2.THRESH_TOZERO),
# 	("THRESH_TOZERO_INV", cv2.THRESH_TOZERO_INV)]
# for (threshName, threshMethod) in methods:
# 	(T, thresh) = cv2.threshold(gray, args["threshold"], 255, threshMethod)
# 	cv2.imshow(threshName, thresh)
# 	cv2.waitKey(0)

# k-means clustering color quantization
# Z = thresh.reshape((-1,1))
# Z = np.float32(Z)
# criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 10, 1.0)
# K = 26
# ret,label,center=cv2.kmeans(Z,K,None,criteria,10,cv2.KMEANS_RANDOM_CENTERS)
# center = np.uint8(center)
# res = center[label.flatten()]
# res2 = res.reshape((thresh.shape))
# show_image(res2)

# Connected components
thresh_inv = cv2.bitwise_not(thresh)
nb_components, output, stats, centroids = cv2.connectedComponentsWithStats(thresh_inv, connectivity=8)
sizes = stats[1:, -1]
nb_components = nb_components - 1
min_size = 100
reduced_img = np.zeros((output.shape))
for i in range(0, nb_components):
    if sizes[i] >= min_size:
        reduced_img[output == i + 1] = 255
reduced_img = invert_image(reduced_img)

# Morphological Operations
# thresh = cv2.dilate(reduced_img, None, iterations=1)
# thresh = cv2.erode(reduced_img, None, iterations=2)

# Write output image to disk
output_image = reduced_img
cv2.imwrite(args["output"], output_image)
show_image(masked_img)
show_image(thresh)
show_image(reduced_img)


# Extract dark pixels and perform cluster recognition (letters)
# Image denoising
# Fill
