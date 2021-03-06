#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include "StdDmaIndication.h"
#include "MemServerRequest.h"
#include "MMURequest.h"
#include "EncoderTestRequest.h"
#include "EncoderTestIndication.h"

sem_t test_sem;

int burstLen = 16;
int numWords = 0x1000/4;
size_t test_sz = numWords*sizeof(unsigned long int);
size_t alloc_sz = test_sz;

class EncoderTestIndication : public EncoderTestIndicationWrapper {
public:
  unsigned int rDataCnt;
  virtual void encoderTestDone(uint32_t v){
    printf( "EncoderTest::encoderTestDone(mismatch = %x)\n", v);
    sem_post(&test_sem);
  }
  EncoderTestIndication(int id) : EncoderTestIndicationWrapper(id){}
};

int main(int argc, char **argv) {
    EncoderTestRequestProxy *device = new EncoderTestRequestProxy(IfcNames_EncoderTestRequestS2H);
    EncoderTestIndication deviceIndication(IfcNames_EncoderTestIndicationH2S);
    MemServerRequestProxy *hostMemServerRequest = new MemServerRequestProxy(IfcNames_MemServerRequestS2H);
    MMURequestProxy *dmap = new MMURequestProxy(IfcNames_MMURequestS2H);
    DmaManager *dma = new DmaManager(dmap);
    MemServerIndication hostMemServerIndication(hostMemServerRequest, IfcNames_MemServerIndicationH2S);
    MMUIndication hostMMUIndication(dma, IfcNames_MMUIndicationH2S);

    const std::string path="../../data/xgmii.data";
    std::ifstream traceinfo(path.c_str());
    std::string line;

    int srcAlloc;
    srcAlloc = portalAlloc(alloc_sz, 0);
    unsigned long int *srcBuffer = (unsigned long int *)portalMmap(srcAlloc, alloc_sz);

    for (int i = 0; i < numWords; /*NONE*/ ) {
        std::getline(traceinfo, line);
        std::istringstream iss(line);
        std::string first_64;
        iss >> first_64;
        std::string second_64;
        iss >> second_64;
        srcBuffer[i++] = strtoul(second_64.c_str(), NULL, 16); /*second_64 is LSB*/
        srcBuffer[i++] = strtoul(first_64.c_str(), NULL, 16);
    }

    portalCacheFlush(srcAlloc, srcBuffer, alloc_sz, 1);
    unsigned int ref_srcAlloc = dma->reference(srcAlloc);
    printf( "Main::starting read %08x\n", numWords);
    device->startEncoder(ref_srcAlloc, numWords, burstLen, 1);
    sem_wait(&test_sem);
    return 0;
}
