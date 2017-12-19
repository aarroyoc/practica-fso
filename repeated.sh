#!/bin/bash

sort salida.txt | uniq -c | sort -nr
