# Copyright (c) 2020 The SandboxTEE Authors. All rights reserved.
# Use of this source code is governed by a GPLv2 license that can be
# found in the LICENSE file.

# $^ : dependency list
# # $@ : target

CC = gcc

CFLAGS=-I./

LDFLAGS=-L/usr/local/lib/ 
LDLIBS= -lm -lwolfssl

COMMON_OBJS=ifc.o util.o

OBJECTS=dt_ad_network.o cf_ad_network.o network_analyzer.o rtb_ssp.o rtb_ad_exchange.o rtb_dsp1.o rtb_dsp2.o rtb_analyzer1.o rtb_analyzer2.o ifc.o util.o

TARGET=dt_ad_network.out cf_ad_network.out network_analyzer.out rtb_ssp.out rtb_ad_exchange.out rtb_dsp1.out rtb_dsp2.out rtb_analyzer1.out rtb_analyzer2.out

all:  $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) -o $@ $(@:.out=.o) util.o ifc.o $(LDFLAGS) $(LDLIBS)
	 
clean:
	rm -f *.o
	rm -f $(TARGET)
					 

	
