# BLE based project for Industrial IoT use case

# About the project
  - In this project, we send user input over Bluetooth Low Energy(BLE) command to the target device(QCS610). On receiving the set of commands from the user on to the target board, camera Image will be captured and sent to the amazon aws s3 bucket. 

## Dependencies
- Ubuntu System 18.04 or above
- Install python 3.5 or above on the host system. 
- Install Adb tool (Android debugging bridge) on host system.
- Valid microsoft aws user account

## Prerequisites 
- Setting up the  Yocto build environment.
- Install the nRF connect android app on android mobile.
- Create a  s3 bucket service on AWS website.

### Build procedure for application in the Yocto Environment
- In this project, we have implemented ble application on top of the btapp source code. Download the rsp folder from the repo. Replace the existing folder in btapp source with new one.   

**Step-1**: Change directory to the yocto working environment directory
  ```
   $ cd  <working directory> 
  ```
**Step- 2** : Initialize the build environment using the below commands
  ```
  $ export  SHELL=/bin/bash
  $ source poky/qti-conf/set_bb_env.sh
  ```
- Select “qcs610-odk meta-qti-bsp” from the available machines menu. On selecting the machine, the ncurses menu starts. Select “qti-distro-fullstack-perf meta-qti-bsp” from Available Distributions  menu.The shell prompt will now move to the following build directory
 ``` 
 <working directory>/build-qti-distro-fullstack-perf $               
 ```
### building the btapp application
 ```
   $ bitbake -c compile -f bt-app
```  
- After successfully running this command, you can find the bt-app binary file generated in the path “<build_directory>/tmp-gibc/work/armv7ahf-neon/bt-app/git-ro/build/main/btapp”.
    
 push the executable file to the target board
```
   $ adb push btapp /data/ble/
```
###  Building the opencv library for target board 
  -   To install opencv library on the target board, the required meta recipes for opencv is already present in folder “poky/meta-openembedded/meta-oe/ recipes-support/opencv/opencv_3.4.5.bb” file. we need to follow the below steps to build.

  - Run the bitbake command for installing packages.
  ```
    $ bitbake opencv 
  ```  
- Once the build is complete the shared library and include file will be available in “<build_directory>/tmp-glibc/sysroots-components/armv7ahf-neon/opencv/usr”
Push the opencv shared library to the target board 
 ```
   $ cd  ./tmp-glibc/sysroots-components/armv7ahf-neon/opencv/usr/
   $ adb push lib/  /data/ble/
 ``` 
 **Note**: 
  - For more reference refer to the “QCS610/QCS410 Linux Platform Development Kit Quick Start Guide document”. 
  - Also make sure install the all the dependency library from the yocto build to the system (ex: libgphoto2, libv4l-utils) 
  - bb recipes of above  library are available inside meta-oe layer you can directly run bitbake command

### 2. Install the nrf connect app on the android mobile
  - Open the google play store and search the nrf-connect on the search option.  
  - select the “nrf connect for mobile” from the list of available apps. Tap the install  button. Agree the terms and conditions.
  - once the installation process completes, open the nrf connect application and scan for available devices. You can see a list of available ble devices. Make sure that you have turned on the bluetooth connection on your mobile.    

### 3. Create a s3 bucket service on AWS website
  - In order to create the s3 bucket, refer below url from amazon document. 
```
     https://docs.aws.amazon.com/AmazonS3/latest/userguide/creating-bucket.html
```
**Note:** once the bucket is created notedown the bucket name.

### Install boto3 library on board
 To install the boto3 package on qcs610. you need to build and install the following python3 packages on yocto build. below are the list of packages
     
   - python3-boto3
   - python3-botocore
   - python3-jmespath 
   - python3-s3transfer
   - html, date-util, multiprocessing, concurrent etc.

We can place the above mentioned bb recipe in the given folder name "poky/poky/meta-openembedded/meta-python/python/". meta recipes for these packages are available in the meta-recipe from given source repository. Afterwards, run the bitbake command python3-boto3 for each library 

Once the build is complete for all recipes, create a libboto3 directory and copy all the required libraries to the folder, and push this same folder to /usr/lib/python3.5/ folder path of target board.
 ```
    $  adb push libboto3/ /data/ble/ 
 ```

## Steps to build and run the application:
**Step-1**: Initialize the target board with root access.
   ```
      $ adb root 
      $ adb remount 
      $ adb shell  mount -o remount,rw /
      $ adb forward tcp:8900 tcp:8900
   ```
**Step-2**: push the required library and source file to the target board

    ```
       $ adb push capture.py /data/ble
       $ adb push  awssent.py /data/ble
    ``` 
   
**Note**: In file config.py, you need to fill your aws security key details

      - aws_access_key_id,
      - aws_secret_access_key
      - region and s3_bucket_name.
     
 For more information, please refer the below "url https://docs.aws.amazon.com/powershell/latest/userguide/pstools-appendix-sign-up.html"
   
**step3**: setup the target environment
  - To start the application on target board, execute below commands,
    ```
       /# adb shell
       /# 
    ```

To enable wifi connectivity on target board

      ```        
       /# wpa_supplicant -Dnl80211 -iwlan0 -c /etc/misc/wifi/wpa_supplicant.conf -ddddt &
       /# dhcpcd wlan0
      ```  
    
Export the shared library to the LD_LIBRARY_PATH
      
      ```   
        /# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/data/ble/lib/
        /# cd data/ble/
      ```
   
**Step 4**:  Execute the btapp binary on the target board,
     
     ```  
         /data/ble # ./btapp
     ```      
- After executing this command, it will provide a menu option from which we need to select “gap_menu”.  When we input “enable’ in gap_menu, it will turn ON the Bluetooth connection.

- When we input “main_menu’ in gap_menu, it will go back to main window, there you need to give input 'rsp_menu', it will go to rsp_menu, there you need to input rsp_init and rsp_start. once executing this command it will start the ble service and advertising.    

- Open the nrf connect application on the android mobile, and scan for available ble devices, it will show the list of available devices, you can find  the device name QCS610 on the list.Press connection button of the qcs610 device. It will open separate window there you can find different ble services and it characteristics with read and write permission.

- As part of this project we have created one custom service that has two characteristics, one characteristic having read permission and another characteristic having write permission.  Using this we are sending commands to the QCS610 camera for image capture and we can transfer the camera image to the s3 bucket using another ble command. 

- In this application, when we write a ‘capture’ command to the characteristic uuid (0xBB02) on an nrf connect app, qcs 610 device will start the camera application and capture the image frame and store the same in jpg format. 

- Similarly , When we give ‘sent’ command to the characteristic uuid ( 0XBB02) on nrf connect app, qcs 610 device will transfer the captured image file to the amazon s3 bucket.

- If you give ‘stop’ command to the 0XBB02 Characteristic uuid , it will stop advertising the ble.  

- Characteristic uuid (0xDD02) will read the commands sent by characteristic UUID (0XBB02). 

# Download Image
To see the captured Image, you need to log into amazon web service, go to the s3 bucket console, from there you can download the image
