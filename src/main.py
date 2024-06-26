import argparse
from PIL import Image

ASCII_CHARS = '@%#*+=-:. '

def resizeImage(image, newWidth, newHeight):
    resizedImage = image.resize((newWidth, newHeight))
    return resizedImage

def grayscaleImage(image):
    return image.convert('L')

def pixelsToAscii(image):
    pixels = image.getdata()
    asciiStr = ''
    for pixel in pixels:
        asciiStr += ASCII_CHARS[pixel // 26]
    return asciiStr

def main(imagePath, resourceFile, newWidth=30, newHeight=15):
    try:
        image = Image.open(imagePath)
    except Exception as e:
        return -1
    
    if not newHeight:
        width, height = image.size
        newHeight = int(newWidth * height / width)

    image = resizeImage(image, newWidth, newHeight)
    image = grayscaleImage(image)
    
    asciiStr = pixelsToAscii(image)
    imgWidth = image.width
    asciiStrLen = len(asciiStr)
    asciiImg = ''

    for i in range(0, asciiStrLen, imgWidth):
        asciiImg += asciiStr[i:i+imgWidth] + '\n'

    with open(f"resources/{resourceFile}", "w") as resource:
            resource.write(asciiImg)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Convert image to ASCII art and save RGB values')
    parser.add_argument('imagePath', type=str, help='path to the image file')
    parser.add_argument('resourceFile', type=str, help='output text file to save ASCII art and RGB values')
    parser.add_argument('--width', type=int, default=100, help='desired width of ASCII art')
    parser.add_argument('--height', type=int, help='desired height of ASCII art')

    args = parser.parse_args()
    main(args.imagePath, args.resourceFile, args.width, args.height)
    exit(0)
