/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2011, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <sys/time.h>
// #include <iostream>
 // #include <boost/thread/thread.hpp>
// #include <chrono>
// #include <thread>
#include <bcm2835.h>

 // Input on pin 15
#define PIN RPI_GPIO_P1_05


int msleep(unsigned long milisec)
{
    struct timespec req={0};
    time_t sec=(int)(milisec/1000);
    milisec=milisec-(sec*1000);
    req.tv_sec=sec;
    req.tv_nsec=milisec*1000000L;
    while(nanosleep(&req,&req)==-1)
        continue;
    return 1;
}

int main(void)
{
  if (!bcm2835_init())
        return 1;
  CURL *curl;
  CURLcode res;
  struct curl_slist *headers=NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append( headers, "charsets: utf-8");
  char json[1000];

  /* In windows, this will init the winsock stuff */
  // curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();
  struct timeval tv;
  long double prevTimeMs;
  gettimeofday(&tv, NULL);
  prevTimeMs = (tv.tv_sec*1000)+(tv.tv_usec/1000);
  long double impKhz;
  impKhz = 1000;
  long double msInHour;
  msInHour = 3600*1000;
  int first = 1;
  /* get a curl handle */
  if(curl) {
    /* First set the URL that is about to receive our POST. This URL can
       just as well be a https:// URL if that is what should receive the
       data. */
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1);
    // curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
    curl_easy_setopt(curl, CURLOPT_URL, "http://80.112.144.22/meterstand");
    /* Now specify the POST data */
    srand(time(NULL));

    // Setup pin as input
    bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_INPT);
    // With pullup
    bcm2835_gpio_set_pud(PIN, BCM2835_GPIO_PUD_DOWN);

    while(1){
      if (bcm2835_gpio_lev(PIN) == HIGH){
        long secs;
        long tics;
        double kwatt;
        double watt;
        long double curTimeMs;
        gettimeofday(&tv, NULL);
        secs = tv.tv_sec;
        tics = tv.tv_usec;
        curTimeMs = (secs*1000)+(tics/1000);
        // printf("current Time %f\n",curTimeMs);
        kwatt = msInHour / ((curTimeMs-prevTimeMs)*impKhz);
        watt = kwatt*1000;
        // sprintf(json,"{\"unix_time\":%ld,\"microseconds\":%ld,\"miliseconds\":%20.3f,\"watt\":%f,\"kilowatt\":%20.8f}",secs,tics,curTimeMs,watt,kwatt);
        sprintf(json,"{\"unix_time\":%ld,\"microseconds\":%ld,\"watt\":%f,\"kilowatt\":%20.8f}",secs,tics,watt,kwatt);
        //set the previous time to this last seen flash.
        prevTimeMs = curTimeMs;
        // printf("{\"unix_time\":%ld,\"microseconds\":%ld}",secs,tics);
        if(!first)
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);

        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if(res != CURLE_OK){
          fprintf(stderr, "curl_easy_perform() failed: %s\n",
                  curl_easy_strerror(res));
          //error occured. break out or should we just continue?
          // break;
        }
        //test random delay between [0,1500] ms.

        first=0;
      }
      msleep(10);
    }
    curl_easy_cleanup(curl);

    /* always cleanup */
  }
  curl_global_cleanup();
  return 0;
}