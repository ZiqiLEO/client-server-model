//
//  makefile.cpp
//  CS456 A1
//
//  Created by Leo-Ziqi on 2018-06-12.
//  Copyright © 2018 Leo-Ziqi. All rights reserved.
CFLAGS = -std=c++98 -ggdb -pedantic-errors -Wall -Werror

all :

.phony : client server

client : client.cc
g++ $(CFLAGS) client.cc -o $@

server: server.cc
g++ $(CFLAGS) server.cc -o $@
