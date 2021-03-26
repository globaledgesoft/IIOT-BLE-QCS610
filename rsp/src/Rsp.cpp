/*
 * Copyright (c) 2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Rsp.hpp"
#include<string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<unistd.h>

#define LOGTAG "RSP "
#define UNUSED

#define CONNECTABLE 1
#define SCANNABLE 0
#define LEGACYMODE 0
#define ANONYMOUS 0
#define INCLUDETXPOWER 1

Rsp *rsp = NULL;
int serverif, clientif;

using namespace gatt;

#define TRANSPORT 0


extern GattLibService *g_gatt;
GattServer *mServer = NULL;
GattService *mService = NULL;
GattCharacteristic *mgattCharacteristic = NULL;
GattService *mService1 = NULL;
GattCharacteristic *mgattCharacteristic1 = NULL;

GattDescriptor *mgattDescriptor = NULL;
AdvertisingSetParameters *mParameters;
AdvertiseData *mAdvData = NULL;
int retpid=-1;
volatile int action_no =-1;
uint8_t result_value[20];

const Uuid SERVICE_UUID = Uuid::FromString("0000AA01-0000-1000-8000-00805f9b34fb");
const Uuid CHARWRITE_UUID = Uuid::FromString("0000BB02-0000-1000-8000-00805f9b34fb");
const Uuid CHARREAD_UUID = Uuid::FromString("0000DD02-0000-1000-8000-00805f9b34fb");
const Uuid DESC_UUID = Uuid::FromString("0000CC03-0000-1000-8000-00805f9b34fb");

string adv_data = "Remote Start Profile";


class RspServerCallback  :public GattServerCallback{
  public:
  void onConnectionStateChange(string deviceAddress, int status,
                                                              int newState) {
    ALOGD(LOGTAG"%s status: %d",__FUNCTION__,status);
    rsp->StopAdvertisement();
  }

  void onServiceAdded(int status,GattService *service) {
    ALOGD(LOGTAG"%s status: %d",__FUNCTION__,status);
  }

  void onCharacteristicReadRequest(string deviceAddress, int requestId,
                              int offset, GattCharacteristic *characteristic) {
    ALOGD(LOGTAG"%s",__FUNCTION__);
    fprintf(stdout,"charvalue is= %s\n",result_value);
    rsp->SendResponse(deviceAddress,requestId,0,offset,result_value);
  }

  void onCharacteristicWriteRequest(string deviceAddress,int requestId,
        GattCharacteristic *characteristic,bool preparedWrite,bool responseNeeded,
                                                     int offset,uint8_t* value) {
    ALOGD(LOGTAG"%s",__FUNCTION__);
    if (value != NULL)
      sprintf((char *)result_value,"%s",value);
    rsp->WriteResponse(deviceAddress,requestId,0,offset,value);
  }

  void onDescriptorReadRequest(string deviceAddress, int requestId,
                                       int offset, GattDescriptor *descriptor) {
    ALOGD(LOGTAG"%s",__FUNCTION__);
  }

  void onDescriptorWriteRequest(string deviceAddress, int requestId,
                           GattDescriptor *descriptor,bool preparedWrite,
                           bool responseNeeded, int offset, uint8_t * value) {
    ALOGD(LOGTAG"%s",__FUNCTION__);
    rsp->SendResponse(deviceAddress,requestId,0,offset,value);
  }

  void onExecuteWrite(string deviceAddress, int requestId, bool execute) {
    ALOGD(LOGTAG"%s",__FUNCTION__);
  }

  void onNotificationSent(string deviceAddress, int status) {
    ALOGD(LOGTAG"%s",__FUNCTION__);
  }

  void onMtuChanged(string deviceAddress, int mtu) {
    ALOGD(LOGTAG"%s",__FUNCTION__);
  }

  void onPhyUpdate(string deviceAddress,int txPhy, int rxPhy, int status) {
    ALOGD(LOGTAG"%s",__FUNCTION__);
  }

  void onPhyRead(string deviceAddress,int txPhy,int rxPhy,int status) {
    ALOGD(LOGTAG"%s",__FUNCTION__);
  }

  void onConnectionUpdated(string deviceAddress,int interval,
                                  int latency,int timeout,int status) {
    ALOGD(LOGTAG"%s",__FUNCTION__);
  }
};


class RSPAdvertiserCallback  :public AdvertisingSetCallback
{
  public:
  void onAdvertisingSetStarted (AdvertisingSet *advertisingSet, int txPower, int status) {
    ALOGD(LOGTAG"(%s) ", __FUNCTION__);
  }

  void  onAdvertisingDataSet(AdvertisingSet *advertisingset,int status) {
    ALOGD(LOGTAG"(%s)", __FUNCTION__);
  }

  void onAdvertisingSetStopped (AdvertisingSet *advertisingSet) {
    ALOGD(LOGTAG"(%s)",__FUNCTION__);
  }

  void onAdvertisingEnabled (AdvertisingSet *advertisingSet, bool enable, int status) {
    ALOGD(LOGTAG"(%s)",__FUNCTION__);
  }

  void onScanResponseDataSet (AdvertisingSet *advertisingSet, int status) {
    ALOGD(LOGTAG"(%s)",__FUNCTION__);
  }

  void onAdvertisingParametersUpdated (AdvertisingSet *advertisingSet, int txPower, int status) {
    ALOGD(LOGTAG"(%s)",__FUNCTION__);
  }

  void onPeriodicAdvertisingParametersUpdated (AdvertisingSet *advertisingSet, int status) {
    ALOGD(LOGTAG"(%s)",__FUNCTION__);
  }

  void onPeriodicAdvertisingDataSet (AdvertisingSet *advertisingSet, int status) {
    ALOGD(LOGTAG"(%s)",__FUNCTION__);
  }

  void onPeriodicAdvertisingEnabled (AdvertisingSet *advertisingSet, bool enable, int status) {
    ALOGD(LOGTAG"(%s)",__FUNCTION__);
  }

  void onOwnAddressRead (AdvertisingSet *advertisingSet, int addressType, string address) {
    ALOGD(LOGTAG"(%s)",__FUNCTION__);
  }

  void onStartSuccess(AdvertiseSettings *settingsInEffect) {
    ALOGD(LOGTAG "onStartSuccess()");
  }

  void onStartFailure(int errorCode) {
    ALOGE(LOGTAG "onStartFailure()");
  }

};


RspServerCallback *mRspServerCb = NULL;
RSPAdvertiserCallback *mRSPAdvCb = NULL;
GattLeAdvertiser *mAdvertiser = NULL;


Rsp::Rsp(GattLibService* g_gatt) {
  ALOGE(LOGTAG "rsp instantiated");
  fprintf(stdout,"rsp instantiated ");
  mlibservice = g_gatt->getGatt();
}


Rsp::~Rsp() {
  fprintf(stdout, "(%s) RSP DeInitialized",__FUNCTION__);
  SetDeviceState(WLAN_INACTIVE);
  delete(mServer);
  delete(mRSPAdvCb);
  delete(mRspServerCb);
}

bool Rsp::EnableRSP() {
  fprintf(stdout, "(%s) Enable RSP Initiated \n",__FUNCTION__);
  ALOGE(LOGTAG "%s",__FUNCTION__);
  mServer = new GattServer(g_gatt,TRANSPORT);
  mRspServerCb = new RspServerCallback();
  mServer->registerCallback(*mRspServerCb);
  mRSPAdvCb    = new RSPAdvertiserCallback();
  mAdvertiser = GattLeAdvertiser::getGattLeAdvertiser();
  AddService();
  
  return true;
}


void * action(void *arg)
{

while(1) {
if (action_no == 1) {
     // playing 
       retpid = fork();
       int status1;
        action_no = -1; 
       if(retpid == 0){
          char *progName = "python3";
          char *args[] = {progName, "capture.py", NULL};
          execvp(progName, args);

        }              
    waitpid(retpid, &status1, 0);        
}
else if(action_no == 2 ){
       retpid = fork();
       int status1;
        action_no = -1; 
       if(retpid == 0){
          char *progName = "python3";
          char *args[] = {progName, "awssent.py", NULL};
          execvp(progName, args);

        }              
    waitpid(retpid, &status1, 0);        
}
else if(action_no == 3){
  printf("invalid_state");
 action_no = -1; 
} 


usleep(100000);
}
return 0;
}




bool Rsp::StartAdvertisement() {
  pthread_t action_thread;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  pthread_create(&action_thread,&attr,action,(void *)NULL);
  
  std::vector<uint8_t> vec(adv_data.begin(), adv_data.end());
  ALOGE(LOGTAG "%s",__FUNCTION__);
  fprintf(stdout,"Rsp::StartAdvertisement \n");
  SetDeviceState(WLAN_INACTIVE);
  mParameters = AdvertisingSetParameters::Builder()
                            .setConnectable(CONNECTABLE)
                            .setScannable(SCANNABLE)
                            .setLegacyMode(LEGACYMODE)
                            .setAnonymous(ANONYMOUS)
                            .setIncludeTxPower(INCLUDETXPOWER)
                            .setInterval(AdvertisingSetParameters::INTERVAL_MEDIUM)
                            .setTxPowerLevel(AdvertisingSetParameters::TX_POWER_MEDIUM)
                            .build();
  AdvertiseData::Builder builder = AdvertiseData::Builder().setIncludeDeviceName(true)
                                  .setIncludeTxPowerLevel(true);
  builder.addServiceUuid(SERVICE_UUID);
  builder.addServiceData(SERVICE_UUID,vec);
  
  mAdvData = builder.build();
  try {
      mAdvertiser->startAdvertisingSet(mParameters,
                         mAdvData,NULL,NULL,NULL,mRSPAdvCb);
  } catch(const std::exception &ex) {
    ALOGE(LOGTAG"%s start Advertising exception  %s", __FUNCTION__, ex.what());
    return false;
  }
  return true;
}

void Rsp::SendResponse(string deviceAddress, int requestId, int status,
                                              int offset, uint8_t * value) {
  ALOGE(LOGTAG "%s",__FUNCTION__);
  mServer->sendResponse(deviceAddress,requestId,status,offset,value);
}


void Rsp::WriteResponse(string deviceAddress, int requestId, int status,
                                              int offset, uint8_t * value) {
   ALOGE(LOGTAG "%s",__FUNCTION__);
   char array[20]="";

   if(memcmp((char *)value, "stop", 4) == 0)
   { 
       sprintf(array,"%s","stop advertising"); 
       rsp->StopAdvertisement();
   }
   int status1;
   if(memcmp((char *)value, "capture", 7) == 0)
   { 
       sprintf(array,"%s","Image captured");
       action_no = 1; 
   }
   if(memcmp((char *)value, "sent", 4) == 0)
   { 
       sprintf(array,"%s","image transfered"); 
       action_no = 2;
   }
      

  mServer->sendResponse(deviceAddress,requestId,status,offset,(uint8_t *)array);
}


void Rsp::StopAdvertisement(){
  ALOGE(LOGTAG "%s",__FUNCTION__);
  mAdvertiser->stopAdvertising(mRSPAdvCb);
}

bool Rsp::AddService() {
  ALOGE(LOGTAG "%s",__FUNCTION__);
  mService  = new GattService(SERVICE_UUID,GattService::SERVICE_TYPE_PRIMARY);
  AddCharacteristics(CHARWRITE_UUID,GATT_PROP_WRITE, GATT_PERM_WRITE);       // Adding Write charteristic
  mService->addCharacteristic(mgattCharacteristic);
  AddCharacteristics(CHARREAD_UUID,GATT_PROP_READ, GATT_PERM_READ);        // Adding Read charteristics
  mService->addCharacteristic(mgattCharacteristic);
  mServer->addService(*mService);
  return true;
}


bool Rsp::AddCharacteristics(const Uuid UUID, int property, int permission ) {
  ALOGE(LOGTAG "%s",__FUNCTION__);
  mgattCharacteristic = new GattCharacteristic(UUID,property,permission);
  
  return true;
}

bool Rsp::AddDescriptor(const Uuid UUID, int permissions) {
  ALOGE(LOGTAG "%s",__FUNCTION__);
  mgattDescriptor = new GattDescriptor(UUID,permissions);
  mgattCharacteristic->addDescriptor(mgattDescriptor);
  return true;
}



