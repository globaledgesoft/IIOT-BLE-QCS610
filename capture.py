# file for capture the image.
import cv2

def capture_image():
    
    cap = cv2.VideoCapture("qtiqmmfsrc ldc=TRUE !video/x-raw, format=NV12, width=1280, height=720, framerate=30/1 ! videoconvert ! appsink", cv2.CAP_GSTREAMER)    
    count = 15
    while(count):             # skipping the initial bluish frame
        count = count - 1 
        ret ,frame = cap.read()
        if(count == 5):
            cv2.imwrite('/data/ble/image.jpg',frame)
            break
    cap.release()
    print("Image captured")  
   

if __name__ == "__main__":
	capture_image()
