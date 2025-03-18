# For the simplified photoshop, a struct variable was used to store the image and some additional details:
* A three-dimensional matrix (for the three RGB color channels and one channel for grayscale).    
* A copy (struct type) for the CROP, ROTATE, and APPLY commands.
* Indices x1, x2, y1, y2 for the potential SELECT command.
* Height and width indices, some semaphore-like variables, and the magic number (e.g., 3 for P3).
* To identify commands entered in the terminal, the entire line is read and then segmented. Depending on the first word, either "Invalid command" is printed, or the    appropriate command’s if block is executed.

* LOAD – The file is opened, and if the image is not null, the first lines are read (magic number, height, width, and the value 255). Depending on the magic number (2, 3 – ASCII, 5, 6 – binary), the pixel matrix is read accordingly. If the image is in color, pixels are stored in separate channels of the three-dimensional matrix (channel 0 for red, channel 1 for green, and channel 2 for blue). If an image is already in memory, it is freed to efficiently store the new one.

* SELECT – Upon loading an image, the selection indices are automatically initialized to the image corners. If the parameters are valid, the indices store the new values.
* SELECT ALL – The selection indices store the image corners.

* CROP – A copy is used to store the selected sub-image, then the three-dimensional matrix is freed from memory and reallocated according to the cropped image dimensions. Finally, the pixels from the copy are transferred back into the main matrix, and the copy is freed from memory.

* HISTOGRAM – For a grayscale image, the formula from the PDF is used. The frequency of pixel values is stored, pixels are grouped into "bins" so that there are y bins in total, and the maximum sum among the bins is recorded. After applying the formula from the PDF to the bin sums, the corresponding number of asterisks is displayed as required.

* EQUALIZE – For a grayscale image, the pixel frequency is stored, the given formula is applied, and the matrix pixels are updated with their new values.

* ROTATE – After the necessary checks, a copy is used to store the rotated image/sub-image (depending on the selection), rotated 90 degrees to the right. The image/sub-image is rotated as many times as needed to achieve the desired angle. For negative angles, multiple right rotations are performed instead: for example, instead of rotating once to the left, the image is rotated three times to the right. Finally, the copy is freed from memory.

* SAVE – The output filename is read, and it is checked whether "ascii" is specified. The image is saved accordingly (if "ascii" is not specified, it is saved as a binary file), and the magic number is modified if necessary (e.g., a P2 file saved in binary becomes P5).

* APPLY – It is verified whether a valid parameter was provided. If so, the kernel matrix [3][3] is assigned the corresponding values. A copy stores the pixel values from the selection, then the selection is iterated over in the main image matrix, and the pixels are updated with their new values. At the end, the copy is freed from memory.

* EXIT – It is checked whether an image is stored in memory, and an error message is displayed if none exists. If an image is in memory, it is freed.# SImplified-Photoshop
